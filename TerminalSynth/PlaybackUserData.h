#ifndef RT_AUDIO_USER_DATA_H
#define RT_AUDIO_USER_DATA_H

#include "EqualizerOutput.h"
#include "PlaybackDeviceRegister.h"
#include "PlaybackInfo.h"
#include "SignalSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include "SynthSettingsLoader.h"
#include <string>
#include <vector>

class PlaybackUserData
{
public:
	const int FFT_INPUT_SIZE = 1024;
	const int FFT_OUTPUT_SIZE = 32;

public:
	PlaybackUserData(SynthSettingsLoader* synthSettingsLoader);
	~PlaybackUserData();

	bool Initialize();

	SynthSettings* GetSynthSettings() const { return _synthSettingsLoader->GetCurrent(); }
	SoundRegistry* GetEffectRegistry() const { return _effectRegistry; }
	PlaybackInfo* GetPlaybackInfo() const { return _playbackInfo; }
	EqualizerOutput* GetEqualizer() const { return _equalizer; }
	PlaybackDeviceRegister* GetDeviceRegister() const { return _deviceRegister; }

	std::vector<SignalSettings*>* GetEffectRegistryList() const { return _effectList; }

	bool IsInitialized() const { return _initialized; }

	/// <summary>
	/// Used during initialization to finalize the parameters with which to run the device
	/// </summary>
	void UpdateDevice(const std::string& selectedDeviceName, unsigned int samplingRate, unsigned int outputBufferSize, bool selectDevice);

	/// <summary>
	/// Sets selected device
	/// </summary>
	void SelectDevice(const std::string& deviceName);

	/// <summary>
	/// Saves synth settings in the current configuration
	/// </summary>
	void SaveSynthSettings();

private:

	// This is not any of the "registries". This is the initialized list of effect settings. The SoundRegistry* was meant to 
	// be kept separate from other configuration objects (such as SynthSettings*) due to circular dependency.
	//
	std::vector<SignalSettings*>* _effectList;

	SynthSettingsLoader* _synthSettingsLoader;
	SoundRegistry* _effectRegistry;
	PlaybackInfo* _playbackInfo;
	PlaybackDeviceRegister* _deviceRegister;
	EqualizerOutput* _equalizer;

	bool _initialized;
};

#endif