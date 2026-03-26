#pragma once

#ifndef SYNTH_NOTE_PROCESSOR_H
#define SYNTH_NOTE_PROCESSOR_H

#include "Constant.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundSettings.h"
#include "SynthNoteParameters.h"
#include <algorithm>
#include <exception>

class SynthNoteProcessor
{
public:

	SynthNoteProcessor()
	{
		_frequency = 0;
		_lastFrequency = 0;
		_portamentoFrequency = 0;
		_lastSetTimeCursor = 0;
		_samplingRate = 0;
		_parameters = new SynthNoteParameters({
			.mode = SynthNoteMode::Normal,
			.chord = ArpeggiatorChord::TriadMajor,
			.arpeggioBPM = 60,
			.portamentoSeconds = 0.5
			});

		_arpeggioCursor = 0;
		_lastBeatTimeCursor = 0;
		_midiNumber = 0;
		_oscillatorParameters = new OscillatorParameters();
	}
	SynthNoteProcessor(const SoundSettings* settings, const PlaybackInfo* playbackInfo)
	{		
		_frequency = 0;
		_lastFrequency = 0;
		_portamentoFrequency = 0;
		_lastSetTimeCursor = 0;
		_samplingRate = playbackInfo->GetStreamInfo()->streamSampleRate;
		_parameters = new SynthNoteParameters({
			.mode = SynthNoteMode::Normal,
			.chord = ArpeggiatorChord::TriadMajor,
			.arpeggioBPM = 60,
			.portamentoSeconds = 0.5
		});

		_arpeggioCursor = 0;
		_lastBeatTimeCursor = 0;
		_midiNumber = 0;
		_oscillatorParameters = new OscillatorParameters(*settings->GetOscillatorParameters());
	}
	SynthNoteProcessor(const SynthNoteProcessor& copy)
	{
		_frequency = copy.GetFrequency();
		_samplingRate = copy.GetSamplingRate();
		_lastFrequency = 0;
		_portamentoFrequency = 0;
		_lastSetTimeCursor = 0;
		_arpeggioCursor = 0;
		_lastBeatTimeCursor = 0;
		_midiNumber = 0;

		_parameters = new SynthNoteParameters(*copy.GetSynthNoteParameters());
		_oscillatorParameters = new OscillatorParameters(*copy.GetOscillatorParameters());
	}
	~SynthNoteProcessor()
	{
		delete _parameters;
		delete _oscillatorParameters;
	}

	void Initialize(const PlaybackInfo* playbackInfo)
	{
		_samplingRate = playbackInfo->GetStreamInfo()->streamSampleRate;
	}

	void NoteOn(int midiNumber, const PlaybackTime* playbackTime)
	{
		_frequency = TerminalSynth::GetMidiFrequency(midiNumber);
		_lastSetTimeCursor = playbackTime->frameCursor;
		_lastBeatTimeCursor = playbackTime->frameCursor;
		_arpeggioCursor = 0;
		_midiNumber = midiNumber;
	}
	void NoteOff(int midiNumber, const PlaybackTime* playbackTime)
	{
		if (_parameters->mode == SynthNoteMode::Portamento)
			_lastFrequency = _frequency;
	}
	void Clear()
	{

	}

	float GetFundamentalFrequency() const
	{
		return TerminalSynth::GetMidiFrequency(_midiNumber);
	}

	float GetFrequency() const
	{
		return CalculateFrequency(_midiNumber);
	}

	float GetNextFrequency(PlaybackFrame* frame, const PlaybackTime* playbackTime)
	{
		switch (_parameters->mode)
		{
		case SynthNoteMode::Normal:
			return _frequency;
		case SynthNoteMode::Arpeggiator:
			return CalculateFrequency(CalculateArpeggioMidiNumber(playbackTime->frameCursor));
		case SynthNoteMode::Portamento:
		{
			// Update Portamento
			float pornamentoSeconds = (playbackTime->frameCursor - _lastSetTimeCursor) / _samplingRate;
			float lerpValue = std::clamp<float>(pornamentoSeconds / _parameters->portamentoSeconds, 0, 1);

			//_portamentoFrequency = std::lerp(_lastFrequency, _frequency, lerpValue);

			_portamentoFrequency = _lastFrequency + ((_frequency - _lastFrequency) * lerpValue);

			return _portamentoFrequency;
		}
		default:
			throw new std::exception("Unhandled Synth Note Mode: SynthNoteProcessor.h");
		}
	}

	void Update(const SoundSettings* settings)
	{
		_oscillatorParameters->Update(settings->GetOscillatorParameters());

		_parameters->arpeggioBPM = settings->GetNoteParameters()->arpeggioBPM;
		_parameters->chord = settings->GetNoteParameters()->chord;
		_parameters->mode = settings->GetNoteParameters()->mode;
		_parameters->portamentoSeconds = settings->GetNoteParameters()->portamentoSeconds;
	}

protected:

	SynthNoteParameters* GetSynthNoteParameters() const { return _parameters; }
	OscillatorParameters* GetOscillatorParameters() const { return _oscillatorParameters; }
	float GetSamplingRate() const { return _samplingRate; }

private:

	float CalculateFrequency(int midiNumber) const
	{
		return TerminalSynth::GetMidiFrequency(midiNumber + (12 * _oscillatorParameters->GetOctave()));
	}

	unsigned int CalculateArpeggioMidiNumber(size_t timeCursor)
	{
		// Arpeggio BPM (Quarter Notes)
		double beatDiv = 1 / (((_parameters->arpeggioBPM / 60.0f) * 4.0f) / _samplingRate);

		if ((timeCursor - _lastBeatTimeCursor) > beatDiv)
		{
			_arpeggioCursor++;
			_lastBeatTimeCursor = timeCursor;

			// Triads
			if ((int)_parameters->chord < 4 && _arpeggioCursor > 3)
				_arpeggioCursor = 1;

			// Tetrads
			else if ((int)_parameters->chord >= 4 && _arpeggioCursor > 4)
				_arpeggioCursor = 1;
		}

		switch (_parameters->chord)
		{
		case ArpeggiatorChord::TriadMajor:
			return (_arpeggioCursor % 3) == 0 ? _midiNumber + 7 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 4 : _midiNumber;
		case ArpeggiatorChord::TriadMinor:
			return (_arpeggioCursor % 3) == 0 ? _midiNumber + 7 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 3 : _midiNumber;
		case ArpeggiatorChord::TriadDiminished:
			return (_arpeggioCursor % 3) == 0 ? _midiNumber + 6 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 3 : _midiNumber;
		case ArpeggiatorChord::TriadAugmented:
			return (_arpeggioCursor % 3) == 0 ? _midiNumber + 8 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 4 : _midiNumber;
		case ArpeggiatorChord::TetradMajor7:
			return (_arpeggioCursor % 4) == 0 ? _midiNumber + 11 :
				   (_arpeggioCursor % 3) == 0 ? _midiNumber + 7 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 4 : _midiNumber;
		case ArpeggiatorChord::TetradMinor7:
			return (_arpeggioCursor % 4) == 0 ? _midiNumber + 10 :
				   (_arpeggioCursor % 3) == 0 ? _midiNumber + 7 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 3 : _midiNumber;
		case ArpeggiatorChord::TetradDominant7:
			return (_arpeggioCursor % 4) == 0 ? _midiNumber + 10 :
				   (_arpeggioCursor % 3) == 0 ? _midiNumber + 7 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 4 : _midiNumber;
		case ArpeggiatorChord::TetradMinor7b5:
			return (_arpeggioCursor % 4) == 0 ? _midiNumber + 10 :
				   (_arpeggioCursor % 3) == 0 ? _midiNumber + 6 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 3 : _midiNumber;
		case ArpeggiatorChord::TetradDiminished7:
			return (_arpeggioCursor % 4) == 0 ? _midiNumber + 9 :
				   (_arpeggioCursor % 3) == 0 ? _midiNumber + 6 :
				   (_arpeggioCursor % 2) == 0 ? _midiNumber + 3 : _midiNumber;
		default:
			throw new std::exception("Unhandled Arpeggio Chord: SynthNoteProcessor.h");
		}
	}

private:

	float _frequency;
	float _lastFrequency;
	float _portamentoFrequency;
	size_t _lastSetTimeCursor;
	size_t _lastBeatTimeCursor;
	unsigned int _arpeggioCursor;
	unsigned int _midiNumber;

	SynthNoteParameters* _parameters;
	OscillatorParameters* _oscillatorParameters;
	float _samplingRate;
};

#endif