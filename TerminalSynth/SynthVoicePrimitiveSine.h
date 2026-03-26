#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_SINE_H
#define SYNTH_VOICE_PRIMITIVE_SINE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include <cmath>
#include <numbers>

class SynthVoicePrimitiveSine : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePrimitiveSine(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{ }
	~SynthVoicePrimitiveSine() {};

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		float frequency = this->GetFrequency();
		float signalLow = this->GetSignalLow();
		float signalHigh = this->GetSignalHigh();

		float sample = (0.5f * (signalHigh - signalLow) * std::sinf(2.0 * std::numbers::pi * frequency * playbackTime->FromCursor(this->GetSamplingRate()))) + (0.5f * (signalHigh + signalLow));

		frame->SetFrame(sample, sample);
	}
};

#endif