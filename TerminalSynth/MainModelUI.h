#pragma once

#ifndef MAIN_MODEL_UI_H
#define MAIN_MODEL_UI_H

#include "ModelUI.h"
#include "OutputModelUI.h"
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
	OutputModelUI* GetOutputModelUI() const;

private:

	std::string* _name;
	
	SynthTabModelUI* _synthTabModelUI;
	OutputModelUI* _outputModelUI;
};

MainModelUI::MainModelUI(const SynthSettings* synthSettings, const OutputSettings* outputSettings)
{
	_name = new std::string("Terminal Synth");
	_synthTabModelUI = new SynthTabModelUI(synthSettings->GetSoundSettings(), synthSettings->GetSoundBankSettings());
	_outputModelUI = new OutputModelUI(outputSettings);

	// Running Initialization Cycle (this may need redesign if the UI grows any bigger)
	_outputModelUI->ToUI(outputSettings);
}

MainModelUI::MainModelUI(const MainModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_synthTabModelUI = new SynthTabModelUI(*copy.GetSynthTabModelUI());
	_outputModelUI = new OutputModelUI(*copy.GetOutputModelUI());

	// Running Initialization Cycle (this may need redesign if the UI grows any bigger)
	_outputModelUI->ToUI(copy.GetOutputModelUI()->GetOutputSettings());
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
	_outputModelUI->ToUI(source);
}

#endif