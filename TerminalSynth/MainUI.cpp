#pragma once

#include "MainModelUI.h"
#include "MainUI.h"
#include "SoundSettings.h"
#include "SynthInformationUI.h"
#include "SynthTabUI.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

MainUI::MainUI(const MainModelUI& model)
{
	_model = new MainModelUI(model);

	// Tab Headers
	_tabHeaders = new std::vector<std::string>({
		"Output",
		"Synth",
		"Midi"
	});

	_synthInformationUI = new SynthInformationUI("Terminal Synth", ftxui::Color::GreenYellow);
	_synthTabUI = new SynthTabUI(*model.GetSynthTabModelUI());

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
}

void MainUI::Initialize(const MainModelUI& model)
{
	_synthInformationUI->Initialize(*_model->GetOutputModelUI());
	_synthTabUI->Initialize(*_model->GetSynthTabModelUI());

	// Airwin Registry List
	std::vector<std::string> pluginList;
	_model->GetSynthTabModelUI()->GetSoundSettings()->GetEffectRegistry()->GetList(pluginList);

	auto midiSettings = ftxui::Container::Horizontal({

	});

	// Output Tab
	_outputTab = ftxui::Container::Vertical({

		_synthInformationUI->GetComponent() | ftxui::flex_grow,
	});

	// Primary UI
	_tabControlMenu = ftxui::Menu(_tabHeaders, _tabIndex, ftxui::MenuOption::HorizontalAnimated());

	_tabControl = ftxui::Container::Tab({

		_outputTab,
		_synthTabUI->GetComponent(),
		midiSettings,

	}, _tabIndex);

	_mainControl = ftxui::Container::Vertical({
		_tabControlMenu,
		_tabControl,
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

void MainUI::ServicePendingAction()
{
	// Output Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ServicePendingAction();
	}

	// Synth Tab
	else if (*_tabIndex == 1)
	{
		_synthTabUI->ServicePendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 2)
	{

	}
}

void MainUI::UpdateComponent()
{
	// Output Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->UpdateComponent();
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

void MainUI::Tick()
{
	// Output Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->Tick();
	}

	// Synth Tab
	else if (*_tabIndex == 1)
	{
		_synthTabUI->Tick();
	}

	// MIDI Tab
	else if (*_tabIndex == 2)
	{

	}
}

void MainUI::FromUI(MainModelUI& destination)
{
	throw new std::exception("Please use the pointer version of this function FromUI");
}

void MainUI::FromUI(MainModelUI* destination)
{
	// Synth Tab UI
	_synthTabUI->FromUI(destination->GetSynthTabModelUI());
	_synthInformationUI->FromUI(destination->GetOutputModelUI());
}

void MainUI::ToUI(const MainModelUI& source)
{
	throw new std::exception("Please use the pointer version of this function ToUI");
}

void MainUI::ToUI(const MainModelUI* source)
{
	// Synth Information
	_synthInformationUI->ToUI(source->GetOutputModelUI());
}

bool MainUI::GetDirty() const
{
	return _synthTabUI->GetDirty() || _synthInformationUI->GetDirty();
}

void MainUI::ClearDirty()
{
	_synthTabUI->ClearDirty();
	_synthInformationUI->ClearDirty();
}

bool MainUI::HasPendingAction() const
{
	return _synthTabUI->HasPendingAction() || _synthInformationUI->HasPendingAction();
}

void MainUI::ClearPendingAction()
{
	_synthTabUI->ClearPendingAction();
	_synthInformationUI->ClearPendingAction();
}
