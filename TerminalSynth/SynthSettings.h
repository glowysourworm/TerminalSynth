#pragma once

#ifndef SYNTH_CONFIGURATION_H
#define SYNTH_CONFIGURATION_H

#include "EqualizerOutput.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "SignalChainSettings.h"
#include "SynthNoteMap.h"
#include "WindowsKeyCodes.h"

class SynthSettings
{
public:

	SynthSettings(OutputSettings* deviceSettings);
	SynthSettings(const SynthSettings& copy);
	~SynthSettings();

	bool IsDirty() const;

	void ClearDirty();

	void SetOscillator(const OscillatorParameters& value);
	void SetSignalChain(const SignalChainSettings& elements);
	void SetEqualizerOutput(const EqualizerOutput& value);
	void SetOutputSettings(const OutputSettings& value, bool updateDevicePortion, bool updateRTPortion);

	void SetMidiLow(int value);
	void SetMidiHigh(int value);

	void SetMidiNote(WindowsKeyCodes keyCode, int midiNote);

	void SetOutputLeftRight(float value);
	void SetOutputGain(float value);

	int GetMidiLow() const;
	int GetMidiHigh() const;

	SynthNoteMap GetNoteMap() const;
	bool HasMidiNote(WindowsKeyCodes keyCode) const;
	int GetMidiNote(WindowsKeyCodes keyCode) const;
	WindowsKeyCodes GetKeyCode(int midiNote) const;

	OscillatorParameters* GetOscillator() const;
	SignalChainSettings* GetSignalChainRegistry() const;
	OutputSettings* GetOutputSettings() const;
	EqualizerOutput* GetEqualizerOutput() const;

	float GetOutputLeftRight() const;
	float GetOutputGain() const;

public:

	void IterateKeymap(SynthNoteMap::KeymapIterationCallback callback) const;

private:

	SynthNoteMap* _keyMap;

	OutputSettings* _outputSettings;
	EqualizerOutput* _equalizerOutput;

	// Tracks changes to the configuration
	bool _isDirty;

	int _midiLow;
	int _midiHigh;

	OscillatorParameters* _oscillatorParameters;
	SignalChainSettings* _signalChainRegistry;				// Signal Chain (with registry loaded!)

	// Output
	float _leftRight;
	float _gain;
};

#endif