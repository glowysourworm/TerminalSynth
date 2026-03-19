#pragma once
#include <cmath>

#ifndef CONSTANT_H
#define CONSTANT_H

enum class AudioStreamFormat
{
	Float32,
	Int32,
	Int16,
	Int8
};

enum class OscillatorType : int {
	BuiltIn,
	SampleBased,
	HarmonicShaper
};
enum class EnvelopeShape : int {
	Linear = 0,
	Gaussian = 1,
	Gamma = 2
};
enum class BuiltInOscillators : int {
	Sine = 0,
	Square = 1,
	Triangle = 2,
	Sawtooth = 3,
	SynthesizedStringPluck = 4,
	StkVoice = 5
};
enum class ParameterAutomationType : int {
	EnvelopeSweep = 0,
	Oscillator
};
enum class ParameterAutomationOscillator : int {
	Sine = 0,
	Square = 1,
	Triangle = 2,
	Sawtooth = 3,
	Random = 4
};

namespace TerminalSynth
{
	float GetMidiFrequency(int midiNumber)
	{
		return 440.0f * powf(2, ((midiNumber - 69.0f) / 12.0f));
	}

	/// <summary>
	/// Converts hertz to cents (centered at the center frequency). 100 cents is a half-tone.
	/// </summary>
	float HertzToCents(float centerFrequency, float frequency)
	{
		return 1200.0f * log2f(frequency / centerFrequency);
	}

	/// <summary>
	/// Converts cents to hertz with reference to the center frequency
	/// </summary>
	float CentsToHertz(float cents, float centerFrequency)
	{
		return centerFrequency * powf(2.0f, cents / 1200.0f);
	}
}

const int REVERB_COMB_SIZE = 4;
const int REVERB_ALLPASS_SIZE = 4;
const int MIDI_PIANO_LOW_NUMBER = 21;
const int MIDI_PIANO_HIGH_NUMBER = 108;
const int MIDI_PIANO_SIZE = MIDI_PIANO_HIGH_NUMBER - MIDI_PIANO_LOW_NUMBER + 1;
const float SIGNAL_LOW = -1;
const float SIGNAL_HIGH = 1;
const float ENVELOPE_LOW = 0;
const float ENVELOPE_HIGH = 1;

#endif