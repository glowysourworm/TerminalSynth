#pragma once

#ifndef SYNTH_PLAYBACK_DEVICE_H
#define SYNTH_PLAYBACK_DEVICE_H

#include "Accumulator.h"
#include "Constant.h"
#include "OutputSettings.h"
#include "PlaybackDevice.h"
#include "PlaybackFormatTransformer.h"
#include "PlaybackFormatTransformer.h"
#include "PlaybackFrame.h"
#include "SoundRegistry.h"
#include "Synth.h"
#include "SynthSettings.h"
#include "WindowsKeyCodes.h"
#include <Windows.h>
#include <exception>

class SynthPlaybackDevice : public PlaybackDevice
{
public:

	SynthPlaybackDevice();
	~SynthPlaybackDevice();

	bool Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters) override;
	bool Update(SoundRegistry* effectRegistry, const SynthSettings* configuration) override;
	bool SetForPlayback(unsigned int numberOfFrames, double streamTime, const SynthSettings* configuration) override;

	int WritePlaybackBuffer(
		void* playbackBuffer,
		AudioStreamFormat streamFormat,
		unsigned int numberOfFrames, 
		double streamTime, 
		const OutputSettings* outputSettings) override;

	bool GetLastOutput() const override;

	/// <summary>
	/// Returns average output for specified channel from the last frame buffer write
	/// </summary>
	float GetOutputLeft() const;
	float GetOutputRight() const;

private:

	PlaybackFrame* _frame;
	Synth* _synth;
	unsigned int _numberOfChannels;
	unsigned int _samplingRate;
	bool _lastOutput;
	bool _initialized;
};


SynthPlaybackDevice::SynthPlaybackDevice()
{
	_lastOutput = false;
	_numberOfChannels = 0;
	_samplingRate = 0;
	_frame = nullptr;
	_synth = nullptr;
	_initialized = false;
}

SynthPlaybackDevice::~SynthPlaybackDevice()
{
	delete _synth;
	delete _frame;
}

bool SynthPlaybackDevice::Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters)
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

bool SynthPlaybackDevice::Update(SoundRegistry* effectRegistry, const SynthSettings* configuration)
{
	_synth->Update(effectRegistry, configuration->GetDefaultSoundSettings());

	return true;
}

bool SynthPlaybackDevice::SetForPlayback(unsigned int numberOfFrames, double streamTime, const SynthSettings* configuration)
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
		_synth->Set(midiNote, isPressed, streamTime);

		pressedKeys |= isPressed;
	}

	if (!pressedKeys)
	{
		// Check outdated synth note cache (prune for configuration changes)
		_synth->PruneNotePool();
	}

	return pressedKeys;
}

int SynthPlaybackDevice::WritePlaybackBuffer(void* playbackBuffer, AudioStreamFormat streamFormat, unsigned int numberOfFrames, double streamTime, const OutputSettings* outputSettings)
{
	if (!_initialized)
		return -1;

	char* outputBuffer = (char*)playbackBuffer;

	// Transform buffers
	int frameSize = 4;
	char leftBuffer[4];
	char rightBuffer[4];

	// Calculate frame data (BUFFER SIZE = NUMBER OF CHANNELS x NUMBER OF FRAMES)
	for (unsigned int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
	{
		double absoluteTime = streamTime + (frameIndex / (double)_samplingRate);

		// Clear Frame
		_frame->Clear();

		// Get Samples for N channels
		_lastOutput = _synth->GetSample(_frame, absoluteTime, outputSettings);

		// Equalizer Output
		outputSettings->GetEqualizer()->AddSample(_frame->GetLeft(), _frame->GetRight());

		// TRANSFORM STREAM:  The byte stream must match the output format
		PlaybackFormatTransformer::Transform(streamFormat, _frame->GetLeft(), leftBuffer, frameSize);
		PlaybackFormatTransformer::Transform(streamFormat, _frame->GetRight(), rightBuffer, frameSize);

		// Write Transformed Buffer
		for (int index = 0; index < frameSize; index++)
		{
			// Interleved frames
			outputBuffer[(2 * frameIndex * frameSize) + index] = leftBuffer[index];
			outputBuffer[(2 * frameIndex * frameSize) + frameSize + index] = rightBuffer[index];
		}
	}

	return 0;
}

float SynthPlaybackDevice::GetOutputLeft() const
{
	return _frame->GetLeft();
}

float SynthPlaybackDevice::GetOutputRight() const
{
	return _frame->GetRight();
}

bool SynthPlaybackDevice::GetLastOutput() const
{
	return _lastOutput;
}

#endif