#ifndef RT_AUDIO_USER_DATA_H
#define RT_AUDIO_USER_DATA_H

#include "EffectRegistry.h"
#include "SynthSettings.h"
#include <atomic>

class RtAudioUserData
{
public:
	RtAudioUserData()
	{
		_synthSettings = nullptr;
		_effectRegistry = nullptr;

		_initialized = false;
	}

	void Initialize(SynthSettings* synthSettings,
					EffectRegistry* effectRegistry)
	{
		_synthSettings = synthSettings;
		_effectRegistry = effectRegistry;

		_initialized = true;
	}

	SynthSettings* GetSynthSettings() const { return _synthSettings; }
	EffectRegistry* GetEffectRegistry() const { return _effectRegistry; }

	bool IsInitialized() const { return _initialized; }

private:

	SynthSettings* _synthSettings;
	EffectRegistry* _effectRegistry;

	std::atomic<bool> _initialized;			// This will be used once prior to stream running it's first callback (std::atomic is not necessary)
};

#endif