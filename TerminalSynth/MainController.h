#pragma once

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "AtomicLock.h"
#include "AudioController.h"
#include "BaseController.h"
#include "EffectRegistry.h"
#include "LoopTimer.h"
#include "OutputSettings.h"
#include "RtAudioUserData.h"
#include "SynthSettings.h"
#include "UIController.h"

class MainController : public BaseController
{
public:

	MainController(AtomicLock* atomicLock);
	~MainController();

	bool Initialize(SynthSettings* configuration, OutputSettings* parameters, EffectRegistry* effectRegistry) override;
	bool Dispose() override;

	/// <summary>
	/// Runs until the application should exit (this could be put on a separate thread.. if it helps CPU load.. 
	/// MSFT may have some loading difference with a background thread)
	/// </summary>
	void Start() override;

private:

	void Loop();

private:

	RtAudioUserData* _userData;

	// Primary Owner of SynthSettings*
	SynthSettings* _configuration;

	AudioController* _audioController;
	UIController* _uiController;

	LoopTimer* _uiTimer;
};

#endif