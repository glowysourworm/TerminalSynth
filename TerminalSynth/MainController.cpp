#include "AtomicLock.h"
#include "AudioController.h"
#include "BaseController.h"
#include "LoopTimer.h"
#include "MainController.h"
#include "OutputSettings.h"
#include "RtAudioController.h"
#include "RtAudioUserData.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include "UIController.h"
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

MainController::MainController(AtomicLock* atomicLock) : BaseController(atomicLock)
{
	_configuration = nullptr;
	_uiTimer = new LoopTimer(0.075);
	_audioController = new AudioController(atomicLock);
	_uiController = new UIController(atomicLock);
	_userData = new RtAudioUserData();
}

MainController::~MainController()
{
	delete _configuration;
	delete _uiTimer;
	delete _audioController;
	delete _uiController;
}

/// <summary>
/// Initialization function for the synth backend. This must be called before starting the player!
/// </summary>
bool MainController::Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry)
{
	_configuration = configuration;

	// RT AUDIO
	bool success = RtAudioController::Initialize(std::bind(&AudioController::ProcessAudioCallback,
												_audioController,
												std::placeholders::_1,
												std::placeholders::_2,
												std::placeholders::_3,
												std::placeholders::_4));

	// RT AUDIO -> Open Stream (SynthSettings*)(PlaybackParameteres*) (INITIALIZE!)
	success &= RtAudioController::OpenStream((void*)_userData);

	// Airwindows Plugins:  Require sampling rate!
	success &= effectRegistry->Initialize(parameters->GetSamplingRate());

	// Synth Settings Effect Registry:  This will separate out the SignalBase* (which will not be present in the SynthSettings*) (linking issue)
	//
	std::vector<SignalSettings> registryList;

	for (int index = 0; index < effectRegistry->GetCount(); index++)
	{
		std::string name = effectRegistry->GetName(index);
		SignalSettings settings = effectRegistry->GetSettings(name);
		
		registryList.push_back(settings);
	}

	// -> Initialize(..) (completes the configuration's registry)
	configuration->GetSignalChainRegistry()->Initialize(registryList);

	// -> Initilize(..) (the RT Audio playback can now be unblocked)
	_userData->Initialize(configuration, effectRegistry);

	// Audio Controller (for callback)
	success &= _audioController->Initialize(configuration, parameters, effectRegistry);

	// UI Controller (stream must be open) (Starts Thread!)
	success &= _uiController->Initialize(configuration, parameters, effectRegistry);

	return success;
}

void MainController::Start()
{
	_audioController->Start();
	_uiController->Start();

	//...
	this->Loop();
}

bool MainController::Dispose()
{
	// Stops UI thread
	return _audioController->Dispose() && _uiController->Dispose();
}

void MainController::Loop()
{
	// Playback Parameters (to update)
	float streamTime, audioTime, frontendTime, latency, left, right;

	// Primary Loop!!! 
	//
	// Audio Thread:  The RT Audio callback will arrive on their background thread from the audio backend. We
	//				  just have to manage two shared pointers:  OutputSettings*, and SynthSettings*.
	//				  The SynthPlaybackDevice* will be updated, from the synth configuration, during the 
	//				  callback. So, this must be synchronized somehow. Probably a std::atomic which could be
	//				  shared inside the SynthSettings*
	// 
	// UI Thread:  Unfortunately, the UI had to be run on a separate thread; but that won't really matter. We
	//			   can interrupt the thread to post UI updates. We're going to keep track of the UI timer here
	//			   and interrupt it every ~10ms.
	// 
	// Synth Configuration: There must be an additional lock on the SynthSettings* during UI interaction.
	//						This will be an std::atomic inside of the SynthSettings class; and must be
	//						set / unset in the same function! (This will be evident when you're updating it
	//						from the UI Controller)
	// 
	//						Also, this update will be very slow compared to the audio playback (~100ms / ~0.3ms);
	//						and we'll not worry about it more until there's a user end case (like, we're trying to
	//						use a MIDI keyboard and it's not fast enough)
	// 
	// The exit condition should be polled from the UI thread to this thread; and we'll use an interruption to
	// give / send data while the loop runs.
	//
	while (true)
	{
		// We may use these on this thread since we're calling the audio controller from here. The
		// callback from RT Audio WILL NOT SET this pointer's data!
		//
		OutputSettings* playbackParameters = RtAudioController::GetPlaybackParameters();

		// Update Playback Parameters:
		//
		_audioController->GetUpdate(streamTime, audioTime, frontendTime, latency, left, right);
		
		playbackParameters->UpdateRT(streamTime, _uiTimer->GetAvgMilli(), audioTime, frontendTime, latency);


		// CRITICAL SECTION:  This is an update from the UI, which will reset the synth parameters. So,
		//					  it is only allowed every ~100ms at the most.
		//
		//					  The std::mutex is in the UIController* (for To / From UI, and IsDirty)
		//
		//				      The SynthSettings* has an extra std::atomic<bool> for sync. with
		//					  the audio thread callback.
		//
		if (_uiTimer->Mark())
		{
			// Update <- UI (If user has made UI changes, they get set in the SynthSettings*) (SHARED!)
			//
			if (_uiController->IsDirty())
				_uiController->FromUI(_configuration);

			// Update -> UI (OutputSettings*) (NOT SHARED) (Member functions all go through this thread for playback parameters)
			_uiController->ToUI(_configuration);
		}

		// ~3ms, on par with the UI thread (divided by 3.14!) >_<
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
