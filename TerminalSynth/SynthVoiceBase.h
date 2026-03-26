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
	SynthVoiceBase(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo) 
		: SignalParameterizedBase(*settings->GetSynthVoiceSettings())
	{
		SignalChain filters;
		filters.Initialize(soundRegistry, settings->GetSignalChain(), playbackInfo);

		_samplingRate = playbackInfo->GetStreamInfo()->streamSampleRate;
		_oscillatorParameters = new OscillatorParameters(*settings->GetOscillatorParameters());
		_envelope = new Envelope(*settings->GetOscillatorEnvelope());
		_filters = new SignalChain(filters);
		_noteProcessor = new SynthNoteProcessor(settings, playbackInfo);

		_filters->Initialize(soundRegistry, settings->GetSignalChain(), playbackInfo);
		_noteProcessor->Initialize(playbackInfo);
	}
	~SynthVoiceBase()
	{
		delete _oscillatorParameters;
		delete _envelope;
		delete _filters;
		delete _noteProcessor;
	}

	virtual bool HasOutput(const PlaybackTime* playbackTime) const override
	{
		return _envelope->HasOutput(playbackTime);
	}

	// Override to update parameters (for parameter automation)
	virtual void UpdateParameter(int index, float value) override
	{
		
	}

	virtual void NoteOn(int midiNumber, const PlaybackTime* playbackTime)
	{
		SignalBase::Engage(playbackTime);

		_noteProcessor->NoteOn(midiNumber, playbackTime);
		_envelope->Engage(playbackTime);
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
		_samplingRate = playbackInfo->GetStreamInfo()->streamSampleRate;
		_oscillatorParameters->Update(settings->GetOscillatorParameters());
		_envelope->Update(settings->GetOscillatorEnvelope());
		_filters->Update(soundRegistry, settings->GetSignalChain());
		_noteProcessor->Update(settings);
	}

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) = 0;

	float GetOutputLevel(const PlaybackTime* playbackTime) override
	{
		return _envelope->GetEnvelopeLevel(playbackTime);
	}

	float GetSamplingRate() const { return _samplingRate; }
	float GetFrequency() const { return _noteProcessor->GetFundamentalFrequency(); }
	float GetSignalHigh() const { return _oscillatorParameters->GetSignalHigh(); }
	float GetSignalLow() const { return _oscillatorParameters->GetSignalLow(); }

private:

	float _samplingRate;

	OscillatorParameters* _oscillatorParameters;
	Envelope* _envelope;
	SignalChain* _filters;
	SynthNoteProcessor* _noteProcessor;
};

#endif