#pragma once

#ifndef SYNTH_VOICE_PLUCKED_STRING_H
#define SYNTH_VOICE_PLUCKED_STRING_H

#include "BiQuadFilter.h"
#include "CombFilter.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include "SynthVoiceNote.h"
#include <cstdlib>

/// <summary>
/// Plucked String:  This is an implementation of the Karplus-Strong algorithm with two parameters.
/// Parameter 0:  Cutoff Frequency
/// Parameter 1:  Resonance
/// </summary>
class SynthVoicePluckedString : public SynthVoiceDirect
{
public:

	/// <summary>
	/// Creates a synth voice (for direct waveform output); and stores private variables for the parameters.
	/// </summary>
	SynthVoicePluckedString(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
		: SynthVoiceDirect(soundRegistry, settings, playbackInfo)
	{
		this->AddParameter("Cutoff", 100, 10000, 440);
		this->AddParameter("Resonance", 0.0f, 1.0f, 0.3f);
		
		// Create Filters
		_lowPassFilter = new BiQuadFilter(BiQuadFilter::FilterType::LPF, playbackInfo->GetStreamInfo()->streamSampleRate, 440, 0.3);
		_combFilter = new CombFilter(0.05f, 1.0f, false);
	}
	~SynthVoicePluckedString()
	{
		delete _lowPassFilter;
		delete _combFilter;
	}

protected:

	// Bad Pattern:  This override should not be needed
	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		SynthVoiceDirect::SetFrameImpl(frame, playbackTime);
	}

	float GetSample(const SynthVoiceNote* note, const PlaybackTime* playbackTime) override
	{
		// https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis
		//
		// There should be a short burst of noise that dissipates. The filtering should be a simple delay plus a LPF realized
		// using our BiQuad + CombFilter.
		//
		
		// Pluck Time
		float attackTime = 0.001;
		float sample = 0;
		
		// Noise Attack:  The delay + filter will make the line dissipate
		if (playbackTime->FromCursor(note->GetSamplingRate()) < attackTime)
		{
			sample = ((float)rand() / (float)RAND_MAX);
		}
		else
		{
			sample = 0;
		}
		
		// Process Sample
		PlaybackFrame frame(sample, sample);
		_combFilter->SetFrame(&frame, playbackTime);
		_lowPassFilter->SetFrame(&frame, playbackTime);

		return frame.GetLeft();
	}

private:

	BiQuadFilter* _lowPassFilter;
	CombFilter* _combFilter;
};

#endif