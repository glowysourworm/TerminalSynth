#pragma once

#ifndef SIGNAL_FACTORY_H
#define SIGNAL_FACTORY_H

#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "Oscillator.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "WaveTable.h"
#include <functional>

class SignalFactory
{
public:

	SignalFactory(const OutputSettings* outputSettings);
	~SignalFactory();

	float GetFrequency(unsigned int midiNote);
	Oscillator* Generate(unsigned int midiNote, const OscillatorParameters& parameters);

private:

	using GenerateSampleCallback = std::function<float(float frequency, float signalHigh, float signalLow, float absoluteTime)>;

	WaveTable* CreateWaveTable(unsigned int midiNote, const OscillatorParameters& parameters, GenerateSampleCallback sampleCallback);

private:

	void Initialize(const OscillatorParameters& parameters);

private:

	float GenerateTriangleSample(float frequency, float signalHigh, float signalLow, float absoluteTime);
	float GenerateSquareSample(float frequency, float signalHigh, float signalLow, float absoluteTime);
	float GenerateSawtoothSample(float frequency, float signalHigh, float signalLow, float absoluteTime);
	float GenerateSineSample(float frequency, float signalHigh, float signalLow, float absoluteTime);
	float GeneratePluckedStringSample(float frequency, float signalHigh, float signalLow, float absoluteTime);

private:

	const OutputSettings* _outputSettings;
	BiQuadFilter* _lowPassFilter;
	CombFilter* _combFilter;
};

#endif