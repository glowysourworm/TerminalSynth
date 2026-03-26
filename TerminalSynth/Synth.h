#pragma once

#ifndef SYNTH_H
#define SYNTH_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthSettings.h"
#include "SynthVoiceBase.h"
#include "SynthVoicePool.h"

// Class to define static piano notes and store their names / frequencies w.r.t. the SDL keyboard
// defined inputs.
//
class Synth
{

public:
	Synth(const SynthSettings* configuration, unsigned int numberOfChannels, unsigned int samplingRate);
	~Synth();

	// Update Configuration
	void Initialize(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters);

	// Update Configuration
	void Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters);

	// Sets midi notes on / off
	void SetNote(int midiNumber, bool pressed, const PlaybackTime* playbackTime);

	/// <summary>
	/// Synthesizes a full output at the specified stream time. Returns true if there was output this call.
	/// </summary>
	bool GetSample(PlaybackFrame* frame, const PlaybackTime* playbackTime, float gain, float leftRightBalance);

private:

	// Synth Note Pool (maintains a small cache of notes) (these contain envelope, and note processor data)
	SynthVoicePool* _notePool;

	// Post-processing effects	
	SignalChain* _postProcessing;

	unsigned int _numberOfChannels;
	unsigned int _samplingRate;
	unsigned int _octave;

};

#endif