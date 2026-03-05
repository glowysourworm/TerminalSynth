#pragma once

#ifndef MAIN_MODEL_UI_H
#define MAIN_MODEL_UI_H

#include "ModelUI.h"
#include "OutputSettings.h"
#include "SynthSettings.h"
#include "SynthTabModelUI.h"
#include <string>

class MainModelUI : public ModelUI
{
public:

	MainModelUI(const SynthSettings* synthSettings, const OutputSettings* outputSettings);
	MainModelUI(const MainModelUI& copy);
	~MainModelUI();	

	std::string GetName() const override;
	int GetOrder() const override;

	void FromUI(SynthSettings* destination);
	void ToUI(OutputSettings* source);

	SynthTabModelUI* GetSynthTabModelUI() const;
	OutputSettings* GetOutputSettings() const;

private:

	std::string* _name;

	SynthTabModelUI* _synthTabModelUI;

	OutputSettings* _outputSettings;
};

MainModelUI::MainModelUI(const SynthSettings* synthSettings, const OutputSettings* outputSettings)
{
	_name = new std::string("Terminal Synth");
	_synthTabModelUI = new SynthTabModelUI(synthSettings->GetSoundSettings(), synthSettings->GetSoundBankSettings());
	_outputSettings = new OutputSettings(*outputSettings);
}

MainModelUI::MainModelUI(const MainModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_synthTabModelUI = new SynthTabModelUI(*copy.GetSynthTabModelUI());
	_outputSettings = new OutputSettings(*copy.GetOutputSettings());
}

MainModelUI::~MainModelUI()
{
	delete _name;
	delete _synthTabModelUI;
	delete _outputSettings;
}

SynthTabModelUI* MainModelUI::GetSynthTabModelUI() const
{
	return _synthTabModelUI;
}

OutputSettings* MainModelUI::GetOutputSettings() const
{
	return _outputSettings;
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
	_synthTabModelUI->Update(destination->GetSoundSettings(), destination->GetSoundBankSettings());
}
void MainModelUI::ToUI(OutputSettings* source)
{
	_outputSettings->UpdateRT_Audio(
		source->GetStreamTime(), 
		source->GetAvgAudioMilli(),
		source->GetAvgAudioSampleMicro(),
		source->GetAvgAudioLockAcquireNano(),
		source->GetStreamLatency(),
		source->GetLeftChannel(),
		source->GetRightChannel());

	_outputSettings->UpdateRT_UI(
		source->GetAvgUIMilli(),
		source->GetAvgUIDataFetchMicro(),
		source->GetAvgUILockAqcuireNano(),
		source->GetAvgUIRenderingMilli(),
		source->GetAvgUISleepMilli());
}

#endif