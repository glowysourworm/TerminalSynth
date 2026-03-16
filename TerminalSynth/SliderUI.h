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
             const std::string& label,
             const ftxui::Color& titleColor,
             const ftxui::Color& titleColorActive);
    SliderUI(float initialValue,
            float minValue,
            float maxValue,
            float increment,
            const std::string& label,
            const std::string& labelFormat,
            const ftxui::Color& titleColor,
            const ftxui::Color& titleColorActive);
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

public:

    void SetLabel(const std::string& label);

private:

    SliderUI(float initialValue,
        float minValue,
        float maxValue,
        float increment,
        const std::string& label,
        const std::string& labelFormat,
        const ftxui::Color& titleColor,
        const ftxui::Color& titleColorActive,
        bool useLabelFormat);

private:

    bool _isDirty;
    bool _useLabelFormat;

	float* _value;
	float* _minValue;
	float* _maxValue;
	float* _increment;

    ftxui::Color* _titleColor;
    ftxui::Color* _titleColorActive;
	std::string* _label;
    std::string* _labelFormat;

	ftxui::Component _component;
};


SliderUI::SliderUI(float initialValue, 
    float minValue,
    float maxValue,
    float increment,
    const std::string& label,
    const ftxui::Color& titleColor,
    const ftxui::Color& titleColorActive)
    : SliderUI(initialValue, minValue, maxValue, increment, label, label, titleColor, titleColorActive, false)
{}
SliderUI::SliderUI(float initialValue,
    float minValue,
    float maxValue,
    float increment,
    const std::string& label,
    const std::string& labelFormat,
    const ftxui::Color& titleColor,
    const ftxui::Color& titleColorActive)
    : SliderUI(initialValue, minValue, maxValue, increment, label, labelFormat, titleColor, titleColorActive, true)
{}
SliderUI::SliderUI(float initialValue,
    float minValue,
    float maxValue,
    float increment,
    const std::string& label,
    const std::string& labelFormat,
    const ftxui::Color& titleColor,
    const ftxui::Color& titleColorActive,
    bool useLabelFormat)
{
    _value = new float(initialValue);
    _minValue = new float(minValue);
    _maxValue = new float(maxValue);
    _increment = new float(increment);
    _label = new std::string(label);
    _labelFormat = new std::string(labelFormat);
    _titleColor = new ftxui::Color(titleColor);
    _titleColorActive = new ftxui::Color(titleColorActive);
    _isDirty = false;
    _useLabelFormat = useLabelFormat;
}

SliderUI::~SliderUI()
{
    delete _value;
    delete _minValue;
    delete _maxValue;
    delete _increment;
    delete _label;
    delete _labelFormat;
    delete _titleColor;
    delete _titleColorActive;
}

void SliderUI::Initialize(const float& initialValue)
{
    auto slider = ftxui::Slider<float>({
        .value = _value,
        .min = _minValue,
        .max = _maxValue,
        .increment = _increment,
        .color_active = *_titleColorActive,
        .color_inactive = *_titleColor,        
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
    if (_useLabelFormat)
    {
        std::string_view labelFormat(_labelFormat->c_str());

        _label->clear();
        _label->append(std::vformat(labelFormat, std::make_format_args(*_value)));
    }
}

void SliderUI::Tick()
{
}
void SliderUI::SetLabel(const std::string& label)
{
    _label->clear();
    _label->append(label);
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
    return *_label;
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