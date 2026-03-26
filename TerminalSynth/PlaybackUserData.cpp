#include "EqualizerOutput.h"
#include "PlaybackDeviceRegister.h"
#include "PlaybackInfo.h"
#include "PlaybackUserData.h"
#include "SignalSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <exception>
#include <string>
#include <vector>

PlaybackUserData::PlaybackUserData(SynthSettings* synthSettings)
{
	_synthSettings = synthSettings;
	_effectRegistry = new SoundRegistry();
	_playbackInfo = new PlaybackInfo(synthSettings->GetStkEnabled(), synthSettings->GetSoundBankEnabled());	
	_deviceRegister = new PlaybackDeviceRegister();
	_equalizer = new EqualizerOutput(FFT_INPUT_SIZE, FFT_OUTPUT_SIZE);
	_effectList = new std::vector<SignalSettings*>();

	_initialized = false;
}
PlaybackUserData::~PlaybackUserData()
{
	for (int index = 0; index < _effectList->size(); index++)
	{
		// MEMORY!  ~SignalSettings
		delete _effectList->at(index);
	}

	delete _effectList;
	delete _deviceRegister;
	delete _equalizer;
	delete _playbackInfo;
	delete _equalizer;
}
bool PlaybackUserData::Initialize()
{
	if (_initialized)
		throw new std::exception("Trying to re-initialized PlaybackUserData* after it has been initialized");

	std::vector<SignalSettings> effectList;

	bool success = _effectRegistry->Initialize(_playbackInfo, effectList);

	for (int index = 0; index < effectList.size(); index++)
	{
		// MEMORY! ~PlaybackUserData
		_effectList->push_back(new SignalSettings(effectList[index]));
	}

	_initialized = true;

	return success;
}
void PlaybackUserData::UpdateDevice(const std::string& selectedDeviceName, unsigned int samplingRate, unsigned int outputBufferSize, bool selectDevice)
{
	_deviceRegister->UpdateDevice(selectedDeviceName, samplingRate, outputBufferSize, selectDevice);

	_playbackInfo->SetForOutputDevice(
		_deviceRegister->GetDeviceFormat(), 
		_deviceRegister->GetNumberOfChannels(), 
		_deviceRegister->GetSamplingRate(), 
		_deviceRegister->GetSuggestedLatencySeconds());
}

void PlaybackUserData::SelectDevice(const std::string& deviceName)
{
	_deviceRegister->SelectDevice(deviceName);
}
