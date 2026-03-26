#include "PlaybackTime.h"
#include "SignalFactoryCore.h"
#include <algorithm>
#include <cmath>
#include <numbers>

float SignalFactoryCore::GenerateTriangleSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime)
{
	float period = 1 / frequency;
	float periodQuarter = 0.25f * period;
	float high = signalHigh;
	float low = signalLow;
	float sample = 0;

	// Using modulo arithmetic to get the relative period time
	float periodTime = fmod(playbackTime->FromCursor(samplingRate), period);

	// First Quadrant
	if (periodTime < periodQuarter)
	{
		sample = (2.0 * (high - low) / period) * periodTime;
	}

	// Second Quadrant
	else if (periodTime < (2.0 * periodQuarter))
	{
		sample = ((-2.0 * (high - low) / period) * (periodTime - (float)periodQuarter)) - low;
	}

	// Third Quadrant
	else if (periodTime < 3.0 * periodQuarter)
	{
		sample = (-2.0 * (high - low) / period) * (periodTime - (2.0 * periodQuarter));
	}

	// Fourth Quadrant
	else
	{
		sample = ((2.0 * (high - low) / period) * (periodTime - (3.0 * periodQuarter))) + low;
	}

	return sample;
}

float SignalFactoryCore::GenerateSquareSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmod(playbackTime->FromCursor(samplingRate), period);
	float sample = 0;

	if (periodTime < period / 2.0)
		sample = signalHigh;

	else
		sample = signalLow;

	return sample;
}

float SignalFactoryCore::GenerateSawtoothSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmod(playbackTime->FromCursor(samplingRate), period);

	return (((signalHigh - signalLow) / period) * periodTime) + signalLow;
}

float SignalFactoryCore::GenerateSineSample(float frequency, float samplingRate, float signalHigh, float signalLow, const PlaybackTime* playbackTime)
{
	return (0.5f * (signalHigh - signalLow) * sinf(2.0 * std::numbers::pi * frequency * playbackTime->FromCursor(samplingRate))) + (0.5f * (signalHigh + signalLow));
}