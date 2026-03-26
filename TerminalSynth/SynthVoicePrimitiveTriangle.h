#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_TRIANGLE_H
#define SYNTH_VOICE_PRIMITIVE_TRIANGLE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include "SynthVoiceNote.h"
#include <cmath>

class SynthVoicePrimitiveTriangle : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePrimitiveTriangle(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo) 
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{};
	~SynthVoicePrimitiveTriangle() {};

protected:

	// Bad Pattern:  This override should not be needed
	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		SynthVoiceDirect::SetFrameImpl(frame, playbackTime);
	}

	float GetSample(const SynthVoiceNote* note, const PlaybackTime* playbackTime) override
	{
		float period = 1 / note->GetFrequency();
		float periodQuarter = 0.25f * period;
		float high = note->GetSignalHigh();
		float low = note->GetSignalLow();
		float sample = 0;

		// Using modulo arithmetic to get the relative period time
		float periodTime = std::fmod(playbackTime->FromCursor(note->GetSamplingRate()), period);

		// First Quadrant
		if (periodTime < periodQuarter)
		{
			sample = (2.0 * (high - low) / period) * periodTime;
		}

		// Second Quadrant
		else if (periodTime < (2.0 * periodQuarter))
		{
			sample = ((-2.0 * (high - low) / period) * (periodTime - (float)periodQuarter)) - low;
		}

		// Third Quadrant
		else if (periodTime < 3.0 * periodQuarter)
		{
			sample = (-2.0 * (high - low) / period) * (periodTime - (2.0 * periodQuarter));
		}

		// Fourth Quadrant
		else
		{
			sample = ((2.0 * (high - low) / period) * (periodTime - (3.0 * periodQuarter))) + low;
		}

		return sample;
	}
};

#endif