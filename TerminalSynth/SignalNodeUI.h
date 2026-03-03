#pragma once

#ifndef SIGNAL_UI_H
#define SIGNAL_UI_H

#include "SignalNodeModelUI.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class SignalNodeUI : public UIBase<SignalNodeModelUI>
{
public:

	enum class UIAction
	{
		None,
		MoveUp,
		MoveDown,
		Edit,
		Remove
	};

public:

	SignalNodeUI(const SignalNodeModelUI& model);
	~SignalNodeUI();

	void Initialize(const SignalNodeModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const SignalNodeModelUI& source) override;
	void ToUI(const SignalNodeModelUI* source) override;
	void FromUI(SignalNodeModelUI& destination) override;
	void FromUI(SignalNodeModelUI* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

public:

	UIAction GetUIAction() const { return  _uiAction->GetValue(); }
	bool GetHasUIAction() const { return _uiAction->HasChanged(); }

private:

	const wchar_t* _arrowUpUnicode = L"\u2191";
	const wchar_t* _arrowDownUnicode = L"\u2193";
	std::wstring* _arrowUp;
	std::wstring* _arrowDown;

	ftxui::Component _component;
	ftxui::Component _checkBox;

	bool _canReorder;
	bool _canRemove;
	bool _checkboxHover;
	bool _mainHover;

	SignalNodeModelUI* _model;
	
	ValueCapture<bool>* _enabledValue;							// These represent: data; and real-time request values
	ValueCapture<UIAction>* _uiAction;

	bool _isDirty;
};

SignalNodeUI::SignalNodeUI(const SignalNodeModelUI& model)
{
	_checkboxHover = false;
	_mainHover = false;
	_canReorder = model.GetCanReorder();
	_canRemove = model.GetCanRemove();
	_enabledValue = new ValueCapture<bool>(model.GetEnabled());
	_uiAction = new ValueCapture<UIAction>(UIAction::None);
	_model = new SignalNodeModelUI(model);
	_arrowUp = new std::wstring(_arrowUpUnicode);
	_arrowDown = new std::wstring(_arrowDownUnicode);

	_isDirty = false;
}

SignalNodeUI::~SignalNodeUI()
{
	delete _enabledValue;
	delete _uiAction;
	delete _arrowUp;
	delete _arrowDown;
	delete _model;
}

void SignalNodeUI::Initialize(const SignalNodeModelUI& initialValue)
{
	_enabledValue->SetValue(initialValue.GetEnabled());

	_checkBox = ftxui::Checkbox("", _enabledValue->GetRef(), {

		// -> Dirty
		.on_change = [&] { _isDirty = true; }

	}) | ftxui::Hoverable(&_checkboxHover);

	_component = ftxui::Container::Horizontal({

		_checkBox | ftxui::CatchEvent([&](ftxui::Event event) {

			// Check that user can modify the enabled flag
			if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Button::Left && _checkboxHover)
			{
				return _model->GetCanEnable() ? false : true;
			}

			return false;

		}) | ftxui::vcenter | ftxui::hcenter,
		ftxui::Renderer([&] { return ftxui::text(_model->GetName()) | ftxui::vcenter; }) | ftxui::flex_grow,
		ftxui::Button(*_arrowUp, [&] { _uiAction->SetValue(UIAction::MoveUp); }) | ftxui::Maybe(&_canReorder),
		ftxui::Button(*_arrowDown, [&] { _uiAction->SetValue(UIAction::MoveDown); }) | ftxui::Maybe(&_canReorder),
		ftxui::Button("Edit", [&] { _uiAction->SetValue(UIAction::Edit); }),
		ftxui::Button("Remove", [&] { _uiAction->SetValue(UIAction::Remove); }) | ftxui::Maybe(&_canRemove),

	}) | ftxui::Hoverable(&_mainHover) | ftxui::xflex_grow;
}

ftxui::Component SignalNodeUI::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render() | ftxui::borderStyled(_mainHover ? ftxui::Color::BlueLight : ftxui::Color::White);
	});
}

void SignalNodeUI::ServicePendingAction()
{
}

void SignalNodeUI::UpdateComponent()
{
	// Nothing to do here; but, if we wanted, we could rebuild the UI. I'd rather let FTXUI
	// use its renderer (see GetComponent())
}

void SignalNodeUI::Tick()
{
	// Take the real-time changes from FTXUI by using our shared pointer values
	//

	// Mouse Leave
	if (!_mainHover)
	{
		_uiAction->SetValue(UIAction::None);
		_uiAction->Clear();
	}
}

void SignalNodeUI::ToUI(const SignalNodeModelUI& source)
{
	throw new std::exception("Please use  the pointer version of this function ToUI");
}

void SignalNodeUI::ToUI(const SignalNodeModelUI* source)
{
	_enabledValue->SetValue(source->GetEnabled());
	_model->Update(source);
}

void SignalNodeUI::FromUI(SignalNodeModelUI& destination)
{
	throw new std::exception("Please use  the pointer version of this function FromUI");
}

void SignalNodeUI::FromUI(SignalNodeModelUI* destination)
{
	bool enabled = _enabledValue->GetValue();
	destination->Update(_model);
}

bool SignalNodeUI::GetDirty() const
{
	return _enabledValue->HasChanged();
}

void SignalNodeUI::ClearDirty()
{
	_enabledValue->Clear();	
}

bool SignalNodeUI::HasPendingAction() const
{
	return _uiAction->HasChanged();
}

void SignalNodeUI::ClearPendingAction()
{
	_uiAction->Clear();
}

#endif