#include "Constant.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include "SynthNoteMap.h"
#include "SynthSettings.h"
#include "WindowsKeyCodes.h"
#include <string>

SynthSettings::SynthSettings(const std::string& soundBankDirectory)
{
	_keyMap = new SynthNoteMap();
	_soundSettings = new SoundSettings();
	_soundBankSettings = new SoundBankSettings(soundBankDirectory);		// May fail during a try / catch. Settings will be empty, but useable.

	_midiLow = MIDI_PIANO_LOW_NUMBER;
	_midiHigh = MIDI_PIANO_HIGH_NUMBER;

	_oversamplingFactor = 1.0;

	_isDirty = false;
}
SynthSettings::SynthSettings(const SynthSettings& copy)
{
	_keyMap = new SynthNoteMap(copy.GetNoteMap());
	_soundBankSettings = new SoundBankSettings(*copy.GetSoundBankSettings());
	_soundSettings = new SoundSettings(*copy.GetSoundSettings());

	_midiLow = copy.GetMidiLow();
	_midiHigh = copy.GetMidiHigh();
	
	_oversamplingFactor = copy.GetOversamplingFactor();
	_isDirty = false;
}
SynthSettings::~SynthSettings()
{
	delete _keyMap;
	delete _soundSettings;
	delete _soundBankSettings;
}
bool SynthSettings::IsDirty() const
{
	return _isDirty;
}
void SynthSettings::ClearDirty()
{
	_isDirty = false;
}

void SynthSettings::SetDirty()
{
	// Some change to a nested object caused dirty status
	_isDirty = true;		
}
SynthNoteMap SynthSettings::GetNoteMap() const
{
	return *_keyMap;
}
SoundBankSettings* SynthSettings::GetSoundBankSettings() const
{
	return _soundBankSettings;
}
SoundSettings* SynthSettings::GetSoundSettings() const
{
	return _soundSettings;
}
void SynthSettings::IterateKeymap(SynthNoteMap::KeymapIterationCallback callback) const
{
	_keyMap->Iterate(callback);
}
int SynthSettings::GetMidiLow() const
{
	return _midiLow;
}
int SynthSettings::GetMidiHigh() const
{
	return _midiHigh;
}

float SynthSettings::GetOversamplingFactor() const
{
	return _oversamplingFactor;
}

bool SynthSettings::HasMidiNote(WindowsKeyCodes keyCode) const
{
	return _keyMap->HasMidiNote(keyCode);
}

int SynthSettings::GetMidiNote(WindowsKeyCodes keyCode) const
{
	return _keyMap->GetMidiNote(keyCode);
}

WindowsKeyCodes SynthSettings::GetKeyCode(int midiNote) const
{
	return _keyMap->GetKeyCode(midiNote);
}

void SynthSettings::SetMidiLow(int value)
{
	if (_midiLow != value)
		_isDirty = true;

	_midiLow = value;
}
void SynthSettings::SetMidiHigh(int value)
{
	if (_midiHigh != value)
		_isDirty = true;

	_midiHigh = value;
}
void SynthSettings::SetMidiNote(WindowsKeyCodes keyCode, int midiNote)
{
	_keyMap->Add(keyCode, midiNote);
	_isDirty = true;
}
void SynthSettings::SetOversamplingFactor(float value)
{
	_oversamplingFactor = value;
	_isDirty = true;
}

