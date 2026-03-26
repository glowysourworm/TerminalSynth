#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_SINE_H
#define SYNTH_VOICE_PRIMITIVE_SINE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include "SynthVoiceNote.h"
#include <cmath>
#include <numbers>

class SynthVoicePrimitiveSine : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePrimitiveSine(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{ }
	~SynthVoicePrimitiveSine() {};

protected:

	// Bad Pattern:  This override should not be needed
	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		SynthVoiceDirect::SetFrameImpl(frame, playbackTime);
	}

	float GetSample(const SynthVoiceNote* note, const PlaybackTime* playbackTime) override
	{
		float frequency = note->GetFrequency();
		float signalLow = note->GetSignalLow();
		float signalHigh = note->GetSignalHigh();

		return (0.5f * (signalHigh - signalLow) * std::sinf(2.0 * std::numbers::pi * frequency * playbackTime->FromCursor(note->GetSamplingRate()))) + (0.5f * (signalHigh + signalLow));
	}
};

#endif