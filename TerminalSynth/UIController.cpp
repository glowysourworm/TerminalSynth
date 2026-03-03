#include "AtomicLock.h"
#include "BaseController.h"
#include "MainUI.h"
#include "OutputSettings.h"
#include "SoundRegistry.h"
#include "StopWatch.h"
#include "SynthSettings.h"
#include "UIController.h"
#include <chrono>
#include <cmath>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>

UIController::UIController(AtomicLock* atomicLock) : BaseController(atomicLock)
{
	_thread = nullptr;
	_mainUI = nullptr;
	_stopWatch = new StopWatch();
}

UIController::~UIController()
{
	delete _mainUI;
	delete _thread;
	delete _stopWatch;
}

bool UIController::Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry)
{
	// FTX-UI (Terminal Loop / Renderer)
	// 
	// Their backend will handle interaction, resizing, and redrawing. The document should be
	// updated on this thread (AFAIK) which is captured in the lambda function(s). 
	// 
	// After more working with FTXUI, it is using std::move, and std::make_shared, and std::shared_ptr
	// to try to keep stack allocations, and move them to the heap. I'm not as familiar with this pattern;
	// but it is very difficult to reproduce an inheritance pattern. Though, not impossible, it would
	// be a better use of time to just leave it on our stack, which will accomplish the same task. 
	// 
	// There is a slight memory leak which may be in FTXUI; but I'm not sure, yet, if RT Audio has issues,
	// or just our SynthNote* allocations (probably not).
	//
	// https://arthursonzogni.github.io/FTXUI/
	//

	_mainUI = new MainUI(*configuration);
	_mainUI->Initialize(*configuration);

	return true;
}

bool UIController::Dispose()
{
	_thread->join();
	delete _thread;
	_thread = nullptr;

	return true;
}

void UIController::Start()
{
	_thread = new std::thread(&UIController::ThreadStart, this);
}

bool UIController::IsDirty() const
{
	// std::atomic wait loop
	this->Lock->AcquireLock();

	bool isDirty = _mainUI->GetDirty();

	// std::atomic end loop
	this->Lock->Release();

	return isDirty;
}

void UIController::FromUI(SynthSettings* configuration)
{
	// std::atomic wait loop
	this->Lock->AcquireLock();

	_mainUI->FromUI(configuration);
	_mainUI->ClearDirty();				// Top-down clear

	// std::atomic end loop
	this->Lock->Release();
}

void UIController::ToUI(SynthSettings* parameters)
{
	// std::atomic wait loop
	this->Lock->AcquireLock();

	_mainUI->ToUI(parameters);

	// std::atomic end loop
	this->Lock->Release();
}

void UIController::ThreadStart()
{
	auto screen = ftxui::ScreenInteractive::TerminalOutput();
	auto loop = ftxui::Loop(&screen, _mainUI->GetComponent());

	// FTXUI has an option to create an event loop (this will run their backend UI code)
	//
	// https://arthursonzogni.com/FTXUI/doc/examples_2component_2custom_loop_8cpp-example.html#_a8
	//

	
	while (!loop.HasQuitted())
	{
		// Design:		There is a long wait period for back and forth with the UI thread from
		//				the audio thread. So, updating won't be very costly, or disruptive to
		//				playback. 
		// 
		//				The shared pointer SynthSettings* is the primary configuration pointer
		//				for the PlaybackDevice* to share with the front end UI. The AudioController*
		//				is on the main thread except for the RTAudio callback - which has a very
		//				high priority compared to the UI thread, and the main thread.
		// 
		//				The SynthSettings* pointer is only shared during the locked critical 
		//				sections (see AtomicLock*) for the FromUI and ToUI calls, which enter
		//				the UIController (see above) and force a synchronization wait loop for
		//				a small period of time. 
		// 
		//				** During that critical section, we still have our free UI thread to call
		//				Tick() / UpdateComponent() / etc... to service UI interaction; but the
		//				data won't be collected until the next FromUI, or distributed using ToUI,
		//				which will be from the main thread (once again).
		//
		// Procedure:
		//
		// 1) Allow one of our UIBase::Tick cycles to process
		// 2) Check for servicing of UI components
		// 3) After service flags have cleared, use UpdateComponent (no more than once every 10ms to 15ms)
		// 4) Sleep the UI thread to comprise approx 10ms - 15ms (using a interval timer if necessary)
		// 5) Allow one cycle of the FTXUI loop to tick (after our preparation)

		// Reset our loop stabilizer!
		_stopWatch->mark();

		// One UIBase* Cycle
		_mainUI->Tick();

		// Pending Action (from Tick())
		if (_mainUI->HasPendingAction())
			_mainUI->ServicePendingAction();


		// Sleep Cycle (Try to use approx 10ms of total time in the while loop)
		//
		long currentMicro = (long)_stopWatch->peekMicro();
		long sleepTime = (long)(fmax(fmin(SLEEP_TIME_MICRO, SLEEP_TIME_MICRO - currentMicro), 0.0));
		
		if (sleepTime > 0.0)
			std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));


		// These were added to help create UI classes. The stack-oriented rendering
		// architecture of FTXUI is tricky to get to provide an update each call. You
		// basically have to either follow their UI inheritance pattern (closely), or
		// you have to add something to trigger re-rendering!
		//
		_mainUI->UpdateComponent();

		// Use custom event to force one UI update
		screen.PostEvent(ftxui::Event::Custom);
		loop.RunOnce();


		// Sleep Cycle (Any extra time will be slept here) (it may be beneficial to split 
		//			    this up with the upper thread sleep)
		//
		currentMicro = (long)_stopWatch->peekMicro();
		sleepTime = (long)(fmax(fmin(SLEEP_TIME_MICRO, SLEEP_TIME_MICRO - currentMicro), 0.0));

		if (sleepTime > 0.0)
			std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
	}
}
