#pragma once

#include "CheckListUI.h"
#include "EqualizerOutput.h"
#include "MainUI.h"
#include "OscillatorParameters.h"
#include "OscillatorUI.h"
#include "OutputUI.h"
#include "SignalChainSettings.h"
#include "SignalChainUI.h"
#include "SignalSettings.h"
#include "SoundBankSettings.h"
#include "SynthInformationUI.h"
#include "SynthSettings.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

MainUI::MainUI(const SoundBankSettings* soundBankSettings, const std::string& title, const ftxui::Color& titleColor) : UIBase(title, title, titleColor)
{
	// Tab Headers
	_tabHeaders = new std::vector<std::string>({
		"Output",
		"Synth",
		"Midi"
	});

	_airwinPluginListUI = new CheckListUI(ftxui::Color::White);
	_signalChainUI = new SignalChainUI();
	_synthInformationUI = new SynthInformationUI("Terminal Synth", ftxui::Color::GreenYellow);
	_oscillatorUI = new OscillatorUI(soundBankSettings, "Oscillator", "Oscillator", ftxui::Color::Blue);
	_outputUI = new OutputUI("Output", ftxui::Color::Green);

	_scrollY = new float(0);
	_tabIndex = new int(0);
}

MainUI::~MainUI()
{
	delete _airwinPluginListUI;
	delete _tabHeaders;
	delete _scrollY;
	delete _tabIndex;
	delete _signalChainUI;
	delete _synthInformationUI;
	delete _oscillatorUI;
	delete _outputUI;
}

void MainUI::Initialize(const SynthSettings& configuration)
{
	UIBase::Initialize(configuration);

	_signalChainSettings = configuration.GetSignalChainRegistry();

	// Airwin Registry List
	std::vector<std::string> pluginList;
	configuration.GetSignalChainRegistry()->GetRegistryList(pluginList);

	_synthInformationUI->Initialize(*configuration.GetOutputSettings());
	_oscillatorUI->Initialize(*configuration.GetOscillator());
	_outputUI->Initialize(configuration);
	_signalChainUI->Initialize(*configuration.GetSignalChainRegistry());
	_airwinPluginListUI->Initialize(pluginList);

	auto midiSettings = ftxui::Container::Horizontal({

	});

	_synthTab = ftxui::Container::Horizontal({

		_airwinPluginListUI->GetComponent(),

		ftxui::Container::Vertical({
			_oscillatorUI->GetComponent() | ftxui::flex_grow | ftxui::border,
			_signalChainUI->GetComponent() | ftxui::flex_grow
		}) | ftxui::flex_grow

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
		_synthTab | ftxui::flex_grow,
		midiSettings | ftxui::flex_grow,

		}, _tabIndex) | ftxui::CatchEvent([&](ftxui::Event event) {

		// Only allow mouse events through
		if (event.is_mouse())
		{
			return false;
		}

		// Cancel keyboard events
		return true;

	}) | ftxui::flex_grow;

	_mainControl = ftxui::Container::Vertical({
		_tabControlMenu,
		_tabControl
	});
}

ftxui::Component MainUI::GetComponent()
{
	return _mainControl;
}

void MainUI::UpdateComponent(bool clearDirty)
{
	// Output Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->UpdateComponent(clearDirty);
		_outputUI->UpdateComponent(clearDirty);
	}

	// Synth Tab
	else if (*_tabIndex == 1)
	{
		if (_airwinPluginListUI->GetDirty())
		{
			// Re-Populate Signal Chain
			std::vector<std::string> selectedList;
			
			_airwinPluginListUI->FromUI(selectedList, clearDirty);
			
			// Create a new signal chain
			SignalChainSettings signalChain;
			for (int index = 0; index < selectedList.size(); index++)
			{
				SignalSettings settings = _signalChainSettings->GetFromRegistry(selectedList[index]);
				signalChain.SignalAdd(settings);
			}

			_signalChainUI->ToUI(signalChain);
		}

		// Performance:  Better to send a delegate to the CheckboxUI for the checked event
		//
		_airwinPluginListUI->UpdateComponent(clearDirty);
		_oscillatorUI->UpdateComponent(clearDirty);
		_signalChainUI->UpdateComponent(clearDirty);
	}

	// MIDI Tab
	else if (*_tabIndex == 2)
	{

	}
}

void MainUI::FromUI(SynthSettings& configuration, bool clearDirty)
{
	SignalChainSettings signalChain;
	OscillatorParameters parameters(*configuration.GetOscillator());
	EqualizerOutput output;

	// Oscillator
	_oscillatorUI->FromUI(parameters, clearDirty);
	configuration.SetOscillator(parameters);

	// Signal Chain
	_signalChainUI->FromUI(signalChain, clearDirty);
	configuration.SetSignalChain(signalChain);

	// Output
	_outputUI->FromUI(configuration, clearDirty);

	if (clearDirty)
		this->ClearDirty();
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
	return _oscillatorUI->GetDirty() ||
		   _airwinPluginListUI->GetDirty() ||
		   _signalChainUI->GetDirty() ||
		   _outputUI->GetDirty();
}
