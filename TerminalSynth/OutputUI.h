#pragma once

#ifndef OUTPUT_UI_H
#define OUTPUT_UI_H

#include "SliderUI.h"
#include "SynthSettings.h"
#include "UIBase.h"
#include <exception>
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

    void ServicePendingAction() override;
    void UpdateComponent() override;
    void Tick() override;

	void ToUI(const SynthSettings& source) override;
    void ToUI(const SynthSettings* source) override;
	void FromUI(SynthSettings& destination) override;
    void FromUI(SynthSettings* destination) override;

	bool GetDirty() const override;
    void ClearDirty() override;

    bool HasPendingAction() const override;
    void ClearPendingAction() override;

private:

	SliderUI* _gainUI;
	SliderUI* _leftRightUI;

	float _left;
	float _right;

    std::string* _title;
    ftxui::Color* _titleColor;
};


OutputUI::OutputUI(const std::string& title, const ftxui::Color& titleColor) 
{
    _gainUI = new SliderUI(0.85f, 0.00f, 1.0f, 0.01f, "Gain", "Gain:       {:.2f}", titleColor);
    _leftRightUI = new SliderUI(0.5f, 0.0f, 1.0f, 0.01f, "L/R", "L/R:        {:.2f}", titleColor);

    _left = 0;
    _right = 0;

    _title = new std::string(title);
    _titleColor = new ftxui::Color(titleColor);
}

OutputUI::~OutputUI()
{
    delete _gainUI;
    delete _leftRightUI;
    delete _title;
    delete _titleColor;
}

void OutputUI::Initialize(const SynthSettings& initialValue)
{
    _gainUI->Initialize(initialValue.GetOutputGain());
    _leftRightUI->Initialize(initialValue.GetOutputLeftRight());
}

ftxui::Component OutputUI::GetComponent()
{
    // Output
    auto componentUI = ftxui::Container::Vertical(
    {
        ftxui::Renderer([&] { return ftxui::text(*_title) | ftxui::color(*_titleColor); }),
        ftxui::Renderer([&] {return ftxui::separator(); }),

        _gainUI->GetRenderer(),
        _leftRightUI->GetRenderer(),

        ftxui::Renderer([&] {
            return ftxui::vbox({
                ftxui::separator(),
                ftxui::hbox({
                    ftxui::text("Left ") | ftxui::center,
                    ftxui::border(ftxui::gauge(_left) | ftxui::color(*_titleColor))
                }),
                ftxui::hbox({
                    ftxui::text("Right") | ftxui::center,
                    ftxui::border(ftxui::gauge(_right) | ftxui::color(*_titleColor))
                })
            });
        })
    });

    return componentUI;
}

void OutputUI::ServicePendingAction()
{
}

void OutputUI::UpdateComponent()
{
    _gainUI->UpdateComponent();
    _leftRightUI->UpdateComponent();
}

void OutputUI::Tick()
{
}

bool OutputUI::GetDirty() const
{
    return _gainUI->GetDirty() || _leftRightUI->GetDirty();
}

void OutputUI::ClearDirty()
{
    _gainUI->ClearDirty();
    _leftRightUI->ClearDirty();
}

bool OutputUI::HasPendingAction() const
{
    return false;
}

void OutputUI::ClearPendingAction()
{
}

void OutputUI::ToUI(const SynthSettings& source)
{
    throw new std::exception("Please use the pointer version of this function ToUI");
}
void OutputUI::ToUI(const SynthSettings* source)
{
    _gainUI->ToUI(source->GetOutputGain());
    _leftRightUI->ToUI(source->GetOutputLeftRight());
}
void OutputUI::FromUI(SynthSettings& destination)
{
    throw new std::exception("Please use the pointer version of this function FromUI");
}

void OutputUI::FromUI(SynthSettings* destination)
{
    float gain;
    float leftRight;

    _gainUI->FromUI(gain);
    _leftRightUI->FromUI(leftRight);

    destination->SetOutputGain(gain);
    destination->SetOutputLeftRight(leftRight);
}

#endif