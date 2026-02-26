#pragma once

#ifndef SYNTH_H
#define SYNTH_H

#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SynthNotePool.h"
#include "SynthSettings.h"

// Class to define static piano notes and store their names / frequencies w.r.t. the SDL keyboard
// defined inputs.
//
class Synth
{

public:
	Synth(const SynthSettings* configuration, unsigned int numberOfChannels, unsigned int samplingRate);
	~Synth();

	// Update Configuration
	void Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters);

	// Update Configuration
	void Update(const SoundRegistry* effectRegistry, const SynthSettings* configuration);

	// Sets midi notes on / off
	void Set(int midiNumber, bool pressed, double absoluteTime, const SynthSettings* configuration);

	/// <summary>
	/// Synthesizes a full output at the specified stream time. Returns true if there was output this call.
	/// </summary>
	bool GetSample(PlaybackFrame* frame, double absoluteTime, const SynthSettings* configuration);

	/// <summary>
	/// Gets rid of outdated cache. This should be done after the user has stopped playback for a short
	/// period of time; but may be checked in real time; and will not process without the proper conditions
	/// from the note pool.
	/// </summary>
	void PruneNotePool();

private:

	// Synth Notes by Midi Number
	SynthNotePool* _pianoNotes;

	// Post-processing effects	
	SignalChain* _postProcessing;

	unsigned int _numberOfChannels;
	unsigned int _samplingRate;
};

#endif