#pragma once

#ifndef OSCILLATOR_PARAMETERS_H
#define OSCILLATOR_PARAMETERS_H

#include "Constant.h"
#include "Envelope.h"

class OscillatorParameters
{
public:

	/// <summary>
	/// Constructs an oscillator with built in source
	/// </summary>
	OscillatorParameters(BuiltInOscillators builtInType, float frequency, float signalLow, float signalHigh, const Envelope& envelope)
	{
		_type = OscillatorType::BuiltIn;
		_builtInType = builtInType;
		_frequency = frequency;
		_signalLow = signalLow;
		_signalHigh = signalHigh;
		_envelope = new Envelope(envelope);
	}
	OscillatorParameters(const OscillatorParameters& copy)
	{
		_type = copy.GetType();
		_builtInType = copy.GetBuiltInType();
		_frequency = copy.GetFrequency();
		_signalLow = copy.GetSignalLow();
		_signalHigh = copy.GetSignalHigh();
		_envelope = new Envelope(
			copy.GetEnvelope()->GetAttack(), 
			copy.GetEnvelope()->GetDecay(), 
			copy.GetEnvelope()->GetSustain(), 
			copy.GetEnvelope()->GetRelease(), 
			copy.GetEnvelope()->GetAttackPeak(), 
			copy.GetEnvelope()->GetSustainPeak());
	}
	~OscillatorParameters()
	{
		delete _envelope;
	}

	float GetFrequency() const { return _frequency; }
	float GetSignalLow() const { return _signalLow; }
	float GetSignalHigh() const { return _signalHigh; }
	OscillatorType GetType() const { return _type; }
	BuiltInOscillators GetBuiltInType() const { return _builtInType; }
	Envelope* GetEnvelope() const { return _envelope; }

	void Update(const OscillatorParameters& parameters)
	{
		delete _envelope;

		_type = parameters.GetType();
		_builtInType = parameters.GetBuiltInType();
		_frequency = parameters.GetFrequency();
		_signalLow = parameters.GetSignalLow();
		_signalHigh = parameters.GetSignalHigh();
		_envelope = new Envelope(
			parameters.GetEnvelope()->GetAttack(),
			parameters.GetEnvelope()->GetDecay(),
			parameters.GetEnvelope()->GetSustain(),
			parameters.GetEnvelope()->GetRelease(),
			parameters.GetEnvelope()->GetAttackPeak(),
			parameters.GetEnvelope()->GetSustainPeak());
	}

private:

	float _frequency;
	float _signalLow;
	float _signalHigh;
	OscillatorType _type;
	BuiltInOscillators _builtInType;
	Envelope* _envelope;
};

#endif