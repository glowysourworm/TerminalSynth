#pragma once

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "AtomicLock.h"
#include "AudioController.h"
#include "BaseController.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MainModelUI.h"
#include "MainUI.h"
#include "PlaybackController.h"
#include "PlaybackUserData.h"

class MainController : public BaseController
{
public:

	MainController(AudioController* audioController, AtomicLock* playbackLock);
	~MainController();

	bool Initialize(PlaybackUserData* playbackData) override;
	bool Dispose() override;

	/// <summary>
	/// Runs until the application should exit (this could be put on a separate thread.. if it helps CPU load.. 
	/// MSFT may have some loading difference with a background thread)
	/// </summary>
	void Start() override;

private:

	void Loop();

private:

	const long LOOP_PERIOD_MICRO = 15000;

	AudioController* _audioController;
	PlaybackController* _playbackController;

	PlaybackUserData* _userData;
	
	LoopTimer* _uiTimer;
	IntervalTimer* _uiDataFetchTimer;
	IntervalTimer* _uiLockAcquireTimer;
	IntervalTimer* _uiRenderTimer;
	IntervalTimer* _uiSleepTimer;

	MainUI* _mainUI;
	MainModelUI* _mainModelUI;
};

#endif