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
		_voiceType = SynthVoiceType::Primitive;
		_primitiveVoiceType = PrimitiveSynthVoices::Sine;
		_terminalVoiceType = TerminalSynthVoices::SynthesizedStringPluck;
		_stkVoiceType = StkSynthVoices::StkBeeThree;
		_soundBank = new std::string("");
		_soundName = new std::string("");
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
	OscillatorParameters(SynthVoiceType voiceType,
					 	 PrimitiveSynthVoices builtInType,
						 TerminalSynthVoices terminalVoiceType,
						 StkSynthVoices stkVoiceType,
						 const std::string& soundBank,
						 const std::string& soundName,
						 float signalLow, 
						 float signalHigh)
	{
		_voiceType = voiceType;
		_primitiveVoiceType = builtInType;
		_terminalVoiceType = terminalVoiceType;
		_stkVoiceType = stkVoiceType;
		_soundBank = new std::string(soundBank);
		_soundName = new std::string(soundName);
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
		_voiceType = copy.GetVoiceType();
		_primitiveVoiceType = copy.GetPrimitiveVoiceType();
		_terminalVoiceType = copy.GetTerminalVoiceType();
		_stkVoiceType = copy.GetStkVoiceType();
		_soundBank = new std::string(copy.GetSoundBank());
		_soundName = new std::string(copy.GetSoundName());
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

	float GetSignalLow() const { return _signalLow; }
	float GetSignalHigh() const { return _signalHigh; }
	unsigned int GetOctave() const { return _octave; }
	std::string GetSoundBank() const { return *_soundBank; }
	std::string GetSoundName() const { return *_soundName; }
	SynthVoiceType GetVoiceType() const { return _voiceType; }
	PrimitiveSynthVoices GetPrimitiveVoiceType() const { return _primitiveVoiceType; }
	TerminalSynthVoices GetTerminalVoiceType() const { return _terminalVoiceType; }
	StkSynthVoices GetStkVoiceType() const { return _stkVoiceType; }
	float GetWaveshaperSidebandCents() const { return _waveshaperSidebandCents; }
	float GetWaveshaperRandomPhaseAmplitude() const { return _waveshaperRandomPhaseAmplitude; }
	std::vector<float>* GetWaveshaperHarmonics() const { return _waveshaperHarmonics; }

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
	void SetVoiceType(SynthVoiceType value) { _voiceType = value; }
	void SetPrimitiveVoiceType(PrimitiveSynthVoices value) { _primitiveVoiceType = value; }
	void SetTerminalVoiceType(TerminalSynthVoices value) { _terminalVoiceType = value; }
	void SetStkVoiceType(StkSynthVoices value) { _stkVoiceType = value; }
	void SetWaveshaperSidebandCents(float value) { _waveshaperSidebandCents = value; }
	void SetWaveshaperRandomPhaseAmplitude(float value) { _waveshaperRandomPhaseAmplitude = value; }

	// Creates a hash code for identifying the TYPE of oscillator (based only just enums and the sound bank names)
	size_t GetHashCode() const
	{
		std::hash<int> intHasher;
		std::hash<float> floatHasher;
		std::hash<std::string> stringHasher;

		// Just these data are involved in uniqueness
		size_t hash1 = intHasher((int)_voiceType);
		size_t hash2 = intHasher((int)_primitiveVoiceType);
		size_t hash3 = intHasher((int)_terminalVoiceType);
		size_t hash4 = intHasher((int)_stkVoiceType);
		size_t hash5 = stringHasher(*_soundBank);
		size_t hash6 = stringHasher(*_soundName);

		// Result -> seed (first parameter)
		TerminalSynth::HashCombine(hash1, hash2);
		TerminalSynth::HashCombine(hash1, hash3);
		TerminalSynth::HashCombine(hash1, hash4);
		TerminalSynth::HashCombine(hash1, hash5);
		TerminalSynth::HashCombine(hash1, hash6);

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
		isDirty |= _signalLow != source->GetSignalLow();
		isDirty |= _signalHigh != source->GetSignalHigh();
		isDirty |= _voiceType != source->GetVoiceType();
		isDirty |= _primitiveVoiceType != source->GetPrimitiveVoiceType();
		isDirty |= _terminalVoiceType != source->GetTerminalVoiceType();
		isDirty |= _stkVoiceType != source->GetStkVoiceType();
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

		_signalLow = source->GetSignalLow();
		_signalHigh = source->GetSignalHigh();
		_octave = source->GetOctave();
		_voiceType = source->GetVoiceType();
		_primitiveVoiceType = source->GetPrimitiveVoiceType();
		_terminalVoiceType = source->GetTerminalVoiceType();
		_stkVoiceType = source->GetStkVoiceType();
		_soundBank->append(source->GetSoundBank());
		_soundName->append(source->GetSoundName());

		return isDirty;
	}

public:

	void Save(std::ostream& stream)
	{
		stream << *_soundBank;
		stream << *_soundName;
		stream << _octave;
		stream << _signalLow;
		stream << _signalHigh;
		stream << (int)_voiceType;
		stream << (int)_primitiveVoiceType;
		stream << (int)_terminalVoiceType;
		stream << (int)_stkVoiceType;
		stream << _waveshaperSidebandCents;
		stream << _waveshaperRandomPhaseAmplitude;

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			stream << _waveshaperHarmonics->at(index);
		}

	}
	void Read(std::istream& stream)
	{
		int voiceType, primitiveVoiceType, terminalVoiceType, stkVoiceType;

		stream >> *_soundBank;
		stream >> *_soundName;
		stream >> _octave;
		stream >> _signalLow;
		stream >> _signalHigh;
		stream >> voiceType;
		stream >> primitiveVoiceType;
		stream >> terminalVoiceType;
		stream >> stkVoiceType;
		stream >> _waveshaperSidebandCents;
		stream >> _waveshaperRandomPhaseAmplitude;

		for (int index = 0; index < _waveshaperHarmonics->size(); index++)
		{
			stream >> _waveshaperHarmonics->at(index);
		}

		_voiceType = (SynthVoiceType)voiceType;
		_primitiveVoiceType = (PrimitiveSynthVoices)primitiveVoiceType;
		_terminalVoiceType = (TerminalSynthVoices)terminalVoiceType;
		_stkVoiceType = (StkSynthVoices)stkVoiceType;
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
			_octave == other->GetOctave() &&
			_signalLow == other->GetSignalLow() &&
			_signalHigh == other->GetSignalHigh() &&
			_voiceType == other->GetVoiceType() &&
			_primitiveVoiceType == other->GetPrimitiveVoiceType() &&
			_terminalVoiceType == other->GetTerminalVoiceType() &&
			_stkVoiceType == other->GetStkVoiceType() &&
			_waveshaperRandomPhaseAmplitude == other->GetWaveshaperRandomPhaseAmplitude() &&
			_waveshaperSidebandCents == other->GetWaveshaperSidebandCents();
	}

private:

	std::string* _soundBank;
	std::string* _soundName;
	unsigned int _octave;
	float _signalLow;
	float _signalHigh;
	SynthVoiceType _voiceType;
	PrimitiveSynthVoices _primitiveVoiceType;
	TerminalSynthVoices _terminalVoiceType;
	StkSynthVoices _stkVoiceType;

	float _waveshaperSidebandCents;
	float _waveshaperRandomPhaseAmplitude;
	
	std::vector<float>* _waveshaperHarmonics;
};

#endif