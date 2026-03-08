#include "AtomicLock.h"
#include "AudioController.h"
#include "Constant.h"
#include "OutputSettings.h"
#include "PlaybackFormatTransformer.h"
#include "PortAudioController.h"
#include "RtAudioUserData.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <exception>
#include <portaudio.h>
#include <string>

// STATIC INITIALIZER -------------------------
PortAudioController* PortAudioController::This;
PaStream* PortAudioController::Stream;
AudioStreamFormat PortAudioController::Format;
// --------------------------------------------

PortAudioController::PortAudioController(AtomicLock* playbackLock) : AudioController(playbackLock)
{
	_outputSettings = nullptr;
	_initialized = false;
	_audioCallback = nullptr;

	// STATIC INSTANCE! (SEE IF WE CAN FIX THE STATIC-ONLY CALLBACK!)
	PortAudioController::This = this;
}
PortAudioController::~PortAudioController()
{
	Dispose();
}
void PortAudioController::Start()
{
	// Nothing to do 
}
int PortAudioController::AudioCallback(
	const void* input, void* outputBuffer,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	if (!_initialized)
		throw new std::exception("Port Audio Controller not initialzed! Backend thread still running!");
	
	if (!this->IsStreamOpen())
		throw new std::exception("Port Audio Controller stream not open! Backend thread still running!");

	auto streamInfo = Pa_GetStreamInfo(PortAudioController::Stream);

	// TRY THIS!!! TIME OFFSET!
	// 
	//timeInfo->outputBufferDacTime
	
	// Audio Callback:  Casting (void*) user data to our synth configuration! And, the output buffer!
	//
	return (*_audioCallback)(outputBuffer, PortAudioController::Format, frameCount, timeInfo->currentTime, streamInfo->outputLatency, (RtAudioUserData*)userData);
}

bool PortAudioController::Initialize(SynthSettings* configuration, OutputSettings* outputSettings, SoundRegistry* effectRegistry, const AudioCallbackDelegate& audioCallback)
{
	if (_initialized)
		throw new std::exception("Port Audio Controller already initialzed! Must call Dispose() before re-initializing the backend");

	try
	{
		PaError error = Pa_Initialize();

		HandleError(error);

		auto hostApiIndex = Pa_GetDefaultHostApi();

		for (int index = 0; index < Pa_GetHostApiCount(); index++)
		{
			if (Pa_GetHostApiInfo(index)->type == PaHostApiTypeId::paWASAPI)
				hostApiIndex = index;
		}

		auto hostApi = Pa_GetHostApiInfo(hostApiIndex);

		_audioCallback = new AudioCallbackDelegate(audioCallback);
		_outputSettings = outputSettings;

		// Host API
		_outputSettings->SetHostApi(hostApi->name);

		// Output Device List
		//
		auto selectedDevice = outputSettings->GetSelectedDevice();
		auto defaultDeviceIndex = Pa_GetDefaultOutputDevice();

		for (int index = 0; index < Pa_GetDeviceCount(); index++)
		{
			auto deviceInfo = Pa_GetDeviceInfo(index);

			if (deviceInfo->maxOutputChannels <= 0)
				continue;

			std::string deviceFormat;
			std::string deviceParagraph;

			// DEVICE FORMAT: No generic function from port audio!
			//PaWinWaveFormat format;
			//PaWasapi_GetDeviceDefaultFormat(&format, 1, index);
			
			GetDeviceFormatString(deviceInfo, deviceFormat);
			GetDeviceFormatParagraph(deviceInfo, deviceParagraph);

			_outputSettings->AddDevice(
				index,
				deviceFormat,
				deviceParagraph,
				deviceInfo->name,
				deviceInfo->defaultSampleRate,
				deviceInfo->maxOutputChannels, 512, index == defaultDeviceIndex);
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

bool PortAudioController::Dispose()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before disposing the stream");

	try
	{
		if (this->IsStreamRunning())
			this->StopStream();

		if (this->IsStreamOpen())
			this->CloseStream();

		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

bool PortAudioController::OpenStream(void* userData)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	try
	{
		// Output Device
		//
		auto outputDevice = _outputSettings->GetSelectedDevice();

		PaStreamParameters outputParameters;
		outputParameters.channelCount = outputDevice->GetNumberOfChannels();
		outputParameters.device = outputDevice->GetId();
		outputParameters.sampleFormat = paInt16;
		outputParameters.hostApiSpecificStreamInfo = NULL;
		outputParameters.suggestedLatency = _outputSettings->GetStreamLatency();

		// STREAM FORMAT SETTING!
		PortAudioController::Format = AudioStreamFormat::Int16;

		// The typedef callback definition needs a conversion for using a non-static function!
		//
		auto error = Pa_OpenStream(&PortAudioController::Stream, 
									NULL, 
									&outputParameters, 
									outputDevice->GetSamplingRate(), 
									paFramesPerBufferUnspecified, 
									paNoFlag, 

									// NEED CONVERSION:  There needs to be a way to do this callback without static methods
									//
									[](const void* input, void* outputBuffer,
										unsigned long frameCount,
										const PaStreamCallbackTimeInfo* timeInfo,
										PaStreamCallbackFlags statusFlags,
										void* userData)
									{
										return PortAudioController::This->AudioCallback(input, outputBuffer, frameCount, timeInfo, statusFlags, userData);
									},
									userData);

		HandleError(error);

		auto streamInfo = Pa_GetStreamInfo(PortAudioController::Stream);

		_outputSettings->UpdateDevice(
			outputDevice->GetDeviceName(),
			streamInfo->sampleRate,
			0,
			true);

		_outputSettings->SetStreamLatency(streamInfo->outputLatency);
	}
	catch (std::exception ex)
	{
		return false;
	}

	return true;
}

bool PortAudioController::CloseStream()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	if (!this->IsStreamOpen())
		throw new std::exception("RT Audio Controller stream not open! Must call OpenStream() to open the stream");

	try
	{
		auto error = Pa_CloseStream(PortAudioController::Stream);

		HandleError(error);

		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

bool PortAudioController::StartStream()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	if (this->IsStreamRunning())
		throw new std::exception("RT Audio Controller stream already running!");

	auto error = Pa_StartStream(PortAudioController::Stream);

	HandleError(error);

	return true;
}

bool PortAudioController::StopStream()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	if (!this->IsStreamRunning())
		throw new std::exception("RT Audio Controller stream already stopped!");

	auto error = Pa_StopStream(PortAudioController::Stream);

	HandleError(error);

	return true;
}

bool PortAudioController::IsStreamOpen()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	auto result = Pa_IsStreamActive(PortAudioController::Stream);

	HandleError(result);

	return result == 1;	// Active (Zero => Non-Active)
}

bool PortAudioController::IsStreamRunning()
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	auto result = Pa_IsStreamStopped(PortAudioController::Stream);

	HandleError(result);

	return result == 0;	// Stopped = 1, Running = 0
}

void PortAudioController::HandleError(const PaError& error)
{
	// Error codes are all less than zero
	if (error >= 0)
		return;

	// ERROR
	else
	{
		const char* errorText = Pa_GetErrorText(error);
		throw new std::exception(errorText);
	}
}

void PortAudioController::GetDeviceFormatString(const PaDeviceInfo* deviceInfo, std::string& destination) const
{
	destination.clear();
	destination = std::to_string(deviceInfo->defaultSampleRate) + " (Hz), " + std::to_string(deviceInfo->maxOutputChannels) + " Channels";
}

void PortAudioController::GetDeviceFormatParagraph(const PaDeviceInfo* deviceInfo, std::string& destination) const
{
	destination.clear();

	destination += std::string(deviceInfo->name) + "\n";
	destination += "Sample Rate:	            " + std::to_string(deviceInfo->defaultSampleRate) + "\n";
	destination += "Number of Channels:      " + std::to_string(deviceInfo->maxOutputChannels) + "\n";
}
