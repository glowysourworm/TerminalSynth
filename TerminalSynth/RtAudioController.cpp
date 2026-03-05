#include "OutputSettings.h"
#include "RtAudioController.h"
#include "RtAudioUserData.h"
#include <RtAudio.h>
#include <exception>
#include <functional>
#include <string>

int RtAudioController::AudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Backend thread still running!");

	if (!this->IsStreamOpen())
		throw new std::exception("RT Audio Controller stream not open! Backend thread still running!");

	// Audio Callback:  Casting (void*) user data to our synth configuration! And, the output buffer!
	//
    return (*_audioCallback)((float*)outputBuffer, nFrames, streamTime, (RtAudioUserData*)userData);
}

void RtAudioController::ErrorCallback(RtAudioErrorType type, const std::string& errorText)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Backend thread still running!");

	if (!this->IsStreamOpen())
		throw new std::exception("RT Audio Controller stream not open! Backend thread still running!");

	_lastErrorText->clear();
	_lastErrorText->append(errorText);
	_lastErrorType = type;
}

RtAudioController::RtAudioController()
{
	_outputSettings = nullptr;
	_initialized = false;

	_audioCallback = nullptr;
	_instance = nullptr;
	_outputDevice = nullptr;

	_lastErrorType = RtAudioErrorType::RTAUDIO_UNKNOWN_ERROR;
	_lastErrorText = new std::string("");
}
RtAudioController::~RtAudioController()
{
	Dispose();
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

		// Initialized
		_initialized = true;
	}
	catch (std::exception ex)
	{
		// Initialize Failure
		_initialized = true;
	}

	return _initialized;
}

bool RtAudioController::Dispose()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before disposing the stream");

	try
	{
		if (_outputSettings != nullptr)
		{
			delete _outputSettings;
			_outputSettings = nullptr;
		}
		if (_outputDevice != nullptr)
		{
			delete _outputDevice;
			_outputDevice = nullptr;
		}

		if (_instance->isStreamRunning())
			_instance->stopStream();

		if (_instance->isStreamOpen())
			_instance->closeStream();

		// RESET 
		delete _instance;
		delete _audioCallback;
		delete _lastErrorText;

		_instance = nullptr;
		_audioCallback = nullptr;
		_lastErrorText = nullptr;

		// Must Re-Initialize!
		_initialized = false;

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
		RtAudio::StreamParameters outputParameters;

		// Output Device
		//
		auto outputDeviceIndex = _instance->getDefaultOutputDevice();
		auto outputDevice = _instance->getDeviceInfo(outputDeviceIndex);
		_outputDevice = new RtAudio::DeviceInfo(outputDevice);
		
		outputParameters.deviceId = outputDevice.ID;
		outputParameters.nChannels = outputDevice.outputChannels;
		outputParameters.firstChannel = 0;

		RtAudio::StreamOptions options;

		//options.flags |= RTAUDIO_SCHEDULE_REALTIME;					
		options.numberOfBuffers = 4;						// Has to do with audio format!
		//options.flags |= RTAUDIO_HOG_DEVICE;
		options.flags |= RTAUDIO_MINIMIZE_LATENCY;
		//options.flags |= RTAUDIO_NONINTERLEAVED; 


		// Output Buffer Calculation: ~device period (ms) * (s / ms) * (samples / s) = [samples]
		//
		// RT Audio:  (see openStream comments) will try to calculate a desired buffer size based on this input
		//			  value. So, we'll send it something the device likes; and see what it comes back with.
		//
		//unsigned int outputBufferFrameSize = (unsigned int)(10.6667 * 0.001 * outputDevice.preferredSampleRate);
		unsigned int outputBufferFrameSize = 32;
		unsigned int frontendFrameSize = outputBufferFrameSize;

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
							outputDevice.preferredSampleRate,		// Device Sampling Rate
							&outputBufferFrameSize,					// Device (preferred) Frame Size (RT Audio will adjust this)
							audioCallback,							// Audio Callback
							userData,								// void* shared data
							&options);

		auto hostApi = _instance->getCurrentApi();
		auto deviceFormat = std::to_string(outputDevice.preferredSampleRate) + " (smp/sec), " + std::to_string(outputDevice.outputChannels) + " channels";

		// Set Playback Parameters:  These are created in int main(); but they're invalid until
		//							 we set "initialized" to true, as long as the controller pattern
		//							 is strictly kept, you won't have a private pointer to them.
		//
		_outputSettings->UpdateDevice(
						_instance->getApiDisplayName(hostApi),
						deviceFormat,
						outputDevice.name,
						outputDevice.preferredSampleRate,
						outputDevice.outputChannels,
						outputBufferFrameSize);

		// Start Stream!
		_instance->startStream();
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
