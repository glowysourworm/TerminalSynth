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

	void NoteOn(int midiNumber, const PlaybackTime* playbackTime) override
	{
		SynthVoiceBase::NoteOn(midiNumber, playbackTime);
	}

	void NoteOff(int midiNumber, const PlaybackTime* playbackTime)
	{
		SynthVoiceBase::NoteOff(midiNumber, playbackTime);
	}

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		SynthVoiceBase::SetFrameImpl(frame, playbackTime);
	}

	// Override to create a class with parameter automation
	virtual void UpdateParameter(int index, float value)
	{
		
	}

	// Override to handle synth voice envelopes
	virtual bool HasOutput(const PlaybackTime* playbackTime) const
	{
		return true;
	};
};

#endif