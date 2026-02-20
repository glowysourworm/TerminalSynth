#pragma once

#ifndef COMBFILTER_H
#define COMBFILTER_H

#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <queue>

/// <summary>
/// Parameter 0:  Delay	   (s)
/// Parameter 1:  Gain
/// Parameter 2:  Feedback (bool)
/// </summary>
class CombFilter : public SignalBase
{
public:

	// Initializes a feed-back comb filter
	CombFilter(float delaySeconds, float gain, bool feedback);
	~CombFilter();

	void Initialize(const SignalSettings* settings, const OutputSettings* parameters) override;
	void SetFrame(PlaybackFrame* frame, float absoluteTime) override;
	bool HasOutput(float absoluteTime) const override;

private:

	std::queue<float>* _bufferL;
	std::queue<float>* _bufferR;
};

#endif