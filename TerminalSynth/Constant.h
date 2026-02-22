#pragma once
#include <cmath>

#ifndef CONSTANT_H
#define CONSTANT_H

enum class OscillatorType : int {
	BuiltIn,
	SampleBased
};
enum class BuiltInOscillators : int {
	Sine = 0,
	Square = 1,
	Triangle = 2,
	Sawtooth = 3,
	SynthesizedStringPluck = 4
};
enum class EnvelopeFilterType : int {
	Constant = 0,
	Oscillator,
	EnvelopeSweep
};

namespace TerminalSynth
{
	float GetMidiFrequency(int midiNumber)
	{
		return 440.0f * powf(2, ((midiNumber - 69.0f) / 12.0f));
	}
}

const int REVERB_COMB_SIZE = 4;
const int REVERB_ALLPASS_SIZE = 4;
const int MIDI_PIANO_LOW_NUMBER = 21;
const int MIDI_PIANO_HIGH_NUMBER = 108;
const int MIDI_PIANO_SIZE = MIDI_PIANO_HIGH_NUMBER - MIDI_PIANO_LOW_NUMBER + 1;
const float SIGNAL_LOW = -1;
const float SIGNAL_HIGH = 1;

#endif