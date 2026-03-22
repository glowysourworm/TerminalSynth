#pragma once

#ifndef SYNTH_VOICE_WAVE_TABLE_H
#define SYNTH_VOICE_WAVE_TABLE_H

#include "PlaybackFrame.h"
#include "PlaybackTime.h"
#include "SynthVoiceBase.h"

class SynthVoiceWaveTable : public SynthVoiceBase
{
	virtual bool HasOutput(const PlaybackTime* playbackTime) const = 0;
	virtual void Engage(const PlaybackTime* playbackTime) = 0;
	virtual void DisEngage(const PlaybackTime* playbackTime) = 0;
	virtual void Clear() = 0;

protected:

	virtual void SetFrameImpl(PlaybackFrame* frame) = 0;
};

#endif