#pragma once

#include "ControlPanelUI.h"
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
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

MainUI::MainUI(const MainModelUI& model)
{
	_model = new MainModelUI(model);

	// Tab Headers
	_tabHeaders = new std::vector<std::string>({
		"About",
		"Control Panel",
		"Input",
		"Effects",
		"Midi"
	});

	_controlPanelUI = new ControlPanelUI();
	_synthInformationUI = new SynthInformationUI("Terminal Synth", ftxui::Color::GreenYellow);
	_inputUI = new InputUI(*model.GetInputModelUI());
	_effectsUI = new EffectsUI(*model.GetEffectsModelUI());


	_scrollY = new float(0);
	_tabIndex = new int(0);

	_saveDialogInput = new std::string("");
	_saveDialogTitle = new std::string("Save Synth Voice");
	_showSaveDialog = false;
	_processSaveSoundSettings = false;
}

MainUI::~MainUI()
{
	delete _controlPanelUI;
	delete _inputUI;
	delete _effectsUI;
	delete _tabHeaders;
	delete _scrollY;
	delete _tabIndex;
	delete _synthInformationUI;
	delete _saveDialogInput;
	delete _saveDialogTitle;
}

void MainUI::Initialize(const MainModelUI& model)
{
	_controlPanelUI->Initialize(false);
	_synthInformationUI->Initialize(*_model->GetOutputModelUI());
	_inputUI->Initialize(*_model->GetInputModelUI());
	_effectsUI->Initialize(*_model->GetEffectsModelUI());

	auto midiSettings = ftxui::Container::Horizontal({

	});

	// Primary UI
	_tabControlMenu = ftxui::Menu(_tabHeaders, _tabIndex, ftxui::MenuOption::HorizontalAnimated());

	_tabControl = ftxui::Container::Tab({

		_synthInformationUI->GetComponent(),
		_controlPanelUI->GetComponent(),
		_inputUI->GetComponent(),
		_effectsUI->GetComponent(),
		midiSettings,

	}, _tabIndex);

	_mainControl = ftxui::Container::Vertical({

		// Tabs
		_tabControlMenu,

		// Tab Content
		_tabControl,

	}) | ftxui::CatchEvent([&](ftxui::Event event) {

		// Passthrough
		if (event.is_mouse())
			return false;

		// Cancel
		return true;

	}) | ftxui::flex_grow;
}

ftxui::Component MainUI::GetComponent()
{
	auto modal = ftxui::Container::Vertical({

		ftxui::Renderer([&] { return ftxui::text(*_saveDialogTitle); }),
		ftxui::Renderer([] { return ftxui::separator(); }),

		ftxui::Container::Vertical({

			ftxui::Container::Horizontal({

				ftxui::Renderer([] { return ftxui::text("Synth Voice Name: "); }) | ftxui::vcenter,
				ftxui::Input(_saveDialogInput) | ftxui::border,

			}) | ftxui::bgcolor(ftxui::Color::RGBA(0,0,255,50)),

			ftxui::Container::Horizontal({
				ftxui::Button("Cancel", [&]
				{
					_showSaveDialog = false;
					_processSaveSoundSettings = false;
					_saveDialogInput->clear();
				}),
				ftxui::Button("Ok", [&]
				{
					_processSaveSoundSettings = true;
				})
			}) | ftxui::align_right

		})

	}) | ftxui::border | ftxui::bgcolor(ftxui::Color::Black) | ftxui::size(ftxui::WidthOrHeight::WIDTH, ftxui::Constraint::EQUAL, 50);

	return _mainControl | ftxui::Modal(modal, &_showSaveDialog);
}

void MainUI::ServicePendingAction()
{
	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ServicePendingAction();
	}

	// Control Panel
	else if (*_tabIndex == 1)
	{
		// Pending Action is being used here:
		//
		// 1) Show Modal
		// 2) Click Ok / Cancel
		// 3) Process Save
		// 4) Hide Modal
		//
		// Dirty status for the tab header will be picked up on the next
		// ToUI iteration.
		//
		_showSaveDialog = true;

		// Next Iteration (after user clicks OK)
		if (_processSaveSoundSettings)
		{
			// ... (save)
			//_model->GetEffectsModelUI()->GetSoundSettings()->Save()

			_processSaveSoundSettings = false;
			_showSaveDialog = false;					// Hide Dialog (clear pending action)
			_controlPanelUI->ClearPendingAction();
		}
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		_inputUI->ServicePendingAction();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		_effectsUI->ServicePendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
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

	// Control Panel
	else if (*_tabIndex == 1)
	{
		_controlPanelUI->UpdateComponent();
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		_inputUI->UpdateComponent();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		_effectsUI->UpdateComponent();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
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

	// Control Panel
	else if (*_tabIndex == 1)
	{
		_controlPanelUI->Tick();
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		_inputUI->Tick();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		_effectsUI->Tick();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
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

	// Control Panel
	else if (*_tabIndex == 1)
	{
		bool dummy;
		_controlPanelUI->FromUI(dummy);
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		_inputUI->FromUI(destination->GetInputModelUI());
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		_effectsUI->FromUI(destination->GetEffectsModelUI());
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
	{

	}
}

void MainUI::ToUI(const MainModelUI& source)
{
	throw new std::exception("Please use the pointer version of this function ToUI");
}

void MainUI::ToUI(const MainModelUI* source)
{
	bool soundSettingsChanged = source->HaveSoundSettingsChanged();

	// Control Panel Tab Header
	_tabHeaders->at(1) = soundSettingsChanged ? "Control Panel*" : "Control Panel";

	// Control Panel
	_controlPanelUI->SetDirtyStatus(soundSettingsChanged);

	// Synth Information
	_synthInformationUI->ToUI(source->GetOutputModelUI());
}

bool MainUI::GetDirty() const
{
	// These may be run per-tab

	// About Tab
	if (*_tabIndex == 0)
	{
		return _synthInformationUI->GetDirty();
	}

	// Control Panel
	else if (*_tabIndex == 1)
	{
		return _controlPanelUI->GetDirty();
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		return _inputUI->GetDirty();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		return _effectsUI->GetDirty();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
	{

	}

	return false;
}

void MainUI::ClearDirty()
{
	// These may be run per-tab

	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ClearDirty();
	}

	// Control Panel
	else if (*_tabIndex == 1)
	{
		_controlPanelUI->ClearDirty();
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		_inputUI->ClearDirty();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		_effectsUI->ClearDirty();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
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

	// Control Panel
	else if (*_tabIndex == 1)
	{
		return _controlPanelUI->HasPendingAction() || _showSaveDialog;		// Part of modal processing
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		return _inputUI->HasPendingAction();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		return _effectsUI->HasPendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
	{

	}

	return false;
}

void MainUI::ClearPendingAction()
{
	// About Tab
	if (*_tabIndex == 0)
	{
		_synthInformationUI->ClearPendingAction();
	}

	// Control Panel
	else if (*_tabIndex == 1)
	{
		// Modal Processing (see ServicePendingAction)
	}

	// Input Tab
	else if (*_tabIndex == 2)
	{
		_inputUI->ClearPendingAction();
	}

	// Effects Tab
	else if (*_tabIndex == 3)
	{
		_effectsUI->ClearPendingAction();
	}

	// MIDI Tab
	else if (*_tabIndex == 4)
	{

	}
}
