#include "SynthSettings.h"
#include "SynthSettingsLoader.h"
#include "WindowsKeyCodes.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>

/// <summary>
/// This will eventually come from a file, so there'll be some new component
/// </summary>
SynthSettings* SynthSettingsLoader::CreateDefeaultConfiguration(bool setAsCurrent)
{
	SynthSettings* configuration = new SynthSettings();

	// Oversampling
	configuration->SetOversamplingFactor(10);

	// Octave 1
	configuration->SetMidiNote(WindowsKeyCodes::Z, 21);
	configuration->SetMidiNote(WindowsKeyCodes::X, 22);
	configuration->SetMidiNote(WindowsKeyCodes::C, 23);
	configuration->SetMidiNote(WindowsKeyCodes::V, 24);
	configuration->SetMidiNote(WindowsKeyCodes::B, 25);
	configuration->SetMidiNote(WindowsKeyCodes::N, 26);
	configuration->SetMidiNote(WindowsKeyCodes::M, 27);
	configuration->SetMidiNote(WindowsKeyCodes::COMMA, 28);
	configuration->SetMidiNote(WindowsKeyCodes::PERIOD, 29);
	configuration->SetMidiNote(WindowsKeyCodes::QUESTION_MARK, 30);

	// Octave 2
	configuration->SetMidiNote(WindowsKeyCodes::A, 33);
	configuration->SetMidiNote(WindowsKeyCodes::S, 34);
	configuration->SetMidiNote(WindowsKeyCodes::D, 35);
	configuration->SetMidiNote(WindowsKeyCodes::F, 36);
	configuration->SetMidiNote(WindowsKeyCodes::G, 37);
	configuration->SetMidiNote(WindowsKeyCodes::H, 38);
	configuration->SetMidiNote(WindowsKeyCodes::J, 39);
	configuration->SetMidiNote(WindowsKeyCodes::K, 40);
	configuration->SetMidiNote(WindowsKeyCodes::L, 41);
	configuration->SetMidiNote(WindowsKeyCodes::SEMICOLON, 42);
	configuration->SetMidiNote(WindowsKeyCodes::APOSTROPHE, 43);

	// Octave 3
	configuration->SetMidiNote(WindowsKeyCodes::Q, 44);
	configuration->SetMidiNote(WindowsKeyCodes::W, 45);
	configuration->SetMidiNote(WindowsKeyCodes::E, 46);
	configuration->SetMidiNote(WindowsKeyCodes::R, 47);
	configuration->SetMidiNote(WindowsKeyCodes::T, 48);
	configuration->SetMidiNote(WindowsKeyCodes::Y, 49);
	configuration->SetMidiNote(WindowsKeyCodes::U, 50);
	configuration->SetMidiNote(WindowsKeyCodes::I, 51);
	configuration->SetMidiNote(WindowsKeyCodes::O, 52);
	configuration->SetMidiNote(WindowsKeyCodes::P, 53);
	configuration->SetMidiNote(WindowsKeyCodes::LEFT_BRACKET, 54);
	configuration->SetMidiNote(WindowsKeyCodes::RIGHT_BRACKET, 55);

	// Octave 4
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_1, 56);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_2, 57);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_3, 58);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_4, 59);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_5, 60);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_6, 61);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_7, 62);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_8, 63);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_9, 64);
	configuration->SetMidiNote(WindowsKeyCodes::NUMBER_0, 65);
	configuration->SetMidiNote(WindowsKeyCodes::MINUS, 66);
	configuration->SetMidiNote(WindowsKeyCodes::PLUS, 67);

	if (setAsCurrent)
	{
		if (_instance != nullptr)
			delete _instance;

		_instance = configuration;
	}

	return configuration;
}

SynthSettings* SynthSettingsLoader::LoadConfiguration(const std::string& fileName, bool setAsCurrent)
{
	try
	{
		if (!std::filesystem::exists(fileName))
			return CreateDefeaultConfiguration(setAsCurrent);

		SynthSettings loadSettings;
		std::ifstream stream(fileName);
		loadSettings.Read(stream);

		stream.close();

		if (setAsCurrent)
		{
			if (_instance != nullptr)
				delete _instance;

			_instance = new SynthSettings(loadSettings);

			return _instance;
		}
		else
			return new SynthSettings(loadSettings);
	}
	catch (std::exception ex)
	{
		return CreateDefeaultConfiguration(setAsCurrent);
	}
}

void SynthSettingsLoader::SaveConfiguration()
{
	SaveConfiguration(_instance, false);
}

void SynthSettingsLoader::SaveConfiguration(SynthSettings* configuration, bool setAsCurrent)
{
	SaveConfiguration(configuration, "", setAsCurrent);
}
void SynthSettingsLoader::SaveConfiguration(SynthSettings* configuration, const std::string& fileName, bool setAsCurrent)
{
	try
	{
		std::string file(DEFAULT_CONFIG_FILE_NAME);

		if (fileName != "" && std::filesystem::exists(fileName))
			file = fileName;

		std::ofstream stream(file, std::ios::trunc | std::ios::binary);
		
		configuration->Save(stream);

		stream.flush();
		stream.close();

		if (setAsCurrent)
		{
			if (_instance != nullptr)
				delete _instance;

			_instance = configuration;
		}
	}
	catch (std::exception& ex)
	{
		// Log or just exit
	}
}