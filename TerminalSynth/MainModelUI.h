#pragma once

#ifndef MAIN_MODEL_UI_H
#define MAIN_MODEL_UI_H

#include "ModelUI.h"
#include "OutputModelUI.h"
#include "PlaybackInfo.h"
#include "PlaybackUserData.h"
#include "SynthSettings.h"
#include "SynthTabModelUI.h"
#include <string>

class MainModelUI : public ModelUI
{
public:

	MainModelUI(const PlaybackUserData* playbackData);
	MainModelUI(const MainModelUI& copy);
	~MainModelUI();	

	std::string GetName() const override;
	int GetOrder() const override;

	void FromUI(SynthSettings* destination);
	void ToUI(const PlaybackUserData* playbackData);

	SynthTabModelUI* GetSynthTabModelUI() const;
	OutputModelUI* GetOutputModelUI() const;

	bool HaveSoundSettingsChanged() const;

private:

	std::string* _name;
	
	SynthTabModelUI* _synthTabModelUI;
	OutputModelUI* _outputModelUI;

	bool _haveSoundSettingsChanged;
};

MainModelUI::MainModelUI(const PlaybackUserData* playbackData)
{
	_name = new std::string("Terminal Synth");
	_synthTabModelUI = new SynthTabModelUI(playbackData->GetEffectRegistryList(), playbackData->GetSynthSettings()->GetDefaultSoundSettings(), playbackData->GetSynthSettings()->GetSoundBankSettings());
	_outputModelUI = new OutputModelUI(playbackData);

	// Running Initialization Cycle (this may need redesign if the UI grows any bigger)
	_outputModelUI->ToUI(playbackData);

	_haveSoundSettingsChanged = false;
}

MainModelUI::MainModelUI(const MainModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_synthTabModelUI = new SynthTabModelUI(*copy.GetSynthTabModelUI());
	_outputModelUI = new OutputModelUI(*copy.GetOutputModelUI());

	// Running Initialization Cycle (this may need redesign if the UI grows any bigger)
	//_outputModelUI->ToUI(copy.GetOutputModelUI()->GetPlaybackInfo(), copy.GetOutputModelUI()->GetEqualizerOutput());

	_haveSoundSettingsChanged = false;
}

MainModelUI::~MainModelUI()
{
	delete _name;
	delete _synthTabModelUI;
	delete _outputModelUI;
}

SynthTabModelUI* MainModelUI::GetSynthTabModelUI() const
{
	return _synthTabModelUI;
}

OutputModelUI* MainModelUI::GetOutputModelUI() const
{
	return _outputModelUI;
}

bool MainModelUI::HaveSoundSettingsChanged() const
{
	return _haveSoundSettingsChanged;
}

std::string MainModelUI::GetName() const
{
	return *_name;
}

int MainModelUI::GetOrder() const
{
	return 0;
}

void MainModelUI::FromUI(SynthSettings* destination)
{
	_synthTabModelUI->Update(destination->GetDefaultSoundSettings(), destination->GetSoundBankSettings());
	destination->SetGain(_outputModelUI->GetGain());
	destination->SetLeftRightBalance(_outputModelUI->GetLeftRightBalance());
}
void MainModelUI::ToUI(const PlaybackUserData* playbackData)
{
	_outputModelUI->ToUI(playbackData);

	_haveSoundSettingsChanged = _synthTabModelUI->GetSoundSettings()->IsEqual(playbackData->GetSynthSettings()->GetCurrentSoundSettings());
}

#endif