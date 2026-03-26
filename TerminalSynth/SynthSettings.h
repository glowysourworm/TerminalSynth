#pragma once

#ifndef SYNTH_CONFIGURATION_H
#define SYNTH_CONFIGURATION_H

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

	/// <summary>
	/// Constructor to enable:  Sound Settings (save / open)
	/// </summary>
	SynthSettings();

	/// <summary>
	/// Constructor to enable:  Sound Settings (save / open), Sound Bank (load / wave tables)
	/// </summary>
	SynthSettings(const std::string& soundBankDir);

	/// <summary>
	/// Constructor to enable:  Sound Settings (save / open), Sound Bank (load / wave tables), STK synth voices
	/// </summary>
	SynthSettings(const std::string& soundBankDir, const std::string& stkRawWaveDir);

private:

	SynthSettings(const std::string& soundBankDir, const std::string& stkRawWaveDir, bool soundBankEnabled, bool stkEnabled);

public:

	SynthSettings(const SynthSettings& copy);
	~SynthSettings();

	// Features:  Sound Bank, Sound Settings (voices), STK
	SoundBankSettings* GetSoundBankSettings() const;
	SoundSettings* GetSoundSettings(int index) const;
	SoundSettings* GetDefaultSoundSettings() const;
	SoundSettings* GetCurrentSoundSettings() const;

	// Sound Settings
	int GetSoundSettingsCount() const;
	void GetSoundSettingsList(std::vector<std::string>& destination);

	/// <summary>
	/// Saves sound settings, with selected name, with option to save as a new copy (with a new name).
	/// </summary>
	void SaveSoundSettings(const SoundSettings* soundSettings, const std::string& saveAsName, bool saveAs);

	std::string GetSoundBankDirectory() const;
	std::string GetStkRawWaveDirectory() const;

	bool GetStkEnabled() const;
	bool GetSoundBankEnabled() const;

	void DisableStk();

	// Dirty Status
	bool IsDirty() const;
	void ClearDirty();
	void SetDirty();

	// MIDI Key Code
	WindowsKeyCodes GetKeyCode(int midiNote) const;
	SynthNoteMap GetNoteMap() const;
	bool HasMidiNote(WindowsKeyCodes keyCode) const;
	int GetMidiNote(WindowsKeyCodes keyCode) const;
	void SetMidiNote(WindowsKeyCodes keyCode, int midiNote);

	// Misc Settings
	void SetGain(float value);
	void SetLeftRightBalance(float value);
	void SetMidiLow(int value);
	void SetMidiHigh(int value);
	void SetOversamplingFactor(float value);

	float GetGain() const;
	float GetLeftRightBalance() const;
	int GetMidiLow() const;
	int GetMidiHigh() const;
	float GetOversamplingFactor() const;

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

	std::string* _stkRawWaveDirectory;

	bool _stkEnabled;
	bool _soundBankEnabled;

	// Saved list of sound settings
	std::vector<SoundSettings*>* _soundSettingsList;

	int _midiLow;
	int _midiHigh;

	float _oversamplingFactor;

	// Output Parameters
	float _gain;
	float _leftRightBalance;

	// Tracks changes to the configuration
	bool _isDirty;
};

#endif