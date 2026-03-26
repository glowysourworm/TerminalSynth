#pragma once

#ifndef SYNTH_PLAYBACK_DEVICE_H
#define SYNTH_PLAYBACK_DEVICE_H

#include "PlaybackDevice.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "Synth.h"
#include "SynthSettings.h"
#include "ValueCapture.h"
#include "WindowsKeyCodes.h"
#include <Windows.h>
#include <exception>
#include <functional>
#include <map>
#include <utility>

class SynthPlaybackDevice : public PlaybackDevice
{
public:

	SynthPlaybackDevice();
	~SynthPlaybackDevice();

	bool Initialize(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters) override;
	bool Update(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters) override;
	bool SetForFrame(const PlaybackTime& playbackTime, const SynthSettings* configuration) override;
	bool WriteSample(PlaybackFrame& playbackFrame, const PlaybackTime& playbackTime, float gain, float leftRightBalance) override;

private:

	using KeyCodeIterator = std::function<void(WindowsKeyCodes keyCode, int midiNumber, bool isPressed)>;

	void IterateKeyCodes(const SynthSettings* configuration, const KeyCodeIterator& callback);

private:

	Synth* _synth;
	unsigned int _numberOfChannels;
	unsigned int _samplingRate;
	bool _lastOutput;
	bool _initialized;

	std::map<int, ValueCapture<bool>*>* _keyCaptures;
};


SynthPlaybackDevice::SynthPlaybackDevice()
{
	_lastOutput = false;
	_numberOfChannels = 0;
	_samplingRate = 0;
	_synth = nullptr;
	_initialized = false;

	_keyCaptures = new std::map<int, ValueCapture<bool>*>();
}

SynthPlaybackDevice::~SynthPlaybackDevice()
{
	delete _synth;

	for (auto iter = _keyCaptures->begin(); iter != _keyCaptures->end(); ++iter)
	{
		delete iter->second;
	}

	delete _keyCaptures;
}

bool SynthPlaybackDevice::Initialize(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters)
{
	_numberOfChannels = parameters->GetStreamInfo()->streamChannels;
	_samplingRate = parameters->GetStreamInfo()->streamSampleRate;

	_synth = new Synth(configuration, _numberOfChannels, _samplingRate);
	_keyCaptures = new std::map<int, ValueCapture<bool>*>();

	// Key Captures
	auto keyCaptures = _keyCaptures;

	// Key Captures -> Initialize
	IterateKeyCodes(configuration, [&keyCaptures](WindowsKeyCodes keyCode, int midiNumber, bool isPressed) {

		keyCaptures->insert(std::make_pair((int)keyCode, new ValueCapture<bool>(false)));
	});

	// Update synth configuration
	_synth->Initialize(effectRegistry, configuration, parameters);

	_initialized = true;

	return _initialized;
}

bool SynthPlaybackDevice::Update(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters)
{
	_synth->Update(effectRegistry, configuration->GetCurrentSoundSettings(), parameters);

	return true;
}

bool SynthPlaybackDevice::SetForFrame(const PlaybackTime& playbackTime, const SynthSettings* configuration)
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	bool pressedKeys = false;

	// Iterate Key Codes (cross platform solutions will just make the platform. The windows specific ones need to be wrapped anyway.)
	//
	auto keyCaptures = _keyCaptures;
	auto synth = _synth;

	IterateKeyCodes(configuration, [&keyCaptures, &synth, &playbackTime, &pressedKeys](WindowsKeyCodes keyCode, int midiNumber, bool isPressed) {

		// Set (if already cleared)
		if (!keyCaptures->at((int)keyCode)->HasChanged())
			 keyCaptures->at((int)keyCode)->SetValue(isPressed);

		// Engage / Dis-Engage
		if (keyCaptures->at((int)keyCode)->HasChanged())
		{
			// Set Note (on / off)
			synth->SetNote(midiNumber, keyCaptures->at((int)keyCode)->GetValue(), &playbackTime);

			// Reset Capture
			keyCaptures->at((int)keyCode)->Clear();
		}

		pressedKeys |= isPressed;
	});

	return pressedKeys;
}

void SynthPlaybackDevice::IterateKeyCodes(const SynthSettings* configuration, const KeyCodeIterator& callback)
{
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

		// Check Configuration
		if (!configuration->HasMidiNote((WindowsKeyCodes)keyCode))
			continue;

		// Midi Note (from the key map)
		int midiNumber = configuration->GetMidiNote((WindowsKeyCodes)keyCode);

		// Pressed
		bool isPressed = GetAsyncKeyState(keyCode) & 0x8000;

		// Callback
		callback((WindowsKeyCodes)keyCode, midiNumber, isPressed);
	}
}

bool SynthPlaybackDevice::WriteSample(PlaybackFrame& playbackFrame, const PlaybackTime& playbackTime, float gain, float leftRightBalance)
{
	if (!_initialized)
		throw new std::exception("Trying to use SynthPlaybackDevice before initializing:  SynthPlaybackDevice.h");

	// Get Samples for N channels
	return _synth->GetSample(&playbackFrame, &playbackTime, gain, leftRightBalance);
}

#endif