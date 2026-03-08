#pragma once

#ifndef PORT_AUDIO_CONTROLLER_H
#define PORT_AUDIO_CONTROLLER_H

#include "AtomicLock.h"
#include "AudioController.h"
#include "Constant.h"
#include "OutputSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <portaudio.h>
#include <string>

/// <summary>
/// Port Audio:  This is the controller for the port audio calls. NOTE! THERE CAN ONLY BE ONCE INSTANCE!!! 
/// </summary>
class PortAudioController : public AudioController
{
public:

	/// <summary>
	/// Primary RT Audio Callback:  They have a separate thread managing the device audio. So, this will be on their thread; and we 
	/// will process all of our SynthPlaybackDevice* work here - including key strokes. 
	/// </summary>
	/// <param name="outputBuffer">Output Audio Buffer (see RTAudio initialization)</param>
	/// <param name="inputBuffer">Input Audio (used for recording or duplex mode)</param>
	/// <param name="frameCount">Number of audio frames (Frame = { channel 1, .. channel N }, usually L/R channels)</param>
	/// <param name="timeInfo">Stream time information</param>
	/// <param name="statusFlags">Port Audio stream status</param>
	/// <param name="userData">This will contain a pointer to the synth configuration. However, it is not thread safe!</param>
	/// <returns>Error indicator for port audio</returns>
	int AudioCallback(const void* inputBuffer, void* outputBuffer,
							  unsigned long frameCount,
							  const PaStreamCallbackTimeInfo* timeInfo,
							  PaStreamCallbackFlags statusFlags,
							  void* userData);

	static int FooAudioCallback(const void* inputBuffer, void* outputBuffer,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void* userData) {
		return 0;
	}

private:

	static PortAudioController* This;
	static PaStream* Stream;
	static AudioStreamFormat Format;

public:

	PortAudioController(AtomicLock* playbackLock);
	~PortAudioController();

	/// <summary>
	/// Initialization function for the synth backend. This must be called before starting the player!
	/// </summary>
	bool Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry, const AudioCallbackDelegate& audioCallback) override;

	/// <summary>
	/// Starts any threads associated with the controller, after initialization.
	/// </summary>
	void Start() override;

	/// <summary>
	/// Disposes of backend, and controller resources
	/// </summary>
	bool Dispose() override;

	/// <summary>
	/// Opens the RT Audio backend stream with specified user data
	/// </summary>
	/// <param name="userData">Arbitrary data sent along with the backend stream</param>
	bool OpenStream(void* userData) override;
	bool CloseStream() override;

	bool StartStream() override;
	bool StopStream() override;

	bool IsStreamOpen() override;
	bool IsStreamRunning() override;

private:

	void HandleError(const PaError& error);
	void GetDeviceFormatString(const PaDeviceInfo* deviceInfo, std::string& destination) const;
	void GetDeviceFormatParagraph(const PaDeviceInfo* deviceInfo, std::string& destination) const;

private:

	AudioCallbackDelegate* _audioCallback;

	OutputSettings* _outputSettings;

	bool _initialized;

};

#endif