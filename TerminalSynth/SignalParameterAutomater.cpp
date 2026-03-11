#include "Constant.h"
#include "OscillatorParameters.h"
#include "PlaybackInfo.h"
#include "PlaybackFrame.h"
#include "SignalFactoryCore.h"
#include "SignalParameter.h"
#include "SignalParameterAutomater.h"
#include <exception>

SignalParameterAutomater::SignalParameterAutomater()
{
	_frame = new PlaybackFrame(0, 0, 1);
	_signalFactory = nullptr;
	_signalFactoryRandom = nullptr;
	_oscillatorParameters = new OscillatorParameters(OscillatorType::BuiltIn, BuiltInOscillators::Sine, "", "", 1.0f, ENVELOPE_LOW, ENVELOPE_HIGH);
	_oscillatorParametersRandom = new OscillatorParameters(OscillatorType::BuiltIn, BuiltInOscillators::Random, "", "", 1.0f, ENVELOPE_LOW, ENVELOPE_HIGH);
	_type = ParameterAutomationType::EnvelopeSweep;
	_oscillatorType = ParameterAutomationOscillator::Sine;
}

SignalParameterAutomater::~SignalParameterAutomater()
{
	if (_signalFactory != nullptr)
		delete _signalFactory;

	if (_signalFactoryRandom != nullptr)
		delete _signalFactoryRandom;

	delete _frame;
	delete _oscillatorParameters;
}

void SignalParameterAutomater::Initialize(const PlaybackInfo* parameters)
{
	_signalFactory = new SignalFactoryCore(parameters->GetStreamInfo()->streamSampleRate);
	_signalFactoryRandom = new SignalFactoryCore(parameters->GetStreamInfo()->streamSampleRate);

	_signalFactory->Reset(_oscillatorParameters);
	_signalFactoryRandom->Reset(_oscillatorParametersRandom);
}

void SignalParameterAutomater::Update(const SignalParameter* parameter)
{
	if (!parameter->GetAutomationEnabled())
		throw new std::exception("Trying to call parameter automater for non-enabled automation");

	_type = parameter->GetAutomationType();
	_oscillatorType = parameter->GetAutomationOscillator(); 
	_oscillatorParameters->SetFrequency(parameter->GetAutomationFrequency());
	_oscillatorParametersRandom->SetFrequency(parameter->GetAutomationFrequency());
}

float SignalParameterAutomater::GetValue(const PlaybackFrame* frame, double absoluteTime) const
{
	switch (_type)
	{
	case ParameterAutomationType::EnvelopeSweep:
		return frame->GetEnvelopeLevel();
		break;
	case ParameterAutomationType::Oscillator:
		switch (_oscillatorType)
		{
		case ParameterAutomationOscillator::Sine:
			return _signalFactory->GenerateSineSample(_oscillatorParameters->GetFrequency(), absoluteTime);
		case ParameterAutomationOscillator::Square:
			return _signalFactory->GenerateSquareSample(_oscillatorParameters->GetFrequency(), absoluteTime);
		case ParameterAutomationOscillator::Triangle:
			return _signalFactory->GenerateTriangleSample(_oscillatorParameters->GetFrequency(), absoluteTime);
		case ParameterAutomationOscillator::Sawtooth:
			return _signalFactory->GenerateSawtoothSample(_oscillatorParameters->GetFrequency(), absoluteTime);
		case ParameterAutomationOscillator::Random:
			return _signalFactoryRandom->GenerateRandomSample(_oscillatorParametersRandom->GetFrequency(), absoluteTime);
		default:
			throw new std::exception("Unhandled automation oscillator type:  SignalParameterAutomater.h");
		}
		break;
	default:
		throw new std::exception("Unhandled automation type:  SignalParameterAutomater.h");
	}
}
