#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "Constant.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactory.h"
#include "SignalFactoryCore.h"
#include <cstdlib>
#include <exception>

SignalFactory::SignalFactory(const OutputSettings* outputSettings)
{
	_outputSettings = outputSettings;

	_lowPassFilter = nullptr;
	_combFilter = nullptr;

	_core = new SignalFactoryCore(outputSettings->GetSamplingRate());
}

SignalFactory::~SignalFactory()
{
	delete _lowPassFilter;
	delete _combFilter;
	delete _core;
}

void SignalFactory::Reset(const OscillatorParameters* parameters)
{
	if (_lowPassFilter != nullptr)
		delete _lowPassFilter;

	if (_combFilter != nullptr)
		delete _combFilter;

	_core->Reset(parameters);

	// Synthesized Plucked String
	float cornerFrequency = parameters->GetFrequency();				// [0, F_s]
	float resonance = 0.01;

	// Create Filters
	_lowPassFilter = new BiQuadFilter(BiQuadFilter::FilterType::LPF, _outputSettings->GetSamplingRate(), cornerFrequency, resonance);
	_combFilter = new CombFilter(0.05f, 1.0f, false);

	// Signal settings don't apply, here, since there are no other parameters to add to
	// our local effects.
	_lowPassFilter->Initialize(_outputSettings);
	_combFilter->Initialize(_outputSettings);
}

float SignalFactory::GetFrequency(unsigned int midiNote)
{
	return TerminalSynth::GetMidiFrequency(midiNote);		// We need a namespace!
}

void SignalFactory::GenerateSample(const OscillatorParameters* parameters, PlaybackFrame* frame, float absoluteTime)
{
	int samplingRate = _outputSettings->GetSamplingRate();
	float signalHigh = parameters->GetSignalHigh();
	float signalLow = parameters->GetSignalLow();
	float frequency = parameters->GetFrequency();
	float monoSample = 0;

	// Create Sample
	switch (parameters->GetType())
	{
	case OscillatorType::BuiltIn:
	{
		switch (parameters->GetBuiltInType())
		{
		case BuiltInOscillators::Sine:
			monoSample = this->GenerateSineSample(parameters->GetFrequency(), absoluteTime);
			break;
		case BuiltInOscillators::Square:
			monoSample = this->GenerateSquareSample(parameters->GetFrequency(), absoluteTime);
			break;
		case BuiltInOscillators::Triangle:
			monoSample = this->GenerateTriangleSample(parameters->GetFrequency(), absoluteTime);
			break;
		case BuiltInOscillators::Sawtooth:
			monoSample = this->GenerateSawtoothSample(parameters->GetFrequency(), absoluteTime);
			break;
		case BuiltInOscillators::SynthesizedStringPluck:
			monoSample = this->GeneratePluckedStringSample(parameters->GetFrequency(), parameters->GetSignalHigh(), parameters->GetSignalLow(), absoluteTime);
			break;
		case BuiltInOscillators::Random:
			monoSample = this->GenerateRandomSample(parameters->GetFrequency(), absoluteTime);
			break;
		default:
			throw new std::exception("Unhandled built in oscillator type");
		}
	}
	break;
	case OscillatorType::SampleBased:
		monoSample = this->GenerateSineSample(parameters->GetFrequency(), absoluteTime);
		break;
	default:
		throw new std::exception("Unhandled oscillator type");
	}

	frame->SetFrame(monoSample, monoSample, frame->GetEnvelopeLevel());
}

float SignalFactory::GenerateTriangleSample(float frequency, float sampleTime)
{
	return _core->GenerateTriangleSample(frequency, sampleTime);
}

float SignalFactory::GenerateSquareSample(float frequency, float sampleTime)
{
	return _core->GenerateSquareSample(frequency, sampleTime);
}

float SignalFactory::GenerateSawtoothSample(float frequency, float sampleTime)
{
	return _core->GenerateSawtoothSample(frequency, sampleTime);
}

float SignalFactory::GenerateSineSample(float frequency, float sampleTime)
{
	return _core->GenerateSineSample(frequency, sampleTime);
}

float SignalFactory::GenerateRandomSample(float frequency, float absoluteTime)
{
	return _core->GenerateRandomSample(frequency, absoluteTime);
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
	PlaybackFrame frame(0, 0, 1);
	_combFilter->SetFrame(&frame, sampleTime);
	_lowPassFilter->SetFrame(&frame, sampleTime);

	// Mix with input
	frame.SetFrame(frame.GetLeft() + sample, frame.GetRight() + sample, 1);

	return frame.GetLeft();
}
