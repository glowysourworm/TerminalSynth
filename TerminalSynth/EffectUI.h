#pragma once

#ifndef EFFECT_UI_H
#define EFFECT_UI_H

#include "SignalParameter.h"
#include "SignalSettings.h"
#include "SliderUI.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class EffectUI : public UIBase<SignalSettings>
{
public:

	EffectUI(const std::string& name, const std::string& label, const ftxui::Color& labelColor);
	~EffectUI();

	void Initialize(const SignalSettings& effect) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const SignalSettings& source) override;
	void FromUI(SignalSettings& destination, bool clearDirty) override;

	bool GetDirty() const override;

private:

	void GetWhiteSpace(std::string& whiteSpace, int length);

private:

	ftxui::Component _component;

	std::vector<SliderUI*>* _parameterUIs;
};

EffectUI::EffectUI(const std::string& name, const std::string& label, const ftxui::Color& labelColor)
	: UIBase(name, label, labelColor)
{
	_parameterUIs = new std::vector<SliderUI*>();
}

EffectUI::~EffectUI()
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		delete _parameterUIs->at(index);
	}

	delete _parameterUIs;
}

void EffectUI::Initialize(const SignalSettings& parameters)
{
	UIBase::Initialize(parameters);

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

		SliderUI* sliderUI = new SliderUI(initialValue, minValue, maxValue, (maxValue - minValue) / 100.0f, labelStr, labelFormat, this->GetLabelColor());

		sliderUI->Initialize(initialValue);

		_parameterUIs->push_back(sliderUI);
	}

	_component = ftxui::Container::Vertical({
		ftxui::Renderer([&] { return ftxui::text(this->GetLabel()) | ftxui::color(this->GetLabelColor()); }),
		ftxui::Renderer([&] { return ftxui::separator(); })
		});

	// We have to get components from our UI container for the vector - which will re-render each time.
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_component->Add(_parameterUIs->at(index)->GetComponent());
	}
}

ftxui::Component EffectUI::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render() | ftxui::flex_grow | ftxui::border;
	});
}

void EffectUI::UpdateComponent(bool clearDirty)
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		_parameterUIs->at(index)->UpdateComponent(clearDirty);
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

void EffectUI::ToUI(const SignalSettings& source)
{
}

void EffectUI::FromUI(SignalSettings& destination, bool clearDirty)
{
	for (int index = 0; index < _parameterUIs->size(); index++)
	{
		// NEED ANOTHER STRING PARAMETER
		SignalParameter parameter(_parameterUIs->at(index)->GetName(), 0.0f, 0.0f, 1.0f);

		float setting = 0;

		// Get UI Setting
		_parameterUIs->at(index)->FromUI(setting, clearDirty);
		
		parameter.SetValue(setting);

		// -> (set by index)
		destination.AddParameter(parameter);
	}

	if (clearDirty)
		this->ClearDirty();
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