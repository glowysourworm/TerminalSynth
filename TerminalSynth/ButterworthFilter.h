#pragma once
#define BUDDA_Q_SCALE 6.f

#ifndef BUTTERWORTH_FILTER_H
#define BUTTERWORTH_FILTER_H

#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"

class ButterworthFilter : public SignalBase
{
public:

	ButterworthFilter(int samplingRate, float gain);
	~ButterworthFilter() override;

	void Initialize(const SignalSettings* configuration, const OutputSettings* parameters) override;
	void SetFrame(PlaybackFrame* frame, float absoluteTime) override;
	bool HasOutput(float absoluteTime) const override;

	void SetFilter(float cutoff, float resonance);

private:

	float Apply(float sample, float absoluteTime);

private:

	float outputGain;								// Second gain applied depending on cutoff / resonance
	float t0, t1, t2, t3;
	float coef0, coef1, coef2, coef3;
	float history1, history2, history3, history4;
	float min_cutoff, max_cutoff;
};
#endif