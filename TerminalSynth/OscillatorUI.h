#pragma once

#ifndef OSCILLATOR_UI_H
#define OSCILLATOR_UI_H

#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "SliderUI.h"
#include "SoundBankSettings.h"
#include "SoundSourceUI.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

class OscillatorUI : public UIBase<OscillatorParameters>
{
public:
	OscillatorUI(const SoundBankSettings* soundBankSettings, const std::string& name, const std::string& label, const ftxui::Color& labelColor);
	~OscillatorUI();

	void Initialize(const OscillatorParameters& parameters) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const OscillatorParameters& source) override;
	void FromUI(OscillatorParameters& destination, bool clearDirty) override;

private:

	ftxui::Component _component;

	// Sound Source UI
	SoundSourceUI* _soundSourceUI;

	// Slider UI's
	SliderUI* _attack;
	SliderUI* _decay;
	SliderUI* _release;
	SliderUI* _attackPeak;
	SliderUI* _sustainPeak;
};

OscillatorUI::OscillatorUI(const SoundBankSettings* soundBankSettings, const std::string& name, const std::string& label, const ftxui::Color& labelColor)
	: UIBase(name, label, labelColor)
{
	_soundSourceUI = new SoundSourceUI(soundBankSettings, labelColor);

	_attack = new SliderUI(0.1f, 0.01f, 1.0f, 0.01f, "Attack", "Attack   (s) {:.2f}", ftxui::Color::White);
	_decay = new SliderUI(0.1f, 0.01f, 2.0f, 0.01f, "Decay", "Decay    (s) {:.2f}", ftxui::Color::White);
	_release = new SliderUI(0.3f, 0.01f, 3.0f, 0.01f, "Release", "Release  (s) {:.2f}", ftxui::Color::White);
	_attackPeak = new SliderUI(0.9f, 0.01f, 1.0f, 0.01f, "Limit", "Limit        {:.2f}", ftxui::Color::White);
	_sustainPeak = new SliderUI(0.65f, 0.01f, 1.0f, 0.01f, "Sustain", "Sustain      {:.2f}", ftxui::Color::White);
}
OscillatorUI::~OscillatorUI()
{
	UIBase::~UIBase();

	delete _soundSourceUI;
	delete _attack;
	delete _decay;
	delete _release;
	delete _attackPeak;
	delete _sustainPeak;
}

void OscillatorUI::Initialize(const OscillatorParameters& parameters)
{
	UIBase::Initialize(parameters);

	Envelope envelope = *parameters.GetEnvelope();

	_soundSourceUI->Initialize(parameters);
	_attack->Initialize(envelope.GetAttack());
	_decay->Initialize(envelope.GetDecay());
	_release->Initialize(envelope.GetRelease());
	_attackPeak->Initialize(envelope.GetAttackPeak());
	_sustainPeak->Initialize(envelope.GetSustainPeak());

	auto envelopeUI = ftxui::Container::Vertical(
	{
		ftxui::Renderer([&] { return ftxui::text("Envelope") | ftxui::color(this->GetLabelColor()); }),
		ftxui::Renderer([&] { return ftxui::separator(); }),

		_attack->GetComponent(),
		_decay->GetComponent(),
		_release->GetComponent(),

		ftxui::Renderer([&] { return ftxui::separator(); }),

		_attackPeak->GetComponent(),
		_sustainPeak->GetComponent()
	});

	auto oscillatorUI = ftxui::Container::Vertical(
	{
		ftxui::Renderer([&] { return ftxui::text(this->GetLabel()) | ftxui::color(this->GetLabelColor()); }),
		ftxui::Renderer([&] { return ftxui::separator(); }),

		_soundSourceUI->GetComponent() | ftxui::flex_grow
	});

	_component = ftxui::Container::Horizontal({

		oscillatorUI | ftxui::flex_grow,
		envelopeUI | ftxui::flex_grow

	}) | ftxui::flex_grow;
}

ftxui::Component OscillatorUI::GetComponent()
{
	return _component;
}

void OscillatorUI::UpdateComponent(bool clearDirty)
{
	_soundSourceUI->UpdateComponent(clearDirty);
	_attack->UpdateComponent(clearDirty);
	_decay->UpdateComponent(clearDirty);
	_release->UpdateComponent(clearDirty);
	_attackPeak->UpdateComponent(clearDirty);
	_sustainPeak->UpdateComponent(clearDirty);

	if (clearDirty)
		this->ClearDirty();
}


void OscillatorUI::ToUI(const OscillatorParameters& source)
{

}

void OscillatorUI::FromUI(OscillatorParameters& destination, bool clearDirty)
{
	float attack, decay, release, attackPeak, sustainPeak;

	_soundSourceUI->FromUI(destination, clearDirty);
	_attack->FromUI(attack, clearDirty);
	_decay->FromUI(decay, clearDirty);
	_release->FromUI(release, clearDirty);
	_attackPeak->FromUI(attackPeak, clearDirty);
	_sustainPeak->FromUI(sustainPeak, clearDirty);

	destination.GetEnvelope()->Set(attack, decay, 0, release, attackPeak, sustainPeak);

	if (clearDirty)
		this->ClearDirty();
}

#endif
