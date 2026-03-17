#pragma once

#ifndef SIGNAL_FACTORY_H
#define SIGNAL_FACTORY_H

#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "OscillatorParameters.h"
#include "PlaybackInfo.h"
#include "SignalFactoryCore.h"
#include "WaveTable.h"

class SignalFactory
{
public:

	SignalFactory(const PlaybackInfo* outputSettings);
	~SignalFactory();

	/// <summary>
	/// (MEMORY!) Creates new wavetable with the provided parameters! Wave sample rate may be over / under sampled.
	/// </summary>
	WaveTable* GenerateWaveTable(const OscillatorParameters& parameters, int midiNumber, float waveSamplingRate);

private:

	WaveTable* GenerateOscillatorWaveTable(const OscillatorParameters& parameters, int midiNumber, float waveSamplingRate);
	WaveTable* GenerateHarmonicWaveTable(const OscillatorParameters& parameters, int midiNumber, float waveSamplingRate);

private:

	/// <summary>
	/// Sets the signal factory for the specified oscillator parameters, and resets all filters
	/// </summary>
	void Reset(const OscillatorParameters* parameters);

	float GenerateTriangleSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSquareSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSawtoothSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSineSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateRandomSample(float frequency, size_t timeCursor, double streamTime);
	float GeneratePluckedStringSample(float frequency, float signalLow, float signalHigh, size_t timeCursor, double streamTime);

private:

	const PlaybackInfo* _outputSettings;
	BiQuadFilter* _lowPassFilter;
	CombFilter* _combFilter;

	SignalFactoryCore* _core;
};

#endif