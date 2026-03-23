#pragma once

#ifndef SYNTH_NOTE_PARAMETERS_UI_H
#define SYNTH_NOTE_PARAMETERS_UI_H

#include "Constant.h"
#include "SliderUI.h"
#include "SynthNoteParameters.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class SynthNoteParametersUI : public UIBase<SynthNoteParameters>
{
public:

	SynthNoteParametersUI(const SynthNoteParameters& parameters);
	~SynthNoteParametersUI();

	void Initialize(const SynthNoteParameters& parameters) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const SynthNoteParameters& source) override;
	void ToUI(const SynthNoteParameters* source) override;
	void FromUI(SynthNoteParameters& destination) override;
	void FromUI(SynthNoteParameters* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

private:

	ftxui::Component _component;

	// Synth Note Parameters
	SliderUI* _arpeggiatorBPMUI;
	SliderUI* _portamentoSecondsUI;

	// Oscillator Selected Index
	ValueCapture<int>* _noteModeSelectedIndex;
	ValueCapture<int>* _arpeggiatorChordSelectedIndex;

	// Oscillator Choices
	std::vector<std::string>* _noteModeItems;
	std::vector<std::string>* _arpeggiatorChordItems;
};

SynthNoteParametersUI::SynthNoteParametersUI(const SynthNoteParameters& parameters)
{
	_noteModeItems = new std::vector<std::string>({
		"Multiple (Normal)",
		"Single (Normal)",
		"Single Portamento",
		"Single Arpeggiator"
	});
	_arpeggiatorChordItems = new std::vector<std::string>({
		"Major Triad",
		"Minor Triad",
		"Diminished Triad",
		"Augmented Triad",
		"Major 7",
		"Minor 7",
		"Dominant 7",
		"Minor 7b5",
		"Diminished 7"
	});

	_noteModeSelectedIndex = new ValueCapture<int>((int)parameters.mode);
	_arpeggiatorChordSelectedIndex = new ValueCapture<int>((int)parameters.chord);

	_arpeggiatorBPMUI = new SliderUI(parameters.arpeggioBPM, 60.0f, 240.0f, 1.0f, "ArpeggiatorBPM", "Arpeggiator BPM {:3.0f} ", ftxui::Color::Blue, ftxui::Color::BlueLight);
	_portamentoSecondsUI = new SliderUI(parameters.portamentoSeconds, 0.0f, 1.0f, 0.01f, "Portamento", "Portamento      {:.2f}", ftxui::Color::Blue, ftxui::Color::BlueLight);
}

SynthNoteParametersUI::~SynthNoteParametersUI()
{
	UIBase::~UIBase();

	delete _noteModeItems;
	delete _arpeggiatorChordItems;
	delete _noteModeSelectedIndex;
	delete _arpeggiatorChordSelectedIndex;
	delete _arpeggiatorBPMUI;
	delete _portamentoSecondsUI;
}

void SynthNoteParametersUI::Initialize(const SynthNoteParameters& parameters)
{
	_arpeggiatorBPMUI->Initialize(parameters.arpeggioBPM);
	_portamentoSecondsUI->Initialize(parameters.portamentoSeconds);

	_component = ftxui::Container::Vertical({

		ftxui::Renderer([&] { return ftxui::text("Synth Mode") | ftxui::color(ftxui::Color::GreenYellow); }),
		ftxui::Renderer([&] { return ftxui::separator(); }),

		ftxui::Dropdown(_noteModeItems, _noteModeSelectedIndex->GetRef()),
		ftxui::Dropdown(_arpeggiatorChordItems, _arpeggiatorChordSelectedIndex->GetRef()),

		_arpeggiatorBPMUI->GetComponent(),
		_portamentoSecondsUI->GetComponent()

	}) | ftxui::CatchEvent([&](ftxui::Event event) {

		// Passthrough
		if (event.is_mouse())
			return false;

		// Cancel
		return true;

	}) | ftxui::border;
}

ftxui::Component SynthNoteParametersUI::GetComponent()
{
	return _component;
}

void SynthNoteParametersUI::ServicePendingAction()
{
}

void SynthNoteParametersUI::UpdateComponent()
{
	_arpeggiatorBPMUI->UpdateComponent();
	_portamentoSecondsUI->UpdateComponent();
}

void SynthNoteParametersUI::Tick()
{
}

void SynthNoteParametersUI::ToUI(const SynthNoteParameters& source)
{
	throw new std::exception("Please use the pointer version of this function ToUI");
}

void SynthNoteParametersUI::ToUI(const SynthNoteParameters* source)
{

}

bool SynthNoteParametersUI::GetDirty() const
{
	return _arpeggiatorBPMUI->GetDirty() ||
		_portamentoSecondsUI->GetDirty() ||
		_noteModeSelectedIndex->HasChanged() ||
		_arpeggiatorChordSelectedIndex->HasChanged();
}

void SynthNoteParametersUI::ClearDirty()
{
	_arpeggiatorBPMUI->ClearDirty();
	_portamentoSecondsUI->ClearDirty();
	_noteModeSelectedIndex->Clear();
	_arpeggiatorChordSelectedIndex->Clear();
}

bool SynthNoteParametersUI::HasPendingAction() const
{
	return false;
}

void SynthNoteParametersUI::ClearPendingAction()
{
}

void SynthNoteParametersUI::FromUI(SynthNoteParameters& destination)
{
	throw new std::exception("Please use the pointer version of this function FromUI");
}

void SynthNoteParametersUI::FromUI(SynthNoteParameters* destination)
{
	float arpeggioBPM, pornamentoSeconds;

	_arpeggiatorBPMUI->FromUI(arpeggioBPM);
	_portamentoSecondsUI->FromUI(pornamentoSeconds);

	destination->arpeggioBPM = arpeggioBPM;
	destination->portamentoSeconds = pornamentoSeconds;
	destination->mode = (SynthNoteMode)_noteModeSelectedIndex->GetValue();
	destination->chord = (ArpeggiatorChord)_arpeggiatorChordSelectedIndex->GetValue();
}

#endif	