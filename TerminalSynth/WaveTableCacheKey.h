#pragma once

#ifndef WAVE_TABLE_CACHE_KEY_H
#define WAVE_TABLE_CACHE_KEY_H

#include "OscillatorParameters.h"
#include <string>
#include <type_traits>

class WaveTableCacheKey
{
public:
	WaveTableCacheKey() {};

	virtual size_t GetHashCode() = 0;
};

class WTCacheKey_Oscillator : public WaveTableCacheKey
{
public:

	WTCacheKey_Oscillator(const OscillatorParameters& parameters, unsigned int midiNote, unsigned int samplingRate)
	{
		_parameters = new OscillatorParameters(parameters);
		_sampleRate = samplingRate;
		_midiNote = midiNote;
		_numberFrames = (1.0f / parameters.GetFrequency()) * samplingRate;					// Could be over / under sampled (by design)
	}
	~WTCacheKey_Oscillator()
	{
		delete _parameters;
	}

	size_t GetHashCode() override
	{
		std::hash<double> hash;
		
		return hash((double)_parameters->GetFrequency() + (double)_parameters->GetBuiltInType());
	}

	OscillatorParameters GetParameters() const { return *_parameters; }
	unsigned int GetSampleRate() const { return _sampleRate; }
	unsigned int GetNumberOfFrames() const { return _numberFrames; }
	unsigned int GetMidiNote() const { return _midiNote; }

private:
	OscillatorParameters* _parameters;
	unsigned int _sampleRate;
	unsigned int _numberFrames;
	unsigned int _midiNote;
};

class WTCacheKey_SoundBank : public WaveTableCacheKey
{
public:

	WTCacheKey_SoundBank(const std::string& soundBank, const std::string& name, const std::string& fileName)
	{
		_soundBank = new std::string(soundBank);
		_name = new std::string(name);
		_fileName = new std::string(fileName);

		_sampleRate = 0;
		_numberChannels = 0;
		_numberFrames = 0;

	}
	~WTCacheKey_SoundBank()
	{
		delete _soundBank;
		delete _name;
		delete _fileName;
	}

	size_t GetHashCode() override
	{
		std::hash<std::string> hash;

		return hash(*_fileName);
	}

	void SetSoundFileData(unsigned int samplingRate, unsigned int numberChannels, unsigned int numberFrames)
	{
		_sampleRate = samplingRate;
		_numberChannels = numberChannels;
		_numberFrames = numberFrames;
	}

	std::string GetName() const { return *_name; }
	std::string GetSoundBank() const { return *_soundBank; }
	std::string GetFileName() const { return *_fileName; }

	unsigned int GetSampleRate() const { return _sampleRate; }
	unsigned int GetNumberOfChannels() const { return _numberChannels; }
	unsigned int GetNumberOfFrames() const { return _numberFrames; }

private:
	std::string* _soundBank;
	std::string* _name;
	std::string* _fileName;

	unsigned int _sampleRate;
	unsigned int _numberChannels;
	unsigned int _numberFrames;
};

#endif