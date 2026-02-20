#pragma once

#ifndef SYNTH_EFFECT_H
#define SYNTH_EFFECT_H

#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <string>

class SynthEffect : public SignalBase
{
	SynthEffect(const std::string& name, const SignalSettings& parameters);
	~SynthEffect();

	void Initialize(const SignalSettings* configuration, const OutputSettings* parameters) override;
	void SetFrame(PlaybackFrame* frame, float absoluteTime) override;
	bool HasOutput(float absoluteTime) const override;

private:

	SignalSettings* _parameters;
};
#endif
