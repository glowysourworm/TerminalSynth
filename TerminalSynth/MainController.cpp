#include "AtomicLock.h"
#include "AudioController.h"
#include "BaseController.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MainController.h"
#include "MainModelUI.h"
#include "MainUI.h"
#include "OutputSettings.h"
#include "PlaybackController.h"
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
#include <string>
#include <thread>
#include <vector>

MainController::MainController(AudioController* audioController, AtomicLock* playbackLock) : BaseController(playbackLock)
{
	_audioController = audioController;
	_playbackController = new PlaybackController(playbackLock);

	_configuration = nullptr;
	_mainUI = nullptr;
	_mainModelUI = nullptr;
	_uiTimer = new LoopTimer(0.075);
	_uiDataFetchTimer = new IntervalTimer();
	_uiLockAcquireTimer = new IntervalTimer();
	_uiRenderTimer = new IntervalTimer();
	_uiSleepTimer = new IntervalTimer();	
	_userData = new RtAudioUserData();
}

MainController::~MainController()
{
	delete _configuration;
	delete _mainUI;
	delete _mainModelUI;
	delete _uiTimer;
	delete _uiDataFetchTimer;
	delete _uiLockAcquireTimer;
	delete _uiRenderTimer;
	delete _uiSleepTimer;
	delete _playbackController;
	delete _userData;	
}

/// <summary>
/// Initialization function for the synth backend. This must be called before starting the player!
/// </summary>
bool MainController::Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry)
{
	_configuration = configuration;

	// RT / PORT AUDIO
	bool success = _audioController->Initialize(
		configuration, 
		parameters, 
		effectRegistry, 
		std::bind(&PlaybackController::ProcessAudioCallback,
			_playbackController,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
			std::placeholders::_5,
			std::placeholders::_6));

	// RT AUDIO -> Open Stream (SynthSettings*)(PlaybackParameteres*) (INITIALIZE!)
	success &= _audioController->OpenStream((void*)_userData);

	// Synth Settings Effect Registry:  This will separate out the SignalBase* (which will not be present in the SynthSettings*) (linking issue)
	//
	std::vector<SignalSettings> registryList;

	// Airwindows Plugins:  Require sampling rate!
	success &= effectRegistry->Initialize(parameters, registryList);

	// -> Initialize(..) (completes the configuration's registry)
	configuration->GetEffectRegistry()->Initialize(registryList);

	// -> Initilize(..) (the RT Audio playback can now be unblocked)
	_userData->Initialize(configuration, effectRegistry, parameters);

	// Audio Controller (for callback)
	success &= _playbackController->Initialize(configuration, parameters, effectRegistry);

	// Main UI Initialize
	_mainModelUI = new MainModelUI(configuration, parameters);
	_mainUI = new MainUI(*_mainModelUI);
	_mainUI->Initialize(*_mainModelUI);

	return success;
}

void MainController::Start()
{
	_playbackController->Start();
	_audioController->StartStream();

	//...
	this->Loop();
}

bool MainController::Dispose()
{
	// Stops UI thread
	return _playbackController->Dispose() && _audioController->Dispose();
}

void MainController::Loop()
{
	auto screen = ftxui::ScreenInteractive::TerminalOutput();
	auto loop = ftxui::Loop(&screen, _mainUI->GetComponent());
	
	// Store for detecting device change!
	std::string currentDevice = _mainModelUI->GetOutputModelUI()->GetOutputSettings()->GetDeviceName();

	// FTXUI has an option to create an event loop (this will run their backend UI code)
	//
	// https://arthursonzogni.com/FTXUI/doc/examples_2component_2custom_loop_8cpp-example.html#_a8
	//

	// Primary Loop!!! 
	//
	// Audio Thread:  The RT Audio callback will arrive on their background thread from the audio backend. We
	//				  just have to manage two shared pointers:  OutputSettings*, and SynthSettings*.
	//				  The SynthPlaybackDevice* will be updated, from the synth configuration, during the 
	//				  callback. So, this must be synchronized somehow. Probably a std::atomic which could be
	//				  shared inside the SynthSettings*
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
		// Design:		There is a long wait period for back and forth with the UI thread from
		//				the audio thread. So, updating won't be very costly, or disruptive to
		//				playback. 
		// 
		//				The shared pointer RtAudioUserData* may be synchronized through the 
		//				AtomicLock* (see below, and on AudioController* playback)
		// 
		//				The settings have been copied off to the UI portion and can be used
		//				outside of the lock. The settings are propagated using the UIBase* 
		//				component design, which is detailed in the procedure below. 
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
		_uiTimer->Reset();

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
			_uiLockAcquireTimer->Reset();
			this->PlaybackLock->AcquireLock();
			_uiLockAcquireTimer->Mark();

			_uiDataFetchTimer->Reset();

			// UI -> Model -> Configuration
			_mainUI->FromUI(_mainModelUI);
			_mainModelUI->FromUI(_configuration);	

			// Also, need the Gain and L/R Balance
			_userData->GetOutputSettings()->SetGain(_mainModelUI->GetOutputModelUI()->GetOutputSettings()->GetGain());
			_userData->GetOutputSettings()->SetLeftRightBalance(_mainModelUI->GetOutputModelUI()->GetOutputSettings()->GetLeftRightBalance());

			_uiDataFetchTimer->Mark();

			// -> Forward dirty status to AudioController
			_configuration->SetDirty();

			this->PlaybackLock->Release();

			// ~ END ~
			//
			// Dirty status has been forwarded to the SynthSettings* (see IsDirty)
			//
		}

		// DEVICE CHANGE! (OutputSettings* was set for a new device)
		if (_mainModelUI->GetOutputModelUI()->GetOutputSettings()->GetDeviceName() != currentDevice)
		{
			// Stop / Re-Start Audio Stream
			currentDevice = _mainModelUI->GetOutputModelUI()->GetOutputSettings()->GetDeviceName();

			if (_audioController->IsStreamRunning())
				_audioController->StopStream();

			if (_audioController->IsStreamOpen())
				_audioController->CloseStream();

			// Starts new stream with currently selected device (OutputSettings*)
			_audioController->OpenStream(_userData);
			_audioController->StartStream();
		}

		// *** NON-THERAD-SAFE *** (This should be safe outside of the lock for getting / setting simple data)
		_userData->GetOutputSettings()->UpdateRT_UI(
			_uiTimer->GetAvgMilli(),
			_uiDataFetchTimer->AvgMicro(),
			_uiLockAcquireTimer->AvgNano(),
			_uiRenderTimer->AvgMilli(),
			_uiSleepTimer->AvgMilli());

		_mainModelUI->ToUI(_userData->GetSynthSettings(), _userData->GetOutputSettings());
		// ***********************
		
		_mainUI->ToUI(_mainModelUI);
		_mainUI->ClearDirty();

		// This portion of the loop will run FTXUI, which does not depend on the data
		// pointers shared by other threads.
		//
		// FTXUI: Use custom event to force one UI update
		//
		_uiRenderTimer->Reset();

		// Force a refresh (there could be a way to optimize this for CPU usage)
		screen.PostEvent(ftxui::Event::Custom);
		loop.RunOnce();

		_uiRenderTimer->Mark();
		
		// Sleep Cycle:  Sleep for the extra time to shoot for our target loop time
		//
		long sleepTime = fmax(fmin(LOOP_PERIOD_MICRO - _uiTimer->PeekMicro(), LOOP_PERIOD_MICRO), 0);

		_uiSleepTimer->Reset();

		if (sleepTime > 0.0)
			std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));

		_uiSleepTimer->Mark();

		_uiTimer->Mark();
	}
}
