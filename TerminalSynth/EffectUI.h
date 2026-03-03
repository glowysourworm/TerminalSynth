#pragma once

#ifndef EFFECT_UI_H
#define EFFECT_UI_H

#include "SignalParameter.h"
#include "SignalSettings.h"
#include "SliderUI.h"
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
	void UpdateComponent() override;

	void ToUI(const SignalSettings& source) override;
	void ToUI(const SignalSettings* source) override;
	void FromUI(SignalSettings& destination) override;
	void FromUI(SignalSettings* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	void GetWhiteSpace(std::string& whiteSpace, int length);

private:

	ftxui::Component _component;

	std::string* _category;
	std::string* _infoText;
	std::string* _name;
	ftxui::Color* _labelColor;

	std::vector<SliderUI*>* _parameterUIs;
};

EffectUI::EffectUI(const std::string& name,
					const std::string& category,
					const std::string& infoText,
					const ftxui::Color& labelColor)
{
	_parameterUIs = new std::vector<SliderUI*>();
	_category = new std::string(category);
	_infoText = new std::string(infoText);
	_labelColor = new ftxui::Color(labelColor);
	_name = new std::string(name);
}

EffectUI::EffectUI(const SignalSettings& initialValue)
{
	_parameterUIs = new std::vector<SliderUI*>();
	_category = new std::string(initialValue.GetCategory());
	_infoText = new std::string(initialValue.GetInfoText());
	_labelColor = new ftxui::Color(ftxui::Color::White);
	_name = new std::string(initialValue.GetName());
}

EffectUI::~EffectUI()
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		delete _parameterUIs->at(index);
	}

	delete _parameterUIs;
	delete _category;
	delete _infoText;
	delete _name;
	delete _labelColor;
}

void EffectUI::Initialize(const SignalSettings& parameters)
{
	int maxLabelLength = -1;

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

		float initialValue = parameters.GetParameterValue(index);
		float minValue = parameters.GetParameterMin(index);
		float maxValue = parameters.GetParameterMax(index);

		SliderUI* sliderUI = new SliderUI(initialValue, minValue, maxValue, (maxValue - minValue) / 100.0f, labelStr, labelFormat, *_labelColor);

		sliderUI->Initialize(initialValue);

		_parameterUIs->push_back(sliderUI);
	}

	_component = ftxui::Container::Vertical({
		ftxui::Renderer([&] { return ftxui::text(*_name) | ftxui::color(*_labelColor); }),
		ftxui::Renderer([&] { return ftxui::separator(); })
	});

	// We have to get components from our UI container for the vector - which will re-render each time.
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_component->Add(_parameterUIs->at(index)->GetComponent());
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

void EffectUI::UpdateComponent()
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_parameterUIs->at(index)->UpdateComponent();
	}
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

void EffectUI::ToUI(const SignalSettings& source)
{
}

void EffectUI::ToUI(const SignalSettings* source)
{
}

void EffectUI::FromUI(SignalSettings& destination)
{
	throw new std::exception("Please use the pointer version of this function FromUI");
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
		// NEED ANOTHER STRING PARAMETER
		SignalParameter parameter(_parameterUIs->at(index)->GetName(), 0.0f, 0.0f, 1.0f);

		float setting = 0;

		// Get UI Setting
		_parameterUIs->at(index)->FromUI(setting);

		parameter.SetValue(setting);

		// -> (set by index)
		destination->AddParameter(parameter);
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