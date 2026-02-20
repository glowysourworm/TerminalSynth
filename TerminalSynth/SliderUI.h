#pragma once

#ifndef SLIDER_UI_H
#define SLIDER_UI_H

#include "UIBase.h"
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <string_view>

class SliderUI : public UIBase<float>
{
public:

	SliderUI(float initialValue,
			 float minValue,
			 float maxValue,
			 float increment,
             const std::string& name,
			 const std::string& labelFormat,
			 const ftxui::Color& titleColor);
	~SliderUI();

	void Initialize(const float& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

    void ToUI(const float& source) override;
    void FromUI(float& destination, bool clearDirty) override;


private:

	float* _value;
	float* _minValue;
	float* _maxValue;
	float* _increment;

	std::string* _label;
	std::string* _labelFormat;

	ftxui::Component _component;
};


SliderUI::SliderUI(float initialValue,
    float minValue,
    float maxValue,
    float increment,
    const std::string& name,
    const std::string& labelFormat,
    const ftxui::Color& titleColor) : UIBase(name, labelFormat, titleColor)
{
    _value = new float(initialValue);
    _minValue = new float(minValue);
    _maxValue = new float(maxValue);
    _increment = new float(increment);
    _labelFormat = new std::string(labelFormat);
    _label = new std::string();
}

SliderUI::~SliderUI()
{
    delete _value;
    delete _minValue;
    delete _maxValue;
    delete _increment;
    delete _labelFormat;
    delete _label;
}

void SliderUI::Initialize(const float& initialValue)
{
    UIBase::Initialize(initialValue);

    // Create Component
    _component = ftxui::Slider(_label, _value, _minValue, _maxValue, _increment)
        | ftxui::color(this->GetLabelColor())
        | ftxui::CatchEvent([&](ftxui::Event event)
        {
            // Only allow mouse events through
            if (event.mouse().button == ftxui::Mouse::Left &&
                event.is_mouse())
            {
                this->SetDirty();
                return false;
            }

            // Cancel keyboard events
            return true;
        });
}

ftxui::Component SliderUI::GetComponent()
{
    return _component;
}

void SliderUI::UpdateComponent(bool clearDirty)
{
    // Update Label
    auto labelFormat = std::string_view(_labelFormat->c_str());

    _label->erase(0, _label->size());
    _label->append(std::vformat(labelFormat, std::make_format_args(*_value)));

    if (clearDirty)
        this->ClearDirty();
}

void SliderUI::FromUI(float& destination, bool clearDirty)
{
    destination = *_value;

    if (clearDirty)
        this->ClearDirty();
}

void SliderUI::ToUI(const float& newValue)
{
    (*_value) = newValue;
}

#endif