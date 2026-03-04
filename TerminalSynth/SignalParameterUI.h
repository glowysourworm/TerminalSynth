#pragma once

#ifndef SIGNALPARAMETER_UI_H
#define SIGNALPARAMETER_UI_H

#include "CheckboxModelUI.h"
#include "CheckboxUI.h"
#include "Constant.h"
#include "SignalParameter.h"
#include "SliderUI.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class SignalParameterUI : public UIBase<SignalParameter>
{
public:

	SignalParameterUI(const SignalParameter& model, const std::string& labelFormat);
	~SignalParameterUI();

	void Initialize(const SignalParameter& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const SignalParameter& source) override;
	void ToUI(const SignalParameter* source) override;
	void FromUI(SignalParameter& destination) override;
	void FromUI(SignalParameter* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

private:

	const float AUTOMATION_FREQ_MIN = 0.05f;
	const float AUTOMATION_FREQ_MAX = 2.0f;

	ftxui::Component _component;

	SignalParameter* _model;									// Local copy
	CheckboxModelUI* _automationEnableModel;

	CheckboxUI* _automationEnableUI;
	SliderUI* _valueUI;
	SliderUI* _automationLowUI;
	SliderUI* _automationHighUI;
	SliderUI* _automationFrequencyUI;

	ValueCapture<int>* _automationTypeSelectedIndex;
	ValueCapture<int>* _automationOscillatorSelectedIndex;

	std::vector<std::string>* _automationTypeChoices;
	std::vector<std::string>* _automationOscillatorChoices;
};

SignalParameterUI::SignalParameterUI(const SignalParameter& model, const std::string& labelFormat)
{
	_automationTypeSelectedIndex = new ValueCapture<int>(0);
	_automationOscillatorSelectedIndex = new ValueCapture<int>(0);

	_model = new SignalParameter(model);
	_automationEnableModel = new CheckboxModelUI("Automate", model.GetAutomationEnabled(), 0);

	_automationEnableUI = new CheckboxUI(*_automationEnableModel);
	_valueUI = new SliderUI(model.GetValue(), model.GetMin(), model.GetMax(), (model.GetMax() - model.GetMin()) / 100.0f, model.GetName(), labelFormat, ftxui::Color::White);
	_automationLowUI = new SliderUI(model.GetAutomationLow(), model.GetMin(), model.GetMax(), (model.GetMax() - model.GetMin()) / 100.0f, "Low", "Low    {:.2f}", ftxui::Color::White);
	_automationHighUI = new SliderUI(model.GetAutomationHigh(), model.GetMin(), model.GetMax(), (model.GetMax() - model.GetMin()) / 100.0f, "High", "High   {:.2f}", ftxui::Color::White);
	_automationFrequencyUI = new SliderUI(model.GetAutomationFrequency(), AUTOMATION_FREQ_MIN, AUTOMATION_FREQ_MAX, AUTOMATION_FREQ_MIN , "Freq", "Freq   {:.2f}", ftxui::Color::White);

	_automationTypeChoices = new std::vector<std::string>({
		"EnvelopeSweep",
		"Oscillator"	
	});
	_automationOscillatorChoices = new std::vector<std::string>({
		"Sine",
		"Square",
		"Triangle",
		"Sawtooth",
		"Random"
	});
}

SignalParameterUI::~SignalParameterUI()
{
	delete _automationTypeSelectedIndex;
	delete _automationOscillatorSelectedIndex;
	delete _automationTypeChoices;
	delete _automationOscillatorChoices;
	delete _automationEnableUI;
	delete _valueUI;
	delete _automationLowUI;
	delete _automationHighUI;
	delete _automationFrequencyUI;
}

void SignalParameterUI::Initialize(const SignalParameter& initialValue)
{
	_model->Update(&initialValue, false);
	_automationEnableModel->SetIsChecked(initialValue.GetAutomationEnabled());

	_valueUI->Initialize(initialValue.GetValue());
	_automationEnableUI->Initialize(*_automationEnableModel);
	_automationLowUI->Initialize(initialValue.GetAutomationLow());
	_automationHighUI->Initialize(initialValue.GetAutomationHigh());
	_automationFrequencyUI->Initialize(initialValue.GetAutomationFrequency());

	auto automationType = ftxui::Dropdown(_automationTypeChoices, _automationTypeSelectedIndex->GetRef());
	auto automationOscillatorType = ftxui::Dropdown(_automationOscillatorChoices, _automationOscillatorSelectedIndex->GetRef());

	// Signal Parameter:  Show the [ slider, automation enable], (separator), [automation settings]
	_component = ftxui::Container::Vertical({

		ftxui::Container::Horizontal({

			_valueUI->GetComponent() | ftxui::xflex_grow,
			_automationEnableUI->GetComponent()

		})  | ftxui::xflex_grow,

		// Parameter Automation
		ftxui::Renderer([&] { return ftxui::separator(); }) 
			| ftxui::Maybe([&]() { return _automationEnableUI->GetIsChecked(); }) 
			| ftxui::flex_grow,

		ftxui::Container::Vertical({

			ftxui::Container::Horizontal({

				automationType,
				automationOscillatorType | ftxui::Maybe([&]() { return _automationTypeSelectedIndex->GetValue() == (int)ParameterAutomationType::Oscillator; })

			}) | ftxui::Maybe([&]() { return _automationEnableUI->GetIsChecked(); })
   			   | ftxui::flex_grow,

			_automationFrequencyUI->GetComponent()
				| ftxui::Maybe([&]() { return _automationEnableUI->GetIsChecked() && _automationTypeSelectedIndex->GetValue() == (int)ParameterAutomationType::Oscillator; })
				| ftxui::flex_grow,

			_automationLowUI->GetComponent()
				| ftxui::Maybe([&]() { return _automationEnableUI->GetIsChecked(); })
				| ftxui::flex_grow,

			_automationHighUI->GetComponent()
				| ftxui::Maybe([&]() { return _automationEnableUI->GetIsChecked(); })
				| ftxui::flex_grow

		}) | ftxui::bgcolor(ftxui::Color::RGBA(0, 0, 255, 20))

	}) | ftxui::xflex_grow | ftxui::bgcolor(ftxui::Color::RGB(0, 0, 0));
}

ftxui::Component SignalParameterUI::GetComponent()
{
	return _component;
	//return ftxui::Renderer(_component, [&] {
	//	return _component->Render();
	//});
}

void SignalParameterUI::ServicePendingAction()
{
}

void SignalParameterUI::UpdateComponent()
{
	_automationLowUI->UpdateComponent();
	_automationHighUI->UpdateComponent();
	_valueUI->UpdateComponent();
	_automationEnableUI->UpdateComponent();
	_automationFrequencyUI->UpdateComponent();
}

void SignalParameterUI::Tick()
{

}

void SignalParameterUI::ToUI(const SignalParameter& source)
{
	throw new std::exception("Please use  the pointer version of this function ToUI");
}

void SignalParameterUI::ToUI(const SignalParameter* source)
{
	_model->Update(source, true);		// Overwrite allows changing the parameter name
	
	_valueUI->ToUI(source->GetValue());

	_automationEnableModel->SetIsChecked(source->GetAutomationEnabled());
	_automationEnableUI->ToUI(_automationEnableModel);
	_automationTypeSelectedIndex->SetValue((int)source->GetAutomationType());
	_automationOscillatorSelectedIndex->SetValue((int)source->GetAutomationOscillator());
	_automationLowUI->ToUI(source->GetAutomationLow());
	_automationHighUI->ToUI(source->GetAutomationHigh());
	_automationFrequencyUI->ToUI(source->GetAutomationFrequency());
}

void SignalParameterUI::FromUI(SignalParameter& destination)
{
	throw new std::exception("Please use  the pointer version of this function FromUI");
}

void SignalParameterUI::FromUI(SignalParameter* destination)
{
	// Update Model
	float value, automationLow, automationHigh, automationFrequency;

	_valueUI->FromUI(value);
	_automationEnableUI->FromUI(_automationEnableModel);
	_automationLowUI->FromUI(automationLow);
	_automationHighUI->FromUI(automationHigh);
	_automationFrequencyUI->FromUI(automationFrequency);

	_model->SetValue(value);
	_model->SetAutomationEnabled(_automationEnableModel->GetIsChecked());
	_model->SetAutomationType((ParameterAutomationType)_automationTypeSelectedIndex->GetValue());
	_model->SetAutomationOscillator((ParameterAutomationOscillator)_automationOscillatorSelectedIndex->GetValue());
	_model->SetAutomationLow(automationLow);
	_model->SetAutomationHigh(automationHigh);
	_model->SetAutomationFrequency(automationFrequency);

	destination->Update(_model, false);
}

bool SignalParameterUI::GetDirty() const
{
	return _automationTypeSelectedIndex->HasChanged() ||
		_automationOscillatorSelectedIndex->HasChanged() ||
		_automationEnableUI->GetDirty() ||
		_valueUI->GetDirty() ||
		_automationLowUI->GetDirty() ||
		_automationHighUI->GetDirty() ||
		_automationFrequencyUI->GetDirty();
}

void SignalParameterUI::ClearDirty()
{
	_automationTypeSelectedIndex->Clear();
	_automationOscillatorSelectedIndex->Clear();
	_automationLowUI->ClearDirty();
	_automationHighUI->ClearDirty();
	_automationEnableUI->ClearDirty();
	_valueUI->ClearDirty();
	_automationFrequencyUI->ClearDirty();
}

bool SignalParameterUI::HasPendingAction() const
{
	return false;
}

void SignalParameterUI::ClearPendingAction()
{
	
}

#endif