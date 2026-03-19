#pragma once

#ifndef SYNTH_VOICE_DIRECT_H
#define SYNTH_VOICE_DIRECT_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalFactoryCore.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"

class SynthVoiceDirect : public SynthVoiceBase
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoiceDirect(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo);
	~SynthVoiceDirect();

	void Initialize(const PlaybackInfo* playbackInfo) override;

protected:

	void SetFrameImpl(PlaybackFrame* frame) override;

private:

	SignalFactoryCore* _core;
};

#endif