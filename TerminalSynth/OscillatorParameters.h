#pragma once

#ifndef OSCILLATOR_PARAMETERS_H
#define OSCILLATOR_PARAMETERS_H

#include "Constant.h"
#include "Envelope.h"
#include <string>

class OscillatorParameters
{
public:

	/// <summary>
	/// Constructs an oscillator with built in source
	/// </summary>
	OscillatorParameters(OscillatorType type,
						 BuiltInOscillators builtInType, 
						 const std::string& soundBank,
						 const std::string& soundName,
						 float frequency, 
						 float signalLow, 
						 float signalHigh, 
						 const Envelope& envelope)
	{
		_type = type;
		_builtInType = builtInType;
		_soundBank = new std::string(soundBank);
		_soundName = new std::string(soundName);
		_frequency = frequency;
		_signalLow = signalLow;
		_signalHigh = signalHigh;
		_envelope = new Envelope(envelope);
	}
	OscillatorParameters(const OscillatorParameters& copy)
	{
		_type = copy.GetType();
		_builtInType = copy.GetBuiltInType();
		_soundBank = new std::string(copy.GetSoundBank());
		_soundName = new std::string(copy.GetSoundName());
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
		delete _soundBank;
		delete _soundName;
	}

	float GetFrequency() const { return _frequency; }
	float GetSignalLow() const { return _signalLow; }
	float GetSignalHigh() const { return _signalHigh; }
	std::string GetSoundBank() const { return *_soundBank; }
	std::string GetSoundName() const { return *_soundName; }
	OscillatorType GetType() const { return _type; }
	BuiltInOscillators GetBuiltInType() const { return _builtInType; }
	Envelope* GetEnvelope() const { return _envelope; }

	void SetFrequency(float value) { _frequency = value; }
	void SetSignalLow(float value) { _signalLow = value; }
	void SetSignalHigh(float value) { _signalHigh = value; }
	void SetSoundBank(const std::string& value) 
	{ 
		_soundBank->clear();
		_soundBank->append(value);
	}
	void SetSoundName(const std::string& value) 
	{ 
		_soundName->clear();
		_soundName->append(value);
	}
	void SetType(OscillatorType value) { _type = value; }
	void SetBuiltInType(BuiltInOscillators value) { _builtInType = value; }
	void SetEnvelope(const Envelope& value) { _envelope->Set(value); }

	void Update(const OscillatorParameters& source)
	{
		_soundBank->clear();
		_soundName->clear();

		_frequency = source.GetFrequency();
		_signalLow = source.GetSignalLow();
		_signalHigh = source.GetSignalHigh();
		_type = source.GetType();
		_builtInType = source.GetBuiltInType();
		_soundBank->append(source.GetSoundBank());
		_soundName->append(source.GetSoundName());
		_envelope->Set(*source.GetEnvelope());
	}

private:

	std::string* _soundBank;
	std::string* _soundName;
	float _frequency;
	float _signalLow;
	float _signalHigh;
	OscillatorType _type;
	BuiltInOscillators _builtInType;
	Envelope* _envelope;
};

#endif