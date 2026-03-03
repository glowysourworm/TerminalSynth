#pragma once

#ifndef SLIDER_UI_H
#define SLIDER_UI_H

#include "UIBase.h"
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
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

    void ServicePendingAction() override;
    void UpdateComponent() override;
    void Tick() override;

    void ToUI(const float& source) override;
    void ToUI(const float* source) override;
    void FromUI(float& destination) override;
    void FromUI(float* destination) override;

    std::string GetName() const;

    bool HasPendingAction() const override;
    void ClearPendingAction() override;

    bool GetDirty() const override;
    void ClearDirty() override;

private:

    bool _isDirty;

	float* _value;
	float* _minValue;
	float* _maxValue;
	float* _increment;

    ftxui::Color* _titleColor;
    std::string* _name;
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
    const ftxui::Color& titleColor)
{
    _value = new float(initialValue);
    _minValue = new float(minValue);
    _maxValue = new float(maxValue);
    _increment = new float(increment);
    _labelFormat = new std::string(labelFormat);
    _label = new std::string();
    _name = new std::string(name);
    _titleColor = new ftxui::Color(titleColor);
    _isDirty = false;
}

SliderUI::~SliderUI()
{
    delete _value;
    delete _minValue;
    delete _maxValue;
    delete _increment;
    delete _labelFormat;
    delete _label;
    delete _name;
    delete _titleColor;
}

void SliderUI::Initialize(const float& initialValue)
{
    auto slider = ftxui::Slider<float>({
        .value = _value,
        .min = _minValue,
        .max = _maxValue,
        .increment = _increment,
        .color_active = *_titleColor,
        .on_change = [&] { _isDirty = true; }
    });

    // Create Component
    _component = ftxui::Container::Horizontal({
        ftxui::Renderer([&] { return ftxui::text(*_label); }),
        slider
    });
}

ftxui::Component SliderUI::GetComponent()
{
    return _component;
}

void SliderUI::ServicePendingAction()
{
}

void SliderUI::UpdateComponent()
{
    // Update Label
    auto labelFormat = std::string_view(_labelFormat->c_str());

    _label->erase(0, _label->size());
    _label->append(std::vformat(labelFormat, std::make_format_args(*_value)));
}

void SliderUI::Tick()
{
}

void SliderUI::FromUI(float& destination)
{
    destination = *_value;
}
void SliderUI::FromUI(float* destination)
{
    *destination = *_value;
}
void SliderUI::ToUI(const float& source)
{
    (*_value) = source;
}
void SliderUI::ToUI(const float* source)
{
    (*_value) = *source;
}
std::string SliderUI::GetName() const
{
    return *_name;
}

bool SliderUI::HasPendingAction() const
{
    return false;
}

void SliderUI::ClearPendingAction()
{
}

bool SliderUI::GetDirty() const
{
    return _isDirty;
}

void SliderUI::ClearDirty()
{
    _isDirty = false;
}
#endif