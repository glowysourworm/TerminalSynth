#pragma once

#ifndef SOUND_SOURCE_UI_H
#define SOUND_SOURCE_UI_H

#include "Constant.h"
#include "OscillatorParameters.h"
#include "SoundBankSettings.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class OscillatorUI : public UIBase<OscillatorParameters>
{
public:

	OscillatorUI(const SoundBankSettings* soundBankSettings, const ftxui::Color& labelColor);
	~OscillatorUI();

	void Initialize(const OscillatorParameters& parameters) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent() override;

	void ToUI(const OscillatorParameters& source) override;
	void FromUI(OscillatorParameters& destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	ftxui::Component _component;

	const SoundBankSettings* _soundBankSettings;

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
	_soundBankSettings = soundBankSettings;

	std::vector<std::string> soundBanks = _soundBankSettings->GetSoundBanks();
	std::string initialSoundBank = soundBanks.size() > 0 ? soundBanks[0] : "";
	std::vector<std::string> soundNames = initialSoundBank.size() > 0 ? _soundBankSettings->GetSoundNames(initialSoundBank) : std::vector<std::string>();

	_soundSourceChoices = new std::vector<std::string>({
		"Oscillators",
		"Sample Banks"
	});
	_oscillatorItems = new std::vector<std::string>({
		"Sine",
		"Square",
		"Triangle",
		"Sawtooth",
		"SynthesizedStringPluck"
	});
	_soundBankItems = new std::vector<std::string>(soundBanks);
	_soundNameItems = new std::vector<std::string>(soundNames);

	_oscillatorSelectedIndex = new ValueCapture<int>(0);
	_soundSourceChoiceIndex = new ValueCapture<int>(0);
	_soundBankSelectedIndex = new ValueCapture<int>(0);
	_soundNameSelectedIndex = new ValueCapture<int>(0);
}

OscillatorUI::~OscillatorUI()
{
	UIBase::~UIBase();

	delete _soundBankItems;
	delete _soundNameItems;
	delete _soundSourceChoices;
	delete _oscillatorItems;

	delete _soundSourceChoiceIndex;
	delete _soundBankSelectedIndex;
	delete _soundNameSelectedIndex;
	delete _oscillatorSelectedIndex;
}

void OscillatorUI::Initialize(const OscillatorParameters& parameters)
{
	auto soundSourceChoiceUI = ftxui::Dropdown(_soundSourceChoices, _soundSourceChoiceIndex->GetRef());
	auto oscillatorItemsUI = ftxui::Dropdown(_oscillatorItems, _oscillatorSelectedIndex->GetRef());
	auto soundBankItemsUI = ftxui::Dropdown(_soundBankItems, _soundBankSelectedIndex->GetRef());
	auto soundNameItemsUI = ftxui::Dropdown(_soundNameItems, _soundNameSelectedIndex->GetRef());

	_component = ftxui::Container::Vertical({
			soundSourceChoiceUI,
			oscillatorItemsUI | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 0; }),
			soundBankItemsUI | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 1; }),
			soundNameItemsUI | ftxui::Maybe([&] { return _soundSourceChoiceIndex->GetValue() == 1; }),
		}) | ftxui::CatchEvent([&] (ftxui::Event event) {



			// Passthrough
			if (event.is_mouse())
				return false;

			// Cancel
			return true;
		});
}

ftxui::Component OscillatorUI::GetComponent()
{
	return _component;
}

void OscillatorUI::UpdateComponent()
{
	if (_soundBankSelectedIndex->HasChanged())
	{
		std::string soundBank = _soundBankItems->at(_soundBankSelectedIndex->GetValue());

		// Update Sound Names
		std::vector<std::string> soundNames = _soundBankSettings->GetSoundNames(soundBank);

		_soundNameItems->clear();

		for (int index = 0; index < soundNames.size(); index++)
		{
			_soundNameItems->push_back(soundNames[index]);
		}
	}
}

void OscillatorUI::ToUI(const OscillatorParameters& source)
{
	_soundSourceChoiceIndex->SetValue((int)source.GetType());
	_oscillatorSelectedIndex->SetValue((int)source.GetBuiltInType());

	_soundBankSelectedIndex->SetValue(0);
	_soundNameSelectedIndex->SetValue(0);

	std::string soundBank = source.GetSoundBank();
	std::string soundName = source.GetSoundName();

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

	std::vector<std::string> soundNames = _soundBankSettings->GetSoundNames(soundBank);

	for (int index = 0; index < soundNames.size(); index++)
	{
		if (soundNames.at(index) == soundName)
		{
			_soundNameSelectedIndex->SetValue(index);
			break;
		}
	}
}

bool OscillatorUI::GetDirty() const
{
	return _soundSourceChoiceIndex->HasChanged() ||
		_oscillatorSelectedIndex->HasChanged() ||
		_soundBankSelectedIndex->HasChanged() ||
		_soundNameSelectedIndex->HasChanged();
}

void OscillatorUI::ClearDirty()
{
	_soundSourceChoiceIndex->Clear();
	_oscillatorSelectedIndex->Clear();
	_soundBankSelectedIndex->Clear();
	_soundNameSelectedIndex->Clear();
}

void OscillatorUI::FromUI(OscillatorParameters& destination)
{
	destination.SetType((OscillatorType)_soundSourceChoiceIndex->GetValue());
	destination.SetBuiltInType((BuiltInOscillators)_oscillatorSelectedIndex->GetValue());

	if (_soundBankItems->size() > 0)
		destination.SetSoundBank(_soundBankItems->at(_soundBankSelectedIndex->GetValue()));

	if (_soundNameItems->size() > 0)
		destination.SetSoundName(_soundNameItems->at(_soundNameSelectedIndex->GetValue()));
}

#endif	