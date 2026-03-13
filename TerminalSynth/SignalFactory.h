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

	/// <summary>
	/// Sets the signal factory for the specified oscillator parameters, and resets all filters
	/// </summary>
	void Reset(const OscillatorParameters* parameters);

	float GenerateTriangleSample(float frequency, float absoluteTime);
	float GenerateSquareSample(float frequency, float absoluteTime);
	float GenerateSawtoothSample(float frequency, float absoluteTime);
	float GenerateSineSample(float frequency, float absoluteTime);
	float GenerateRandomSample(float frequency, float absoluteTime);
	float GeneratePluckedStringSample(float frequency, float signalLow, float signalHigh, float absoluteTime);

private:

	const PlaybackInfo* _outputSettings;
	BiQuadFilter* _lowPassFilter;
	CombFilter* _combFilter;

	SignalFactoryCore* _core;
};

#endif