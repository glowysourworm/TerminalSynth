#include "Constant.h"
#include "Envelope.h"
#include "EqualizerOutput.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "SignalChainSettings.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include "SynthNoteMap.h"
#include "SynthSettings.h"
#include "WindowsKeyCodes.h"
#include <string>

SynthSettings::SynthSettings(OutputSettings* deviceSettings, const std::string& soundBankDirectory)
{
	_keyMap = new SynthNoteMap();
	_isDirty = false;

	_soundSettings = new SoundSettings();
	_outputSettings = deviceSettings;
	_equalizerOutput = new EqualizerOutput();	
	_soundBankSettings = new SoundBankSettings(soundBankDirectory);		// May fail during a try / catch. Settings will be empty, but useable.

	_midiLow = MIDI_PIANO_LOW_NUMBER;
	_midiHigh = MIDI_PIANO_HIGH_NUMBER;

	_oversamplingFactor = 1.0;

	_leftRight = 0.5f;
	_gain = 1.0f;
}
SynthSettings::SynthSettings(const SynthSettings& copy)
{
	if (_keyMap != nullptr)
		delete _keyMap;

	delete _soundBankSettings;
	delete _soundSettings;
	delete _outputSettings;
	delete _equalizerOutput;

	_soundBankSettings = new SoundBankSettings(*copy.GetSoundBankSettings());

	_outputSettings = copy.GetOutputSettings();
	_equalizerOutput = copy.GetEqualizerOutput();

	_midiLow = copy.GetMidiLow();
	_midiHigh = copy.GetMidiHigh();
	
	_soundSettings = new SoundSettings(*copy.GetSoundSettings());

	_keyMap = new SynthNoteMap(copy.GetNoteMap());

	_leftRight = copy.GetOutputLeftRight();
	_gain = copy.GetOutputGain();
}
SynthSettings::~SynthSettings()
{
	if (_keyMap != nullptr)
		delete _keyMap;

	delete _soundSettings;
	delete _outputSettings;
	delete _equalizerOutput;
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

void SynthSettings::SetSoundBankSettings(const SoundBankSettings& parameters)
{
	_soundBankSettings = new SoundBankSettings(parameters);
}

void SynthSettings::SetSoundSettings(const SoundSettings& settings)
{
	_isDirty |= _soundSettings->Update(settings);
}
void SynthSettings::SetEqualizerOutput(const EqualizerOutput& value)
{
	// RT Playback
	_equalizerOutput->left = value.left;
	_equalizerOutput->right = value.right;
}
void SynthSettings::SetOutputSettings(const OutputSettings& value, bool updateDevicePortion, bool updateRTPortion)
{
	// Initialize
	if (updateDevicePortion)
	{
		_outputSettings->UpdateDevice(
			value.GetHostApi(),
			value.GetDeviceFormat(),
			value.GetDeviceName(),
			value.GetSamplingRate(),
			value.GetNumberOfChannels(),
			value.GetOutputBufferFrameSize());
	}

	// RT Playback
	if (updateRTPortion)
	{
		_outputSettings->UpdateRT(
			value.GetStreamTime(),
			value.GetAvgUIMilli(),
			value.GetAvgAudioMilli(),
			value.GetAvgFrontendMilli(),
			value.GetStreamLatency());
	}
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

OutputSettings* SynthSettings::GetOutputSettings() const
{
	return _outputSettings;
}

EqualizerOutput* SynthSettings::GetEqualizerOutput() const
{
	return _equalizerOutput;
}

float SynthSettings::GetOutputLeftRight() const
{
	return _leftRight;
}
float SynthSettings::GetOutputGain() const
{
	return _gain;
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
}
void SynthSettings::SetOversamplingFactor(float value)
{
	_oversamplingFactor = value;
}
void SynthSettings::SetOutputLeftRight(float value)
{
	if (_leftRight != value)
		_isDirty = true;

	_leftRight = value;
}
void SynthSettings::SetOutputGain(float value)
{
	if (_gain != value)
		_isDirty = true;

	_gain = value;
}

