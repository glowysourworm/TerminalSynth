#pragma once

#ifndef CHECKBOX_UI_H
#define CHECKBOX_UI_H

#include "CheckboxModelUI.h"
#include "UIBase.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class CheckboxUI : public UIBase<CheckboxModelUI>
{
public:

	CheckboxUI(const CheckboxModelUI& initialValue);
	~CheckboxUI();

	void Initialize(const CheckboxModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void FromUI(CheckboxModelUI& destination) override;
	void FromUI(CheckboxModelUI* destination) override;
	void ToUI(const CheckboxModelUI& source) override;
	void ToUI(const CheckboxModelUI* source) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

public:

	bool GetIsChecked() const { return *_value; }

private:

	ftxui::Component _component;

	bool _isDirty;
	bool* _value;
	std::string* _label;
	ftxui::Color* _labelColor;
};

CheckboxUI::CheckboxUI(const CheckboxModelUI& initialValue)
{
	_labelColor = new ftxui::Color(ftxui::Color::White);
	_label = new std::string(initialValue.GetName());
	_value = new bool(initialValue.GetIsChecked());
	_isDirty = false;
}

CheckboxUI::~CheckboxUI()
{
	delete _label;
	delete _labelColor;
	delete _value;
}

void CheckboxUI::Initialize(const CheckboxModelUI& initialValue)
{
	_component = ftxui::Checkbox(_label, _value, { .on_change = [&] {
		_isDirty = true;
	} });
}

ftxui::Component CheckboxUI::GetComponent()
{
	return _component;
}

void CheckboxUI::ServicePendingAction()
{
}

void CheckboxUI::UpdateComponent()
{
}

void CheckboxUI::Tick()
{
}

void CheckboxUI::FromUI(CheckboxModelUI& destination)
{
	throw new std::exception("Please use pointer version of this function");
}

void CheckboxUI::FromUI(CheckboxModelUI* destination)
{
	destination->SetName(*_label);
	destination->SetIsChecked(*_value);
}

void CheckboxUI::ToUI(const CheckboxModelUI& source)
{
	throw new std::exception("Please use pointer version of this function");
}

void CheckboxUI::ToUI(const CheckboxModelUI* source)
{
	*_value = source->GetIsChecked();
}

bool CheckboxUI::GetDirty() const
{
	return _isDirty;
}

void CheckboxUI::ClearDirty()
{
	_isDirty = false;
}

bool CheckboxUI::HasPendingAction() const
{
	return _isDirty;		// Makes sense to have the checking of the box be the "pending action";
							// but clearing it depends on retrieving the data (so don't clear it
							// as a pending action.
}

void CheckboxUI::ClearPendingAction()
{
}

#endif