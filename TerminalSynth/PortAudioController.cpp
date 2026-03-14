#include "AtomicLock.h"
#include "AudioController.h"
#include "Constant.h"
#include "PlaybackInfo.h"
#include "PlaybackUserData.h"
#include "PortAudioController.h"
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

	if (!this->IsStreamRunning())
		throw new std::exception("Port Audio Controller stream not running!");

	switch (statusFlags)
	{
	case 0:						// Nothing to report
		break;
	case paInputUnderflow:
		break;
	case paInputOverflow:
		break;
	case paOutputUnderflow:
		break;
	case paOutputOverflow:
		break;
	case paPrimingOutput:
		break;
	default:
		throw new std::exception("Unhandled port audio status flag:  PortAudioController.cpp");
	}

	// Audio Callback:  Casting (void*) user data to our synth configuration! And, the output buffer!
	//
	(PaStreamCallbackResult)(*_audioCallback)(outputBuffer, PortAudioController::Format, frameCount, timeInfo->currentTime, 0, (PlaybackUserData*)userData);

	return 0;
}

bool PortAudioController::Initialize(PlaybackUserData* playbackData, const AudioCallbackDelegate& audioCallback)
{
	if (_initialized)
		throw new std::exception("Port Audio Controller already initialzed! Must call Dispose() before re-initializing the backend");

	try
	{
		PaError error = Pa_Initialize();

		HandleError(error);

		auto hostApiIndex = Pa_GetDefaultHostApi();

		//for (int index = 0; index < Pa_GetHostApiCount(); index++)
		//{
		//	if (Pa_GetHostApiInfo(index)->type == PaHostApiTypeId::paWDMKS)
		//		hostApiIndex = index;
		//}

		auto hostApi = Pa_GetHostApiInfo(hostApiIndex);

		auto hostError = Pa_GetLastHostErrorInfo();

		if (hostError != NULL)
			HandleError(hostError->errorCode);

		_audioCallback = new AudioCallbackDelegate(audioCallback);

		// Host API
		playbackData->GetPlaybackInfo()->SetForHostApi(hostApi->name);

		// Output Device List
		//
		auto selectedDevice = playbackData->GetDeviceRegister()->GetSelectedDevice();
		auto defaultDeviceIndex = Pa_GetDefaultOutputDevice();
		bool deviceSelected = false;

		for (int index = 0; index < Pa_GetDeviceCount(); index++)
		{
			auto deviceInfo = Pa_GetDeviceInfo(index);

			// Host Api (Check)
			if (deviceInfo->hostApi != hostApiIndex)
				continue;

			// Output Channels (Check)
			if (deviceInfo->maxOutputChannels <= 0)
				continue;

			std::string deviceFormat;
			std::string deviceParagraph;

			// DEVICE FORMAT: No generic function from port audio!
			//PaWinWaveFormat format;
			//PaWasapi_GetDeviceDefaultFormat(&format, 1, index);
			
			// BRUTE FORCE FORMAT:  Try querying their backend, otherwise, it's "device specific"
			//
			PaStreamParameters outputParams;
			outputParams.channelCount = deviceInfo->maxOutputChannels;
			outputParams.device = index;
			outputParams.hostApiSpecificStreamInfo = NULL;
			outputParams.suggestedLatency = deviceInfo->defaultLowOutputLatency;

			AudioStreamFormat sampleFormat;
			bool sampleFormatFound = false;

			// Float 32
			outputParams.sampleFormat = this->FormatTo(AudioStreamFormat::Float32);

			if (Pa_IsFormatSupported(NULL, &outputParams, deviceInfo->defaultSampleRate) == 0)
			{
				sampleFormat = AudioStreamFormat::Float32;
				sampleFormatFound = true;
			}
				

			// Int 32
			outputParams.sampleFormat = this->FormatTo(AudioStreamFormat::Int32);

			if (!sampleFormatFound && Pa_IsFormatSupported(NULL, &outputParams, deviceInfo->defaultSampleRate) == 0)
			{
				sampleFormat = AudioStreamFormat::Int32;
				sampleFormatFound = true;
			}
				

			// Int 16
			outputParams.sampleFormat = this->FormatTo(AudioStreamFormat::Int16);

			if (!sampleFormatFound && Pa_IsFormatSupported(NULL, &outputParams, deviceInfo->defaultSampleRate) == 0)
			{
				sampleFormat = AudioStreamFormat::Int16;
				sampleFormatFound = true;
			}
				

			// Int 8
			outputParams.sampleFormat = this->FormatTo(AudioStreamFormat::Int8);

			if (!sampleFormatFound && Pa_IsFormatSupported(NULL, &outputParams, deviceInfo->defaultSampleRate) == 0)
			{
				sampleFormat = AudioStreamFormat::Int8;
				sampleFormatFound = true;
			}

			if (!sampleFormatFound)
				continue;
			
			GetDeviceFormatString(sampleFormat, deviceFormat);
			GetDeviceFormatParagraph(deviceInfo, deviceParagraph);

			playbackData->GetDeviceRegister()->AddDevice(
				index,
				deviceFormat,
				deviceParagraph,
				deviceInfo->name,
				sampleFormat,
				deviceInfo->defaultSampleRate,
				deviceInfo->maxOutputChannels, 
				512,													// The number of output frames should be device / api specific				
				deviceInfo->defaultLowOutputLatency,
			   (index == defaultDeviceIndex) || !deviceSelected);

			deviceSelected = true;
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

		auto error = Pa_Terminate();

		HandleError(error);

		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

bool PortAudioController::OpenStream(PlaybackUserData* userData)
{
	if (!_initialized)
		throw new std::exception("RT Audio Controller not initialzed! Must call Initialize() before opening the stream");

	try
	{
		// Output Device
		//
		auto outputDevice = userData->GetDeviceRegister()->GetSelectedDevice();

		PaStreamParameters outputParameters;
		outputParameters.channelCount = outputDevice->GetNumberOfChannels();
		outputParameters.device = outputDevice->GetId();
		outputParameters.sampleFormat = this->FormatTo(outputDevice->GetDeviceFormat());
		outputParameters.hostApiSpecificStreamInfo = NULL;
		outputParameters.suggestedLatency = outputDevice->GetSuggestedLatencySeconds();

		// STREAM FORMAT SETTING!
		PortAudioController::Format = outputDevice->GetDeviceFormat();
		
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

		userData->UpdateDevice(
			outputDevice->GetDeviceName(),
			streamInfo->sampleRate,
			outputDevice->GetBufferFrameSize(),
			true);

		userData->GetPlaybackInfo()->GetStreamInfo()->streamActualLatency = streamInfo->outputLatency;
		userData->GetPlaybackInfo()->GetStreamInfo()->streamSampleRate = streamInfo->sampleRate;
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

void PortAudioController::GetDeviceFormatString(AudioStreamFormat streamFormat, std::string& destination) const
{
	destination.clear();

	switch (streamFormat)
	{
	case AudioStreamFormat::Float32:
		destination.append("Float 32 bit");
		break;
	case AudioStreamFormat::Int32:
		destination.append("Int 32 bit");
		break;
	case AudioStreamFormat::Int16:
		destination.append("Int 16 bit");
		break;
	case AudioStreamFormat::Int8:
		destination.append("Int 8 bit");
		break;
	default:
		throw new std::exception("Unhandled format type:  PortAudioController.cpp");
	}
}

void PortAudioController::GetDeviceFormatParagraph(const PaDeviceInfo* deviceInfo, std::string& destination) const
{
	destination.clear();

	destination += std::string(deviceInfo->name) + "\n";
	destination += "Sample Rate:	            " + std::to_string(deviceInfo->defaultSampleRate) + "\n";
	destination += "Number of Channels:      " + std::to_string(deviceInfo->maxOutputChannels) + "\n";
}

PaSampleFormat PortAudioController::FormatTo(AudioStreamFormat format) const
{
	switch (format)
	{
	case AudioStreamFormat::Float32:
		return paFloat32;
	case AudioStreamFormat::Int32:
		return paInt32;
	case AudioStreamFormat::Int16:
		return paInt16;
	case AudioStreamFormat::Int8:
		return paInt8;
	default:
		throw new std::exception("Unhandled format type:  PortAudioController.cpp");
	}
}

AudioStreamFormat PortAudioController::FormatFrom(PaSampleFormat format) const
{
	switch (format)
	{
	case paFloat32:
		return AudioStreamFormat::Float32;
	case paInt32:
		return AudioStreamFormat::Int32;
	case paInt16:
		return AudioStreamFormat::Int16;
	case paInt8:
		return AudioStreamFormat::Int8;
	default:
		throw new std::exception("Unhandled format type:  PortAudioController.cpp");
	}
}
