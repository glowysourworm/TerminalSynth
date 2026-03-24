#include "Constant.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include "SynthNoteMap.h"
#include "SynthSettings.h"
#include "WindowsKeyCodes.h"
#include <istream>
#include <ostream>
#include <string>
#include <vector>

SynthSettings::SynthSettings(const std::string& soundSettingsDir)
	: SynthSettings(soundSettingsDir, "", "", false, false)
{ }

SynthSettings::SynthSettings(const std::string& soundSettingsDir, const std::string& soundBankDir)
	: SynthSettings(soundSettingsDir, soundBankDir, "", true, false)
{}

SynthSettings::SynthSettings(const std::string& soundSettingsDir, const std::string& soundBankDir, const std::string& stkRawWaveDir)
	: SynthSettings(soundSettingsDir, soundBankDir, stkRawWaveDir, true, true)
{}

SynthSettings::SynthSettings(const std::string& soundSettingsDir, const std::string& soundBankDir, const std::string& stkRawWaveDir, bool soundBankEnabled, bool stkEnabled)
{
	_keyMap = new SynthNoteMap();
	_defaultSoundSettings = new SoundSettings("Default");
	_currentSoundSettings = _defaultSoundSettings;						// DO NOT DELETE!
	_soundBankSettings = new SoundBankSettings(soundBankDir);			// May fail during a try / catch. Settings will be empty, but useable.
	_soundSettingsDirectory = new std::string(soundSettingsDir);
	_stkRawWaveDirectory = new std::string(stkRawWaveDir);

	_soundBankEnabled = soundBankEnabled;
	_stkEnabled = stkEnabled;

	_soundSettingsList = new std::vector<SoundSettings*>();

	_midiLow = MIDI_PIANO_LOW_NUMBER;
	_midiHigh = MIDI_PIANO_HIGH_NUMBER;
	_gain = 1.0f;
	_leftRightBalance = 0.5f;

	_oversamplingFactor = 1.0;

	_isDirty = false;
}
SynthSettings::SynthSettings(const SynthSettings& copy)
{
	_keyMap = new SynthNoteMap(copy.GetNoteMap());
	_soundBankSettings = new SoundBankSettings(*copy.GetSoundBankSettings());
	_defaultSoundSettings = new SoundSettings(*copy.GetDefaultSoundSettings());
	_currentSoundSettings = copy.GetCurrentSoundSettings();							// DO NOT DELETE!

	_soundSettingsList = new std::vector<SoundSettings*>();
	for (int index = 0; index < copy.GetSoundSettingsCount(); index++)
	{
		// MEMORY! ~SynthSettings, ~SoundSettings
		_soundSettingsList->push_back(new SoundSettings(*copy.GetSoundSettings(index)));
	}

	_soundSettingsDirectory = new std::string(copy.GetSoundSettingsDirectory());
	_stkRawWaveDirectory = new std::string(copy.GetStkRawWaveDirectory());

	_soundBankEnabled = copy.GetSoundBankEnabled();
	_stkEnabled = copy.GetStkEnabled();

	_midiLow = copy.GetMidiLow();
	_midiHigh = copy.GetMidiHigh();
	
	_oversamplingFactor = copy.GetOversamplingFactor();
	_isDirty = false;
}
SynthSettings::~SynthSettings()
{
	for (int index = 0; index < _soundSettingsList->size(); index++)
	{
		// MEMORY! ~SoundSettings
		delete _soundSettingsList->at(index);
	}

	delete _keyMap;
	delete _defaultSoundSettings;
	delete _soundSettingsList;
	delete _soundBankSettings;

	delete _soundSettingsDirectory;
	delete _stkRawWaveDirectory;
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
SoundSettings* SynthSettings::GetDefaultSoundSettings() const
{
	return _defaultSoundSettings;
}
SoundSettings* SynthSettings::GetCurrentSoundSettings() const
{
	return _currentSoundSettings;
}
std::string SynthSettings::GetSoundSettingsDirectory() const
{
	return *_soundSettingsDirectory;
}
std::string SynthSettings::GetSoundBankDirectory() const
{
	return _soundBankSettings->GetSoundBankDirectory();
}
std::string SynthSettings::GetStkRawWaveDirectory() const
{
	return *_stkRawWaveDirectory;
}
bool SynthSettings::GetStkEnabled() const
{
	return _stkEnabled;
}
bool SynthSettings::GetSoundBankEnabled() const
{
	return _soundBankEnabled;
}
void SynthSettings::Save(std::ostream& stream)
{
	// Save:  KeyMap, User Sound Settings

	// Key Map
	_keyMap->Save(stream);
	
	// User List Count
	stream << _soundSettingsList->size();

	// User List
	for (int index = 0; index < _soundSettingsList->size(); index++)
	{
		_soundSettingsList->at(index)->Save(stream);
	}
}
void SynthSettings::Read(std::istream& stream)
{
	if (_keyMap != nullptr)
		delete _keyMap;

	for (int index = 0; index < _soundSettingsList->size(); index++)
	{
		delete _soundSettingsList->at(index);
	}

	_soundSettingsList->clear();

	// Read:  KeyMap, User Sound Settings
	size_t listSize = 0;

	// Key Map
	SynthNoteMap keyMap;
	keyMap.Read(stream);

	_keyMap = new SynthNoteMap(keyMap);

	// User List Count
	stream >> listSize;

	// User List
	for (int index = 0; index < listSize; index++)
	{
		SoundSettings settings;
		settings.Read(stream);

		_soundSettingsList->push_back(new SoundSettings(settings));
	}
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

float SynthSettings::GetGain() const
{
	return _gain;
}

float SynthSettings::GetLeftRightBalance() const
{
	return _leftRightBalance;
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

void SynthSettings::SetGain(float value)
{
	if (_gain != value)
		_isDirty = true;

	_gain = value;
}
void SynthSettings::SetLeftRightBalance(float value)
{
	if (_leftRightBalance != value)
		_isDirty = true;

	_leftRightBalance = value;
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
void SynthSettings::SaveCurrentSoundSettings(const std::string& name)
{
	SoundSettings* userSettings = new SoundSettings(name);

	// Copy from current settings
	userSettings->Update(_currentSoundSettings);

	// Store these and save to file (also)
	_soundSettingsList->push_back(userSettings);

	_isDirty = true;
}
void SynthSettings::SetOversamplingFactor(float value)
{
	_oversamplingFactor = value;
	_isDirty = true;
}

int SynthSettings::GetSoundSettingsCount() const
{
	return _soundSettingsList->size();
}

SoundSettings* SynthSettings::GetSoundSettings(int index) const
{
	return _soundSettingsList->at(index);
}

void SynthSettings::GetSoundSettingsList(std::vector<std::string>& destination)
{
	for (int index = 0; index < _soundSettingsList->size(); index++)
	{
		destination.push_back(_soundSettingsList->at(index)->GetName());
	}
}

