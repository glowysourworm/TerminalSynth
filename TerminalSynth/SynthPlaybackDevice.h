#pragma once

#ifndef SYNTH_PLAYBACK_DEVICE_H
#define SYNTH_PLAYBACK_DEVICE_H

#include "Accumulator.h"
#include "EffectRegistry.h"
#include "OutputSettings.h"
#include "PlaybackBuffer.h"
#include "PlaybackDevice.h"
#include "PlaybackFrame.h"
#include "Synth.h"
#include "SynthSettings.h"
#include "WindowsKeyCodes.h"
#include <Windows.h>
#include <exception>

template<SignalValue TSignal>
class SynthPlaybackDevice : public PlaybackDevice<TSignal>
{
public:

	SynthPlaybackDevice();
	~SynthPlaybackDevice();

	bool Initialize(const EffectRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters) override;
	bool Update(const EffectRegistry* effectRegistry, const SynthSettings* configuration) override;
	bool SetForPlayback(unsigned int numberOfFrames, double streamTime, const SynthSettings* configuration) override;

	int WritePlaybackBuffer(
		TSignal* playbackBuffer,
		unsigned int numberOfFrames, 
		double streamTime, 
		const SynthSettings* configuration) override;

	bool GetLastOutput() const override;

	/// <summary>
	/// Returns average output for specified channel from the last frame buffer write
	/// </summary>
	TSignal GetOutputLeft() const;
	TSignal GetOutputRight() const;

private:

	PlaybackFrame* _frame;
	Synth* _synth;
	Accumulator<TSignal>* _outputLeft;
	Accumulator<TSignal>* _outputRight;
	unsigned int _numberOfChannels;
	unsigned int _samplingRate;
	bool _lastOutput;
	bool _initialized;
};


template<SignalValue TSignal>
SynthPlaybackDevice<TSignal>::SynthPlaybackDevice()
{
	_lastOutput = false;
	_numberOfChannels = 0;
	_samplingRate = 0;
	_frame = nullptr;
	_synth = nullptr;
	_initialized = false;
	_outputLeft = new Accumulator<TSignal>(true);
	_outputRight = new Accumulator<TSignal>(true);
}

template<SignalValue TSignal>
SynthPlaybackDevice<TSignal>::~SynthPlaybackDevice()
{
	delete _synth;
	delete _frame;
	delete _outputLeft;
	delete _outputRight;
}

template<SignalValue TSignal>
bool SynthPlaybackDevice<TSignal>::Initialize(const EffectRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters)
{
	_numberOfChannels = parameters->GetNumberOfChannels();
	_samplingRate = parameters->GetSamplingRate();

	_synth = new Synth(configuration, _numberOfChannels, _samplingRate);
	_frame = new PlaybackFrame();

	// Update synth configuration
	_synth->Initialize(effectRegistry, configuration, parameters);

	_initialized = true;

	return _initialized;
}

template<SignalValue TSignal>
bool SynthPlaybackDevice<TSignal>::Update(const EffectRegistry* effectRegistry, const SynthSettings* configuration)
{
	_synth->Update(effectRegistry, configuration);

	return true;
}

template<SignalValue TSignal>
bool SynthPlaybackDevice<TSignal>::SetForPlayback(unsigned int numberOfFrames, double streamTime, const SynthSettings* configuration)
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	bool pressedKeys = false;

	// Iterate Key Codes (probably the most direct method)
	//
	for (int keyCode = (int)WindowsKeyCodes::NUMBER_0; keyCode <= (int)WindowsKeyCodes::PERIOD; keyCode++)
	{
		// Check that enum is defined
		if (keyCode < 0x30 ||
			keyCode == 0x40 ||
			(keyCode > 0x5A && keyCode < 0x80) ||
			(keyCode > 0x80 && keyCode < 0xBB) ||
			(keyCode > 0xBF && keyCode < 0xDB) ||
			(keyCode > 0xDE))
			continue;

		if (!configuration->HasMidiNote((WindowsKeyCodes)keyCode))
			continue;

		// Pressed
		bool isPressed = GetAsyncKeyState(keyCode) & 0x8000;

		// Midi Note
		int midiNote = configuration->GetMidiNote((WindowsKeyCodes)keyCode);

		// Engage / Dis-Engage
		_synth->Set(midiNote, isPressed, streamTime, configuration);

		pressedKeys |= isPressed;
	}

	return pressedKeys;
}

template<SignalValue TSignal>
int SynthPlaybackDevice<TSignal>::WritePlaybackBuffer(TSignal* playbackBuffer, unsigned int numberOfFrames, double streamTime, const SynthSettings* configuration)
{
	if (!_initialized)
		return -1;

	TSignal* outputBuffer = (TSignal*)playbackBuffer;

	// Calculate frame data (BUFFER SIZE = NUMBER OF CHANNELS x NUMBER OF FRAMES)
	for (unsigned int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
	{
		double absoluteTime = streamTime + (frameIndex / (double)_samplingRate);

		// Clear Frame
		_frame->Clear();

		// Get Samples for N channels
		_lastOutput = _synth->GetSample(_frame, absoluteTime, configuration);

		// Interleved frames
		outputBuffer[(2 * frameIndex)] = _frame->GetLeft();
		outputBuffer[(2 * frameIndex) + 1] = _frame->GetRight();

		// Keep accumulator value for output UI
		_outputLeft->Add(_frame->GetLeft());
		_outputRight->Add(_frame->GetRight());
	}

	return 0;
}

template<SignalValue TSignal>
TSignal SynthPlaybackDevice<TSignal>::GetOutputLeft() const
{
	return _outputLeft->GetAvg();
}

template<SignalValue TSignal>
inline TSignal SynthPlaybackDevice<TSignal>::GetOutputRight() const
{
	return _outputRight->GetAvg();
}

template<SignalValue TSignal>
bool SynthPlaybackDevice<TSignal>::GetLastOutput() const
{
	return _lastOutput;
}

#endif