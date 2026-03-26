#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_SQUARE_H
#define SYNTH_VOICE_PRIMITIVE_SQUARE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include <cmath>

class SynthVoicePrimitiveSquare : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePrimitiveSquare(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{ }
	~SynthVoicePrimitiveSquare() {};

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		// Using modulo arithmetic to get the relative period time
		float samplingRate = this->GetSamplingRate();
		float period = 1 / this->GetFrequency();
		float periodTime = std::fmod(playbackTime->FromCursor(samplingRate), period);
		float signalHigh = this->GetSignalHigh();
		float signalLow = this->GetSignalLow();
		float sample = 0;

		if (periodTime < period / 2.0)
			sample = signalHigh;
		else
			sample = signalLow;

		frame->SetFrame(sample, sample);
	}
};

#endif