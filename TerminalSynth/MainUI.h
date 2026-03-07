#pragma once

#ifndef MAIN_UI_H
#define MAIN_UI_H

#include "MainModelUI.h"
#include "SynthInformationUI.h"
#include "SynthTabUI.h"
#include "UIBase.h"
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class MainUI : public UIBase<MainModelUI>
{
public:

	MainUI(const MainModelUI& model);
	~MainUI();

	void Initialize(const MainModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void FromUI(MainModelUI& destination) override;
	void FromUI(MainModelUI* destination) override;

	void ToUI(const MainModelUI& source) override;
	void ToUI(const MainModelUI* source) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

private:

	std::vector<std::string>* _tabHeaders;

	ftxui::Component _mainControl;
	ftxui::Component _tabControl;							// Shared Pointer (std::shared_pointer)
	ftxui::Component _tabControlMenu;
	ftxui::Color _buttonColor;

	// Output Tab
	ftxui::Component _outputTab;

	float* _scrollY;
	int* _tabIndex;

	MainModelUI* _model;

	SynthInformationUI* _synthInformationUI;
	
	// Synth Tab
	SynthTabUI* _synthTabUI;

};

#endif