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

	SignalFactoryCore(float samplingRate);
	~SignalFactoryCore();

	float GetFrequency(unsigned int midiNote);

	/// <summary>
	/// Sets the signal factory for the specified oscillator parameters, and resets all filters
	/// </summary>
	void Reset(const OscillatorParameters* parameters);

	float GenerateTriangleSample(float frequency, float absoluteTime);
	float GenerateSquareSample(float frequency, float absoluteTime);
	float GenerateSawtoothSample(float frequency, float absoluteTime);
	float GenerateSineSample(float frequency, float absoluteTime);
	float GenerateRandomSample(float frequency, float absoluteTime);

private:

	float _samplingRate;
	float _signalHigh;
	float _signalLow;

	std::vector<float>* _randomQuadrantValues;
};

#endif