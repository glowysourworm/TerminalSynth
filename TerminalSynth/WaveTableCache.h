#pragma once

#include "OutputSettings.h"
#include "WaveTable.h"
#include <string>
#include <vector>
#include <map>

class WaveTableCache
{
protected:

	class CacheKey
	{
	public:

		CacheKey(const std::string& soundBank, const std::string& name)
		{
			_soundBank = new std::string(soundBank);
			_name = new std::string(name);
			_hashCode = new std::string(soundBank + name);
		}
		~CacheKey()
		{
			delete _soundBank;
			delete _name;
			delete _hashCode;
		}

		std::string GetName() const { return *_name; }
		std::string GetSoundBank() const { return *_soundBank; }
		std::string GetHashCode() const { return *_hashCode; }

	private:
		std::string* _soundBank;
		std::string* _name;
		std::string* _hashCode;
	};

public:

	WaveTableCache(const OutputSettings* outputSettings);
	~WaveTableCache();

	/// <summary>
	/// Creates WaveTable* instances for each supported file type in the directory (.wav, .aiff)
	/// </summary>
	/// <param name="soundBankDir">Full path to sound bank directory</param>
	bool CreateSoundBank(const std::string& soundBankDir);

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

private:

	std::map<std::string, WaveTable*>* _cache;
};