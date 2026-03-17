#pragma once

#ifndef OSCILLATOR_PARAMETERS_H
#define OSCILLATOR_PARAMETERS_H

#include "Constant.h"
#include "Utility.h"
#include <cmath>
#include <istream>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

class OscillatorParameters
{
public:

	const int WAVESHAPER_HARMONICS_COUNT = 10;

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
		_octave = 0;

		_waveshaperSidebandCents = 0.0f;
		_waveshaperRandomPhaseAmplitude = 0.0f;
		_waveshaperHarmonics = new std::vector<float>(WAVESHAPER_HARMONICS_COUNT);

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			_waveshaperHarmonics->at(index) = 1 / std::sqrt(index + 1);
		}
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
		_octave = 0;

		_waveshaperSidebandCents = 0.0f;
		_waveshaperRandomPhaseAmplitude = 0.0f;
		_waveshaperHarmonics = new std::vector<float>(WAVESHAPER_HARMONICS_COUNT);

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			_waveshaperHarmonics->at(index) = 1 / std::sqrt(index + 1);
		}
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
		_octave = copy.GetOctave();
		
		_waveshaperSidebandCents = copy.GetWaveshaperSidebandCents();
		_waveshaperRandomPhaseAmplitude = copy.GetWaveshaperRandomPhaseAmplitude();
		_waveshaperHarmonics = new std::vector<float>(*copy.GetWaveshaperHarmonics());
	}
	~OscillatorParameters()
	{
		delete _soundBank;
		delete _soundName;
		delete _waveshaperHarmonics;
	}

	float GetFrequency() const { return _frequency; }
	float GetSignalLow() const { return _signalLow; }
	float GetSignalHigh() const { return _signalHigh; }
	unsigned int GetOctave() const { return _octave; }
	std::string GetSoundBank() const { return *_soundBank; }
	std::string GetSoundName() const { return *_soundName; }
	OscillatorType GetType() const { return _type; }
	BuiltInOscillators GetBuiltInType() const { return _builtInType; }
	float GetWaveshaperSidebandCents() const { return _waveshaperSidebandCents; }
	float GetWaveshaperRandomPhaseAmplitude() const { return _waveshaperRandomPhaseAmplitude; }
	std::vector<float>* GetWaveshaperHarmonics() const { return _waveshaperHarmonics; }

	void SetFrequency(float value) { _frequency = value; }
	void SetSignalLow(float value) { _signalLow = value; }
	void SetSignalHigh(float value) { _signalHigh = value; }
	void SetOctave(unsigned int value) { _octave = value; }
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
	void SetWaveshaperSidebandCents(float value) { _waveshaperSidebandCents = value; }
	void SetWaveshaperRandomPhaseAmplitude(float value) { _waveshaperRandomPhaseAmplitude = value; }

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
	bool Update(const OscillatorParameters* source)
	{
		bool isDirty = false;

		isDirty |= *_soundBank != source->GetSoundBank();
		isDirty |= *_soundName != source->GetSoundName();
		isDirty |= _frequency != source->GetFrequency();
		isDirty |= _signalLow != source->GetSignalLow();
		isDirty |= _signalHigh != source->GetSignalHigh();
		isDirty |= _type != source->GetType();
		isDirty |= _builtInType != source->GetBuiltInType();
		isDirty |= _octave != source->GetOctave();
		isDirty |= _waveshaperSidebandCents != source->GetWaveshaperSidebandCents();
		isDirty |= _waveshaperRandomPhaseAmplitude != source->GetWaveshaperRandomPhaseAmplitude();

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			isDirty |= _waveshaperHarmonics->at(index) != source->GetWaveshaperHarmonics()->at(index);

			_waveshaperHarmonics->at(index) = source->GetWaveshaperHarmonics()->at(index);
		}
		
		_soundBank->clear();
		_soundName->clear();

		_frequency = source->GetFrequency();
		_signalLow = source->GetSignalLow();
		_signalHigh = source->GetSignalHigh();
		_octave = source->GetOctave();
		_type = source->GetType();
		_builtInType = source->GetBuiltInType();
		_soundBank->append(source->GetSoundBank());
		_soundName->append(source->GetSoundName());

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

public:

	void Save(std::ostream& stream)
	{
		stream << *_soundBank;
		stream << *_soundName;
		stream << _frequency;
		stream << _octave;
		stream << _signalLow;
		stream << _signalHigh;
		stream << (int)_type;
		stream << (int)_builtInType;
		stream << _waveshaperSidebandCents;
		stream << _waveshaperRandomPhaseAmplitude;

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			stream << _waveshaperHarmonics->at(index);
		}

	}
	void Read(std::istream& stream)
	{
		int type, builtInType;

		stream >> *_soundBank;
		stream >> *_soundName;
		stream >> _frequency;
		stream >> _octave;
		stream >> _signalLow;
		stream >> _signalHigh;
		stream >> type;
		stream >> builtInType;
		stream >> _waveshaperSidebandCents;
		stream >> _waveshaperRandomPhaseAmplitude;

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			stream >> _waveshaperHarmonics->at(index);
		}

		_type = (OscillatorType)type;
		_builtInType = (BuiltInOscillators)builtInType;
	}

public:

	bool IsEqual(const OscillatorParameters* other) const
	{
		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			if (_waveshaperHarmonics->at(index) != other->GetWaveshaperHarmonics()->at(index))
				return false;
		}

		return *_soundBank == other->GetSoundBank() &&
			*_soundName == other->GetSoundName() &&
			_frequency == other->GetFrequency() &&
			_octave == other->GetOctave() &&
			_signalLow == other->GetSignalLow() &&
			_signalHigh == other->GetSignalHigh() &&
			_type == other->GetType() &&
			_builtInType == other->GetBuiltInType() &&
			_waveshaperRandomPhaseAmplitude == other->GetWaveshaperRandomPhaseAmplitude() &&
			_waveshaperSidebandCents == other->GetWaveshaperSidebandCents();
	}

private:

	bool IsEqual(const OscillatorParameters& other)
	{
		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			if (_waveshaperHarmonics->at(index) != other.GetWaveshaperHarmonics()->at(index))
				return false;
		}

		return other.GetSoundName() == this->GetSoundName() &&
			other.GetSoundBank() == this->GetSoundBank() &&
			other.GetFrequency() == this->GetFrequency() &&
			other.GetSignalLow() == this->GetSignalLow() &&
			other.GetSignalHigh() == this->GetSignalHigh() &&
			other.GetType() == this->GetType() &&
			other.GetBuiltInType() == this->GetBuiltInType() &&
			other.GetOctave() == this->GetOctave() &&
			other.GetWaveshaperRandomPhaseAmplitude() == this->GetWaveshaperRandomPhaseAmplitude() &&
			other.GetWaveshaperSidebandCents() == this->GetWaveshaperSidebandCents();
	}

private:

	std::string* _soundBank;
	std::string* _soundName;
	float _frequency;						// This gets set with the octave:  see SynthNotePool*
	unsigned int _octave;
	float _signalLow;
	float _signalHigh;
	OscillatorType _type;
	BuiltInOscillators _builtInType;

	float _waveshaperSidebandCents;
	float _waveshaperRandomPhaseAmplitude;
	
	std::vector<float>* _waveshaperHarmonics;
};

#endif