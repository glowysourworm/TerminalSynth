#pragma once

#ifndef SYNTH_VOICE_BASE_H
#define SYNTH_VOICE_BASE_H

#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalBase.h"
#include "SignalChain.h"
#include "SignalParameterizedBase.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthNoteProcessor.h"

class SynthVoiceBase : public SignalParameterizedBase
{
public:

	/// <summary>
	/// Creates a synth voice; and stores private variables for the parameters.
	/// </summary>
	SynthVoiceBase(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo) 
		: SignalParameterizedBase(*settings->GetSynthVoiceSettings())
	{
		SignalChain filters;
		filters.Initialize(soundRegistry, settings->GetSignalChain(), playbackInfo);

		_parameters = new OscillatorParameters(*settings->GetOscillatorParameters());
		_envelope = new Envelope(*settings->GetOscillatorEnvelope());
		_filters = new SignalChain(filters);
		_noteProcessor = new SynthNoteProcessor(settings, playbackInfo);
		_currentFrequency = 100;
	}
	~SynthVoiceBase()
	{
		delete _parameters;
		delete _envelope;
		delete _filters;
		delete _noteProcessor;
	}

	virtual bool HasOutput(const PlaybackTime* playbackTime) const
	{
		return _envelope->HasOutput(playbackTime);
	}
	virtual void NoteOn(int midiNumber, const PlaybackTime* playbackTime)
	{
		SignalBase::Engage(playbackTime);
		
		_noteProcessor->NoteOn(midiNumber, playbackTime);
		_envelope->Engage(playbackTime);

		// Set local frequency (our copy)
		_currentFrequency = _noteProcessor->GetFundamentalFrequency();
	}
	virtual void NoteOff(int midiNumber, const PlaybackTime* playbackTime)
	{
		SignalBase::DisEngage(playbackTime);

		_envelope->DisEngage(playbackTime);
		_noteProcessor->NoteOff(midiNumber, playbackTime);
	}
	virtual void Clear()
	{

	}

	virtual void Update(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
	{
		_parameters->Update(settings->GetOscillatorParameters());
		_envelope->Update(settings->GetOscillatorEnvelope());
		_filters->Update(soundRegistry, settings->GetSignalChain());
		_noteProcessor->Update(settings);
	}

public:

	OscillatorParameters* GetOscillatorParameters() const { return _parameters; }
	Envelope* GetEnvelope() const { return _envelope; }
	SignalChain* GetFilters() const { return _filters; }
	SynthNoteProcessor* GetNoteProcessor() const { return _noteProcessor; }

protected:

	virtual void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) = 0;

	// Fundamental frequency of current note being played
	float GetCurrentFrequency() const { return _currentFrequency; }

private:

	float _currentFrequency;

	OscillatorParameters* _parameters;
	Envelope* _envelope;
	SignalChain* _filters;
	SynthNoteProcessor* _noteProcessor;
};

#endif