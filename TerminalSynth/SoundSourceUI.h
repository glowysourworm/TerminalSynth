#pragma once

#ifndef SOUND_SOURCE_UI_H
#define SOUND_SOURCE_UI_H

#include "Constant.h"
#include "OscillatorParameters.h"
#include "SoundBankSettings.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>

class SoundSourceUI : public UIBase<OscillatorParameters>
{
public:
	SoundSourceUI(const SoundBankSettings* soundBankSettings, const ftxui::Color& labelColor);
	~SoundSourceUI();

	void Initialize(const OscillatorParameters& parameters) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const OscillatorParameters& source) override;
	void FromUI(OscillatorParameters& destination, bool clearDirty) override;

private:

	ftxui::Component _component;

	const SoundBankSettings* _soundBankSettings;

	// Oscillator Selected Index
	int* _soundSourceChoiceIndex;
	int* _oscillatorSelectedIndex;
	int* _soundBankSelectedIndex;
	int* _soundNameSelectedIndex;

	// Oscillator Choices
	std::vector<std::string>* _soundSourceChoices;
	std::vector<std::string>* _oscillatorItems;
	std::vector<std::string>* _soundBankItems;
	std::vector<std::string>* _soundNameItems;
};

SoundSourceUI::SoundSourceUI(const SoundBankSettings* soundBankSettings, const ftxui::Color& labelColor)
	: UIBase("SoundSourceUI", "Sound Source", labelColor)
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

	_oscillatorSelectedIndex = new int(0);
	_soundSourceChoiceIndex = new int(0);
	_soundBankSelectedIndex = new int(0);
	_soundNameSelectedIndex = new int(0);
}

SoundSourceUI::~SoundSourceUI()
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

void SoundSourceUI::Initialize(const OscillatorParameters& parameters)
{
	auto soundSourceChoiceUI = ftxui::Dropdown(_soundSourceChoices, _soundSourceChoiceIndex);
	auto oscillatorItemsUI = ftxui::Dropdown(_oscillatorItems, _oscillatorSelectedIndex);
	auto soundBankItemsUI = ftxui::Dropdown(_soundBankItems, _soundBankSelectedIndex);
	auto soundNameItemsUI = ftxui::Dropdown(_soundNameItems, _soundNameSelectedIndex);

	_component = ftxui::Container::Horizontal({
		ftxui::Container::Vertical({
			soundSourceChoiceUI,
			oscillatorItemsUI
		}),
		ftxui::Container::Vertical({
			soundBankItemsUI,
			soundNameItemsUI
		})
	});
}

ftxui::Component SoundSourceUI::GetComponent()
{
	return _component;
}

void SoundSourceUI::UpdateComponent(bool clearDirty)
{

}

void SoundSourceUI::ToUI(const OscillatorParameters& source)
{
	*_soundSourceChoiceIndex = (int)source.GetType();
	*_oscillatorSelectedIndex = (int)source.GetBuiltInType();

	*_soundBankSelectedIndex = 0;
	*_soundNameSelectedIndex = 0;

	std::string soundBank = source.GetSoundBank();
	std::string soundName = source.GetSoundName();

	if (soundBank == "")
		return;

	for (int index = 0; index < _soundBankItems->size(); index++)
	{
		if (_soundBankItems->at(index) == soundBank)
		{
			*_soundBankSelectedIndex = index;
			break;
		}
	}

	std::vector<std::string> soundNames = _soundBankSettings->GetSoundNames(soundBank);

	for (int index = 0; index < soundNames.size(); index++)
	{
		if (soundNames.at(index) == soundName)
		{
			*_soundNameSelectedIndex = index;
			break;
		}
	}
}

void SoundSourceUI::FromUI(OscillatorParameters& destination, bool clearDirty)
{
	destination.SetType((OscillatorType)*_soundSourceChoiceIndex);
	destination.SetBuiltInType((BuiltInOscillators)*_oscillatorSelectedIndex);

	if (_soundBankItems->size() > 0)
		destination.SetSoundBank(_soundBankItems->at(*_soundBankSelectedIndex));

	if (_soundNameItems->size() > 0)
		destination.SetSoundName(_soundNameItems->at(*_soundNameSelectedIndex));
}

#endif	