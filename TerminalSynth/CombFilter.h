#pragma once

#ifndef COMBFILTER_H
#define COMBFILTER_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalParameterizedBase.h"
#include <queue>

/// <summary>
/// Parameter 0:  Delay	   (s)
/// Parameter 1:  Gain
/// Parameter 2:  Feedback (bool)
/// </summary>
class CombFilter : public SignalParameterizedBase
{
public:

	// Initializes a feed-back comb filter
	CombFilter(float delaySeconds, float gain, bool feedback);
	~CombFilter();

	void Initialize(const PlaybackInfo* parameters) override;
	bool HasOutput(const PlaybackTime* playbackTime) const override;

	void UpdateParameter(int index, float parameterValue) override {};		// Doesn't support parameter automation

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override;

private:

	std::queue<float>* _bufferL;
	std::queue<float>* _bufferR;

	// Needed pre-initialization
	float _delaySeconds; 
	float _gain; 
	bool _feedback;
};

#endif