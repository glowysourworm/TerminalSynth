#pragma once

#ifndef SYNTH_VOICE_DIRECT_H
#define SYNTH_VOICE_DIRECT_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"

class SynthVoiceDirect : public SynthVoiceBase
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoiceDirect(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceBase(soundRegistry, settings, playbackInfo)
	{}
	~SynthVoiceDirect() {};

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) = 0;
};

#endif