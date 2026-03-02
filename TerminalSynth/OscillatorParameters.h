#pragma once

#ifndef OSCILLATOR_PARAMETERS_H
#define OSCILLATOR_PARAMETERS_H

#include "Constant.h"
#include "Utility.h"
#include <string>
#include <type_traits>

class OscillatorParameters
{
public:

	OscillatorParameters()
	{
		_type = OscillatorType::BuiltIn;
		_builtInType = BuiltInOscillators::Sine;
		_soundBank = new std::string("");
		_soundName = new std::string("");
		_frequency = TerminalSynth::GetMidiFrequency(60);
		_signalLow = SIGNAL_LOW;
		_signalHigh = SIGNAL_HIGH;
	}

	/// <summary>
	/// Constructs an oscillator with built in source
	/// </summary>
	OscillatorParameters(OscillatorType type,
						 BuiltInOscillators builtInType, 
						 const std::string& soundBank,
						 const std::string& soundName,
						 float frequency, 
						 float signalLow, 
						 float signalHigh)
	{
		_type = type;
		_builtInType = builtInType;
		_soundBank = new std::string(soundBank);
		_soundName = new std::string(soundName);
		_frequency = frequency;
		_signalLow = signalLow;
		_signalHigh = signalHigh;
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
	}
	~OscillatorParameters()
	{
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

	size_t GetHashCode() const
	{
		std::hash<int> intHasher;
		std::hash<float> floatHasher;
		std::hash<std::string> stringHasher;

		// Just these data are involved in uniqueness
		size_t hash1 = intHasher((int)_type);
		size_t hash2 = intHasher((int)_builtInType);
		size_t hash3 = stringHasher(*_soundBank);
		size_t hash4 = stringHasher(*_soundName);
		size_t hash5 = floatHasher(_frequency);

		// Result -> seed (first parameter)
		TerminalSynth::HashCombine(hash1, hash2);
		TerminalSynth::HashCombine(hash1, hash3);
		TerminalSynth::HashCombine(hash1, hash4);
		TerminalSynth::HashCombine(hash1, hash5);

		return hash1;
	}

	/// <summary>
	/// Updates parameters; and returns true if there were changes
	/// </summary>
	bool Update(const OscillatorParameters& source)
	{
		bool isDirty = false;

		isDirty |= *_soundBank != source.GetSoundBank();
		isDirty |= *_soundName != source.GetSoundName();
		isDirty |= _frequency != source.GetFrequency();
		isDirty |= _signalLow != source.GetSignalLow();
		isDirty |= _signalHigh != source.GetSignalHigh();
		isDirty |= _type != source.GetType();
		isDirty |= _builtInType != source.GetBuiltInType();
		
		_soundBank->clear();
		_soundName->clear();

		_frequency = source.GetFrequency();
		_signalLow = source.GetSignalLow();
		_signalHigh = source.GetSignalHigh();
		_type = source.GetType();
		_builtInType = source.GetBuiltInType();
		_soundBank->append(source.GetSoundBank());
		_soundName->append(source.GetSoundName());

		return isDirty;
	}

	bool operator==(const OscillatorParameters& parameters)
	{
		return IsEqual(parameters);
	}
	bool operator!=(const OscillatorParameters& parameters)
	{
		return !IsEqual(parameters);
	}

private:

	bool IsEqual(const OscillatorParameters& other)
	{
		return other.GetSoundName() == this->GetSoundName() &&
			other.GetSoundBank() == this->GetSoundBank() &&
			other.GetFrequency() == this->GetFrequency() &&
			other.GetSignalLow() == this->GetSignalLow() &&
			other.GetSignalHigh() == this->GetSignalHigh() &&
			other.GetType() == this->GetType() &&
			other.GetBuiltInType() == this->GetBuiltInType();
	}

private:

	std::string* _soundBank;
	std::string* _soundName;
	float _frequency;
	float _signalLow;
	float _signalHigh;
	OscillatorType _type;
	BuiltInOscillators _builtInType;
};

#endif