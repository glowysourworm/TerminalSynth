#pragma once

#ifndef SYNTH_EFFECT_H
#define SYNTH_EFFECT_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <string>

class SynthEffect : public SignalBase
{
public:

	SynthEffect(const std::string& name, const SignalSettings& parameters);
	~SynthEffect();

	void Initialize(const PlaybackInfo* parameters) override;
	bool HasOutput() const override;

	void UpdateParameter(int index, float value) override;

protected:

	void SetFrameImpl(PlaybackFrame* frame) override;

private:
};
#endif
