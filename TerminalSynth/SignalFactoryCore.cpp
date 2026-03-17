#include "Constant.h"
#include "OscillatorParameters.h"
#include "SignalFactoryCore.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <vector>

SignalFactoryCore::SignalFactoryCore(float samplingRate)
{
	_randomQuadrantValues = new std::vector<float>();
	_samplingRate = samplingRate;
	_signalHigh = SIGNAL_HIGH;
	_signalLow = SIGNAL_LOW;
}

SignalFactoryCore::~SignalFactoryCore()
{
	delete _randomQuadrantValues;
}

void SignalFactoryCore::Reset(const OscillatorParameters* parameters)
{
	_signalHigh = parameters->GetSignalHigh();
	_signalLow = parameters->GetSignalLow();
	_randomQuadrantValues->clear();

	for (int index = 0; index < RANDOM_OSCILLATOR_LENGTH; index++)
	{
		_randomQuadrantValues->push_back(((_signalHigh - _signalLow) * (rand() / (float)RAND_MAX)) + _signalLow);
	}
}

float SignalFactoryCore::GetFrequency(unsigned int midiNote)
{
	return TerminalSynth::GetMidiFrequency(midiNote);		// We need a namespace!
}

float SignalFactoryCore::GenerateTriangleSample(float frequency, size_t timeCursor, double streamTime)
{
	float period = 1 / frequency;
	float periodQuarter = 0.25f * period;
	float high = _signalHigh;
	float low = _signalLow;
	float sample = 0;

	// Using modulo arithmetic to get the relative period time
	float periodTime = fmod(streamTime, period);

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

float SignalFactoryCore::GenerateSquareSample(float frequency, size_t timeCursor, double streamTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmod(streamTime, period);
	float sample = 0;

	if (periodTime < period / 2.0)
		sample = _signalHigh;

	else
		sample = _signalLow;

	return sample;
}

float SignalFactoryCore::GenerateSawtoothSample(float frequency, size_t timeCursor, double streamTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmod(streamTime, period);

	return (((_signalHigh - _signalLow) / period) * periodTime) + _signalLow;
}

float SignalFactoryCore::GenerateSineSample(float frequency, size_t timeCursor, double streamTime)
{
	return (0.5f * (_signalHigh - _signalLow) * sinf(2.0 * std::numbers::pi * frequency * streamTime)) + (0.5f * (_signalHigh + _signalLow));
}

float SignalFactoryCore::GenerateRandomSample(float frequency, size_t timeCursor, double streamTime)
{
	float period = 1 / frequency;

	// Using modulo arithmetic to get the relative period time
	float periodTime = fmod(streamTime, period);

	float periodDiv = (period / _randomQuadrantValues->size());
	float periodBucket = periodTime / periodDiv;

	float periodBucketIndex = 0;
	float remainder = std::modf(periodBucket, &periodBucketIndex);
	int periodIndex = std::min<int>(periodBucketIndex, _randomQuadrantValues->size() - 1);
	periodIndex = std::max<int>(periodIndex, 0);

	return _randomQuadrantValues->at(periodIndex);
}
