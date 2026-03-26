#pragma once

#ifndef EFFECT_UI_H
#define EFFECT_UI_H

#include "SignalParameter.h"
#include "SignalParameterUI.h"
#include "SignalSettings.h"
#include "UIBase.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class EffectUI : public UIBase<SignalSettings>
{
public:

	EffectUI(const std::string& name, 
			 const std::string& category, 
			 const std::string& infoText, 
			 const ftxui::Color& labelColor);
	EffectUI(const SignalSettings& initialValue);
	~EffectUI();

	void Initialize(const SignalSettings& effect) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const SignalSettings& source) override;
	void ToUI(const SignalSettings* source) override;
	void FromUI(SignalSettings& destination) override;
	void FromUI(SignalSettings* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

private:

	void GetWhiteSpace(std::string& whiteSpace, int length);

private:

	ftxui::Component _component;

	std::string* _category;
	std::string* _infoText;
	std::string* _name;
	ftxui::Color* _labelColor;

	SignalSettings* _model;										// Local copy

	std::vector<SignalParameterUI*>* _parameterUIs;
};

EffectUI::EffectUI(const std::string& name,
					const std::string& category,
					const std::string& infoText,
					const ftxui::Color& labelColor)
{
	_parameterUIs = new std::vector<SignalParameterUI*>();
	_category = new std::string(category);
	_infoText = new std::string(infoText);
	_labelColor = new ftxui::Color(labelColor);
	_model = new SignalSettings();
	_name = new std::string(name);
}

EffectUI::EffectUI(const SignalSettings& initialValue)
{
	_parameterUIs = new std::vector<SignalParameterUI*>();
	_category = new std::string(initialValue.GetCategory());
	_infoText = new std::string(initialValue.GetInfoText());
	_labelColor = new ftxui::Color(ftxui::Color::White);
	_model = new SignalSettings(initialValue);
	_name = new std::string(initialValue.GetName());
}

EffectUI::~EffectUI()
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		delete _parameterUIs->at(index);
	}

	delete _model;
	delete _parameterUIs;
	delete _category;
	delete _infoText;
	delete _name;
	delete _labelColor;
}

void EffectUI::Initialize(const SignalSettings& parameters)
{
	int maxLabelLength = -1;

	_model->Update(&parameters, true);

	// Parameters (measure)
	for (int index = 0; index < parameters.GetParameterCount(); index++)
	{
		std::string labelStr = parameters.GetParameterName(index);

		// @_@
		if (maxLabelLength < labelStr.size() || maxLabelLength == -1)
			maxLabelLength = labelStr.size();
	}

	// Parameters
	for (int index = 0; index < parameters.GetParameterCount(); index++)
	{
		std::string labelStr = parameters.GetParameterName(index);
		std::string whiteSpace("");

		GetWhiteSpace(whiteSpace, maxLabelLength - labelStr.size());

		std::string labelFormat = labelStr + ":" + whiteSpace + "  {:.2f}";

		SignalParameterUI* parameterUI = new SignalParameterUI(*parameters.GetParameter(index), labelFormat);

		parameterUI->Initialize(*parameters.GetParameter(index));

		_parameterUIs->push_back(parameterUI);
	}

	_component = ftxui::Container::Vertical({
		ftxui::Renderer([&] { return ftxui::text(*_name) | ftxui::color(*_labelColor); }),
		ftxui::Renderer([&] { return ftxui::separator(); })
	});

	// We have to get components from our UI container for the vector - which will re-render each time.
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_component->Add(_parameterUIs->at(index)->GetComponent());

		if (index != _parameterUIs->size() - 1)
			_component->Add(ftxui::Renderer([] {return ftxui::separator(); }));
	}

	_component->Add(ftxui::Renderer([&] {
		return ftxui::separator();
	}));
	_component->Add(ftxui::Renderer([&] {
		return ftxui::paragraph(*_infoText);
	}));
}

ftxui::Component EffectUI::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render() | ftxui::flex_grow | ftxui::border;
	});
}

void EffectUI::ServicePendingAction()
{

}

void EffectUI::UpdateComponent()
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_parameterUIs->at(index)->UpdateComponent();
	}
}

void EffectUI::Tick()
{
	// Nothing in UI sub-functions
}

bool EffectUI::GetDirty() const
{
	bool isDirty = false;

	for (int index = 0; index < _parameterUIs->size() && !isDirty; index++)
	{
		isDirty |= _parameterUIs->at(index)->GetDirty();
	}

	return isDirty;
}

void EffectUI::ClearDirty()
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_parameterUIs->at(index)->ClearDirty();
	}
}

bool EffectUI::HasPendingAction() const
{
	return false;
}

void EffectUI::ClearPendingAction()
{
	
}

void EffectUI::ToUI(const SignalSettings& source)
{
}

void EffectUI::ToUI(const SignalSettings* source)
{
}

void EffectUI::FromUI(SignalSettings& destination)
{
	destination.SetName(*_name);
	this->FromUI(&destination);
}

void EffectUI::FromUI(SignalSettings* destination)
{
	if (destination->GetName() != *_name)
		throw new std::exception("Name mismatch EffectUI::FromUI");

	// Name (const), Category, Info Text
	// destination->SetName(*_name);
	destination->SetCategory(*_category);
	destination->SetInfoText(*_infoText);

	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		SignalParameter* parameter = _model->GetParameter(index);

		// -> FromUI
		_parameterUIs->at(index)->FromUI(parameter);

		// Add / Update
		if (destination->GetParameterCount() <= index)
			destination->AddParameter(*parameter);

		else if (destination->GetParameterName(index) != parameter->GetName())
			destination->AddParameter(*parameter);

		else
			destination->UpdateParameter(index, parameter);
	}
}

void EffectUI::GetWhiteSpace(std::string& whiteSpace, int length)
{
	whiteSpace.clear();
	for (int index = 0; index < length; index++)
	{
		whiteSpace.append(" ");
	}
}


#endif