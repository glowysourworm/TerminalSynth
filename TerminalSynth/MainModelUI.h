#pragma once

#ifndef MAIN_MODEL_UI_H
#define MAIN_MODEL_UI_H

#include "EffectsModelUI.h"
#include "InputModelUI.h"
#include "ModelUI.h"
#include "OutputModelUI.h"
#include "PlaybackUserData.h"
#include "SynthSettings.h"
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

	EffectsModelUI* GetEffectsModelUI() const;
	OutputModelUI* GetOutputModelUI() const;
	InputModelUI* GetInputModelUI() const;

	bool HaveSoundSettingsChanged() const;

private:

	std::string* _name;
	
	EffectsModelUI* _effectsModelUI;
	InputModelUI* _inputModelUI;
	OutputModelUI* _outputModelUI;

	bool _haveSoundSettingsChanged;
};

MainModelUI::MainModelUI(const PlaybackUserData* playbackData)
{
	_name = new std::string("Terminal Synth");
	_effectsModelUI = new EffectsModelUI(playbackData);
	_outputModelUI = new OutputModelUI(playbackData);
	_inputModelUI = new InputModelUI(playbackData);

	// Running Initialization Cycle (this may need redesign if the UI grows any bigger)
	_outputModelUI->ToUI(playbackData);

	_haveSoundSettingsChanged = false;
}

MainModelUI::MainModelUI(const MainModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_effectsModelUI = new EffectsModelUI(*copy.GetEffectsModelUI());
	_outputModelUI = new OutputModelUI(*copy.GetOutputModelUI());
	_inputModelUI = new InputModelUI(*copy.GetInputModelUI());

	// Running Initialization Cycle (this may need redesign if the UI grows any bigger)
	//_outputModelUI->ToUI(copy.GetOutputModelUI()->GetPlaybackInfo(), copy.GetOutputModelUI()->GetEqualizerOutput());

	_haveSoundSettingsChanged = false;
}

MainModelUI::~MainModelUI()
{
	delete _name;
	delete _effectsModelUI;
	delete _outputModelUI;
	delete _inputModelUI;
}

EffectsModelUI* MainModelUI::GetEffectsModelUI() const
{
	return _effectsModelUI;
}

OutputModelUI* MainModelUI::GetOutputModelUI() const
{
	return _outputModelUI;
}
InputModelUI* MainModelUI::GetInputModelUI() const
{
	return _inputModelUI;
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
	destination->SetGain(_outputModelUI->GetGain());
	destination->SetLeftRightBalance(_outputModelUI->GetLeftRightBalance());
	
	_effectsModelUI->From(destination->GetCurrentSoundSettings(), destination->GetSoundBankSettings());

	// Overwrites Oscillator / Envelope
	_inputModelUI->From(destination->GetCurrentSoundSettings()->GetOscillatorParameters(),
						destination->GetCurrentSoundSettings()->GetOscillatorEnvelope());
}
void MainModelUI::ToUI(const PlaybackUserData* playbackData)
{
	_outputModelUI->ToUI(playbackData);

	_haveSoundSettingsChanged = _effectsModelUI->GetSoundSettings()->IsEqual(playbackData->GetSynthSettings()->GetCurrentSoundSettings());
}

#endif