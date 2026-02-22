#pragma once

#ifndef WAVE_TABLE_CACHE_H
#define WAVE_TABLE_CACHE_H

#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "SignalFactory.h"
#include "SynthSettings.h"
#include "WaveTable.h"
#include "WaveTableCacheKey.h"
#include <map>
#include <string>
#include <vector>

class WaveTableCache
{
public:

	WaveTableCache();
	~WaveTableCache();

	bool Initialize(const SynthSettings* synthSettings, const OutputSettings* outputSettings);

	/// <summary>
	/// Loads the list of sound bank(s) (last directory name(s)) into the user's destination vector
	/// </summary>
	void GetSoundBanks(std::vector<std::string>& destination);

	/// <summary>
	/// Loads a list of sound bank wave table names from the desired sound bank
	/// </summary>
	void GetSoundNames(const std::string& soundBank, std::vector<std::string>& destination);

	/// <summary>
	/// Returns the closest possible wave table from the requested sound bank, with 
	/// the requested fundamental frequency. The WaveTable* will be setup for the requested
	/// frequency, no matter how it was built.
	/// </summary>
	/// <param name="soundBankName">Name of directory where the sound library is located (see GetSoundBanks())</param>
	/// <param name="soundName">Name of wave table located in the sound bank (see GetSoundNames())</param>
	/// <param name="fundamentalFrequency">Requested frequency, usually from the MIDI note, for the wave table</param>
	/// <returns>Best fit WaveTable* instance - not to be deleted!</returns>
	WaveTable* Get(const std::string& soundBankName, const std::string& soundName, float fundamentalFrequency);

	/// <summary>
	/// Returns the closest possible wave table for the requested oscillator. If midi note numbers were used, then 
	/// the table should be one-to-one with the audio stream.
	/// </summary>
	/// <returns>Best fit Oscillator* instance - not to be deleted!</returns>
	WaveTable* Get(const OscillatorParameters& parameters, int midiNumber);

private:

	/// <summary>
	/// Creates WaveTable* instance for the sound file name, and puts it in the cache.
	/// </summary>
	/// <param name="soundBankDir">Full path to sound bank directory</param>
	bool CreateWaveTable(WTCacheKey_SoundBank* cacheKey);

	/// <summary>
	/// Creates WaveTable* instance for the oscillator, and puts it in the cache.
	/// </summary>
	bool CreateWaveTable(WTCacheKey_Oscillator* cacheKey);

private:

	bool Initialize_SoundBanks(const SynthSettings* synthSettings, const OutputSettings* outputSettings);
	bool Initialize_Oscillators(const SynthSettings* synthSettings, const OutputSettings* outputSettings);

private:

	SignalFactory* _signalFactory;

	// Cache Keys (loaded during initialization)
	std::vector<WTCacheKey_SoundBank*>* _soundBankList;
	std::vector<WTCacheKey_Oscillator*>* _oscillatorList;

	// Lazy loaded during playback
	std::map<size_t, WaveTable*>* _cacheSoundBank;
	std::map<size_t, WaveTable*>* _cacheOscillator;

	unsigned int _systemSamplingRate;
	unsigned int _oscillatorSamplingRate;						// This may be set differently - which is compensated for by the WaveTable*
																// which takes two sampling rates. The sound file sampling rate is read by
																// libsndfile, and will be used for the sound bank samples.
};

#endif