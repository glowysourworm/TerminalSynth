#pragma once

#ifndef PLAYBACK_CONTROLLER_H
#define PLAYBACK_CONTROLLER_H

#include "AtomicLock.h"
#include "BaseController.h"
#include "Constant.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MidiPlaybackDevice.h"
#include "OutputSettings.h"
#include "PlaybackClock.h"
#include "RtAudioUserData.h"
#include "SoundRegistry.h"
#include "SynthPlaybackDevice.h"
#include "SynthSettings.h"
#include <string>

class PlaybackController : public BaseController
{
public:

	PlaybackController(AtomicLock* playbackLock);
	~PlaybackController();

	bool Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry) override;
	bool Dispose() override;
	void Start() override;

	/// <summary>
	/// Switches between midi / regular mode
	/// a file to load. 
	/// </summary>
	void SetMidiMode(const std::string& midiFile);

	/// <summary>
	/// Switches between midi / synth mode
	/// </summary>
	void SetSynthMode();

public:

	/// <summary>
	/// Processes audio from the RT Audio backend - which fills the output buffer with the provided
	/// number of samples (interleved frames).
	/// </summary>
	/// <param name="outputBuffer">RT Audio buffer</param>
	/// <param name="numberOfFrames">Number of (L/R) frames to process</param>
	/// <param name="streamTime">Current stream time from RT Audio</param>
	/// <param name="configuration">This should be the SynthSettings* which is provided on this thread to process, also.</param>
	int ProcessAudioCallback(void* outputBuffer, AudioStreamFormat streamFormat, unsigned int numberOfFrames, double streamTime, double streamLatency, RtAudioUserData* userData);

private:

	bool _midiMode;	
	bool _initialized;

	SynthPlaybackDevice* _synthDevice;
	MidiPlaybackDevice* _midiDevice;

	PlaybackClock* _streamClock;
	LoopTimer* _audioTimer;
	IntervalTimer* _audioSampleTimer;
	IntervalTimer* _audioLockAcquireTimer;
};

#endif