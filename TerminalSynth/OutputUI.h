#pragma once

#ifndef OUTPUT_UI_H
#define OUTPUT_UI_H

#include "SliderUI.h"
#include "SynthSettings.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class OutputUI : public UIBase<SynthSettings>
{
public:
	OutputUI(const std::string& title, const ftxui::Color& titleColor);
	~OutputUI();

	void Initialize(const SynthSettings& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const SynthSettings& source) override;
	void FromUI(SynthSettings& destination, bool clearDirty) override;

	bool GetDirty() const override;

private:

	SliderUI* _gainUI;
	SliderUI* _leftRightUI;

	float _left;
	float _right;
};


OutputUI::OutputUI(const std::string& title, const ftxui::Color& titleColor) 
    : UIBase(title, title, titleColor)
{
    _gainUI = new SliderUI(0.85f, 0.00f, 1.0f, 0.01f, "Gain", "Gain:       {:.2f}", titleColor);
    _leftRightUI = new SliderUI(0.5f, 0.0f, 1.0f, 0.01f, "L/R", "L/R:        {:.2f}", titleColor);

    _left = 0;
    _right = 0;
}

OutputUI::~OutputUI()
{
    delete _gainUI;
    delete _leftRightUI;
}

void OutputUI::Initialize(const SynthSettings& initialValue)
{
    UIBase::Initialize(initialValue);

    _gainUI->Initialize(initialValue.GetOutputGain());
    _leftRightUI->Initialize(initialValue.GetOutputLeftRight());
}

ftxui::Component OutputUI::GetComponent()
{
    // Output
    auto componentUI = ftxui::Container::Vertical(
    {
        ftxui::Renderer([&] { return ftxui::text(this->GetLabel()) | ftxui::color(this->GetLabelColor()); }),
        ftxui::Renderer([&] {return ftxui::separator(); }),

        _gainUI->GetRenderer(),
        _leftRightUI->GetRenderer(),

        ftxui::Renderer([&] {
            return ftxui::vbox({
                ftxui::separator(),
                ftxui::hbox({
                    ftxui::text("Left ") | ftxui::center,
                    ftxui::border(ftxui::gauge(_left) | ftxui::color(this->GetLabelColor()))
                }),
                ftxui::hbox({
                    ftxui::text("Right") | ftxui::center,
                    ftxui::border(ftxui::gauge(_right) | ftxui::color(this->GetLabelColor()))
                })
            });
        })
    });

    return componentUI;
}

void OutputUI::UpdateComponent(bool clearDirty)
{
    _gainUI->UpdateComponent(clearDirty);
    _leftRightUI->UpdateComponent(clearDirty);

    if (clearDirty)
        this->ClearDirty();
}

bool OutputUI::GetDirty() const
{
    return _gainUI->GetDirty() || _leftRightUI->GetDirty();
}

void OutputUI::ToUI(const SynthSettings& source)
{
    _gainUI->ToUI(source.GetOutputGain());
    _leftRightUI->ToUI(source.GetOutputLeftRight());
}
void OutputUI::FromUI(SynthSettings& destination, bool clearDirty)
{
    float gain;
    float leftRight;

    _gainUI->FromUI(gain, clearDirty);
    _leftRightUI->FromUI(leftRight, clearDirty);

    destination.SetOutputGain(gain);
    destination.SetOutputLeftRight(leftRight);

    if (clearDirty)
        this->ClearDirty();
}

#endif