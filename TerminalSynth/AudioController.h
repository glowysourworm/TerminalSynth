#pragma once

#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include "AtomicLock.h"
#include "BaseController.h"
#include "OutputSettings.h"
#include "RtAudioUserData.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <exception>
#include <functional>
#include <string>

/// <summary>
/// Static functions for RT Audio callbacks; and the container for the RtAudio* instance
/// </summary>
class AudioController : public BaseController
{
public:

	/// <summary>
	/// Callback to process RT Audio data (which has a callback with extra variables we don't need)
	/// </summary>
	using AudioCallbackDelegate = std::function<int(float* outputBuffer, unsigned int numberFrames, double streamTime, double streamLatench, RtAudioUserData* userData)>;

public:

	AudioController(AtomicLock* playbackLock) : BaseController(playbackLock) {};
	~AudioController() {};

	bool Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry) override
	{
		throw new std::exception("Please use the Initialize function with the audio callback");
	}

	/// <summary>
	/// Initialization function for the synth backend. This must be called before starting the player!
	/// </summary>
	virtual bool Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry, const AudioCallbackDelegate& audioCallback) = 0;

	/// <summary>
	/// Starts any threads associated with the controller, after initialization.
	/// </summary>
	virtual void Start() = 0;

	/// <summary>
	/// Disposes of backend, and controller resources
	/// </summary>
	virtual bool Dispose() = 0;

	/// <summary>
	/// Opens the RT Audio backend stream with specified user data
	/// </summary>
	/// <param name="userData">Arbitrary data sent along with the backend stream</param>
	virtual bool OpenStream(void* userData) = 0;
	virtual bool CloseStream() = 0;

	virtual bool StartStream() = 0;
	virtual bool StopStream() = 0;

	virtual bool IsStreamOpen() = 0;
	virtual bool IsStreamRunning() = 0;

};

#endif