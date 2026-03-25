#pragma once

#ifndef CONTROL_PANEL_UI_H
#define CONTROL_PANEL_UI_H

#include "ControlPanelModelUI.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class ControlPanelUI : public UIBase<ControlPanelModelUI>
{
public:

	ControlPanelUI();
	~ControlPanelUI();

	void Initialize(const ControlPanelModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const ControlPanelModelUI& source) override;
	void ToUI(const ControlPanelModelUI* source) override;
	void FromUI(ControlPanelModelUI& ControlPanelModelUI) override;
	void FromUI(ControlPanelModelUI* ControlPanelModelUI) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

public:

	void SetDirtyStatus(bool soundSettingsDirty);

private:

	ftxui::Component _component;
	ftxui::Component _modalComponent;

	bool _soundSettingsDirty;

	ValueCapture<bool>* _saveClicked;
};

ControlPanelUI::ControlPanelUI()
{
	_saveClicked = new ValueCapture<bool>(false);
	_soundSettingsDirty = false;
}

ControlPanelUI::~ControlPanelUI()
{
	delete _saveClicked;
}

void ControlPanelUI::Initialize(const ControlPanelModelUI& initialValue)
{
	_saveClicked->SetValue(false);
	_saveClicked->Clear();

	_soundSettingsDirty = false;

	// Control Panel
	_component = ftxui::Container::Vertical({

		ftxui::Renderer([] { return ftxui::text("Control Panel"); }),
		ftxui::Renderer([] { return ftxui::separator(); }) | ftxui::Maybe([&] {return _soundSettingsDirty; }),

		ftxui::Container::Horizontal({

			ftxui::Renderer([] { return ftxui::text("Synth Voice(s)"); }) | ftxui::vcenter,

			ftxui::Button("Save Voice", [&] { _saveClicked->SetValue(true); })
				| ftxui::bgcolor(ftxui::Color::RGBA(0, 0, 255, 20))
				| ftxui::Maybe([&] {return _soundSettingsDirty; })

		}) | ftxui::Maybe([&] {return _soundSettingsDirty; }),



	}) | ftxui::border;
}

ftxui::Component ControlPanelUI::GetComponent()
{
	return _component;
}

void ControlPanelUI::ServicePendingAction()
{
}

void ControlPanelUI::UpdateComponent()
{
}

void ControlPanelUI::Tick()
{
}

void ControlPanelUI::ToUI(const ControlPanelModelUI& source)
{
}

void ControlPanelUI::ToUI(const ControlPanelModelUI* source)
{
}

void ControlPanelUI::FromUI(ControlPanelModelUI& destination)
{
}

void ControlPanelUI::FromUI(ControlPanelModelUI* destination)
{
}

bool ControlPanelUI::GetDirty() const
{
	return false;
}

void ControlPanelUI::ClearDirty()
{
	
}

void ControlPanelUI::SetDirtyStatus(bool soundSettingsDirty)
{
	// This can be moved to a control panel ui model to follow the pattern
	_soundSettingsDirty = soundSettingsDirty;
}

bool ControlPanelUI::HasPendingAction() const
{
	return _saveClicked->GetValue() && _saveClicked->HasChanged();
}

void ControlPanelUI::ClearPendingAction()
{
	_saveClicked->SetValue(false);
	_saveClicked->Clear();
}

#endif