#include "Oscillator.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include "WaveTable.h"
#include <string>

Oscillator::Oscillator(const std::string& name, const OscillatorParameters& parameters, WaveTable* waveTable)  : SignalBase(name)
{
	_parameters = new OscillatorParameters(parameters);
	_waveTable = waveTable;
	_initialized = false;
}

Oscillator::~Oscillator()
{
	delete _parameters;
	delete _waveTable;
}

void Oscillator::SetParameters(const OscillatorParameters& parameters)
{
	_parameters->Update(parameters);
}

void Oscillator::Initialize(const SignalSettings* configuration, const OutputSettings* parameters)
{
	SignalBase::Initialize(configuration, parameters);

	_initialized = true;
}

void Oscillator::SetFrame(PlaybackFrame* frame, float absoluteTime)
{
	_waveTable->SetFrame(frame, absoluteTime);

	float envelopeLevel = _parameters->GetEnvelope()->GetEnvelopeLevel(absoluteTime);

	frame->SetFrame(envelopeLevel * frame->GetLeft(), envelopeLevel * frame->GetRight());
}

bool Oscillator::HasOutput(float absoluteTime) const
{
	return _parameters->GetEnvelope()->HasOutput(absoluteTime);
}

void Oscillator::Engage(double absoluteTime)
{
	_parameters->GetEnvelope()->Engage(absoluteTime);
}

void Oscillator::DisEngage(double absoluteTime)
{
	_parameters->GetEnvelope()->DisEngage(absoluteTime);
}
