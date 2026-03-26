#include "AtomicLock.h"
#include "MainController.h"
#include "PlaybackUserData.h"
#include "PortAudioController.h"
#include "SynthSettings.h"
#include "SynthSettingsLoader.h"
#include "Windows.h"
#include <Stk.h>
#include <exception>

int main(int argc, char* argv[], char* envp[])
{
	SynthSettings* configuration = nullptr;
	SynthSettingsLoader configLoader;

	// Read config file
	if (argc > 1)
	{
		configuration = configLoader.LoadConfiguration(argv[1], true);
	}

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
	PlaybackUserData* userData = new PlaybackUserData(&configLoader);

	SetConsoleTitleA("Terminal Synth");

	if (!controller.Initialize(userData))
		return -1;

	controller.Start();

	if (!controller.Dispose())
		return -1;

	return 0;
}
