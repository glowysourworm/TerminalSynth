#pragma once

#ifndef INPUT_UI_H
#define INPUT_UI_H

#include "ControlPanelUI.h"
#include "EnvelopeUI.h"
#include "InputModelUI.h"
#include "OscillatorUI.h"
#include "SynthNoteParametersUI.h"
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

	void SetControlPanelStatus(bool soundSettingsChanged);

private:

	ftxui::Component _component;

	// Control Panel
	ControlPanelUI* _controlPanelUI;

	// Editors
	OscillatorUI* _oscillatorUI;
	EnvelopeUI* _envelopeUI;
	SynthNoteParametersUI* _synthNoteParametersUI;
};

InputUI::InputUI(const InputModelUI& model)
{
	_envelopeUI = new EnvelopeUI(*model.GetEnvelope());
	_oscillatorUI = new OscillatorUI(model.GetSoundBankSettings(), *model.GetOscillatorParameters());
	_synthNoteParametersUI = new SynthNoteParametersUI(*model.GetSynthNoteParamters());
	_controlPanelUI = new ControlPanelUI();
}
InputUI::~InputUI()
{
	// Control Panel
	delete _controlPanelUI;

	// Editors
	delete _oscillatorUI;
	delete _envelopeUI;
	delete _synthNoteParametersUI;
}

void InputUI::Initialize(const InputModelUI& model)
{
	_oscillatorUI->Initialize(*model.GetOscillatorParameters());
	_envelopeUI->Initialize(*model.GetEnvelope());
	_synthNoteParametersUI->Initialize(*model.GetSynthNoteParamters());
	_controlPanelUI->Initialize(false);

	_component = ftxui::Container::Vertical({

		_controlPanelUI->GetComponent(),

		ftxui::Container::Horizontal({

			_oscillatorUI->GetComponent() | ftxui::flex_grow,
			_envelopeUI->GetComponent() | ftxui::flex_grow

		}),

		ftxui::Container::Horizontal({
			_synthNoteParametersUI->GetComponent() | ftxui::flex_grow
		})

	});
}

void InputUI::SetControlPanelStatus(bool soundSettingsChanged)
{
	_controlPanelUI->SetDirtyStatus(soundSettingsChanged);
}

ftxui::Component InputUI::GetComponent()
{
	return _component;
}

void InputUI::ServicePendingAction()
{
	if (_controlPanelUI->HasPendingAction())
	{
		_controlPanelUI->ServicePendingAction();
	}
}

void InputUI::UpdateComponent()
{
	_oscillatorUI->UpdateComponent();
	_envelopeUI->UpdateComponent();
	_synthNoteParametersUI->UpdateComponent();
}

void InputUI::Tick()
{
	_oscillatorUI->Tick();
	_envelopeUI->Tick();
	_synthNoteParametersUI->Tick();
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
	_synthNoteParametersUI->FromUI(destination->GetSynthNoteParamters());
}

bool InputUI::HasPendingAction() const
{
	return _controlPanelUI->HasPendingAction();
}

void InputUI::ClearPendingAction()
{
	_oscillatorUI->ClearPendingAction();
	_envelopeUI->ClearPendingAction();
	_synthNoteParametersUI->ClearPendingAction();
	_controlPanelUI->ClearPendingAction();
}

bool InputUI::GetDirty() const
{
	return _oscillatorUI->GetDirty() || _envelopeUI->GetDirty() || _synthNoteParametersUI->GetDirty();
}

void InputUI::ClearDirty()
{
	_oscillatorUI->ClearDirty();
	_envelopeUI->ClearDirty();
	_synthNoteParametersUI->ClearDirty();
}

#endif