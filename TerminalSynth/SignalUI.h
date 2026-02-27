#pragma once

#ifndef SIGNAL_UI_H
#define SIGNAL_UI_H

#include "SignalModelUI.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class SignalUI : public UIBase<SignalModelUI>
{
public:

	SignalUI(const SignalModelUI& model, bool canEnableOrDisable, bool canMoveUpOrDown);
	~SignalUI();

	void Initialize(const SignalModelUI& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const SignalModelUI& source) override;
	void FromUI(SignalModelUI& destination, bool clearDirty) override;

	bool GetDirty() const override;

public:

	bool GetMoveUpFlag() const { return  _moveUpValue->GetValue(); }
	bool GetMoveDownFlag() const { return  _moveDownValue->GetValue(); }
	bool GetEditFlag() const { return  _editValue->GetValue(); }
	bool GetRemoveFlag() const { return  _removeValue->GetValue(); }

private:

	const wchar_t* _arrowUpUnicode = L"\u2191";
	const wchar_t* _arrowDownUnicode = L"\u2193";
	std::wstring* _arrowUp;
	std::wstring* _arrowDown;

	ftxui::Component _component;
	ftxui::Component _checkBox;

	bool _canEnableDisable;
	bool _canMoveUpOrDown;
	bool _checkboxHover;
	bool _mainHover;
	
	ValueCapture<bool>* _enabledValue;							// These represent: data; and real-time request values
	ValueCapture<bool>* _moveUpValue;
	ValueCapture<bool>* _moveDownValue;
	ValueCapture<bool>* _editValue;
	ValueCapture<bool>* _removeValue;
};

SignalUI::SignalUI(const SignalModelUI& model, bool canEnableOrDisable, bool canMoveUpOrDown) : UIBase(model.GetName(), model.GetName(), ftxui::Color::White)
{
	_canEnableDisable = canEnableOrDisable;
	_canMoveUpOrDown = canMoveUpOrDown;
	_checkboxHover = false;
	_mainHover = false;
	_enabledValue = new ValueCapture<bool>(model.GetEnabled());
	_moveUpValue = new ValueCapture<bool>(false);
	_moveDownValue = new ValueCapture<bool>(false);
	_editValue = new ValueCapture<bool>(false);
	_removeValue = new ValueCapture<bool>(false);

	_arrowUp = new std::wstring(_arrowUpUnicode);
	_arrowDown = new std::wstring(_arrowDownUnicode);
}

SignalUI::~SignalUI()
{
	delete _enabledValue;
	delete _moveUpValue;
	delete _moveDownValue;
	delete _editValue;
	delete _removeValue;
	delete _arrowUp;
	delete _arrowDown;
}

void SignalUI::Initialize(const SignalModelUI& initialValue)
{
	_enabledValue->SetValue(initialValue.GetEnabled());

	_checkBox = ftxui::Checkbox("", _enabledValue->GetRef(), {

		.on_change = [&] { this->SetDirty(); }

	}) | ftxui::Hoverable(&_checkboxHover);

	_component = ftxui::Container::Horizontal({
		_checkBox | ftxui::CatchEvent([&](ftxui::Event event) {

			// Check that user can modify the enabled flag
			if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Button::Left && _checkboxHover)
			{
				return _canEnableDisable ? false : true;
			}

			return false;

		}) | ftxui::vcenter | ftxui::hcenter,
		ftxui::Renderer([&] { return ftxui::text(this->GetName()) | ftxui::vcenter; }) | ftxui::flex_grow,
		ftxui::Button(*_arrowUp, [&] { _moveUpValue->SetValue(true); }) | ftxui::Maybe(&_canMoveUpOrDown),
		ftxui::Button(*_arrowDown, [&] { _moveDownValue->SetValue(true); }) | ftxui::Maybe(&_canMoveUpOrDown),
		ftxui::Button("Edit", [&] { _editValue->SetValue(true); }),
		ftxui::Button("Remove", [&] { _removeValue->SetValue(true); }),

		}) | ftxui::Hoverable(&_mainHover) | ftxui::xflex_grow;
}

ftxui::Component SignalUI::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render() | ftxui::borderStyled(_mainHover ? ftxui::Color::BlueLight : ftxui::Color::White);
	});
}

void SignalUI::UpdateComponent(bool clearDirty)
{
	if (clearDirty)
	{
		// Data
		_enabledValue->Clear();

		// Real-time flags
		_moveUpValue->Clear();
		_moveDownValue->Clear();
		_editValue->Clear();
		_removeValue->Clear();

		this->ClearDirty();
	}
		
}

void SignalUI::ToUI(const SignalModelUI& source)
{
	_enabledValue->SetValue(source.GetEnabled());
}

void SignalUI::FromUI(SignalModelUI& destination, bool clearDirty)
{
	bool enabled = _enabledValue->GetValue();
	
	destination.SetEnabled(enabled);

	if (clearDirty)
	{
		// Data
		_enabledValue->Clear();

		// Real-time flags
		_moveUpValue->Clear();
		_moveDownValue->Clear();
		_editValue->Clear();
		_removeValue->Clear();

		this->ClearDirty();
	}
}

bool SignalUI::GetDirty() const
{
	return _enabledValue->HasChanged() || 
		_moveUpValue->HasChanged() ||
		_moveDownValue->HasChanged() ||
		_editValue->HasChanged() ||
		_removeValue->HasChanged();
}

#endif