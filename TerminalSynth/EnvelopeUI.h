#pragma once

#ifndef ENVELOPE_UI_H
#define ENVELOPE_UI_H

#include "Envelope.h"
#include "SliderUI.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

class EnvelopeUI : public UIBase<Envelope>
{
public:

	EnvelopeUI();
	~EnvelopeUI();

	void Initialize(const Envelope& envelope) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void FromUI(Envelope& destination, bool clearDirty) override;
	void ToUI(const Envelope& source) override;

	bool GetDirty() const override;

private:

	ftxui::Component _component;

	// Slider UI's
	SliderUI* _attack;
	SliderUI* _decay;
	SliderUI* _release;
	SliderUI* _attackPeak;
	SliderUI* _sustainPeak;
};

EnvelopeUI::EnvelopeUI() : UIBase("Envelope", "Envelope", ftxui::Color::White)
{
	_attack = new SliderUI(0.1f, 0.01f, 1.0f, 0.01f, "Attack", "Attack   (s) {:.2f}", ftxui::Color::White);
	_decay = new SliderUI(0.1f, 0.01f, 2.0f, 0.01f, "Decay", "Decay    (s) {:.2f}", ftxui::Color::White);
	_release = new SliderUI(0.3f, 0.01f, 3.0f, 0.01f, "Release", "Release  (s) {:.2f}", ftxui::Color::White);
	_attackPeak = new SliderUI(0.9f, 0.01f, 1.0f, 0.01f, "Limit", "Limit        {:.2f}", ftxui::Color::White);
	_sustainPeak = new SliderUI(0.65f, 0.01f, 1.0f, 0.01f, "Sustain", "Sustain      {:.2f}", ftxui::Color::White);
}

EnvelopeUI::~EnvelopeUI()
{
	UIBase::~UIBase();

	delete _attack;
	delete _decay;
	delete _release;
	delete _attackPeak;
	delete _sustainPeak;
}

void EnvelopeUI::Initialize(const Envelope& envelope)
{
	_attack->Initialize(envelope.GetAttack());
	_decay->Initialize(envelope.GetDecay());
	_release->Initialize(envelope.GetRelease());
	_attackPeak->Initialize(envelope.GetAttackPeak());
	_sustainPeak->Initialize(envelope.GetSustainPeak());

	_component = ftxui::Container::Vertical(
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
}

ftxui::Component EnvelopeUI::GetComponent()
{
	return _component;
}

void EnvelopeUI::UpdateComponent(bool clearDirty)
{
	_attack->UpdateComponent(clearDirty);
	_decay->UpdateComponent(clearDirty);
	_release->UpdateComponent(clearDirty);
	_attackPeak->UpdateComponent(clearDirty);
	_sustainPeak->UpdateComponent(clearDirty);

	if (clearDirty)
	{
		this->ClearDirty();
	}
}

void EnvelopeUI::FromUI(Envelope& destination, bool clearDirty)
{
	float attack, decay, release, attackPeak, sustainPeak;

	_attack->FromUI(attack, clearDirty);
	_decay->FromUI(decay, clearDirty);
	_release->FromUI(release, clearDirty);
	_attackPeak->FromUI(attackPeak, clearDirty);
	_sustainPeak->FromUI(sustainPeak, clearDirty);

	destination.Set(attack, decay, 0, release, attackPeak, sustainPeak);

	if (clearDirty)
		this->ClearDirty();
}

void EnvelopeUI::ToUI(const Envelope& source)
{
}

bool EnvelopeUI::GetDirty() const
{
	return _attack->GetDirty() ||
		   _decay->GetDirty() ||
		   _release->GetDirty() ||
		   _attackPeak->GetDirty() ||
		   _sustainPeak->GetDirty();
}

#endif