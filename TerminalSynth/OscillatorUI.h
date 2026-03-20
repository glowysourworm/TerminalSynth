#pragma once

#ifndef SOUND_SOURCE_UI_H
#define SOUND_SOURCE_UI_H

#include "Constant.h"
#include "OscillatorParameters.h"
#include "SliderUI.h"
#include "SoundBankSettings.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <cmath>
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

class OscillatorUI : public UIBase<OscillatorParameters>
{
public:

	OscillatorUI(const SoundBankSettings* soundBankSettings, const ftxui::Color& labelColor);
	~OscillatorUI();

	void Initialize(const OscillatorParameters& parameters) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const OscillatorParameters& source) override;
	void ToUI(const OscillatorParameters* source) override;
	void FromUI(OscillatorParameters& destination) override;
	void FromUI(OscillatorParameters* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

private:

	ftxui::Component _component;

	// Octave Selector
	SliderUI* _octaveUI;

	// Waveshaper Parameters
	SliderUI* _waveshaperSidebandCentsUI;
	SliderUI* _waveshaperPhaseAmplitudeUI;

	// Waveshaper Harmonics
	std::vector<SliderUI*>* _waveshaperUIs;

	// Sound Bank Map
	std::map<std::string, std::vector<std::string>*>* _soundBankMap;

	// Oscillator Selected Index
	ValueCapture<int>* _soundSourceChoiceIndex;
	ValueCapture<int>* _oscillatorSelectedIndex;
	ValueCapture<int>* _soundBankSelectedIndex;
	ValueCapture<int>* _soundNameSelectedIndex;

	// Oscillator Choices
	std::vector<std::string>* _soundSourceChoices;
	std::vector<std::string>* _oscillatorItems;
	std::vector<std::string>* _soundBankItems;
	std::vector<std::string>* _soundNameItems;


};

OscillatorUI::OscillatorUI(const SoundBankSettings* soundBankSettings, const ftxui::Color& labelColor)
{
	std::vector<std::string> soundBanks = soundBankSettings->GetSoundBanks();
	std::string initialSoundBank = soundBanks.size() > 0 ? soundBanks[0] : "";
	std::vector<std::string> soundNames = initialSoundBank.size() > 0 ? soundBankSettings->GetSoundNames(initialSoundBank) : std::vector<std::string>();

	_soundSourceChoices = new std::vector<std::string>({
		"Oscillators",
		"Sample Banks",
		"Harmonic Shaper"
	});
	_oscillatorItems = new std::vector<std::string>({
		"Sine",
		"Square",
		"Triangle",
		"Sawtooth",
		"SynthesizedStringPluck",
		"STK Rhodey",
		"STK Bee Three",
		"STK Clarinet",
		"STK Drummer",
		"STK Flute",
		"STK FM Voices",
		"STK Guitar",
		"STK Heavy Metal",
		"STK Mandolin",
		"STK Moog",
		"STK Saxofony",
		"STK Shakers",
		"STK Sitar",
		"STK Tube Bell",
		"STK VoicForm",
		"STK Whistle",
		"STK Wurley"
	});
	_soundBankItems = new std::vector<std::string>(soundBanks);
	_soundNameItems = new std::vector<std::string>(soundNames);

	_oscillatorSelectedIndex = new ValueCapture<int>(0);
	_soundSourceChoiceIndex = new ValueCapture<int>(0);
	_soundBankSelectedIndex = new ValueCapture<int>(0);
	_soundNameSelectedIndex = new ValueCapture<int>(0);

	_octaveUI = new SliderUI(0, 0.0f, 3.0f, 1.0f, "Octave", "Octave {:2.0f}", ftxui::Color::Blue, ftxui::Color::BlueLight);
	_waveshaperSidebandCentsUI = new SliderUI(0, 0.0f, 1.0f, 0.01f, "Sideband", "Sideband        {:.2f}", ftxui::Color::Blue, ftxui::Color::BlueLight);
	_waveshaperPhaseAmplitudeUI = new SliderUI(0, 0.0f, 1.0f, 0.01f, "Phase Amplitude", "Phase Amplitude {:.2f}", ftxui::Color::Blue, ftxui::Color::BlueLight);

	_waveshaperUIs = new std::vector<SliderUI*>();
	_soundBankMap = new std::map<std::string, std::vector<std::string>*>();

	for (int index = 0; index < soundBanks.size(); index++)
	{
		auto soundNames = new std::vector<std::string>(soundBankSettings->GetSoundNames(soundBanks[index]));

		_soundBankMap->insert(std::make_pair(soundBanks[index], soundNames));
	}
}

OscillatorUI::~OscillatorUI()
{
	UIBase::~UIBase();

	for (auto iter = _soundBankMap->begin(); iter != _soundBankMap->end(); ++iter)
	{
		// std::vector<std::string>*
		delete iter->second;
	}

	for (int index = 0; index < _waveshaperUIs->size(); index++)
	{
		// ~SliderUI
		delete _waveshaperUIs->at(index);
	}

	delete _waveshaperUIs;
	delete _waveshaperSidebandCentsUI;
	delete _waveshaperPhaseAmplitudeUI;

	delete _soundBankItems;
	delete _soundNameItems;
	delete _soundSourceChoices;
	delete _oscillatorItems;

	delete _soundSourceChoiceIndex;
	delete _soundBankSelectedIndex;
	delete _soundNameSelectedIndex;
	delete _oscillatorSelectedIndex;

	delete _soundBankMap;
}

void OscillatorUI::Initialize(const OscillatorParameters& parameters)
{
	_octaveUI->Initialize(parameters.GetOctave());
	_waveshaperSidebandCentsUI->Initialize(parameters.GetWaveshaperSidebandCents());
	_waveshaperPhaseAmplitudeUI->Initialize(parameters.GetWaveshaperRandomPhaseAmplitude());

	auto waveshaperContainer = ftxui::Container::Vertical({
		_waveshaperSidebandCentsUI->GetComponent(),
		_waveshaperPhaseAmplitudeUI->GetComponent(),
	});

	// Waveshaper UI
	for (int index = 0; index < parameters.GetWaveshaperHarmonics()->size(); index++)
	{
		SliderUI* sliderUI = new SliderUI(parameters.GetWaveshaperHarmonics()->at(index), 0, 1.0f, 0.01f, "Harmonic", "Harmonic [" + std::to_string(index) + "] ({:.2f})", ftxui::Color::Orange3, ftxui::Color::Orange1);
		sliderUI->Initialize(parameters.GetWaveshaperHarmonics()->at(index));
		_waveshaperUIs->push_back(sliderUI);

		waveshaperContainer->Add(sliderUI->GetComponent());
	}

	auto soundSourceChoiceUI = ftxui::Dropdown(_soundSourceChoices, _soundSourceChoiceIndex->GetRef());
	auto oscillatorItemsUI = ftxui::Dropdown(_oscillatorItems, _oscillatorSelectedIndex->GetRef());
	auto soundBankItemsUI = ftxui::Dropdown(_soundBankItems, _soundBankSelectedIndex->GetRef());
	auto soundNameItemsUI = ftxui::Dropdown(_soundNameItems, _soundNameSelectedIndex->GetRef());

	_component = ftxui::Container::Vertical({

			ftxui::Renderer([&] { return ftxui::text("Oscillator") | ftxui::color(ftxui::Color::GreenYellow); }),
			ftxui::Renderer([&] { return ftxui::separator(); }),
			soundSourceChoiceUI,
			oscillatorItemsUI | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 0; }),
			soundBankItemsUI | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 1; }),
			soundNameItemsUI | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 1; }),
			_octaveUI->GetComponent() | ftxui::border,

			waveshaperContainer | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 2; }) | ftxui::border

		}) | ftxui::CatchEvent([&] (ftxui::Event event) {

			// Passthrough
			if (event.is_mouse())
				return false;

			// Cancel
			return true;

		}) | ftxui::border;
}

ftxui::Component OscillatorUI::GetComponent()
{
	return _component;
}

void OscillatorUI::ServicePendingAction()
{

}

void OscillatorUI::UpdateComponent()
{
	_octaveUI->UpdateComponent();
	_waveshaperSidebandCentsUI->UpdateComponent();
	_waveshaperPhaseAmplitudeUI->UpdateComponent();

	for (int index = 0; index < _waveshaperUIs->size(); index++)
	{
		_waveshaperUIs->at(index)->UpdateComponent();
	}

	if (_soundBankSelectedIndex->HasChanged())
	{
		std::string soundBank = _soundBankItems->at(_soundBankSelectedIndex->GetValue());

		// Update Sound Names
		std::vector<std::string>* soundNames = _soundBankMap->at(soundBank);

		_soundNameItems->clear();

		for (int index = 0; index < soundNames->size(); index++)
		{
			_soundNameItems->push_back(soundNames->at(index));
		}
	}
}

void OscillatorUI::Tick()
{
}

void OscillatorUI::ToUI(const OscillatorParameters& source)
{
	throw new std::exception("Please use the pointer version of this function ToUI");
}

void OscillatorUI::ToUI(const OscillatorParameters* source)
{
	_soundSourceChoiceIndex->SetValue((int)source->GetType());
	_oscillatorSelectedIndex->SetValue((int)source->GetBuiltInType());

	_soundBankSelectedIndex->SetValue(0);
	_soundNameSelectedIndex->SetValue(0);

	std::string soundBank = source->GetSoundBank();
	std::string soundName = source->GetSoundName();

	if (soundBank == "")
		return;

	for (int index = 0; index < _soundBankItems->size(); index++)
	{
		if (_soundBankItems->at(index) == soundBank)
		{
			_soundBankSelectedIndex->SetValue(index);
			break;
		}
	}

	std::vector<std::string>* soundNames = _soundBankMap->at(soundBank);

	for (int index = 0; index < soundNames->size(); index++)
	{
		if (soundNames->at(index) == soundName)
		{
			_soundNameSelectedIndex->SetValue(index);
			break;
		}
	}
}

bool OscillatorUI::GetDirty() const
{
	bool isDirty = false;

	for (int index = 0; index < _waveshaperUIs->size(); index++)
	{
		isDirty |= _waveshaperUIs->at(index)->GetDirty();
	}

	return isDirty || 
		_waveshaperSidebandCentsUI->GetDirty() ||
		_waveshaperPhaseAmplitudeUI->GetDirty() ||
		_soundSourceChoiceIndex->HasChanged() ||
		_oscillatorSelectedIndex->HasChanged() ||
		_soundBankSelectedIndex->HasChanged() ||
		_soundNameSelectedIndex->HasChanged() ||
		_octaveUI->GetDirty();
}

void OscillatorUI::ClearDirty()
{
	for (int index = 0; index < _waveshaperUIs->size(); index++)
	{
		_waveshaperUIs->at(index)->ClearDirty();
	}

	_waveshaperSidebandCentsUI->ClearDirty();
	_waveshaperPhaseAmplitudeUI->ClearDirty();
	_soundSourceChoiceIndex->Clear();
	_oscillatorSelectedIndex->Clear();
	_soundBankSelectedIndex->Clear();
	_soundNameSelectedIndex->Clear();
	_octaveUI->ClearDirty();
}

bool OscillatorUI::HasPendingAction() const
{
	return false;
}

void OscillatorUI::ClearPendingAction()
{
}

void OscillatorUI::FromUI(OscillatorParameters& destination)
{
	throw new std::exception("Please use the pointer version of this function FromUI");
}

void OscillatorUI::FromUI(OscillatorParameters* destination)
{
	float octave, waveshaperPhaseAmplitude, waveshaperSidebandCents;

	_octaveUI->FromUI(octave);
	_waveshaperPhaseAmplitudeUI->FromUI(waveshaperPhaseAmplitude);
	_waveshaperSidebandCentsUI->FromUI(waveshaperSidebandCents);

	for (int index = 0; index < _waveshaperUIs->size(); index++)
	{
		float waveshaperValue;

		_waveshaperUIs->at(index)->FromUI(waveshaperValue);

		destination->GetWaveshaperHarmonics()->at(index) = waveshaperValue;
	}

	destination->SetOctave((unsigned int)ceil(octave));
	destination->SetType((OscillatorType)_soundSourceChoiceIndex->GetValue());
	destination->SetBuiltInType((BuiltInOscillators)_oscillatorSelectedIndex->GetValue());

	if (_soundBankItems->size() > 0)
		destination->SetSoundBank(_soundBankItems->at(_soundBankSelectedIndex->GetValue()));

	if (_soundNameItems->size() > 0)
		destination->SetSoundName(_soundNameItems->at(_soundNameSelectedIndex->GetValue()));
}

#endif	