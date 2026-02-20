#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "Constant.h"
#include "Oscillator.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactory.h"
#include "WaveTable.h"
#include <cmath>
#include <cstdlib>
#include <exception>
#include <functional>
#include <numbers>
#include <string>

SignalFactory::SignalFactory(const OutputSettings* outputSettings)
{
	_outputSettings = outputSettings;

	_lowPassFilter = nullptr;
	_combFilter = nullptr;
}

SignalFactory::~SignalFactory()
{
	delete _lowPassFilter;
	delete _combFilter;
}

void SignalFactory::Initialize(const OscillatorParameters& parameters)
{
	if (_lowPassFilter != nullptr)
		delete _lowPassFilter;

	if (_combFilter != nullptr)
		delete _combFilter;

	// Synthesized Plucked String
	float cornerFrequency = parameters.GetFrequency();				// [0, F_s]
	float resonance = 0.01;

	// Create Filters
	_lowPassFilter = new BiQuadFilter(BiQuadFilter::FilterType::LPF, _outputSettings->GetSamplingRate(), cornerFrequency, resonance);
	_combFilter = new CombFilter(0.05f, 1.0f, false);

	// Signal settings don't apply, here, since there are no other parameters to add to
	// our local effects.
	_lowPassFilter->Initialize(nullptr, _outputSettings);
	_combFilter->Initialize(nullptr, _outputSettings);
}

float SignalFactory::GetFrequency(unsigned int midiNote)
{
	return 440.0f * powf(2, ((midiNote - 69.0f) / 12.0f));
}

Oscillator* SignalFactory::Generate(unsigned int midiNote, const OscillatorParameters& parameters)
{
	// RESET STATE-CARRYING FILTERS
	Initialize(parameters);

	WaveTable* waveTable;
	std::string name;

	switch (parameters.GetType())
	{
	case OscillatorType::BuiltIn:
	{
		switch (parameters.GetBuiltInType())
		{
		case BuiltInOscillators::Sine:
			waveTable = SignalFactory::CreateWaveTable(midiNote, parameters,
				std::bind(&SignalFactory::GenerateSineSample, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));
			name = "Sine Wave";
			break;
		case BuiltInOscillators::Square:
			waveTable = SignalFactory::CreateWaveTable(midiNote, parameters,
				std::bind(&SignalFactory::GenerateSquareSample, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));
			name = "Square Wave";
			break;
		case BuiltInOscillators::Triangle:
			waveTable = SignalFactory::CreateWaveTable(midiNote, parameters,
				std::bind(&SignalFactory::GenerateTriangleSample, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));
			name = "Triangle Wave";
			break;
		case BuiltInOscillators::Sawtooth:
			waveTable = SignalFactory::CreateWaveTable(midiNote, parameters,
				std::bind(&SignalFactory::GenerateSawtoothSample, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));
			name = "Sawtooth Wave";
			break;
		case BuiltInOscillators::SynthesizedStringPluck:
			waveTable = SignalFactory::CreateWaveTable(midiNote, parameters,
				std::bind(&SignalFactory::GeneratePluckedStringSample, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));
			name = "String Pluck Wave";
			break;
		default:
			throw new std::exception("Unhandled built in oscillator type");
		}
	}
	break;
	case OscillatorType::SampleBased:
		waveTable = SignalFactory::CreateWaveTable(midiNote, parameters,
			std::bind(&SignalFactory::GenerateSineSample, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				std::placeholders::_4));
		name = "Sampled";
		break;
	default:
		throw new std::exception("Unhandled oscillator type");
	}

	return new Oscillator(name, parameters, waveTable);
}

WaveTable* SignalFactory::CreateWaveTable(unsigned int midiNote, const OscillatorParameters& parameters, GenerateSampleCallback sampleCallback)
{
	int samplingRate = _outputSettings->GetSamplingRate();
	float signalHigh = parameters.GetSignalHigh();
	float signalLow = parameters.GetSignalLow();
	float frequency = parameters.GetFrequency();
	WaveTable* result = new WaveTable(midiNote, frequency, samplingRate);

	// Reset Filters
	_lowPassFilter->Clear();
	_combFilter->Clear();

	// WaveTable manages a buffer of frames for a single period of oscillation
	//
	result->CreateSamples([&sampleCallback, &samplingRate, &frequency, &signalHigh, &signalLow](float sampleTime, float& leftSample, float& rightSample) {

		// Create Sample
		float monoSample = sampleCallback(frequency, signalHigh, signalLow, sampleTime);

		leftSample = monoSample;
		rightSample = monoSample;
	});

	return result;
}

float SignalFactory::GenerateTriangleSample(float frequency, float signalHigh, float signalLow, float sampleTime)
{
	float period = 1 / frequency;
	float periodQuarter = 0.25f * period;
	float high = signalHigh;
	float low = signalLow;
	float sample = 0;

	// Using modulo arithmetic to get the relative period time
	float periodTime = fmodf(sampleTime, period);

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

float SignalFactory::GenerateSquareSample(float frequency, float signalHigh, float signalLow, float sampleTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmodf(sampleTime, period);
	float sample = 0;

	if (periodTime < period / 2.0)
		sample = signalHigh;

	else
		sample = signalLow;

	return sample;
}

float SignalFactory::GenerateSawtoothSample(float frequency, float signalHigh, float signalLow, float sampleTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmodf(sampleTime, period);

	return (((signalHigh - signalLow) / period) * periodTime) + signalLow;
}

float SignalFactory::GenerateSineSample(float frequency, float signalHigh, float signalLow, float sampleTime)
{
	return sinf(2.0 * std::numbers::pi * frequency * sampleTime);
}

float SignalFactory::GeneratePluckedStringSample(float frequency, float signalHigh, float signalLow, float sampleTime)
{
	// https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis
	//
	// There should be a short burst of noise that dissipates. The filtering should be a simple delay plus a LPF realized
	// using our BiQuad + CombFilter.
	//

	// Pluck Time
	float attackTime = 0.001;
	float sample = 0;

	// Noise Attack:  The delay + filter will make the line dissipate
	if (sampleTime < attackTime)
	{
		sample = ((float)rand() / (float)RAND_MAX);
	}
	else
	{
		sample = 0;
	}

	// Process Sample
	PlaybackFrame frame(0, 0);
	_combFilter->SetFrame(&frame, sampleTime);
	_lowPassFilter->SetFrame(&frame, sampleTime);

	// Mix with input
	frame.SetFrame(frame.GetLeft() + sample, frame.GetRight() + sample);

	return frame.GetLeft();
}
