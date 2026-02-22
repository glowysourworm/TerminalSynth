#pragma once

#ifndef SOUND_BANK_PARAMETERS_H
#define SOUND_BANK_PARAMETERS_H

#include <exception>
#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

class SoundBankSettings
{
public:

	SoundBankSettings()
	{
		_soundBankDirectory = new std::string();
		_soundBanks = new std::vector<std::string>();
		_soundBankMaps = new std::map<std::string, std::vector<std::string>*>();
		_soundFileMap = new std::map<std::string, std::string>();
	}
	SoundBankSettings(const SoundBankSettings& copy)
	{
		_soundBankDirectory = new std::string(copy.GetSoundBankDirectory());
		_soundBanks = new std::vector<std::string>(copy.GetSoundBanks());
		_soundBankMaps = new std::map<std::string, std::vector<std::string>*>();
		_soundFileMap = new std::map<std::string, std::string>();

		// Sound Banks
		for (int index = 0; index < _soundBanks->size(); index++)
		{
			// Sound Names
			std::vector<std::string>* list = new std::vector<std::string>(copy.GetSoundNames(_soundBanks->at(index)));

			// Sound Bank:  Sound Name Map
			_soundBankMaps->insert(std::make_pair(_soundBanks->at(index), list));

			// Sound Files
			for (int nameIndex = 0; nameIndex < list->size(); nameIndex++)
			{
				// File Name
				std::string fileName = copy.GetSoundFileName(_soundBanks->at(index), list->at(nameIndex));

				_soundFileMap->insert(std::make_pair(list->at(nameIndex), fileName));
			}
		}
	}
	SoundBankSettings(const std::string& soundBankDirectory)
	{
		_soundBankDirectory = new std::string(soundBankDirectory);
		_soundBanks = new std::vector<std::string>();
		_soundBankMaps = new std::map<std::string, std::vector<std::string>*>();
		_soundFileMap = new std::map<std::string, std::string>();

		Initialize(soundBankDirectory);
	}
	~SoundBankSettings()
	{
		for (auto iter = _soundBankMaps->begin(); iter != _soundBankMaps->end(); ++iter)
		{
			delete iter->second;
		}

		delete _soundBankDirectory;
		delete _soundBanks;
		delete _soundBankMaps;
	}

	std::string GetSoundBankDirectory() const { return *_soundBankDirectory; }
	std::string GetSoundFileName(const std::string& soundBank, const std::string& soundName) const
	{
		// Sound Bank + Sound Name -> Sound File (full path)
		return _soundFileMap->at(soundBank + soundName);
	}
	std::vector<std::string> GetSoundBanks() const { return *_soundBanks; }
	std::vector<std::string> GetSoundNames(const std::string& soundBank) const { return *_soundBankMaps->at(soundBank); }

private:

	bool Initialize(const std::string& soundBankDirectory)
	{
		try
		{
			// Sound Library Folder:  /{base folder}/{individual library folder(s)}
			for (auto const& dirEntry : std::filesystem::directory_iterator{ soundBankDirectory })
			{
				// {individual library folder}
				if (dirEntry.is_directory())
				{
					// Sound Bank:  Iterate sound files in sub-directory
					for (auto const& dirSubEntry : std::filesystem::directory_iterator{ dirEntry.path() })
					{
						if (dirSubEntry.path().extension().string() == ".wav" ||
							dirSubEntry.path().extension().string() == ".aif" ||
							dirSubEntry.path().extension().string() == ".aiff")
						{
							int extensionSize = dirSubEntry.path().extension().string().size();
							std::string soundBankName = dirEntry.path().filename().string();
							std::string soundFileName = dirSubEntry.path().filename().string();
							std::string soundName = soundFileName.replace(soundFileName.size() - extensionSize, soundFileName.size() - 1, "");		// Remove Extension

							_soundBanks->push_back(soundBankName);

							if (!_soundBankMaps->contains(soundBankName))
								_soundBankMaps->insert(std::make_pair(soundBankName, new std::vector<std::string>()));

							_soundBankMaps->at(soundBankName)->push_back(soundName);
							_soundFileMap->insert(std::make_pair(soundBankName + soundName, soundFileName));
						}
					}
				}
			}
		}
		catch (std::exception ex)
		{
			return false;
		}

		return true;
	}

private:

	std::string* _soundBankDirectory;

	// Sound Banks
	std::vector<std::string>* _soundBanks;

	// Sound Names by Sound Bank
	std::map<std::string, std::vector<std::string>*>* _soundBankMaps;

	// Sound File Names by Sound Bank + Sound Name
	std::map<std::string, std::string>* _soundFileMap;
};

#endif