#pragma once

#ifndef RT_AUDIO_CONTROLLER_H
#define RT_AUDIO_CONTROLLER_H

#include "AtomicLock.h"
#include "AudioController.h"
#include "Constant.h"
#include "PlaybackInfo.h"
#include "PlaybackUserData.h"
#include "RtAudio.h"
#include <string>

/// <summary>
/// Static functions for RT Audio callbacks; and the container for the RtAudio* instance
/// </summary>
class RtAudioController : public AudioController
{
public:

	/// <summary>
	/// Primary RT Audio Callback:  They have a separate thread managing the device audio. So, this will be on their thread; and we 
	/// will process all of our SynthPlaybackDevice* work here - including key strokes. 
	/// </summary>
	/// <param name="outputBuffer">Output Audio Buffer (see RTAudio initialization)</param>
	/// <param name="inputBuffer">Input Audio (used for recording or duplex mode)</param>
	/// <param name="nFrames">Number of audio frames (Frame = { channel 1, .. channel N }, usually L/R channels)</param>
	/// <param name="streamTime">Stream time in seconds</param>
	/// <param name="status">RT Audio stream status</param>
	/// <param name="userData">This will contain a pointer to the synth configuration. However, it is not thread safe!</param>
	/// <returns>Error indicator to RT Audio</returns>
	int AudioCallback(void* outputBuffer,
					  void* inputBuffer,
					  unsigned int nFrames,
					  double streamTime,
					  RtAudioStreamStatus status,
					  void* userData);

	/// <summary>
	/// Primary RT Audio Error Callback: This will be called on a separate thread when there is a backend error in
	/// RT Audio.
	/// </summary>
	/// <param name="type">Type of error</param>
	/// <param name="errorText">Error text</param>
	void ErrorCallback(RtAudioErrorType type, const std::string& errorText);

public:

	RtAudioController(AtomicLock* playbackLock);
	~RtAudioController();

	/// <summary>
	/// Initialization function for the synth backend. This must be called before starting the player!
	/// </summary>
	bool Initialize(PlaybackUserData* playbackData, const AudioCallbackDelegate& audioCallback) override;

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
	bool OpenStream(PlaybackUserData* userData) override;
	bool CloseStream() override;

	bool StartStream() override;
	bool StopStream() override;

	bool IsStreamOpen() override;
	bool IsStreamRunning() override;

private:

	void GetDeviceFormatString(const RtAudio::DeviceInfo& deviceInfo, std::string& destination) const;
	void GetDeviceFormatParagraph(const RtAudio::DeviceInfo& deviceInfo, std::string& destination) const;

	RtAudioFormat FormatTo(AudioStreamFormat format);
	AudioStreamFormat FormatFrom(RtAudioFormat format);

private:

	AudioCallbackDelegate* _audioCallback;

	RtAudio* _instance;

	RtAudioErrorType _lastErrorType;
	std::string* _lastErrorText;

	bool _initialized;

};

#endif