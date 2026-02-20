#include "OutputSettings.h"
#include "WaveTable.h"
#include "WaveTableCache.h"
#include <filesystem>
#include <map>
#include <string>
#include <vector>

WaveTableCache::WaveTableCache(const OutputSettings* outputSettings)
{
	_cache = new std::map<std::string, WaveTable*>();
}

WaveTableCache::~WaveTableCache()
{
}

bool WaveTableCache::CreateSoundBank(const std::string& soundBankDir)
{
	// Sound Library Folder:  /{base folder}/{individual library folder(s)}
	for (auto const& dirEntry : std::filesystem::directory_iterator{ soundBankDir })
	{
		auto path = dirEntry.path();

		
	}

	return true;
}

void WaveTableCache::GetSoundBanks(std::vector<std::string>& destination)
{
}

void WaveTableCache::GetSoundNames(const std::string& soundBank, std::vector<std::string>& destination)
{
}

WaveTable* WaveTableCache::Get(const std::string& soundBankName, const std::string& soundName, float desiredFrequency)
{
	CacheKey cacheKey(soundBankName, soundName);
	std::string hashCode = cacheKey.GetHashCode();

	WaveTable* result = _cache->at(hashCode);

	// Set for frequency

	return result;
}
