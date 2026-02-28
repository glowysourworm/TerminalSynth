#pragma once

#ifndef MAIN_UI_H
#define MAIN_UI_H

#include "OutputUI.h"
#include "SignalChainSettings.h"
#include "SynthInformationUI.h"
#include "SynthSettings.h"
#include "SynthTabUI.h"
#include "UIBase.h"
#include <ftxui/component/component_base.hpp>
#include <string>
#include <vector>

class MainUI : public UIBase<SynthSettings>
{
public:

	MainUI(const SynthSettings& configuration);
	~MainUI();

	void Initialize(const SynthSettings& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent() override;

	/// <summary>
	/// Collects information from the UI using a prepared configuration (pre-locked!)
	/// </summary>
	void FromUI(SynthSettings& configuration) override;

	/// <summary>
	/// Sets the UI from the update parameters (prepared for use by this component)
	/// </summary>
	void ToUI(const SynthSettings& configuration) override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	std::vector<std::string>* _tabHeaders;

	ftxui::Component _mainControl;
	ftxui::Component _tabControl;							// Shared Pointer (std::shared_pointer)
	ftxui::Component _tabControlMenu;

	// Output Tab
	ftxui::Component _outputTab;

	float* _scrollY;
	int* _tabIndex;

	// Map of effects that have been instantiated. This should contain all instances! The 
	// SynthSettings* cannot contain any instance of SignalBase* because of circular
	// references
	SignalChainSettings* _signalChainSettings;

	SynthInformationUI* _synthInformationUI;
	
	// Synth Tab
	SynthTabUI* _synthTabUI;
	
	// Output Tab
	OutputUI* _outputUI;

};

#endif