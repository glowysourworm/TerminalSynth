#pragma once

#ifndef SYNTH_INFORMATION_UI_H
#define SYNTH_INFORMATION_UI_H

#include "OutputSettings.h"
#include "UIBase.h"
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class SynthInformationUI : public UIBase<OutputSettings>
{
public:
	SynthInformationUI(const std::string& title, const ftxui::Color& titleColor);
	~SynthInformationUI();

	void Initialize(const OutputSettings& settings) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const OutputSettings& source) override;
	void FromUI(OutputSettings& destination, bool clearDirty) override;

private:

	ftxui::Component _component;

	std::string* _hostApi;
	std::string* _deviceName;
	std::string* _streamFormat;
	std::string* _streamBufferSize;
	std::string* _samplingRate;

	std::string* _averageUITime;
	std::string* _averageCallbackTime;
	std::string* _averageFrontendTime;
	std::string* _streamTime;
	std::string* _streamLatency;
};

SynthInformationUI::SynthInformationUI(const std::string& title, const ftxui::Color& titleColor)
	: UIBase(title, title, titleColor)
{
	_hostApi = new std::string("");
	_deviceName = new std::string("");
	_streamFormat = new std::string("");
	_streamBufferSize = new std::string("");
	_samplingRate = new std::string("");

	_averageUITime = new std::string("");
	_averageCallbackTime = new std::string("");
	_averageFrontendTime = new std::string("");
	_streamTime = new std::string("");
	_streamLatency = new std::string("");
}
SynthInformationUI::~SynthInformationUI()
{
	delete _hostApi;
	delete _deviceName;
	delete _streamFormat;
	delete _streamBufferSize;
	delete _samplingRate;

	delete _averageUITime;
	delete _averageCallbackTime;
	delete _averageFrontendTime;
	delete _streamTime;
	delete _streamLatency;
}
void SynthInformationUI::Initialize(const OutputSettings& settings)
{
	UIBase::Initialize(settings);

	// Synth Information
	auto synthInformation = ftxui::Container::Horizontal({

		ftxui::Renderer([&] {
			return ftxui::vbox(
			{
				ftxui::text("Host API:                 " + *_hostApi),
				ftxui::text("Device Name:              " + *_deviceName),
				ftxui::text("Stream Format:            " + *_streamFormat),
				ftxui::text("Stream Buffer Size:       " + *_streamBufferSize),
				ftxui::text("Sample Rate (Hz):         " + *_samplingRate)

			}) | ftxui::flex_grow;
		}),

		ftxui::Renderer([&] { return ftxui::separator(); }),

		ftxui::Renderer([&] {
			return ftxui::vbox(
			{
				ftxui::text(" Stream Time            (s):    " + *_streamTime),
				ftxui::text(" Avg. UI Time          (ms):    " + *_averageUITime),
				ftxui::text(" Avg. Callback Time    (ms):    " + *_averageCallbackTime),
				ftxui::text(" Avg. Frontend Time    (ms):    " + *_averageFrontendTime),
				ftxui::text(" Stream Latency   (samples):    " + *_streamLatency)

			}) | ftxui::flex_grow;
		})
	});

	_component = ftxui::Container::Vertical({

		// Synth Title
		ftxui::Renderer([&]
		{
			return ftxui::vbox({
				ftxui::text(this->GetLabel()) | ftxui::color(this->GetLabelColor()),
				ftxui::separator(),
			});
		}),

		// Synth Information
		synthInformation,

		// Footer
		ftxui::Renderer([&]
		{
			return ftxui::vbox({
				ftxui::separator()
			});
		})

	}) | ftxui::border;
}
ftxui::Component SynthInformationUI::GetComponent()
{
	return _component;
}
void SynthInformationUI::UpdateComponent(bool clearDirty)
{
	if (clearDirty)
		this->ClearDirty();
}
void SynthInformationUI::ToUI(const OutputSettings& source)
{
	_hostApi->clear();
	_hostApi->append(source.GetHostApi());

	_deviceName->clear();
	_deviceName->append(source.GetDeviceName());

	_streamFormat->clear();
	_streamFormat->append(source.GetDeviceFormat());

	_streamBufferSize->clear();
	_streamBufferSize->append(std::format("{} (frames)", source.GetOutputBufferFrameSize()));

	_samplingRate->clear();
	_samplingRate->append(std::to_string(source.GetSamplingRate()));

	_streamTime->clear();
	_streamTime->append(std::format("{:.3f}", source.GetStreamTime()));

	_averageUITime->clear();
	_averageUITime->append(std::format("{:.3f}", source.GetAvgUIMilli()));

	_averageCallbackTime->clear();
	_averageCallbackTime->append(std::format("{:.3f}", source.GetAvgAudioMilli()));

	_averageFrontendTime->clear();
	_averageFrontendTime->append(std::format("{:.3f}", source.GetAvgFrontendMilli()));

	_streamLatency->clear();
	_streamLatency->append(std::to_string(source.GetStreamLatency()));
}
void SynthInformationUI::FromUI(OutputSettings& destination, bool clearDirty)
{
	if (clearDirty)
		this->ClearDirty();
}

#endif