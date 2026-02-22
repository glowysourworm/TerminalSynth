#include "AtomicLock.h"
#include "MainController.h"
#include "OutputSettings.h"
#include "RtAudioController.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include "WaveTableCache.h"
#include "Windows.h"
#include "WindowsKeyCodes.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

/// <summary>
/// This will eventually come from a file, so there'll be some new component
/// </summary>
SynthSettings* CreateConfiguration(OutputSettings* deviceSettings, const std::string& soundBankDirectory)
{
	SynthSettings* configuration = new SynthSettings(deviceSettings, soundBankDirectory);

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

int main(int argc, char* argv[], char* envp[])
{
	// This pointer is shared (see controllers)
	//
	OutputSettings* parameters = RtAudioController::GetPlaybackParametersUnsafe();
	SynthSettings* configuration = CreateConfiguration(parameters, "");

	// Sound Banks
	std::vector<std::string> soundBanks;
	std::map<std::string, std::vector<std::string>> soundBankMap;

	// Arguments:  {Sound Bank Dir}, ..
	if (argc > 1)
	{
		// Sound Library Folder:  /{base folder}/{individual library folder(s)}
		configuration->SetSoundBankDirectory(std::string(argv[1]));

		// Load Sound Banks:  The UI needs a copy of the strings to call a sound bank. The rest of the
		//					  WaveTableCache* will be on the AudioController* thread.
		//
		WaveTableCache waveTableCache;

		// -> Initialize() -> Searches sound bank directories...
		waveTableCache.Initialize(configuration, parameters);
		waveTableCache.GetSoundBanks(soundBanks);
		
		for (int index = 0; index < soundBanks.size(); index++)
		{
			std::vector<std::string> soundNames;
			waveTableCache.GetSoundNames(soundBanks[index], soundNames);
			
			soundBankMap.insert(std::make_pair(soundBanks[index], soundNames));
		}
	}

	SoundRegistry* registry = new SoundRegistry(soundBanks, soundBankMap);									// NEEDS INITIALIZATION (W/ SAMPLING RATE)
	AtomicLock* atomicLock = new AtomicLock();

	// Manual keyboard input
	MainController controller(atomicLock);

	SetConsoleTitleA("Terminal Synth");

	if (!controller.Initialize(configuration, parameters, registry))
		return -1;

	controller.Start();

	if (!controller.Dispose())
		return -1;

	// All threads have stopped, we can now delete the rest of our heap memory
	delete configuration;
	delete atomicLock;
	delete registry;
	delete parameters;

	return 0;
}