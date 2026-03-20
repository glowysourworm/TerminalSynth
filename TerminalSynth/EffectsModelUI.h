#pragma once

#ifndef SYNTH_TAB_MODEL_UI_H
#define SYNTH_TAB_MODEL_UI_H

#include "ModelUI.h"
#include "PlaybackUserData.h"
#include "SignalSettings.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include <string>
#include <vector>

class EffectsModelUI : public ModelUI
{
public:

	EffectsModelUI(const PlaybackUserData* playbackData);
	EffectsModelUI(const EffectsModelUI& copy);
	~EffectsModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void From(SoundSettings* soundDestination, SoundBankSettings* soundBankDestination);

	SoundSettings* GetSoundSettings() const;
	SoundBankSettings* GetSoundBankSettings() const;
	std::vector<SignalSettings*>* GetEffectRegistryList() const;

private:

	std::string* _name;

	SoundSettings* _soundSettings;
	SoundBankSettings* _soundBankSettings;

	// DO NOT DELETE!
	std::vector<SignalSettings*>* _effectRegistryList;
};

EffectsModelUI::EffectsModelUI(const PlaybackUserData* playbackData)
{
	_name = new std::string("Effects");
	_soundSettings = new SoundSettings(*playbackData->GetSynthSettings()->GetCurrentSoundSettings());
	_soundBankSettings = new SoundBankSettings(*playbackData->GetSynthSettings()->GetSoundBankSettings());

	// DO NOT DELETE!
	_effectRegistryList = playbackData->GetEffectRegistryList();
}

EffectsModelUI::EffectsModelUI(const EffectsModelUI& copy)
{
	_name = new std::string("Effects");
	_soundSettings = new SoundSettings(*copy.GetSoundSettings());
	_soundBankSettings = new SoundBankSettings(*copy.GetSoundBankSettings());
	_effectRegistryList = copy.GetEffectRegistryList();
}

EffectsModelUI::~EffectsModelUI()
{
	delete _soundSettings;
	delete _soundBankSettings;
	delete _name;
}

std::string EffectsModelUI::GetName() const
{
	return *_name;
}

int EffectsModelUI::GetOrder() const
{
	return 0;
}
SoundSettings* EffectsModelUI::GetSoundSettings() const
{
	return _soundSettings;
}

SoundBankSettings* EffectsModelUI::GetSoundBankSettings() const
{
	return _soundBankSettings;
}

std::vector<SignalSettings*>* EffectsModelUI::GetEffectRegistryList() const
{
	return _effectRegistryList;
}

void EffectsModelUI::From(SoundSettings* soundDestination, SoundBankSettings* soundBankDestination)
{
	soundDestination->Update(_soundSettings);
	soundBankDestination->Update(_soundBankSettings);
}

#endif