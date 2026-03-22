#pragma once

#ifndef BUTTERWORTH_FILTER_H
#define BUTTERWORTH_FILTER_H
#define BUDDA_Q_SCALE 6.f

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalParameterizedBase.h"

class ButterworthFilter : public SignalParameterizedBase
{
public:

	ButterworthFilter(int samplingRate, float gain);
	~ButterworthFilter();

	void Initialize(const PlaybackInfo* parameters) override;
	bool HasOutput(const PlaybackTime* playbackTime) const override;

	void SetFilter(float cutoff, float resonance);

	void UpdateParameter(int index, float value) override {};

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override;

private:

	float Apply(float sample);

private:

	float outputGain;								// Second gain applied depending on cutoff / resonance
	float t0, t1, t2, t3;
	float coef0, coef1, coef2, coef3;
	float history1, history2, history3, history4;
	float min_cutoff, max_cutoff;
};
#endif