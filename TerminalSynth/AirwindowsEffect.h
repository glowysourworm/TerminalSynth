#pragma once

#ifndef AIRWINDOWS_EFFECT_H
#define AIRWINDOWS_EFFECT_H

#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <airwin_consolidated_base.h>
#include <string>

/// <summary>
/// Airwindows Effect Wrapper:  This will hold a plugin from the airwindows-plugins project. Each plugin will follow the
/// FilterBase initialization path; and has already been properly loaded by the application.
/// </summary>
class AirwindowsEffect : public SignalBase
{
public:

	AirwindowsEffect(AudioEffectX* plugin, const std::string& name, const std::string& category, const std::string& whatText);
	~AirwindowsEffect();

	void Initialize(const SignalSettings* configuration, const OutputSettings* parameters) override;
	void SetFrame(PlaybackFrame* frame, float absoluteTime) override;
	bool HasOutput(float absoluteTime) const override;

	void UpdateParameter(int index, float value) override;

	std::string GetCategory() const;
	std::string GetWhatText() const;

private:

	AudioEffectX* _effect;

	std::string* _category;
	std::string* _whatText;

	float** _input;
	float** _output;
};

#endif