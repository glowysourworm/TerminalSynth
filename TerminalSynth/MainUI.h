#pragma once

#ifndef MAIN_UI_H
#define MAIN_UI_H

#include "ControlPanelUI.h"
#include "EffectsUI.h"
#include "InputUI.h"
#include "MainModelUI.h"
#include "SynthInformationUI.h"
#include "UIBase.h"
#include <ftxui/component/component_base.hpp>
#include <vector>

#include <string>
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

	float* _scrollY;
	int* _tabIndex;

	MainModelUI* _model;

	// Tabs
	ControlPanelUI* _controlPanelUI;
	SynthInformationUI* _synthInformationUI;
	InputUI* _inputUI;
	EffectsUI* _effectsUI;

	// Modal Dialog (multi-purpose save)
	bool _showSaveDialog;
	bool _processSaveSoundSettings;
	std::string* _saveDialogTitle;
	std::string* _saveDialogInput;

};

#endif