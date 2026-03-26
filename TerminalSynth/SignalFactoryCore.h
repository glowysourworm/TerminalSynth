#pragma once

#ifndef SIGNAL_FACTORY_CORE_H
#define SIGNAL_FACTORY_CORE_H

#include "PlaybackTime.h"
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

	float GenerateTriangleSample(float frequency, const PlaybackTime* playbackTime);
	float GenerateSquareSample(float frequency, const PlaybackTime* playbackTime);
	float GenerateSawtoothSample(float frequency, const PlaybackTime* playbackTime);
	float GenerateSineSample(float frequency, const PlaybackTime* playbackTime);
	float GenerateRandomSample(float frequency, const PlaybackTime* playbackTime);

private:

	float _samplingRate;
	float _signalHigh;
	float _signalLow;

	std::vector<float>* _randomQuadrantValues;
};

#endif