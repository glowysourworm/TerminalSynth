#pragma once

#ifndef SYNTH_CONFIGURATION_H
#define SYNTH_CONFIGURATION_H

#include "SignalChainSettings.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include "SynthNoteMap.h"
#include "WindowsKeyCodes.h"
#include <istream>
#include <ostream>
#include <string>
#include <vector>

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

	void SaveCurrentSoundSettings(const std::string& name);

	void SetOversamplingFactor(float value);

	SignalChainSettings* GetEffectRegistry() const;
	
	int GetSoundSettingsCount() const;
	void GetSoundSettingsList(std::vector<std::string>& destination);
	SoundSettings* GetSoundSettings(int index) const;

	int GetMidiLow() const;
	int GetMidiHigh() const;

	float GetOversamplingFactor() const;

	SynthNoteMap GetNoteMap() const;
	bool HasMidiNote(WindowsKeyCodes keyCode) const;
	int GetMidiNote(WindowsKeyCodes keyCode) const;
	WindowsKeyCodes GetKeyCode(int midiNote) const;

	SoundBankSettings* GetSoundBankSettings() const;
	SoundSettings* GetDefaultSoundSettings() const;
	SoundSettings* GetCurrentSoundSettings() const;

public:

	void Save(std::ostream& stream);
	void Read(std::istream& stream);

public:

	void IterateKeymap(SynthNoteMap::KeymapIterationCallback callback) const;

private:

	SynthNoteMap* _keyMap;
	SoundBankSettings* _soundBankSettings;
	SoundSettings* _defaultSoundSettings;										// Signal Chain, Post Processing, Oscillator, Envelope
	SoundSettings* _currentSoundSettings;										// Signal Chain, Post Processing, Oscillator, Envelope
	
	// Saved list of sound settings
	std::vector<SoundSettings*>* _soundSettingsList;

	// Effect Registry:  Complete list of effects from the effect registry
	SignalChainSettings* _effectRegistry;

	int _midiLow;
	int _midiHigh;

	float _oversamplingFactor;

	// Tracks changes to the configuration
	bool _isDirty;
};

#endif