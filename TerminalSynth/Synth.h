#pragma once

#ifndef SYNTH_H
#define SYNTH_H

#include "EffectRegistry.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "SynthNoteQueue.h"
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
	void Initialize(const EffectRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters);

	// Update Configuration
	void Update(const EffectRegistry* effectRegistry, const SynthSettings* configuration);

	// Sets midi notes on / off
	void Set(int midiNumber, bool pressed, double absoluteTime, const SynthSettings* configuration);

	/// <summary>
	/// Synthesizes a full output at the specified stream time. Returns true if there was output this call.
	/// </summary>
	bool GetSample(PlaybackFrame* frame, double absoluteTime, const SynthSettings* configuration);

private:

	// Synth Notes by Midi Number
	SynthNoteQueue* _pianoNotes;

	// Post-processing effects	
	SignalChain* _postProcessing;

	unsigned int _numberOfChannels;
	unsigned int _samplingRate;
};

#endif