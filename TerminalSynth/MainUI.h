#pragma once

#ifndef MAIN_UI_H
#define MAIN_UI_H

#include "CheckListUI.h"
#include "OscillatorUI.h"
#include "OutputUI.h"
#include "SignalChainSettings.h"
#include "SignalChainUI.h"
#include "SoundBankSettings.h"
#include "SynthInformationUI.h"
#include "SynthSettings.h"
#include "UIBase.h"
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class MainUI : public UIBase<SynthSettings>
{
public:

	MainUI(const SoundBankSettings* soundBankSettings, const std::string& title, const ftxui::Color& titleColor);
	~MainUI();

	void Initialize(const SynthSettings& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	/// <summary>
	/// Collects information from the UI using a prepared configuration (pre-locked!)
	/// </summary>
	void FromUI(SynthSettings& configuration, bool clearDirty) override;

	/// <summary>
	/// Sets the UI from the update parameters (prepared for use by this component)
	/// </summary>
	void ToUI(const SynthSettings& configuration) override;

	bool GetDirty() const;

private:

	std::vector<std::string>* _tabHeaders;

	ftxui::Component _mainControl;
	ftxui::Component _tabControl;							// Shared Pointer (std::shared_pointer)
	ftxui::Component _tabControlMenu;

	// Synth Tab
	ftxui::Component _synthTab;

	// Output Tab
	ftxui::Component _outputTab;

	float* _scrollY;
	int* _tabIndex;

	// Map of effects that have been instantiated. This should contain all instances! The 
	// SynthSettings* cannot contain any instance of SignalBase* because of circular
	// references
	SignalChainSettings* _signalChainSettings;

	SynthInformationUI* _synthInformationUI;
	
	// Input
	OscillatorUI* _oscillatorUI;
	SignalChainUI* _signalChainUI;
	CheckListUI* _airwinPluginListUI;
	
	// Output
	OutputUI* _outputUI;

};

#endif