#pragma once

#ifndef SYNTH_VOICE_STK_BEE_THREE_H
#define SYNTH_VOICE_STK_BEE_THREE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include <BeeThree.h>

class SynthVoiceStkBeeThree : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoiceStkBeeThree(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{
		_voice = new stk::BeeThree();

		_voice->clear();
		_voice->setGain(0, 1);
		_voice->setGain(1, 1);
		_voice->setGain(2, 1);
		_voice->setGain(3, 1);
		_voice->setModulationDepth(0.05);
		_voice->setModulationSpeed(6.0);
	}
	~SynthVoiceStkBeeThree() 
	{
		delete _voice;
	};

	void NoteOn(float frequency, const PlaybackTime* playbackTime) override
	{
		SynthVoiceDirect::NoteOn(frequency, playbackTime);

		_voice->noteOn(frequency, 1);
	}

	void NoteOff(float frequency, const PlaybackTime* playbackTime) override
	{
		SynthVoiceDirect::NoteOff(frequency, playbackTime);

		_voice->noteOff(1);
	}

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		float sample = _voice->tick();

		frame->SetFrame(sample, sample);
	}

private:

	stk::BeeThree* _voice;
};

#endif