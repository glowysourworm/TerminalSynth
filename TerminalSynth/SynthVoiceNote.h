#pragma once

#ifndef SYNTH_VOICE_NOTE_H
#define SYNTH_VOICE_NOTE_H

#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthNoteProcessor.h"

class SynthVoiceNote
{
public:

	/// <summary>
	/// Creates a synth voice; and stores private variables for the parameters.
	/// </summary>
	SynthVoiceNote(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo, float frequency)
	{
		SignalChain filters;
		filters.Initialize(soundRegistry, settings->GetSignalChain(), playbackInfo);

		_parameters = new OscillatorParameters(*settings->GetOscillatorParameters());
		_envelope = new Envelope(*settings->GetOscillatorEnvelope());
		_filters = new SignalChain(filters);
		_noteProcessor = new SynthNoteProcessor(settings, playbackInfo);
		_frequency = frequency;
		_samplingRate = playbackInfo->GetStreamInfo()->streamSampleRate;

		_filters->Initialize(soundRegistry, settings->GetSignalChain(), playbackInfo);
		_noteProcessor->Initialize(playbackInfo);
	}
	~SynthVoiceNote()
	{
	}

	bool HasOutput(const PlaybackTime* playbackTime)
	{
		return _envelope->HasOutput(playbackTime);
	}
	void NoteOn(int midiNumber, const PlaybackTime* playbackTime)
	{
		_noteProcessor->NoteOn(midiNumber, playbackTime);
		_envelope->Engage(playbackTime);

		// Set local frequency (our copy)
		_frequency = _noteProcessor->GetFundamentalFrequency();
	}
	void NoteOff(int midiNumber, const PlaybackTime* playbackTime)
	{
		_envelope->DisEngage(playbackTime);
		_noteProcessor->NoteOff(midiNumber, playbackTime);
	}
	void Clear()
	{

	}

	void Update(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
	{
		_parameters->Update(settings->GetOscillatorParameters());
		_envelope->Update(settings->GetOscillatorEnvelope());
		_filters->Update(soundRegistry, settings->GetSignalChain());
		_noteProcessor->Update(settings);
		_samplingRate = playbackInfo->GetStreamInfo()->streamSampleRate;
	}

	float GetFrequency() const { return _frequency; }
	float GetSignalHigh() const { return _parameters->GetSignalHigh(); }
	float GetSignalLow() const { return _parameters->GetSignalLow(); }
	float GetSamplingRate() const { return _samplingRate; }
	float GetEnvelopeLevel(const PlaybackTime* playbackTime) const { return _envelope->GetEnvelopeLevel(playbackTime); }

private:

	float _frequency;
	float _samplingRate;

	OscillatorParameters* _parameters;
	Envelope* _envelope;
	SignalChain* _filters;
	SynthNoteProcessor* _noteProcessor;
};

#endif
