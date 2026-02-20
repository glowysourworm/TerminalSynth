#pragma once

#ifndef CHECKBOX_UI_H
#define CHECKBOX_UI_H

#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class CheckboxUI : public UIBase<bool>
{
public:

	CheckboxUI(const std::string& name,
			   const std::string& label,
			   bool initialValue,
			   const ftxui::Color& titleColor);
	~CheckboxUI();

	void Initialize(const bool& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void FromUI(bool& destination, bool clearDirty) override;
	void ToUI(const bool& source) override;

private:

	ftxui::Component _component;

	bool* _value;
};

CheckboxUI::CheckboxUI(const std::string& name, const std::string& label, bool initialValue, const ftxui::Color& labelColor)
	: UIBase(name, label, labelColor)
{
	_value = new bool(initialValue);
}

CheckboxUI::~CheckboxUI()
{
	delete _value;
}

void CheckboxUI::Initialize(const bool& initialValue)
{
	UIBase::Initialize(initialValue);

	_component = ftxui::Checkbox(this->GetLabel(), _value) | ftxui::CatchEvent([&](ftxui::Event event) {

		// Pass through
		if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Button::Left)
		{
			this->SetDirty();
			return false;
		}

		// Cancel keyboard events
		return true;
	});
}

ftxui::Component CheckboxUI::GetComponent()
{
	return _component;
}

void CheckboxUI::UpdateComponent(bool clearDirty)
{
	if (clearDirty)
		this->ClearDirty();
}

void CheckboxUI::FromUI(bool& destination, bool clearDirty)
{
	destination = *_value;

	if (clearDirty)
		this->ClearDirty();
}

void CheckboxUI::ToUI(const bool& source)
{
	*_value = source;
}

#endif