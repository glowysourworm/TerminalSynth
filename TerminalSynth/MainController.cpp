#include "AtomicLock.h"
#include "AudioController.h"
#include "BaseController.h"
#include "LoopTimer.h"
#include "MainController.h"
#include "MainUI.h"
#include "OutputSettings.h"
#include "RtAudioController.h"
#include "RtAudioUserData.h"
#include "SignalSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <chrono>
#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <functional>
#include <thread>
#include <vector>

MainController::MainController(AtomicLock* playbackLock) : BaseController(playbackLock)
{
	_configuration = nullptr;
	_mainUI = nullptr;
	_uiTimer = new LoopTimer(0.075);
	_audioController = new AudioController(playbackLock);
	_userData = new RtAudioUserData();
	
}

MainController::~MainController()
{
	delete _configuration;
	delete _mainUI;
	delete _uiTimer;
	delete _audioController;
	delete _userData;
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

	// Synth Settings Effect Registry:  This will separate out the SignalBase* (which will not be present in the SynthSettings*) (linking issue)
	//
	std::vector<SignalSettings> registryList;

	// Airwindows Plugins:  Require sampling rate!
	success &= effectRegistry->Initialize(parameters->GetSamplingRate(), registryList);

	// -> Initialize(..) (completes the configuration's registry)
	configuration->GetSoundSettings()->Initialize(registryList);

	// -> Initilize(..) (the RT Audio playback can now be unblocked)
	_userData->Initialize(configuration, effectRegistry);

	// Audio Controller (for callback)
	success &= _audioController->Initialize(configuration, parameters, effectRegistry);

	// Main UI Initialize
	_mainUI = new MainUI(*configuration);
	_mainUI->Initialize(*configuration);

	return success;
}

void MainController::Start()
{
	_audioController->Start();

	//...
	this->Loop();
}

bool MainController::Dispose()
{
	// Stops UI thread
	return _audioController->Dispose();
}

void MainController::Loop()
{
	auto screen = ftxui::ScreenInteractive::TerminalOutput();
	auto loop = ftxui::Loop(&screen, _mainUI->GetComponent());

	// FTXUI has an option to create an event loop (this will run their backend UI code)
	//
	// https://arthursonzogni.com/FTXUI/doc/examples_2component_2custom_loop_8cpp-example.html#_a8
	//

	// We may use these on this thread since we're calling the audio controller from here. The
	// callback from RT Audio WILL NOT SET this pointer's data!
	//
	OutputSettings* playbackParameters = RtAudioController::GetPlaybackParameters();

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
	while (!loop.HasQuitted())
	{
		// Update Playback Parameters:
		//
		_audioController->GetUpdate(streamTime, audioTime, frontendTime, latency, left, right);
		
		// Apply update to our pointer to playback parameters
		//
		playbackParameters->UpdateRT(streamTime, _uiTimer->GetAvgMilli(), audioTime, frontendTime, latency);


		// Design:		There is a long wait period for back and forth with the UI thread from
		//				the audio thread. So, updating won't be very costly, or disruptive to
		//				playback. 
		// 
		//				The shared pointer SynthSettings* is the primary configuration pointer
		//				for the PlaybackDevice* to share with the front end UI. The AudioController*
		//				is on the main thread except for the RTAudio callback - which has a very
		//				high priority compared to the main thread.
		// 
		//				The SynthSettings* pointer is only shared during the locked critical 
		//				sections (see AtomicLock*) for the FromUI and ToUI calls, which force a 
		//				synchronization wait loop for a small period of time using an std::atomic
		//				semaphor.
		//
		// Procedure:
		//
		// 1) Allow one of our UIBase::Tick cycles to process
		// 2) Check for servicing of UI components
		// 3) After service flags have cleared, use UpdateComponent to prepare UI for rendering (and data collection)
		// 4) (CRITICAL SECTION) Process data collection entering the critical section (FromUI)
		// 5) Allow one cycle of the FTXUI loop to tick (after our preparation)
		// 6) Sleep the UI thread to comprise approx 10ms - 15ms (using a interval timer if necessary)
		//
		_uiTimer->Mark();

		// One UIBase* Cycle
		_mainUI->Tick();

		// Pending Action (from Tick())
		if (_mainUI->HasPendingAction())
			_mainUI->ServicePendingAction();

		// These were added to help create UI classes. The stack-oriented rendering
		// architecture of FTXUI is tricky to get to provide an update each call. You
		// basically have to either follow their UI inheritance pattern (closely), or
		// you have to add something to trigger re-rendering!
		//
		_mainUI->UpdateComponent();

		// Update <- UI (If user has made UI changes, they get set in the SynthSettings*) (SHARED!)
		//
		bool uiDirty = _mainUI->GetDirty();

		if (uiDirty)
		{
			// ~ CRITICAL SECTION ~ (Playback must synchronize here!)
			this->PlaybackLock->AcquireLock();

			_mainUI->FromUI(_configuration);

			this->PlaybackLock->Release();

			// ~ END ~
			//
			// Dirty status has been forwarded to the SynthSettings* (see IsDirty)
			//
		}

		// Update -> UI (OutputSettings*) (NOT SHARED) (Member functions all go through this thread for playback parameters)
		_mainUI->ToUI(_configuration);
		_mainUI->ClearDirty();

		// This portion of the loop will run FTXUI, which does not depend on the data
		// pointers shared by other threads.
		//
		// FTXUI: Use custom event to force one UI update
		//
		screen.PostEvent(ftxui::Event::Custom);
		loop.RunOnce();
		
		// Sleep Cycle:  Sleep for the extra time to shoot for our target loop time
		//
		long sleepTime = fmax(fmin(LOOP_PERIOD_MICRO - _uiTimer->PeekMicro(), LOOP_PERIOD_MICRO), 0);

		if (sleepTime > 0.0)
			std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
	}
}
