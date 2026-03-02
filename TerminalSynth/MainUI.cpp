#pragma once

#include "MainUI.h"
#include "OutputUI.h"
#include "SignalChainSettings.h"
#include "SoundSettings.h"
#include "SynthInformationUI.h"
#include "SynthSettings.h"
#include "SynthTabUI.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

MainUI::MainUI(const SynthSettings& configuration)
{
	// Tab Headers
	_tabHeaders = new std::vector<std::string>({
		"Output",
		"Synth",
		"Midi"
	});

	_signalChainSettings = configuration.GetSoundSettings()->GetSignalChain();
	_synthInformationUI = new SynthInformationUI("Terminal Synth", ftxui::Color::GreenYellow);
	_outputUI = new OutputUI("Output", ftxui::Color::Green);
	_synthTabUI = new SynthTabUI(configuration);

	_scrollY = new float(0);
	_tabIndex = new int(0);
}

MainUI::~MainUI()
{
	delete _synthTabUI;
	delete _tabHeaders;
	delete _scrollY;
	delete _tabIndex;
	delete _synthInformationUI;
	delete _outputUI;
}

void MainUI::Initialize(const SynthSettings& configuration)
{
	_synthInformationUI->Initialize(*configuration.GetOutputSettings());
	_outputUI->Initialize(configuration);
	_synthTabUI->Initialize(*configuration.GetSoundSettings());	

	// Airwin Registry List
	std::vector<std::string> pluginList;
	configuration.GetSoundSettings()->GetEffectRegistry()->GetList(pluginList);

	auto midiSettings = ftxui::Container::Horizontal({

	});

	// Output Tab
	_outputTab = ftxui::Container::Vertical({

		_synthInformationUI->GetComponent() | ftxui::flex_grow,
		_outputUI->GetComponent() | ftxui::flex_grow,

	});

	// Primary UI
	_tabControlMenu = ftxui::Menu(_tabHeaders, _tabIndex, ftxui::MenuOption::HorizontalAnimated());

	_tabControl = ftxui::Container::Tab({

		_outputTab | ftxui::flex_grow,
		_synthTabUI->GetComponent() | ftxui::flex_grow,
		midiSettings | ftxui::flex_grow,

	}, _tabIndex) | ftxui::flex_grow;

	_mainControl = ftxui::Container::Vertical({
		_tabControlMenu,
		_tabControl
	}) | ftxui::CatchEvent([&](ftxui::Event event) {

		// Passthrough
		if (event.is_mouse())
			return false;

		// Cancel
		return true;
	});
}

ftxui::Component MainUI::GetComponent()
{
	return _mainControl;
}

void MainUI::UpdateComponent()
{
	// Output Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->UpdateComponent();
		_outputUI->UpdateComponent();
	}

	// Synth Tab
	else if (*_tabIndex == 1)
	{
		_synthTabUI->UpdateComponent();
	}

	// MIDI Tab
	else if (*_tabIndex == 2)
	{

	}
}

void MainUI::FromUI(SynthSettings& synthSettings)
{
	SoundSettings soundSettings = *synthSettings.GetSoundSettings();

	// Signal Chain
	_synthTabUI->FromUI(soundSettings);

	bool isDirty = synthSettings.GetSoundSettings()->Update(soundSettings);		// Nested setting changed

	// Only alert listeners if we have a dirty status
	if (isDirty)
		synthSettings.SetDirty();

	// Output
	_outputUI->FromUI(synthSettings);
}

void MainUI::ToUI(const SynthSettings& configuration)
{
	// Synth Information
	_synthInformationUI->ToUI(*configuration.GetOutputSettings());

	// Synth Output Channels
	_outputUI->ToUI(configuration);
}

bool MainUI::GetDirty() const
{
	return _synthTabUI->GetDirty() ||
		   _outputUI->GetDirty();
}

void MainUI::ClearDirty()
{
	_synthTabUI->ClearDirty();
	_outputUI->ClearDirty();
}
