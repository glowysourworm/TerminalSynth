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
	_samplingRate = 0;											// Initialize

	_frame = new PlaybackFrame(0, 0);

	_oscillatorParameters = new OscillatorParameters(
		SynthVoiceType::Primitive, 
		PrimitiveSynthVoices::Sine, 
		TerminalSynthVoices::SynthesizedStringPluck, 
		StkSynthVoices::StkBeeThree, 
		"", "", ENVELOPE_LOW, ENVELOPE_HIGH);

	_oscillatorFrequency = 0.5;

	_type = ParameterAutomationType::EnvelopeSweep;
	_oscillatorType = ParameterAutomationOscillator::Sine;
	_envelope = new Envelope();
}

SignalParameterAutomater::~SignalParameterAutomater()
{
	delete _frame;
	delete _oscillatorParameters;
	delete _envelope;
}

void SignalParameterAutomater::Initialize(const PlaybackInfo* parameters)
{
	_samplingRate = parameters->GetStreamInfo()->streamSampleRate;
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
			return SignalFactoryCore::GenerateSineSample(_oscillatorFrequency, _samplingRate, _oscillatorParameters->GetSignalHigh(), _oscillatorParameters->GetSignalLow(), playbackTime);
		case ParameterAutomationOscillator::Square:
			return SignalFactoryCore::GenerateSquareSample(_oscillatorFrequency, _samplingRate, _oscillatorParameters->GetSignalHigh(), _oscillatorParameters->GetSignalLow(), playbackTime);
		case ParameterAutomationOscillator::Triangle:
			return SignalFactoryCore::GenerateTriangleSample(_oscillatorFrequency, _samplingRate, _oscillatorParameters->GetSignalHigh(), _oscillatorParameters->GetSignalLow(), playbackTime);
		case ParameterAutomationOscillator::Sawtooth:
			return SignalFactoryCore::GenerateSawtoothSample(_oscillatorFrequency, _samplingRate, _oscillatorParameters->GetSignalHigh(), _oscillatorParameters->GetSignalLow(), playbackTime);
		default:
			throw new std::exception("Unhandled automation oscillator type:  SignalParameterAutomater.h");
		}
		break;
	default:
		throw new std::exception("Unhandled automation type:  SignalParameterAutomater.h");
	}
}
