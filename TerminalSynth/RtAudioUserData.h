#ifndef RT_AUDIO_USER_DATA_H
#define RT_AUDIO_USER_DATA_H

#include "EqualizerOutput.h"
#include "OutputSettings.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"
#include <atomic>

class RtAudioUserData
{
public:
	RtAudioUserData()
	{
		_synthSettings = nullptr;
		_effectRegistry = nullptr;
		_outputSettings = nullptr;

		_initialized = false;
	}

	void Initialize(SynthSettings* synthSettings, SoundRegistry* effectRegistry, OutputSettings* outputSettings)
	{
		_synthSettings = synthSettings;
		_effectRegistry = effectRegistry;
		_outputSettings = outputSettings;

		_initialized = true;
	}

	SynthSettings* GetSynthSettings() const { return _synthSettings; }
	SoundRegistry* GetEffectRegistry() const { return _effectRegistry; }
	OutputSettings* GetOutputSettings() const { return _outputSettings; }
	EqualizerOutput* GetEqualizer() const { return _outputSettings->GetEqualizer(); }

	bool IsInitialized() const { return _initialized; }

private:

	SynthSettings* _synthSettings;
	SoundRegistry* _effectRegistry;
	OutputSettings* _outputSettings;

	std::atomic<bool> _initialized;			// This will be used once prior to stream running it's first callback (std::atomic is not necessary)
};

#endif