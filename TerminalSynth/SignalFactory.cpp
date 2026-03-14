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
#include <limits>
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

	// Sample Playback
	int sampleLength = period * _outputSettings->GetStreamInfo()->streamSampleRate;
	float sampleRate = _outputSettings->GetStreamInfo()->streamSampleRate;

	WaveTable* result = new WaveTable(WaveTable::Mode::Periodic, 
		sampleLength, 
		_outputSettings->GetStreamInfo()->streamSampleRate, 
		_outputSettings->GetStreamInfo()->streamSampleRate);

	SignalFactory* that = this;

	result->CreateSamplesByFrame([&parameters, &that, &sampleRate](int frameIndex, float& leftSample, float& rightSample) {

		float sampleTime = frameIndex / sampleRate;
		float monoSample = 0;

		// Create Sample
		switch (parameters.GetType())
		{
		case OscillatorType::BuiltIn:
		{
			switch (parameters.GetBuiltInType())
			{
			case BuiltInOscillators::Sine:
				monoSample = that->GenerateSineSample(parameters.GetFrequency(), frameIndex, sampleTime);
				break;
			case BuiltInOscillators::Square:
				monoSample = that->GenerateSquareSample(parameters.GetFrequency(), frameIndex, sampleTime);
				break;
			case BuiltInOscillators::Triangle:
				monoSample = that->GenerateTriangleSample(parameters.GetFrequency(), frameIndex, sampleTime);
				break;
			case BuiltInOscillators::Sawtooth:
				monoSample = that->GenerateSawtoothSample(parameters.GetFrequency(), frameIndex, sampleTime);
				break;
			case BuiltInOscillators::SynthesizedStringPluck:
				monoSample = that->GeneratePluckedStringSample(parameters.GetFrequency(), parameters.GetSignalHigh(), parameters.GetSignalLow(), frameIndex, sampleTime);
				break;
			case BuiltInOscillators::Random:
				monoSample = that->GenerateRandomSample(parameters.GetFrequency(), frameIndex, sampleTime);
				break;
			default:
				throw new std::exception("Unhandled built in oscillator type");
			}
		}
		break;
		case OscillatorType::SampleBased:
			monoSample = that->GenerateSineSample(parameters.GetFrequency(), frameIndex, sampleTime);
			break;
		default:
			throw new std::exception("Unhandled oscillator type");
		}

		leftSample = monoSample;
		rightSample = monoSample;
	});

	return result;

	//float frequency = TerminalSynth::GetMidiFrequency(midiNumber + (parameters.GetOctave() * 12));
	//float period = 1 / frequency;

	//// Parameters
	//int numberHarmonics = 1;
	//float sidebandCents = 0.0f;
	//bool randomPhase = false;
	//float randomPhaseAmplitude = 0.1f;

	//// Sample Preparation 
	//int oversampleLength = pow(2, 16);						// Need a number greater than the sampling rate

	//// Sample Playback
	//int sampleLength = period * _outputSettings->GetStreamInfo()->streamSampleRate;

	//WaveTable* result = new WaveTable(WaveTable::Mode::Periodic, 
	//	sampleLength, 
	//	_outputSettings->GetStreamInfo()->streamSampleRate, 
	//	_outputSettings->GetStreamInfo()->streamSampleRate);

	//// IFFT Wave Generation:  Using harmonic banded frequency.. something or other
	////
	//// https://zynaddsubfx.sourceforge.io/doc/PADsynth/PADsynth.htm#Sound_examples
	////

	//std::vector<std::complex<double>> waveArray(oversampleLength);
	//
	//// IFFT:  In frequency space, where is our fundamental frequency?
	//// 
	////		  N/2 -> ((sample rate) / 2)
	////		  
	////		  22050 Hz would be half the nyquist frequency for human hearing
	////		  which is close to our sampling rate, typically. 
	//// 
	////		  I think the sampling rate IS the frequency, actually, with the
	////		  Nyquist theory giving a multiple of 2 to put it near enough to
	////		  the human hearing threshold to give a signal that will be close
	////		  to ideal.
	////
	////		  Oversampled:  Our sample index must be scaled to keep the correct
	////						frequency in the wave array
	////

	//// Harmonics
	//for (int n = 1; n <= numberHarmonics; n++)
	//{
	//	// Harmonic Band: Fundamental * (Sample Length / System Sample Length), which takes the 
	//	//				  oversampled domain into account.
	//	//
	//	float sideBandwidth = TerminalSynth::CentsToHertz(sidebandCents, frequency * n) - (frequency * n);

	//	// Multiply by our sample factor
	//	int centerIndex = (int)((frequency * n) * (oversampleLength / _outputSettings->GetStreamInfo()->streamSampleRate));
	//	int sideBandwidthIndex = (int)(sideBandwidth * (oversampleLength / _outputSettings->GetStreamInfo()->streamSampleRate));

	//	if (centerIndex + sideBandwidthIndex >= waveArray.size())
	//		continue;

	//	for (int index = centerIndex - sideBandwidthIndex; index <= centerIndex + sideBandwidthIndex; index++)
	//	{
	//		// Use Gaussian Window to control the side band amplitude
	//		float gaussWindow = Algorithm::Gaussian3Sigma(index, centerIndex - sideBandwidthIndex, centerIndex + sideBandwidthIndex);

	//		// Produce a harmonic with random phase
	//		waveArray[index] += std::complex<double>(gaussWindow / std::sqrtf(n), randomPhase ? (rand() / (double)RAND_MAX) * randomPhaseAmplitude : 0);
	//	}
	//}

	//double maxSample = std::numeric_limits<double>::min();

	//// IFFT
	//Algorithm::IFFT(&waveArray, maxSample);

	//SignalFactory* that = this;

	//// Load WaveTable*
	//result->CreateSamplesByFrame([&waveArray, &maxSample, &sampleLength](int frameIndex, float& leftSample, float& rightSample) {

	//	// The oversampled time-domain wave array actually has the right "sample rate" to be put
	//	// directly into one period of the output; but with all the harmonic variety added using
	//	// the IFFT
	//	//
	//	leftSample = waveArray[frameIndex].real() / maxSample;
	//	rightSample = waveArray[frameIndex].real() / maxSample;
	//});

	return result;
}

float SignalFactory::GenerateTriangleSample(float frequency, size_t timeCursor, double streamTime)
{
	return _core->GenerateTriangleSample(frequency, timeCursor, streamTime);
}

float SignalFactory::GenerateSquareSample(float frequency, size_t timeCursor, double streamTime)
{
	return _core->GenerateSquareSample(frequency, timeCursor, streamTime);
}

float SignalFactory::GenerateSawtoothSample(float frequency, size_t timeCursor, double streamTime)
{
	return _core->GenerateSawtoothSample(frequency, timeCursor, streamTime);
}

float SignalFactory::GenerateSineSample(float frequency, size_t timeCursor, double streamTime)
{
	return _core->GenerateSineSample(frequency, timeCursor, streamTime);
}

float SignalFactory::GenerateRandomSample(float frequency, size_t timeCursor, double streamTime)
{
	return _core->GenerateRandomSample(frequency, timeCursor, streamTime);
}

float SignalFactory::GeneratePluckedStringSample(float frequency, float signalHigh, float signalLow, size_t timeCursor, double streamTime)
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
	if (streamTime < attackTime)
	{
		sample = ((float)rand() / (float)RAND_MAX);
	}
	else
	{
		sample = 0;
	}

	// Process Sample
	PlaybackFrame frame(0, 0);
	_combFilter->SetFrame(&frame);
	_lowPassFilter->SetFrame(&frame);

	// Mix with input
	frame.SetFrame(frame.GetLeft() + sample, frame.GetRight() + sample);

	return frame.GetLeft();
}
