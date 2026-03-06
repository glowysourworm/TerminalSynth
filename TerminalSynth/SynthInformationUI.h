#pragma once

#ifndef SYNTH_INFORMATION_UI_H
#define SYNTH_INFORMATION_UI_H

#include "OutputModelUI.h"
#include "OutputSettings.h"
#include "OutputUI.h"
#include "UIBase.h"
#include <exception>
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class SynthInformationUI : public UIBase<OutputModelUI>
{
public:
	SynthInformationUI(const std::string& title, const ftxui::Color& titleColor);
	~SynthInformationUI();

	void Initialize(const OutputModelUI& settings) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const OutputModelUI& source) override;
	void ToUI(const OutputModelUI* source) override;
	void FromUI(OutputModelUI& destination) override;
	void FromUI(OutputModelUI* destination) override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	ftxui::Component _component;

	int* _synthInfoWidth;

	std::string* _hostApi;
	std::string* _deviceName;
	std::string* _streamFormat;
	std::string* _streamBufferSize;
	std::string* _samplingRate;

	std::string* _streamTime;
	std::string* _averageAudioMilli;
	std::string* _averageAudioSampleMicro;
	std::string* _averageAudioLockAcquireNano;
	std::string* _streamLatency;

	std::string* _averageUIMilli;
	std::string* _averageUIDataFetchMicro;
	std::string* _averageUILockAcqcuireNano;
	std::string* _averageUIRenderingMilli;
	std::string* _averageUISleepMilli;

	std::string* _title;
	ftxui::Color* _titleColor;

	// Output Tab
	OutputUI* _outputUI;
};

SynthInformationUI::SynthInformationUI(const std::string& title, const ftxui::Color& titleColor)
{
	_synthInfoWidth = new int(30);

	_hostApi = new std::string("");
	_deviceName = new std::string("");
	_streamFormat = new std::string("");
	_streamBufferSize = new std::string("");
	_samplingRate = new std::string("");

	_streamTime = new std::string("");
	_averageAudioMilli = new std::string("");
	_averageAudioSampleMicro = new std::string("");
	_averageAudioLockAcquireNano = new std::string("");
	_streamLatency = new std::string("");

	_averageUIMilli = new std::string("");
	_averageUIDataFetchMicro = new std::string("");
	_averageUILockAcqcuireNano = new std::string("");
	_averageUIRenderingMilli = new std::string("");
	_averageUISleepMilli = new std::string("");

	_title = new std::string(title);
	_titleColor = new ftxui::Color(titleColor);

	_outputUI = new OutputUI("Output", ftxui::Color::GreenYellow);
}
SynthInformationUI::~SynthInformationUI()
{
	delete _synthInfoWidth;

	delete _hostApi;
	delete _deviceName;
	delete _streamFormat;
	delete _streamBufferSize;
	delete _samplingRate;

	delete _streamTime;
	delete _averageAudioMilli;
	delete _averageAudioSampleMicro;
	delete _averageAudioLockAcquireNano;
	delete _streamLatency;

	delete _averageUIMilli;
	delete _averageUIDataFetchMicro;
	delete _averageUILockAcqcuireNano;
	delete _averageUIRenderingMilli;
	delete _averageUISleepMilli;

	delete _title;
	delete _titleColor;

	delete _outputUI;
}
void SynthInformationUI::Initialize(const OutputModelUI& settings)
{
	_outputUI->Initialize(settings);

	auto synthInformation = ftxui::ResizableSplitLeft(
		ftxui::Renderer([&]{

			return ftxui::vbox({
				ftxui::text("Special Thanks!") | ftxui::color(*_titleColor),
				ftxui::separator(),
				ftxui::text("Airwindows:"),
				ftxui::text("FTXUI:"),
				ftxui::text("RT Audio:"),
				ftxui::text("libsndfile:"),
				ftxui::separator(),

				ftxui::text("RT Playback") | ftxui::color(*_titleColor),
				ftxui::separator(),
				ftxui::text("Host API:"),
				ftxui::text("Device Name:"),
				ftxui::text("Stream Format:"),
				ftxui::text("Stream Buffer Size:"),
				ftxui::text("Sample Rate (Hz):"),
				ftxui::separator(),

				ftxui::text("Metrics (Audio)") | ftxui::color(*_titleColor),
				ftxui::separator(),
				ftxui::text("Stream Time (s):"),
				ftxui::text("Avg. Loop Time (ms):"),
				ftxui::text("Avg. Sample Time (us):"),
				ftxui::text("Avg. Lock Acquire (ns):"),
				ftxui::text("Stream Latency (samples):"),
				ftxui::separator(),

				ftxui::text("Metrics (UI)") | ftxui::color(*_titleColor),
				ftxui::separator(),
				ftxui::text("Avg. Loop Time (ms):"),
				ftxui::text("Avg. Data Fetch (us):"),
				ftxui::text("Avg. Lock Acquire (ns):"),
				ftxui::text("Avg. Rendering Time (ms):"),
				ftxui::text("Avg. Sleep Time (ms):")
			});

		}), ftxui::Renderer([&]{

			return ftxui::vbox({
				ftxui::text(""),
				ftxui::separator(),
				ftxui::text("airwindows@github.com") | ftxui::color(ftxui::Color::BlueLight) | ftxui::align_right,
				ftxui::text("ArthurSonzogni@github.com") | ftxui::color(ftxui::Color::BlueLight) | ftxui::align_right,
				ftxui::text("thestk@github.com") | ftxui::color(ftxui::Color::BlueLight) | ftxui::align_right,
				ftxui::text("libsndfile@github.com") | ftxui::color(ftxui::Color::BlueLight) | ftxui::align_right,
				ftxui::separator(),

				ftxui::text(""),
				ftxui::separator(),
				ftxui::text(*_hostApi),
				ftxui::text(*_deviceName),
				ftxui::text(*_streamFormat),
				ftxui::text(*_streamBufferSize),
				ftxui::text(*_samplingRate),
				ftxui::separator(),

				ftxui::text(""),
				ftxui::separator(),
				ftxui::text(*_streamTime) | ftxui::align_right,
				ftxui::text(*_averageAudioMilli) | ftxui::align_right,
				ftxui::text(*_averageAudioSampleMicro) | ftxui::align_right,
				ftxui::text(*_averageAudioLockAcquireNano) | ftxui::align_right,
				ftxui::text(*_streamLatency) | ftxui::align_right,
				ftxui::separator(),

				ftxui::text(""),
				ftxui::separator(),
				ftxui::text(*_averageUIMilli) | ftxui::align_right,
				ftxui::text(*_averageUIDataFetchMicro) | ftxui::align_right,
				ftxui::text(*_averageUILockAcqcuireNano) | ftxui::align_right,
				ftxui::text(*_averageUIRenderingMilli) | ftxui::align_right,
				ftxui::text(*_averageUISleepMilli) | ftxui::align_right,
			});

		}), _synthInfoWidth);

	_component = ftxui::Container::Horizontal({

		// Synth Information
		synthInformation | ftxui::border,

		// Output 
		_outputUI->GetComponent() | ftxui::border | ftxui::flex_grow
	});
}
ftxui::Component SynthInformationUI::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render();
	});
}
void SynthInformationUI::ServicePendingAction()
{
	_outputUI->ServicePendingAction();
}
void SynthInformationUI::UpdateComponent()
{
	_outputUI->UpdateComponent();
}
void SynthInformationUI::Tick()
{
}
void SynthInformationUI::ToUI(const OutputModelUI& source)
{
	throw new std::exception("Pleaes use the pointer version of this function ToUI(..)");
}
void SynthInformationUI::ToUI(const OutputModelUI* source)
{
	const OutputSettings* outputSettings = source->GetOutputSettings();

	_hostApi->clear();
	_hostApi->append(outputSettings->GetHostApi());

	_deviceName->clear();
	_deviceName->append(outputSettings->GetDeviceName());

	_streamFormat->clear();
	_streamFormat->append(outputSettings->GetDeviceFormat());

	_streamBufferSize->clear();
	_streamBufferSize->append(std::format("{} (frames)", outputSettings->GetOutputBufferFrameSize()));

	_samplingRate->clear();
	_samplingRate->append(std::to_string(outputSettings->GetSamplingRate()));

	_streamTime->clear();
	_streamTime->append(std::format("{:.3f}", outputSettings->GetStreamTime()));

	_averageAudioMilli->clear();
	_averageAudioMilli->append(std::format("{:.3f}", outputSettings->GetAvgAudioMilli()));

	_averageAudioSampleMicro->clear();
	_averageAudioSampleMicro->append(std::format("{:.3f}", outputSettings->GetAvgAudioSampleMicro()));

	_averageAudioLockAcquireNano->clear();
	_averageAudioLockAcquireNano->append(std::format("{:.3f}", outputSettings->GetAvgAudioLockAcquireNano()));

	_streamLatency->clear();
	_streamLatency->append(std::to_string(outputSettings->GetStreamLatency()));

	_averageUIMilli->clear();
	_averageUIMilli->append(std::format("{:.3f}", outputSettings->GetAvgUIMilli()));

	_averageUIDataFetchMicro->clear();
	_averageUIDataFetchMicro->append(std::format("{:.3f}", outputSettings->GetAvgUIDataFetchMicro()));

	_averageUILockAcqcuireNano->clear();
	_averageUILockAcqcuireNano->append(std::format("{:.3f}", outputSettings->GetAvgUILockAqcuireNano()));

	_averageUIRenderingMilli->clear();
	_averageUIRenderingMilli->append(std::format("{:.3f}", outputSettings->GetAvgUIRenderingMilli()));

	_averageUISleepMilli->clear();
	_averageUISleepMilli->append(std::format("{:.3f}", outputSettings->GetAvgUISleepMilli()));

	_outputUI->ToUI(source);
}
void SynthInformationUI::FromUI(OutputModelUI& destination)
{
	_outputUI->FromUI(destination);
}

void SynthInformationUI::FromUI(OutputModelUI* destination)
{
	_outputUI->FromUI(destination);
}

bool SynthInformationUI::HasPendingAction() const
{
	return false;
}

void SynthInformationUI::ClearPendingAction()
{
}

bool SynthInformationUI::GetDirty() const
{
	return _outputUI->GetDirty();
}

void SynthInformationUI::ClearDirty()
{
	_outputUI->ClearDirty();
}

#endif