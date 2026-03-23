#pragma once

#include "EffectsUI.h"
#include "InputUI.h"
#include "MainModelUI.h"
#include "MainUI.h"
#include "SynthInformationUI.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

MainUI::MainUI(const MainModelUI& model)
{
	_model = new MainModelUI(model);

	// Tab Headers
	_tabHeaders = new std::vector<std::string>({
		"About",
		"Input",
		"Effects",
		"Midi"
	});

	_synthInformationUI = new SynthInformationUI("Terminal Synth", ftxui::Color::GreenYellow);
	_inputUI = new InputUI(*model.GetInputModelUI());
	_effectsUI = new EffectsUI(*model.GetEffectsModelUI());

	_scrollY = new float(0);
	_tabIndex = new int(0);
}

MainUI::~MainUI()
{
	delete _inputUI;
	delete _effectsUI;
	delete _tabHeaders;
	delete _scrollY;
	delete _tabIndex;
	delete _synthInformationUI;
}

void MainUI::Initialize(const MainModelUI& model)
{
	_synthInformationUI->Initialize(*_model->GetOutputModelUI());
	_inputUI->Initialize(*_model->GetInputModelUI());
	_effectsUI->Initialize(*_model->GetEffectsModelUI());

	_buttonColor = ftxui::Color::RGBA(0, 0, 255, 50);

	auto midiSettings = ftxui::Container::Horizontal({

	});

	// Primary UI
	_tabControlMenu = ftxui::Menu(_tabHeaders, _tabIndex, ftxui::MenuOption::HorizontalAnimated());

	_tabControl = ftxui::Container::Tab({

		_synthInformationUI->GetComponent(),
		_inputUI->GetComponent(),
		_effectsUI->GetComponent(),
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
	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ServicePendingAction();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		_inputUI->ServicePendingAction();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		_effectsUI->ServicePendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{
		
	}
}

void MainUI::UpdateComponent()
{
	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->UpdateComponent();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		_inputUI->UpdateComponent();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		_effectsUI->UpdateComponent();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}

void MainUI::Tick()
{
	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->Tick();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		_inputUI->Tick();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		_effectsUI->Tick();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}

void MainUI::FromUI(MainModelUI& destination)
{
	throw new std::exception("Please use the pointer version of this function FromUI");
}

void MainUI::FromUI(MainModelUI* destination)
{
	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->FromUI(destination->GetOutputModelUI());
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		_inputUI->FromUI(destination->GetInputModelUI());
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		_effectsUI->FromUI(destination->GetEffectsModelUI());
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}

void MainUI::ToUI(const MainModelUI& source)
{
	throw new std::exception("Please use the pointer version of this function ToUI");
}

void MainUI::ToUI(const MainModelUI* source)
{
	// Synth Information
	_synthInformationUI->ToUI(source->GetOutputModelUI());

	if (source->HaveSoundSettingsChanged())
	{
		_inputUI->SetControlPanelStatus(true);
		_effectsUI->SetControlPanelStatus(true);
	}

	else
	{
		_inputUI->SetControlPanelStatus(false);
		_effectsUI->SetControlPanelStatus(false);
	}
}

bool MainUI::GetDirty() const
{
	// These may be run per-tab

	// About Tab
	if (*_tabIndex == 0)
	{
		return _synthInformationUI->GetDirty();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		return _inputUI->GetDirty();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		return _effectsUI->GetDirty();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}

void MainUI::ClearDirty()
{
	// These may be run per-tab

	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ClearDirty();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		_inputUI->ClearDirty();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		_effectsUI->ClearDirty();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}

bool MainUI::HasPendingAction() const
{
	// These may be run per-tab

	// About Tab
	if (*_tabIndex == 0)
	{
		return _synthInformationUI->HasPendingAction();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		return _inputUI->HasPendingAction();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		return _effectsUI->HasPendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}

void MainUI::ClearPendingAction()
{
	// These may be run per-tab

	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ClearPendingAction();
	}

	// Input Tab
	else if (*_tabIndex == 1)
	{
		_inputUI->ClearPendingAction();
	}

	// Effects Tab
	else if (*_tabIndex == 2)
	{
		_effectsUI->ClearPendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 3)
	{

	}
}
