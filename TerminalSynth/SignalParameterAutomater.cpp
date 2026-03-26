#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalFactoryCore.h"
#include "SignalParameter.h"
#include "SignalParameterAutomater.h"
#include <exception>

SignalParameterAutomater::SignalParameterAutomater()
{
	_frame = new PlaybackFrame(0, 0);
	_signalFactory = nullptr;
	_signalFactoryRandom = nullptr;

	_oscillatorParameters = new OscillatorParameters(
		SynthVoiceType::Primitive, 
		PrimitiveSynthVoices::Sine, 
		TerminalSynthVoices::SynthesizedStringPluck, 
		StkSynthVoices::StkBeeThree, 
		"", "", ENVELOPE_LOW, ENVELOPE_HIGH);

	_oscillatorParametersRandom = new OscillatorParameters(
		SynthVoiceType::Primitive,
		PrimitiveSynthVoices::Sine,
		TerminalSynthVoices::SynthesizedStringPluck,
		StkSynthVoices::StkBeeThree,
		"", "", ENVELOPE_LOW, ENVELOPE_HIGH);

	_oscillatorFrequency = 0.5;
	_oscillatorRandomFrequency = 0.5;

	_type = ParameterAutomationType::EnvelopeSweep;
	_oscillatorType = ParameterAutomationOscillator::Sine;
	_envelope = new Envelope();
}

SignalParameterAutomater::~SignalParameterAutomater()
{
	if (_signalFactory != nullptr)
		delete _signalFactory;

	if (_signalFactoryRandom != nullptr)
		delete _signalFactoryRandom;

	delete _frame;
	delete _oscillatorParameters;
	delete _oscillatorParametersRandom;
	delete _envelope;
}

void SignalParameterAutomater::Initialize(const PlaybackInfo* parameters)
{
	_signalFactory = new SignalFactoryCore(parameters->GetStreamInfo()->streamSampleRate);
	_signalFactoryRandom = new SignalFactoryCore(parameters->GetStreamInfo()->streamSampleRate);
}

void SignalParameterAutomater::Update(const SignalParameter* parameter)
{
	if (!parameter->GetAutomationEnabled())
		throw new std::exception("Trying to call parameter automater for non-enabled automation");

	_type = parameter->GetAutomationType();
	_oscillatorType = parameter->GetAutomationOscillator(); 
	_oscillatorParameters->SetSignalLow(parameter->GetAutomationLow());
	_oscillatorParameters->SetSignalHigh(parameter->GetAutomationHigh());

	_oscillatorFrequency = parameter->GetAutomationFrequency();
	_oscillatorRandomFrequency = parameter->GetAutomationFrequency();

	_envelope->Update(parameter->GetAutomationEnvelope());
}

void SignalParameterAutomater::Engage(const PlaybackTime* playbackTime)
{
	_envelope->Engage(playbackTime);
}

void SignalParameterAutomater::DisEngage(const PlaybackTime* playbackTime)
{
	_envelope->DisEngage(playbackTime);
}

float SignalParameterAutomater::GetValue(const PlaybackFrame* frame, const PlaybackTime* playbackTime) const
{
	

	switch (_type)
	{
	case ParameterAutomationType::EnvelopeSweep:
		return _envelope->GetEnvelopeLevel(playbackTime);
		break;
	case ParameterAutomationType::Oscillator:
		switch (_oscillatorType)
		{
		case ParameterAutomationOscillator::Sine:
			return _signalFactory->GenerateSineSample(_oscillatorFrequency, playbackTime);
		case ParameterAutomationOscillator::Square:
			return _signalFactory->GenerateSquareSample(_oscillatorFrequency, playbackTime);
		case ParameterAutomationOscillator::Triangle:
			return _signalFactory->GenerateTriangleSample(_oscillatorFrequency, playbackTime);
		case ParameterAutomationOscillator::Sawtooth:
			return _signalFactory->GenerateSawtoothSample(_oscillatorFrequency, playbackTime);
		case ParameterAutomationOscillator::Random:
			return _signalFactoryRandom->GenerateRandomSample(_oscillatorRandomFrequency, playbackTime);
		default:
			throw new std::exception("Unhandled automation oscillator type:  SignalParameterAutomater.h");
		}
		break;
	default:
		throw new std::exception("Unhandled automation type:  SignalParameterAutomater.h");
	}
}
