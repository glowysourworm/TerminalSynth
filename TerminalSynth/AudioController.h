#pragma once

#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include "AtomicLock.h"
#include "BaseController.h"
#include "EffectRegistry.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MidiPlaybackDevice.h"
#include "OutputSettings.h"
#include "PlaybackClock.h"
#include "RtAudioUserData.h"
#include "SynthPlaybackDevice.h"
#include "SynthSettings.h"
#include <string>

class AudioController : public BaseController
{
public:

	AudioController(AtomicLock* atomicLock);
	~AudioController();

	bool Initialize(SynthSettings* configuration, OutputSettings* parameters, EffectRegistry* effectRegistry) override;
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

	/// <summary>
	/// Simple output function (this will be replaced with an output processing stage)
	/// </summary>
	void GetUpdate(float& streamTime, float& audioTime, float& frontendTime, float& latency, float& left, float& right);

public:

	/// <summary>
	/// Processes audio from the RT Audio backend - which fills the output buffer with the provided
	/// number of samples (interleved frames).
	/// </summary>
	/// <param name="outputBuffer">RT Audio buffer</param>
	/// <param name="numberOfFrames">Number of (L/R) frames to process</param>
	/// <param name="streamTime">Current stream time from RT Audio</param>
	/// <param name="configuration">This should be the SynthSettings* which is provided on this thread to process, also.</param>
	int ProcessAudioCallback(float* outputBuffer, unsigned int numberOfFrames, double streamTime, RtAudioUserData* userData);

private:

	bool _midiMode;	
	bool _initialized;

	SynthPlaybackDevice<float>* _synthDevice;
	MidiPlaybackDevice<float>* _midiDevice;

	PlaybackClock* _streamClock;
	LoopTimer* _audioTimer;
	IntervalTimer* _synthIntervalTimer;

	float _outputL;
	float _outputR;
};

#endif