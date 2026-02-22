#pragma once

#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "AtomicLock.h"
#include "BaseController.h"
#include "MainUI.h"
#include "OutputSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <thread>

class UIController : public BaseController
{
public:

	UIController(AtomicLock* atomicLock);
	~UIController();

	bool Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry) override;
	bool Dispose() override;
	void Start() override;

public:

	/// <summary>
	/// (Shared Main Thread / UI Thread) Returns true if there is a dirty status on the UI. This means that data needs to be
	/// collected during this pass.
	/// </summary>
	bool IsDirty() const;

	/// <summary>
	/// (Shared Main Thread / UI Thread) Takes data from the UI and updates the configuration
	/// </summary>
	void FromUI(SynthSettings* configuration);

	/// <summary>
	/// (Shared Main Thread / UI Thread) Pushes data from the playback parameters to the UI
	/// </summary>
	void ToUI(SynthSettings* configuration);

private:

	void ThreadStart();

private:

	MainUI* _mainUI;

	// This was needed to accomodate the FTXUI stack-based API (ftxui::Screen does not have a way to create a private pointer)
	std::thread* _thread;
};

#endif