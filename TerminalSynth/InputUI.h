#pragma once

#ifndef INPUT_UI_H
#define INPUT_UI_H

#include "EnvelopeUI.h"
#include "InputModelUI.h"
#include "OscillatorUI.h"
#include "UIBase.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

class InputUI : public UIBase<InputModelUI>
{
public:

	InputUI(const InputModelUI& synthSettings);
	~InputUI();

	void Initialize(const InputModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const InputModelUI& source) override;
	void ToUI(const InputModelUI* source) override;
	void FromUI(InputModelUI& destination) override;
	void FromUI(InputModelUI* destination) override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	ftxui::Component _component;

	// Editors
	OscillatorUI* _oscillatorUI;
	EnvelopeUI* _envelopeUI;
};

InputUI::InputUI(const InputModelUI& model)
{
	_envelopeUI = new EnvelopeUI(*model.GetEnvelope());
	_oscillatorUI = new OscillatorUI(model.GetSoundBankSettings(), *model.GetOscillatorParameters());

	_oscillatorUI->Initialize(*model.GetOscillatorParameters());
	_envelopeUI->Initialize(*model.GetEnvelope());
}
InputUI::~InputUI()
{
	// Editors
	delete _oscillatorUI;
	delete _envelopeUI;
}

void InputUI::Initialize(const InputModelUI& model)
{
	// Plugin List | Signal Chain (Vertical) | Effect Editor
	_component = ftxui::Container::Horizontal({

		_oscillatorUI->GetComponent() | ftxui::flex_grow,
		_envelopeUI->GetComponent() | ftxui::flex_grow

	});
}

ftxui::Component InputUI::GetComponent()
{
	return _component;
}

void InputUI::ServicePendingAction()
{

}

void InputUI::UpdateComponent()
{
	_oscillatorUI->UpdateComponent();
	_envelopeUI->UpdateComponent();
}

void InputUI::Tick()
{
	_oscillatorUI->Tick();
	_envelopeUI->Tick();
}

void InputUI::ToUI(const InputModelUI& source)
{

}

void InputUI::ToUI(const InputModelUI* source)
{
}

void InputUI::FromUI(InputModelUI& destination)
{
	throw new std::exception("Pleaes use the pointer version of this function FromUI(..)");
}

void InputUI::FromUI(InputModelUI* destination)
{
	// Signal Input (settings)
	_oscillatorUI->FromUI(destination->GetOscillatorParameters());
	_envelopeUI->FromUI(destination->GetEnvelope());
}

bool InputUI::HasPendingAction() const
{
	return false;
}

void InputUI::ClearPendingAction()
{
	_oscillatorUI->ClearPendingAction();
	_envelopeUI->ClearPendingAction();
}

bool InputUI::GetDirty() const
{
	return _oscillatorUI->GetDirty() || _envelopeUI->GetDirty();
}

void InputUI::ClearDirty()
{
	_oscillatorUI->ClearDirty();
	_envelopeUI->ClearDirty();
}

#endif