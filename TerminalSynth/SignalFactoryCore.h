#pragma once

#ifndef SIGNAL_FACTORY_CORE_H
#define SIGNAL_FACTORY_CORE_H

#include "OscillatorParameters.h"
#include <vector>

/// <summary>
/// Core of SignalFactory* that does not depend on SignalBase*. There can be no filters or complex
/// SignalBase* effects because there will be a circular dependency.
/// </summary>
class SignalFactoryCore
{
public:

	const int RANDOM_OSCILLATOR_LENGTH = 8;

public:

	SignalFactoryCore(float samplingRate);
	~SignalFactoryCore();

	float GetFrequency(unsigned int midiNote);

	/// <summary>
	/// Sets the signal factory for the specified oscillator parameters, and resets all filters
	/// </summary>
	void Reset(const OscillatorParameters* parameters);

	float GenerateTriangleSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSquareSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSawtoothSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSineSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateRandomSample(float frequency, size_t timeCursor, double streamTime);

private:

	float _samplingRate;
	float _signalHigh;
	float _signalLow;

	std::vector<float>* _randomQuadrantValues;
};

#endif