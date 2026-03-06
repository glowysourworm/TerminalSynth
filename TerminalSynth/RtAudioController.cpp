#include "OutputSettings.h"
#include "RtAudioController.h"
#include "RtAudioUserData.h"
#include <RtAudio.h>
#include <exception>
#include <functional>
#include <string>

RtAudioController::RtAudioController()
{
	_instance = nullptr;
	_outputSettings = nullptr;
	_initialized = false;
	_audioCallback = nullptr;
	_lastErrorType = RtAudioErrorType::RTAUDIO_UNKNOWN_ERROR;
	_lastErrorText = new std::string("");
}
RtAudioController::~RtAudioController()
{
	DisposeBackend();

	delete _lastErrorText;
}
int RtAudioController::AudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Backend thread still running!");

	if (!this->IsStreamOpen())
		throw new std::exception("RT Audio Controller stream not open! Backend thread still running!");

	// Audio Callback:  Casting (void*) user data to our synth configuration! And, the output buffer!
	//
    return (*_audioCallback)((float*)outputBuffer, nFrames, streamTime, _instance->getStreamLatency(), (RtAudioUserData*)userData);
}

void RtAudioController::ErrorCallback(RtAudioErrorType type, const std::string& errorText)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Backend thread still running!");

	//if (!this->IsStreamOpen())
	//	throw new std::exception("RT Audio Controller stream not open! Backend thread still running!");

	_lastErrorText->clear();
	_lastErrorText->append(errorText);
	_lastErrorType = type;
}

bool RtAudioController::Initialize(OutputSettings* outputSettings, const AudioCallbackDelegate& callback)
{
	if (_initialized)
		throw new std::exception("RT Audio Controller already initialzed! Must call Dispose() before re-initializing the backend");

	try
	{
		// Buffer Frame Calculation:  How many buffer frames will be appropriate?
		//
		// After some tedious work with the Windows WASAPI, and using our real time
		// loop, it was obvious that there is a difficult problem trying to synchronize
		// the audio backend with the frontend looping. 
		//
		// The approximate time for our primary loop is ~7ms. Rendering the actual 
		// audio is a very small portion of this time, for synchronous setup.
		//
		// RT Audio callbacks are on a separate thread. So, we're going to try to
		// manage our loop so the time to render audio is small. Also, there will
		// be a shared pointer to the SynthSettings*, which will be carefully
		// set by our thread, and read by the other. This is NOT a thread-safe 
		// operation; but we won't care as long as we are just setting primitive 
		// variables.
		//
		// The size of the RT Audio buffer should be set from the device. The WASAPI
		// returned approx ~10.6667ms as the default period. So, we're going to select
		// an audio buffer time of this much; and see how that works. Later, we'll 
		// try to take this number from the device itself.
		//

		auto errorCallback = std::bind(&RtAudioController::ErrorCallback, this,
										std::placeholders::_1,
										std::placeholders::_2);

		_instance = new RtAudio(RtAudio::Api::WINDOWS_WASAPI, errorCallback);
		_audioCallback = new AudioCallbackDelegate(callback);
		_outputSettings = outputSettings;
		_lastErrorText->clear();

		// Host API
		_outputSettings->SetHostApi(_instance->getApiDisplayName(RtAudio::Api::WINDOWS_WASAPI));


		// Output Device List
		//
		auto deviceIds = _instance->getDeviceIds();
		auto selectedDevice = outputSettings->GetSelectedDevice();

		for (int index = 0; index < deviceIds.size(); index++)
		{
			auto deviceInfo = _instance->getDeviceInfo(deviceIds[index]);

			if (deviceInfo.outputChannels <= 0)
				continue;

			std::string deviceFormat;
			std::string deviceParagraph;

			GetDeviceFormatString(deviceInfo, deviceFormat);
			GetDeviceFormatParagraph(deviceInfo, deviceParagraph);

			_outputSettings->AddDevice(
				deviceInfo.ID,
				deviceFormat,
				deviceParagraph,
				deviceInfo.name,
				deviceInfo.preferredSampleRate,
				deviceInfo.outputChannels, 512, deviceInfo.isDefaultOutput);
		}

		// Initialized
		_initialized = true;
	}
	catch (std::exception ex)
	{
		// Initialize Failure
		_initialized = false;
	}

	return _initialized;
}

bool RtAudioController::DisposeBackend()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before disposing the stream");

	try
	{
		if (_instance->isStreamRunning())
			_instance->stopStream();

		if (_instance->isStreamOpen())
			_instance->closeStream();

		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

bool RtAudioController::OpenStream(void* userData)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	try
	{
		// Output Device
		//
		auto outputDevice = _outputSettings->GetSelectedDevice();
		
		RtAudio::StreamParameters outputParameters;
		outputParameters.deviceId = outputDevice->GetId();
		outputParameters.nChannels = outputDevice->GetNumberOfChannels();
		outputParameters.firstChannel = 0;

		RtAudio::StreamOptions options;

		//options.flags |= RTAUDIO_SCHEDULE_REALTIME;					
		options.numberOfBuffers = 4;						// Has to do with audio format!
		//options.flags |= RTAUDIO_HOG_DEVICE;
		//options.flags |= RTAUDIO_MINIMIZE_LATENCY;
		//options.flags |= RTAUDIO_NONINTERLEAVED; 


		// Output Buffer Calculation: ~device period (ms) * (s / ms) * (samples / s) = [samples]
		//
		// RT Audio:  (see openStream comments) will try to calculate a desired buffer size based on this input
		//			  value. So, we'll send it something the device likes; and see what it comes back with.
		//
		//unsigned int outputBufferFrameSize = (unsigned int)(10.6667 * 0.001 * outputDevice.preferredSampleRate);
		unsigned int outputBufferFrameSize = 32;

		auto audioCallback = std::bind(&RtAudioController::AudioCallback, this,
										std::placeholders::_1,
										std::placeholders::_2,
										std::placeholders::_3,
										std::placeholders::_4,
										std::placeholders::_5,
										std::placeholders::_6);

		_instance->openStream(&outputParameters,					// 
							NULL,									// Duplex Mode (input parameters)
							RTAUDIO_FLOAT32,						// RT Audio Format
							outputDevice->GetSamplingRate(),		// Device Sampling Rate
							&outputBufferFrameSize,					// Device (preferred) Frame Size (RT Audio will adjust this)
							audioCallback,							// Audio Callback
							userData,								// void* shared data
							&options);

		// Set Playback Parameters:  These are created in int main(); but they're invalid until
		//							 we set "initialized" to true, as long as the controller pattern
		//							 is strictly kept, you won't have a private pointer to them.
		//
		_outputSettings->UpdateDevice(
						outputDevice->GetDeviceName(),
						_instance->getStreamSampleRate(),
						outputBufferFrameSize,
						true);

		_outputSettings->SetStreamLatency(_instance->getStreamLatency());
	}
	catch (std::exception ex)
	{
		return false;
	}

	return true;
}

bool RtAudioController::CloseStream()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	if (!_instance->isStreamOpen())
		throw new std::exception("RT Audio Controller stream not open! Must call OpenStream() to open the stream");

	try
	{
		_instance->closeStream();

		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

bool RtAudioController::StartStream()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	if (_instance->isStreamRunning())
		throw new std::exception("RT Audio Controller stream already running!");

	// Start Stream!
	return _instance->startStream() == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

bool RtAudioController::StopStream()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	if (!_instance->isStreamRunning())
		throw new std::exception("RT Audio Controller stream already stopped!");

	// Stop Stream!
	return _instance->stopStream() == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

bool RtAudioController::IsStreamOpen()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	return _instance->isStreamOpen();
}

bool RtAudioController::IsStreamRunning()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	return _instance->isStreamRunning();
}

void RtAudioController::GetDeviceFormatString(const RtAudio::DeviceInfo& deviceInfo, std::string& destination) const
{
	destination.clear();
	destination = std::to_string(deviceInfo.preferredSampleRate) + " (Hz), " + std::to_string(deviceInfo.outputChannels) + " Channels";
}

void RtAudioController::GetDeviceFormatParagraph(const RtAudio::DeviceInfo& deviceInfo, std::string& destination) const
{
	destination.clear();

	destination += deviceInfo.name + "\n";
	destination += "Sample Rate:	            " + std::to_string(deviceInfo.currentSampleRate) + "\n";
	destination += "Sample Rate (preferred): " + std::to_string(deviceInfo.currentSampleRate) + "\n";
	destination += "Number of Channels:      " + std::to_string(deviceInfo.outputChannels) + "\n";
}
