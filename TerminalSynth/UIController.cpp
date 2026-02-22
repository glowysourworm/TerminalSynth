#include "AtomicLock.h"
#include "BaseController.h"
#include "SoundRegistry.h"
#include "MainUI.h"
#include "OutputSettings.h"
#include "SynthSettings.h"
#include "UIController.h"
#include <chrono>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/color.hpp>
#include <thread>

UIController::UIController(AtomicLock* atomicLock) : BaseController(atomicLock)
{
	_thread = nullptr;
	_mainUI = nullptr;
}

UIController::~UIController()
{
	delete _mainUI;
	delete _thread;
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

	_mainUI = new MainUI("Terminal Synth", ftxui::Color::GreenYellow);
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

	return true;
}

void UIController::FromUI(SynthSettings* configuration)
{
	// std::atomic wait loop
	this->Lock->AcquireLock();

	_mainUI->FromUI(*configuration, true);

	// std::atomic end loop
	this->Lock->Release();
}

void UIController::ToUI(SynthSettings* parameters)
{
	// std::atomic wait loop
	this->Lock->AcquireLock();

	_mainUI->ToUI(*parameters);

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
		// These were added to help create UI classes. The stack-oriented rendering
		// architecture of FTXUI is tricky to get to provide an update each call. You
		// basically have to either follow their UI inheritance pattern (closely), or
		// you have to add something to trigger re-rendering!
		//
		_mainUI->UpdateComponent(false);

		// Use custom event to force one UI update
		screen.PostEvent(ftxui::Event::Custom);
		loop.RunOnce();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
