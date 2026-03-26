#include "AtomicLock.h"
#include "MainController.h"
#include "PlaybackUserData.h"
#include "PortAudioController.h"
#include "SynthSettings.h"
#include "Windows.h"
#include "WindowsKeyCodes.h"
#include <Stk.h>
#include <string>
#include <string.h>

#include <exception>
#include <filesystem>
#include <fstream>
#define DEFAULT_CONFIG_FILE_NAME ".terminal-synth-config"

/// <summary>
/// This will eventually come from a file, so there'll be some new component
/// </summary>
SynthSettings* CreateDefeaultConfiguration(
	const std::string& soundBankDirectory, 
	const std::string& stkRawWaveDirectory, 
	bool soundBankEnabled,
	bool stkEnabled)
{
	SynthSettings* configuration = nullptr;

	// STK (enable / disable)
	if (stkEnabled)
		configuration = new SynthSettings(soundBankDirectory, stkRawWaveDirectory);

	else if (soundBankEnabled)
		configuration = new SynthSettings(soundBankDirectory);

	else
		configuration = new SynthSettings();

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

	return configuration;
}

SynthSettings* LoadConfiguration(const std::string& fileName)
{
	try
	{
		if (!std::filesystem::exists(fileName))
			return CreateDefeaultConfiguration("", "", false, false);

		SynthSettings loadSettings;
		std::ifstream stream(fileName);
		loadSettings.Read(stream);

		return new SynthSettings(loadSettings);
	}
	catch (std::exception ex)
	{
		return CreateDefeaultConfiguration("", "", false, false);
	}
}

bool GetArg(int argc, char* argv[], const char* key, std::string& destination)
{
	for (int index = 0; index < argc - 1; index++)
	{
		if (std::strcmp(argv[index], key) == 0)
		{
			destination.clear();
			destination.append(argv[index + 1]);

			return true;
		}
	}

	return false;
}

int atexit(int asdf)
{
	return 0;
}

int main(int argc, char* argv[], char* envp[])
{
	std::string configFile = DEFAULT_CONFIG_FILE_NAME;

	// Read config file
	if (argc > 1)
	{
		configFile = argv[1];
	}

	// Configuration
	SynthSettings* configuration = LoadConfiguration(configFile);

	// STK (try set stk settings)
	if (configuration->GetStkEnabled())
	{
		try
		{
			stk::Stk::setRawwavePath(configuration->GetStkRawWaveDirectory());
		}
		catch (std::exception& ex)
		{
			configuration->DisableStk();
		}
	}

	// This pointer is shared (see controllers)
	//
	AtomicLock* playbackLock = new AtomicLock();

	// Manual keyboard input
	PortAudioController audioController(playbackLock);
	MainController controller(&audioController, playbackLock);

	// Primary Shared Pointers:  The OutputSettings* are initialized and maintained by the MainController, with 
	//							 the RtAudioController* providing the host api, and device info.
	//
	PlaybackUserData* userData = new PlaybackUserData(configuration);

	SetConsoleTitleA("Terminal Synth");

	if (!controller.Initialize(userData))
		return -1;

	controller.Start();

	if (!controller.Dispose())
		return -1;

	// All threads have stopped, we can now delete the rest of our heap memory
	delete configuration;
	delete playbackLock;
	delete userData;

	return 0;
}
