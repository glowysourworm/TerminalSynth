#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_SAWTOOTH_H
#define SYNTH_VOICE_PRIMITIVE_SAWTOOTH_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include <cmath>

class SynthVoicePrimitiveSawtooth : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePrimitiveSawtooth(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{ }
	~SynthVoicePrimitiveSawtooth() {};

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		// Using modulo arithmetic to get the relative period time
		float period = 1 / this->GetFrequency();
		float periodTime = std::fmod(playbackTime->FromCursor(this->GetSamplingRate()), period);
		float signalLow = this->GetSignalLow();
		float signalHigh = this->GetSignalHigh();

		float sample = (((signalHigh - signalLow) / period) * periodTime) + signalLow;

		frame->SetFrame(sample, sample);
	}
};

#endif