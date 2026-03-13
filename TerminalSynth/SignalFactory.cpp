#include "Algorithm.h"
#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "Constant.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalFactory.h"
#include "SignalFactoryCore.h"
#include "WaveTable.h"
#include <cmath>
#include <complex>
#include <cstdlib>
#include <exception>
#include <vector>

SignalFactory::SignalFactory(const PlaybackInfo* outputSettings)
{
	_outputSettings = outputSettings;

	_lowPassFilter = nullptr;
	_combFilter = nullptr;

	_core = new SignalFactoryCore(outputSettings->GetStreamInfo()->streamSampleRate);
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
	_lowPassFilter = new BiQuadFilter(BiQuadFilter::FilterType::LPF, _outputSettings->GetStreamInfo()->streamSampleRate, cornerFrequency, resonance);
	_combFilter = new CombFilter(0.05f, 1.0f, false);

	// Signal settings don't apply, here, since there are no other parameters to add to
	// our local effects.
	_lowPassFilter->Initialize(_outputSettings);
	_combFilter->Initialize(_outputSettings);
}

//void SignalFactory::GenerateSample(const OscillatorParameters* parameters, PlaybackFrame* frame, float absoluteTime)
//{
//	int samplingRate = _outputSettings->GetStreamInfo()->streamSampleRate;
//	float signalHigh = parameters->GetSignalHigh();
//	float signalLow = parameters->GetSignalLow();
//	float frequency = parameters->GetFrequency();
//	float monoSample = 0;
//
//	// Create Sample
//	switch (parameters->GetType())
//	{
//	case OscillatorType::BuiltIn:
//	{
//		switch (parameters->GetBuiltInType())
//		{
//		case BuiltInOscillators::Sine:
//			monoSample = this->GenerateSineSample(parameters->GetFrequency(), absoluteTime);
//			break;
//		case BuiltInOscillators::Square:
//			monoSample = this->GenerateSquareSample(parameters->GetFrequency(), absoluteTime);
//			break;
//		case BuiltInOscillators::Triangle:
//			monoSample = this->GenerateTriangleSample(parameters->GetFrequency(), absoluteTime);
//			break;
//		case BuiltInOscillators::Sawtooth:
//			monoSample = this->GenerateSawtoothSample(parameters->GetFrequency(), absoluteTime);
//			break;
//		case BuiltInOscillators::SynthesizedStringPluck:
//			monoSample = this->GeneratePluckedStringSample(parameters->GetFrequency(), parameters->GetSignalHigh(), parameters->GetSignalLow(), absoluteTime);
//			break;
//		case BuiltInOscillators::Random:
//			monoSample = this->GenerateRandomSample(parameters->GetFrequency(), absoluteTime);
//			break;
//		default:
//			throw new std::exception("Unhandled built in oscillator type");
//		}
//	}
//	break;
//	case OscillatorType::SampleBased:
//		monoSample = this->GenerateSineSample(parameters->GetFrequency(), absoluteTime);
//		break;
//	default:
//		throw new std::exception("Unhandled oscillator type");
//	}
//
//	frame->SetFrame(monoSample, monoSample, frame->GetEnvelopeLevel());
//}

WaveTable* SignalFactory::GenerateWaveTable(const OscillatorParameters& parameters, int midiNumber, float waveSamplingRate)
{
	this->Reset(&parameters);

	float frequency = TerminalSynth::GetMidiFrequency(midiNumber + (parameters.GetOctave() * 12));
	float period = 1 / frequency;

	// Oversampled Frame Length
	//int sampleLength = (period * oversampleFactor) * _outputSettings->GetStreamInfo()->streamSampleRate;
	//float sampleRate = _outputSettings->GetStreamInfo()->streamSampleRate * oversampleFactor;

	int sampleLength = pow(2, 16);				// Need a number greater than the sampling rate
	float sampleRate = sampleLength / period;

	WaveTable* result = new WaveTable(WaveTable::Mode::Periodic, sampleLength, sampleRate, _outputSettings->GetStreamInfo()->streamSampleRate);

	// IFFT Wave Generation:  Using harmonic banded frequency.. something or other
	//
	// https://zynaddsubfx.sourceforge.io/doc/PADsynth/PADsynth.htm#Sound_examples
	//

	std::vector<std::complex<double>> waveArray(sampleLength);
	int numberHarmonics = 10;
	
	// IFFT:  In frequency space, where is our fundamental frequency?
	// 
	//		  N/2 -> ((sample rate) / 2)
	//		  
	//		  22050 Hz would be half the nyquist frequency for human hearing
	//		  which is close to our sampling rate, typically. 
	// 
	//		  I think the sampling rate IS the frequency, actually, with the
	//		  Nyquist theory giving a multiple of 2 to put it near enough to
	//		  the human hearing threshold to give a signal that will be close
	//		  to ideal.
	//
	//		  Oversampled:  Our sample index must be scaled to keep the correct
	//						frequency in the wave array
	//

	// Harmonics
	for (int n = 1; n <= numberHarmonics; n++)
	{
		// Harmonic Band: Fundamental * (Sample Length / System Sample Length), which takes the 
		//				  oversampled domain into account.
		//
		float sideBandwidth = TerminalSynth::CentsToHertz(5.0f, frequency * n) - (frequency * n);

		// Multiply by our sample factor
		int centerIndex = (int)((frequency * n) / (sampleLength / _outputSettings->GetStreamInfo()->streamSampleRate));
		int sideBandwidthIndex = sideBandwidth * (sampleLength / _outputSettings->GetStreamInfo()->streamSampleRate);

		if (centerIndex + sideBandwidthIndex >= waveArray.size())
			continue;

		for (int index = centerIndex - sideBandwidthIndex; index <= centerIndex + sideBandwidthIndex; index++)
		{
			// Use Gaussian Window to control the side band amplitude
			float gaussWindow = Algorithm::Gaussian3Sigma(index, centerIndex - sideBandwidthIndex, centerIndex + sideBandwidthIndex);

			// Produce a harmonic with random phase
			waveArray[index] += std::complex<double>(gaussWindow / std::sqrtf(n), rand() / (double)RAND_MAX);
		}
	}

	// IFFT
	Algorithm::IFFT(&waveArray);

	SignalFactory* that = this;

	float maxSample = 0;

	for (int index = 0; index < waveArray.size(); index++)
	{
		if (waveArray[index].real() > maxSample)
			maxSample = waveArray[index].real();
	}

	// Load WaveTable*
	result->CreateSamplesByFrame([&waveArray, &that, &frequency, &maxSample](int frameIndex, float& leftSample, float& rightSample) {

		leftSample = waveArray[frameIndex].real() / maxSample;
		rightSample = waveArray[frameIndex].real() / maxSample;

		//leftSample = waveArray[frameIndex].real() / maxSample;
		//rightSample = waveArray[frameIndex].real() / maxSample;

		//leftSample = that->GenerateSineSample(frequency, sampleTime);
		//rightSample = that->GenerateSineSample(frequency, sampleTime);
	});

	return result;
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
