#pragma once

#ifndef SYNTH_CONFIGURATION_H
#define SYNTH_CONFIGURATION_H

#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include "SynthNoteMap.h"
#include "WindowsKeyCodes.h"
#include <string>

class SynthSettings
{
public:

	SynthSettings(const std::string& soundBankDirectory);
	SynthSettings(const SynthSettings& copy);
	~SynthSettings();

	bool IsDirty() const;
	void ClearDirty();
	void SetDirty();

	void SetMidiLow(int value);
	void SetMidiHigh(int value);
	void SetMidiNote(WindowsKeyCodes keyCode, int midiNote);

	void SetOversamplingFactor(float value);

	int GetMidiLow() const;
	int GetMidiHigh() const;

	float GetOversamplingFactor() const;

	SynthNoteMap GetNoteMap() const;
	bool HasMidiNote(WindowsKeyCodes keyCode) const;
	int GetMidiNote(WindowsKeyCodes keyCode) const;
	WindowsKeyCodes GetKeyCode(int midiNote) const;

	SoundBankSettings* GetSoundBankSettings() const;
	SoundSettings* GetSoundSettings() const;

public:

	void IterateKeymap(SynthNoteMap::KeymapIterationCallback callback) const;

private:

	SynthNoteMap* _keyMap;
	SoundBankSettings* _soundBankSettings;
	SoundSettings* _soundSettings;				// Signal Chain, Post Processing, Effect Registry, Oscillator, Envelope

	int _midiLow;
	int _midiHigh;

	float _oversamplingFactor;

	// Tracks changes to the configuration
	bool _isDirty;
};

#endif