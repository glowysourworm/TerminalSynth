#pragma once

#ifndef SYNTH_VOICE_BASE_H
#define SYNTH_VOICE_BASE_H

#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalBase.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"

class SynthVoiceBase : public SignalBase
{
public:

	/// <summary>
	/// Creates a synth voice; and stores private variables for the parameters.
	/// </summary>
	SynthVoiceBase(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo) : SignalBase(settings->GetName())
	{
		SignalChain filters;
		filters.Initialize(soundRegistry, settings->GetSignalChain(), playbackInfo);

		_parameters = new OscillatorParameters(*settings->GetOscillatorParameters());
		_envelope = new Envelope(*settings->GetOscillatorEnvelope());
		_filters = new SignalChain(filters);

		_midiNumber = 60;
		_frequency = TerminalSynth::GetMidiFrequency(_midiNumber);
	}
	~SynthVoiceBase()
	{
		delete _parameters;
		delete _envelope;
		delete _filters;
	}

	virtual bool HasOutput(double absoluteTime) const
	{
		return _envelope->HasOutput(absoluteTime);
	}
	virtual void NoteOn(int midiNumber, double absoluteTime)
	{
		SignalBase::Engage(absoluteTime);

		_midiNumber = (_parameters->GetOctave() * 12) + midiNumber;
		_frequency = TerminalSynth::GetMidiFrequency(_midiNumber);
		_envelope->Engage(absoluteTime);
	}
	virtual void NoteOff(int midiNumber, double absoluteTime)
	{
		SignalBase::DisEngage(absoluteTime);

		_envelope->DisEngage(absoluteTime);
	}
	virtual void Clear()
	{

	}

	virtual void Update(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
	{
		_parameters->Update(settings->GetOscillatorParameters());
		_envelope->Update(settings->GetOscillatorEnvelope());
		_filters->Update(soundRegistry, settings->GetSignalChain());
	}

public:

	OscillatorParameters* GetOscillatorParameters() { return _parameters; }
	Envelope* GetEnvelope() { return _envelope; }
	SignalChain* GetFilters() { return _filters; }

protected:

	virtual void SetFrameImpl(PlaybackFrame* frame) = 0;

	float GetFrequency() const { return _frequency; }
	unsigned int GetMidiNumber() const { return _midiNumber; }

private:

	OscillatorParameters* _parameters;
	Envelope* _envelope;
	SignalChain* _filters;

	unsigned int _midiNumber;
	float _frequency;

};

#endif