#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_SQUARE_H
#define SYNTH_VOICE_PRIMITIVE_SQUARE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalFactoryCore.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include "SynthVoiceNote.h"

class SynthVoicePrimitiveSquare : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePrimitiveSquare(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{ }
	~SynthVoicePrimitiveSquare() {};

protected:

	// Bad Pattern:  This override should not be needed
	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		SynthVoiceDirect::SetFrameImpl(frame, playbackTime);
	}

	float GetSample(const SynthVoiceNote* note, const PlaybackTime* playbackTime) override
	{
		float sample = SignalFactoryCore::GenerateSquareSample(
			note->GetFrequency(),
			note->GetSamplingRate(),
			note->GetSignalHigh(),
			note->GetSignalLow(),
			playbackTime);

		return note->GetEnvelopeLevel(playbackTime) * sample;
	}
};

#endif