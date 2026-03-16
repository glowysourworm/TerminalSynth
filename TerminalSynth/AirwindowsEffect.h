#pragma once

#ifndef AIRWINDOWS_EFFECT_H
#define AIRWINDOWS_EFFECT_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <airwin_consolidated_base.h>

/// <summary>
/// Airwindows Effect Wrapper:  This will hold a plugin from the airwindows-plugins project. Each plugin will follow the
/// FilterBase initialization path; and has already been properly loaded by the application.
/// </summary>
class AirwindowsEffect : public SignalBase
{
public:

	AirwindowsEffect(const SignalSettings& settings, AudioEffectX* plugin);
	~AirwindowsEffect();

	void Initialize(const PlaybackInfo* outputSettings) override;	
	bool HasOutput() const override;

	void UpdateParameter(int index, float value) override;

protected:

	void SetFrameImpl(PlaybackFrame* frame) override;

private:

	AudioEffectX* _effect;

	float** _input;
	float** _output;
};

#endif