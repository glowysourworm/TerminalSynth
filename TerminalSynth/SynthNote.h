#pragma once

#ifndef SYNTH_NOTE_H
#define SYNTH_NOTE_H

#include "Oscillator.h"
#include "PlaybackFrame.h"

class SynthNote
{
public:

	/// <summary>
	/// Creates a synth note from the provided oscillator. The oscillator is cached by other components, and should
	/// NOT BE DELETED!
	/// </summary>
	/// <param name="oscillator">Oscillator* to a sound source</param>
	/// <param name="midiNumber">Midi number for this note</param>
	/// <param name="samplingRate">Sampling rate for this note</param>
	SynthNote(Oscillator* oscillator, unsigned int midiNumber, unsigned int samplingRate);
	~SynthNote();

	unsigned int GetMidiNumber() const;
	void GetSample(PlaybackFrame* frame, float absoluteTime);
	void AddSample(PlaybackFrame* frame, float absoluteTime);


	bool HasOutput(float absoluteTime);
	void Engage(float absoluteTime);
	void DisEngage(float absoluteTime);

private:

	Oscillator* _oscillator;

	unsigned int _midiNumber;
};

#endif