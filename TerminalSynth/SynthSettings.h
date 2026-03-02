#pragma once

#ifndef SYNTH_CONFIGURATION_H
#define SYNTH_CONFIGURATION_H

#include "EqualizerOutput.h"
#include "OutputSettings.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include "SynthNoteMap.h"
#include "WindowsKeyCodes.h"
#include <string>

class SynthSettings
{
public:

	SynthSettings(OutputSettings* deviceSettings, const std::string& soundBankDirectory);
	SynthSettings(const SynthSettings& copy);
	~SynthSettings();

	bool IsDirty() const;
	void ClearDirty();
	void SetDirty();

	void SetSoundBankSettings(const SoundBankSettings& parameters);
	void SetSoundSettings(const SoundSettings& settings);
	void SetEqualizerOutput(const EqualizerOutput& value);
	void SetOutputSettings(const OutputSettings& value, bool updateDevicePortion, bool updateRTPortion);

	void SetMidiLow(int value);
	void SetMidiHigh(int value);
	void SetMidiNote(WindowsKeyCodes keyCode, int midiNote);

	void SetOversamplingFactor(float value);

	void SetOutputLeftRight(float value);
	void SetOutputGain(float value);	

	int GetMidiLow() const;
	int GetMidiHigh() const;

	float GetOversamplingFactor() const;

	SynthNoteMap GetNoteMap() const;
	bool HasMidiNote(WindowsKeyCodes keyCode) const;
	int GetMidiNote(WindowsKeyCodes keyCode) const;
	WindowsKeyCodes GetKeyCode(int midiNote) const;

	SoundBankSettings* GetSoundBankSettings() const;
	SoundSettings* GetSoundSettings() const;
	OutputSettings* GetOutputSettings() const;
	EqualizerOutput* GetEqualizerOutput() const;

	float GetOutputLeftRight() const;
	float GetOutputGain() const;

public:

	void IterateKeymap(SynthNoteMap::KeymapIterationCallback callback) const;

private:

	SynthNoteMap* _keyMap;
	SoundBankSettings* _soundBankSettings;
	OutputSettings* _outputSettings;
	EqualizerOutput* _equalizerOutput;

	// Tracks changes to the configuration
	bool _isDirty;

	int _midiLow;
	int _midiHigh;

	float _oversamplingFactor;

	SoundSettings* _soundSettings;				// Signal Chain, Post Processing, Effect Registry, Oscillator, Envelope

	// Output
	float _leftRight;
	float _gain;
};

#endif