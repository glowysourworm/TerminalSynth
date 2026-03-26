#pragma once

#ifndef SIGNAL_FACTORY_CORE_H
#define SIGNAL_FACTORY_CORE_H

#include "PlaybackTime.h"

/// <summary>
/// Core of SignalFactory* that does not depend on SignalBase*. There can be no filters or complex
/// SignalBase* effects because there will be a circular dependency.
/// </summary>
class SignalFactoryCore
{
public:

	static float GenerateTriangleSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime);
	static float GenerateSquareSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime);
	static float GenerateSawtoothSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime);
	static float GenerateSineSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime);
};

#endif