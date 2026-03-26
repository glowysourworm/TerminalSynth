#pragma once

#ifndef CONTROL_PANEL_MODEL_UI_H
#define CONTROL_PANEL_MODEL_UI_H

#include "ModelUI.h"
#include "PlaybackUserData.h"
#include "SoundSettings.h"
#include "SynthSettings.h"
#include <string>
#include <vector>

class ControlPanelModelUI : public ModelUI
{
public:

	ControlPanelModelUI(const PlaybackUserData* playbackData);
	ControlPanelModelUI(const ControlPanelModelUI& copy);
	~ControlPanelModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void From(SynthSettings* synthSettings);
	void To(const SynthSettings* synthSettings);

public:

	SoundSettings* GetCurrentSoundSettings() const { return _currentSoundSettings; }

	std::vector<std::string>* GetSoundSettingsList() const { return _soundSettingsList; }

	bool AreSoundSettingsDirty() const { return _soundSettingsDirty; }
	bool GetStkEnabled() const { return _stkEnabled; }
	bool GetSoundBankEnabled() const { return _soundBankEnabled; }

private:

	bool _soundBankEnabled;
	bool _stkEnabled;

	bool _soundSettingsDirty;

	// Sound Settings (synth voice, local copy)
	SoundSettings* _currentSoundSettings;

	std::vector<std::string>* _soundSettingsList;
};

ControlPanelModelUI::ControlPanelModelUI(const PlaybackUserData* playbackData)
{
	_soundSettingsList = new std::vector<std::string>();
		
	// Sound Settings List (of names)
	playbackData->GetSynthSettings()->GetSoundSettingsList(*_soundSettingsList);

	_currentSoundSettings = new SoundSettings(*playbackData->GetSynthSettings()->GetCurrentSoundSettings());
	_soundBankEnabled = playbackData->GetSynthSettings()->GetSoundBankEnabled();
	_stkEnabled = playbackData->GetSynthSettings()->GetStkEnabled();
	_soundSettingsDirty = false;
}

ControlPanelModelUI::ControlPanelModelUI(const ControlPanelModelUI& copy)
{
	_currentSoundSettings = new SoundSettings(*copy.GetCurrentSoundSettings());
	_soundBankEnabled = copy.GetSoundBankEnabled();
	_stkEnabled = copy.GetStkEnabled();
	_soundSettingsDirty = false;
	_soundSettingsList = new std::vector<std::string>(*copy.GetSoundSettingsList());
}

ControlPanelModelUI::~ControlPanelModelUI()
{
	delete _currentSoundSettings;
	delete _soundSettingsList;
}

std::string ControlPanelModelUI::GetName() const
{
	return "ControlPanelModelUI";
}

int ControlPanelModelUI::GetOrder() const
{
	return 0;
}

void ControlPanelModelUI::From(SynthSettings* synthSettings)
{
	// No copy performed here
}
void ControlPanelModelUI::To(const SynthSettings* synthSettings)
{
	_soundSettingsDirty = !_currentSoundSettings->IsEqual(synthSettings->GetCurrentSoundSettings());
}

#endif