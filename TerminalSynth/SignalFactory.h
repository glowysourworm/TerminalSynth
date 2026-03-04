#pragma once

#ifndef SIGNAL_FACTORY_H
#define SIGNAL_FACTORY_H

#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactoryCore.h"
#include "SignalSettings.h"

class SignalFactory
{
public:

	SignalFactory(const OutputSettings* outputSettings);
	~SignalFactory();

	float GetFrequency(unsigned int midiNote);

	/// <summary>
	/// Sets the signal factory for the specified oscillator parameters, and resets all filters
	/// </summary>
	void Reset(const OscillatorParameters* parameters);

	/// <summary>
	/// Generates a sample for the specified stream time
	/// </summary>
	void GenerateSample(const OscillatorParameters* parameters, PlaybackFrame* frame, float absoluteTime);

private:

	float GenerateTriangleSample(float frequency, float absoluteTime);
	float GenerateSquareSample(float frequency, float absoluteTime);
	float GenerateSawtoothSample(float frequency, float absoluteTime);
	float GenerateSineSample(float frequency, float absoluteTime);
	float GenerateRandomSample(float frequency, float absoluteTime);
	float GeneratePluckedStringSample(float frequency, float signalLow, float signalHigh, float absoluteTime);

private:

	const OutputSettings* _outputSettings;
	BiQuadFilter* _lowPassFilter;
	CombFilter* _combFilter;

	SignalFactoryCore* _core;
};

#endif