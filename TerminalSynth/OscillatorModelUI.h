#pragma once

#ifndef OSCILLATOR_MODEL_UI_H
#define OSCILLATOR_MODEL_UI_H

#include "Envelope.h"
#include "OscillatorParameters.h"

class OscillatorModelUI
{
public:

	/// <summary>
	/// Constructs an oscillator with built in source
	/// </summary>
	OscillatorModelUI(const OscillatorParameters& parameters,
					  const Envelope& envelope)
	{
		_parameters = new OscillatorParameters(parameters);
		_envelope = new Envelope(envelope);
	}
	OscillatorModelUI(const OscillatorModelUI& copy)
	{
		_parameters = new OscillatorParameters(*copy.GetParameters());
		_envelope = new Envelope(*copy.GetEnvelope());
	}
	~OscillatorModelUI()
	{
		delete _envelope;
		delete _parameters;
	}

	OscillatorParameters* GetParameters() const { return _parameters; }
	Envelope* GetEnvelope() const { return _envelope; }

	void Update(const OscillatorModelUI& source)
	{
		_parameters->Update(*source.GetParameters());
		_envelope->Set(*source.GetEnvelope());
	}

private:

	OscillatorParameters* _parameters;
	Envelope* _envelope;
};

#endif