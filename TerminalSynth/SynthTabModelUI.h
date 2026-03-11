#pragma once

#ifndef SYNTH_TAB_MODEL_UI_H
#define SYNTH_TAB_MODEL_UI_H

#include "ModelUI.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include "SoundBankSettings.h"
#include "SoundSettings.h"
#include <string>
#include <vector>

class SynthTabModelUI : public ModelUI
{
public:

	SynthTabModelUI(const std::vector<SignalSettings*>* effectRegistryList, const SoundSettings* soundSettings, const SoundBankSettings* soundBankSettings);
	SynthTabModelUI(const SynthTabModelUI& copy);
	~SynthTabModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void Update(SoundSettings* soundDestination, SoundBankSettings* soundBankDestination);

	SoundSettings* GetSoundSettings() const;
	SoundBankSettings* GetSoundBankSettings() const;
	const std::vector<SignalSettings*>* GetEffectRegistry() const;

private:

	std::string* _name;

	SoundSettings* _soundSettings;
	SoundBankSettings* _soundBankSettings;

	// DO NOT DELETE!
	const std::vector<SignalSettings*>* _effectRegistryList;
};

SynthTabModelUI::SynthTabModelUI(const std::vector<SignalSettings*>* effectRegistryList, const SoundSettings* soundSettings, const SoundBankSettings* soundBankSettings)
{
	_name = new std::string("Synth Tab");
	_soundSettings = new SoundSettings(*soundSettings);
	_soundBankSettings = new SoundBankSettings(*soundBankSettings);
	_effectRegistryList = effectRegistryList;
}

SynthTabModelUI::SynthTabModelUI(const SynthTabModelUI& copy)
{
	_name = new std::string("Synth Tab");
	_soundSettings = new SoundSettings(*copy.GetSoundSettings());
	_soundBankSettings = new SoundBankSettings(*copy.GetSoundBankSettings());
	_effectRegistryList = copy.GetEffectRegistry();
}

SynthTabModelUI::~SynthTabModelUI()
{
	delete _soundSettings;
	delete _soundBankSettings;
	delete _name;
}

std::string SynthTabModelUI::GetName() const
{
	return *_name;
}

int SynthTabModelUI::GetOrder() const
{
	return 0;
}

void SynthTabModelUI::Update(SoundSettings* soundDestination, SoundBankSettings* soundBankDestination)
{
	// This update goes the other way >_<
	soundDestination->Update(_soundSettings);
	soundBankDestination->Update(_soundBankSettings);
}

SoundSettings* SynthTabModelUI::GetSoundSettings() const
{
	return _soundSettings;
}

SoundBankSettings* SynthTabModelUI::GetSoundBankSettings() const
{
	return _soundBankSettings;
}

const std::vector<SignalSettings*>* SynthTabModelUI::GetEffectRegistry() const
{
	return _effectRegistryList;
}

#endif