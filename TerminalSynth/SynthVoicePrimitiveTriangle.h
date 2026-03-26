#pragma once

#ifndef SYNTH_VOICE_PRIMITIVE_TRIANGLE_H
#define SYNTH_VOICE_PRIMITIVE_TRIANGLE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalFactoryCore.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"

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

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		float sample = SignalFactoryCore::GenerateTriangleSample(
			this->GetFrequency(),
			this->GetSamplingRate(),
			this->GetSignalHigh(),
			this->GetSignalLow(),
			playbackTime);

		frame->SetFrame(sample, sample);
	}
};

#endif