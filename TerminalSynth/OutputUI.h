#pragma once

#ifndef OUTPUT_UI_H
#define OUTPUT_UI_H

#include "OutputModelUI.h"
#include "PlaybackInfo.h"
#include "SliderUI.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class OutputUI : public UIBase<OutputModelUI>
{
public:
	OutputUI(const std::string& title, const ftxui::Color& titleColor);
	~OutputUI();

	void Initialize(const OutputModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

    void ServicePendingAction() override;
    void UpdateComponent() override;
    void Tick() override;

	void ToUI(const OutputModelUI& source) override;
    void ToUI(const OutputModelUI* source) override;
	void FromUI(OutputModelUI& destination) override;
    void FromUI(OutputModelUI* destination) override;

	bool GetDirty() const override;
    void ClearDirty() override;

    bool HasPendingAction() const override;
    void ClearPendingAction() override;

private:

	SliderUI* _gainUI;
	SliderUI* _leftRightUI;

    ftxui::Component _deviceDropdown;

    ValueCapture<int>* _deviceSelectedIndex;
    std::vector<std::string>* _deviceList;

    std::vector<float*>* _leftEQ;
    std::vector<float*>* _rightEQ;

    std::string* _title;
    ftxui::Color* _titleColor;
};


OutputUI::OutputUI(const std::string& title, const ftxui::Color& titleColor) 
{
    _gainUI = new SliderUI(0.85f, 0.00f, 1.0f, 0.01f, "Gain", "Gain:       {:.2f}", titleColor, titleColor);
    _leftRightUI = new SliderUI(0.5f, 0.0f, 1.0f, 0.01f, "L/R", "L/R:        {:.2f}", titleColor, titleColor);

    _deviceList = new std::vector<std::string>();
    _deviceSelectedIndex = new ValueCapture<int>(0);

    _leftEQ = new std::vector<float*>();
    _rightEQ = new std::vector<float*>();

    _title = new std::string(title);
    _titleColor = new ftxui::Color(titleColor);
}

OutputUI::~OutputUI()
{
    for (int index = 0; index < _leftEQ->size(); index++)
    {
        delete _leftEQ->at(index);
        delete _rightEQ->at(index);
    }

    delete _gainUI;
    delete _leftRightUI;
    delete _deviceList;
    delete _deviceSelectedIndex;
    delete _title;
    delete _titleColor;
    delete _leftEQ;
    delete _rightEQ;
}

void OutputUI::Initialize(const OutputModelUI& settings)
{
    const PlaybackInfo* outputSettings = settings.GetPlaybackInfo();

    _gainUI->Initialize(settings.GetGain());
    _leftRightUI->Initialize(settings.GetLeftRightBalance());

    // Device List
    _deviceList->clear();
    for (int index = 0; index < settings.GetDeviceNameList().size(); index++)
    {
        _deviceList->push_back(settings.GetDeviceNameList().at(index));

        if (settings.GetDeviceNameList().at(index) == settings.GetSelectedDeviceName())
            _deviceSelectedIndex->SetValue(index);
    }

    // Equalizer
    for (int index = 0; index < settings.GetEqualizerOutput()->size(); index++)
    {
        _leftEQ->push_back(new float(0));
        _rightEQ->push_back(new float(0));
    }

    _deviceDropdown = ftxui::Dropdown(_deviceList, _deviceSelectedIndex->GetRef());
    _deviceSelectedIndex->Clear();
}

ftxui::Component OutputUI::GetComponent()
{
    // Output
    auto componentUI = ftxui::Container::Vertical(
    {
        ftxui::Renderer([&] { return ftxui::text(*_title) | ftxui::color(*_titleColor); }),
        ftxui::Renderer([&] {return ftxui::separator(); }),
        _deviceDropdown,
        ftxui::Renderer([&] {return ftxui::separator(); }),

        _gainUI->GetRenderer(),
        _leftRightUI->GetRenderer(),

        ftxui::Renderer([&] { return ftxui::separator(); }),

        ftxui::Renderer([&] { return ftxui::text("EQ Output") | ftxui::color(*_titleColor); }),
        ftxui::Renderer([&] {

            return ftxui::canvas([&](ftxui::Canvas& canvas) {

                // Split the X-Space into channels (L / R / Empty)
                //
                int marginX = 2;
                int divX = (int)((canvas.width() - (2 * marginX)) / (double)(_leftEQ->size()));
                int divXChannel = (int)(divX / 2.0f);
                int divXPadding = 1;
                int divCounter = 0;

                for (int i = 0; i < _leftEQ->size(); i++)
                {
                    for (int k = divXPadding; k < divXChannel - divXPadding; k++)
                    {
                        int leftX = marginX + (i * divX) + k;
                        int rightX = marginX + (i * divX) + divXChannel + k;

                        float colorMult = 255 / (float)canvas.height();

                        for (int j = 0; j < canvas.height(); j++)
                        {
                            // Left Channel
                            if (j > (1 - *_leftEQ->at(i)) * canvas.height())
                                canvas.DrawBlock(leftX, j, true, ftxui::Color::RGB(255 - (j * colorMult), 0, j * colorMult));

                            // Right Channel
                            if (j > (1 - *_rightEQ->at(i)) * canvas.height())
                                canvas.DrawBlock(rightX, j, true, ftxui::Color::RGB(255 - (j * colorMult), 0, j * colorMult));
                        }
                    }
                }

            }) | ftxui::bgcolor(ftxui::Color::RGBA(0, 0, 255, 50)) | ftxui::flex_grow;

        }) | ftxui::flex_grow | ftxui::border

    }) | ftxui::bgcolor(ftxui::Color::RGB(0,0,0));

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
    return _gainUI->GetDirty() || _leftRightUI->GetDirty() || _deviceSelectedIndex->HasChanged();
}

void OutputUI::ClearDirty()
{
    _gainUI->ClearDirty();
    _leftRightUI->ClearDirty();

    _deviceSelectedIndex->Clear();
}

bool OutputUI::HasPendingAction() const
{
    return false;
}

void OutputUI::ClearPendingAction()
{
}

void OutputUI::ToUI(const OutputModelUI& source)
{
    throw new std::exception("Please use the pointer version of this function ToUI");
}
void OutputUI::ToUI(const OutputModelUI* source)
{
    _gainUI->ToUI(source->GetGain());
    _leftRightUI->ToUI(source->GetLeftRightBalance());

    for (int index = 0; index < source->GetEqualizerOutput()->size(); index++)
    {
        *(_leftEQ->at(index)) = source->GetEqualizerOutput()->at(index).GetLeft();
        *(_rightEQ->at(index)) = source->GetEqualizerOutput()->at(index).GetRight();
    }
}
void OutputUI::FromUI(OutputModelUI& destination)
{
    throw new std::exception("Please use the pointer version of this function FromUI");
}

void OutputUI::FromUI(OutputModelUI* destination)
{
    float gain;
    float leftRight;

    _gainUI->FromUI(gain);
    _leftRightUI->FromUI(leftRight);

    destination->SetLeftRightBalance(leftRight);
    destination->SetGain(gain);

    // OUTPUT DEVICE!
    if (_deviceSelectedIndex->HasChanged())
        destination->SetSelectedDeviceName(_deviceList->at(_deviceSelectedIndex->GetValue()));
}

#endif