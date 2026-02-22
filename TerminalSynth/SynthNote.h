#pragma once

#ifndef SYNTH_NOTE_H
#define SYNTH_NOTE_H

#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "WaveTable.h"

class SynthNote
{
public:

	/// <summary>
	/// Creates a synth note from the provided oscillator. The oscillator is cached by other components, and should
	/// NOT BE DELETED!
	/// </summary>
	/// <param name="parameters">Parameters for the oscillator</param>
	/// <param name="waveTable">WaveTable* sound source. THIS SHOULD NOT BE DELETED! (~WaveTableCache)</param>
	/// <param name="midiNumber">Midi number for this note</param>
	/// <param name="samplingRate">Sampling rate for this note</param>
	SynthNote(const OscillatorParameters& parameters, WaveTable* waveTable, unsigned int midiNumber);
	~SynthNote();

	unsigned int GetMidiNumber() const;
	void GetSample(PlaybackFrame* frame, float absoluteTime);
	void AddSample(PlaybackFrame* frame, float absoluteTime);


	bool HasOutput(float absoluteTime);
	void Engage(float absoluteTime);
	void DisEngage(float absoluteTime);

private:

	OscillatorParameters* _parameters;
	WaveTable* _waveTable;

	unsigned int _midiNumber;
};

#endif