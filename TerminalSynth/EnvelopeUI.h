#pragma once

#ifndef ENVELOPE_UI_H
#define ENVELOPE_UI_H

#include "Envelope.h"
#include "SliderUI.h"
#include "UIBase.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

class EnvelopeUI : public UIBase<Envelope>
{
public:

	EnvelopeUI(const Envelope& initialValue);
	~EnvelopeUI();

	void Initialize(const Envelope& envelope) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void FromUI(Envelope& destination) override;
	void FromUI(Envelope* destination) override;
	void ToUI(const Envelope& source) override;
	void ToUI(const Envelope* source) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

private:

	ftxui::Component _component;

	// Slider UI's
	SliderUI* _attack;
	SliderUI* _decay;
	SliderUI* _release;

	SliderUI* _attackPeak;
	SliderUI* _sustainPeak;
};

EnvelopeUI::EnvelopeUI(const Envelope& initialValue)
{
	_attack = new SliderUI(initialValue.GetAttack(), 0.0f, 2.0f, 0.01f, "Attack", "Attack   (s) {:.2f}", ftxui::Color::Blue, ftxui::Color::Blue3);
	_decay = new SliderUI(initialValue.GetDecay(), 0.0f, 1.0f, 0.01f, "Decay", "Decay    (s) {:.2f}", ftxui::Color::Blue, ftxui::Color::Blue3);
	_release = new SliderUI(initialValue.GetRelease(), 0.0f, 3.0f, 0.01f, "Release", "Release  (s) {:.2f}", ftxui::Color::Blue, ftxui::Color::Blue3);

	_attackPeak = new SliderUI(initialValue.GetAttackPeak(), 0.0f, 1.0f, 0.01f, "Attack Peak", "Attack Peak   (s) {:.2f}", ftxui::Color::Blue, ftxui::Color::Blue3);
	_sustainPeak = new SliderUI(initialValue.GetSustainPeak(), 0.0f, 1.0f, 0.01f, "Sustain Peak", "Sustain Peak  (s) {:.2f}", ftxui::Color::Blue, ftxui::Color::Blue3);
}

EnvelopeUI::~EnvelopeUI()
{
	UIBase::~UIBase();

	delete _attack;
	delete _decay;
	delete _release;
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
		ftxui::Renderer([&] { return ftxui::text("Envelope") | ftxui::color(ftxui::Color::GreenYellow); }),
		ftxui::Renderer([&] { return ftxui::separator(); }),

		ftxui::Renderer([&] { return ftxui::separator(); }),

		_attack->GetComponent(),
		_decay->GetComponent(),
		_release->GetComponent(),

		ftxui::Renderer([&] { return ftxui::separator(); }),

		_attackPeak->GetComponent(),
		_sustainPeak->GetComponent()

	}) | ftxui::border | ftxui::xflex_grow;
}

ftxui::Component EnvelopeUI::GetComponent()
{
	return _component;
}

void EnvelopeUI::ServicePendingAction()
{
	// Nothing to do (no reason to call sub-functions)
}

void EnvelopeUI::UpdateComponent()
{
	_attack->UpdateComponent();
	_decay->UpdateComponent();
	_release->UpdateComponent();
	_attackPeak->UpdateComponent();
	_sustainPeak->UpdateComponent();
}

void EnvelopeUI::Tick()
{
	// Nothing to do (no reason to call sub-functions)
}

void EnvelopeUI::FromUI(Envelope& destination)
{
	throw new std::exception("Please use pointer version of this function FromUI");
}

void EnvelopeUI::FromUI(Envelope* destination)
{
	// Go ahead and update attack / release times
	float attack, decay, release, attackPeak, sustainPeak;

	_attack->FromUI(attack);
	_decay->FromUI(decay);
	_release->FromUI(release);
	_attackPeak->FromUI(attackPeak);
	_sustainPeak->FromUI(sustainPeak);

	destination->SetAttack(attack);
	destination->SetDecay(decay);
	destination->SetRelease(release);
	destination->SetAttackPeak(attackPeak);
	destination->SetSustainPeak(sustainPeak);
}

void EnvelopeUI::ToUI(const Envelope& source)
{
}

void EnvelopeUI::ToUI(const Envelope* source)
{
}

bool EnvelopeUI::GetDirty() const
{
	bool isDirty = false;

	isDirty |= _attack->GetDirty();
	isDirty |= _decay->GetDirty();
	isDirty |= _release->GetDirty();
	isDirty |= _attackPeak->GetDirty();
	isDirty |= _sustainPeak->GetDirty();

	return isDirty;
}

void EnvelopeUI::ClearDirty()
{
	_attack->ClearDirty();
	_decay->ClearDirty();
	_release->ClearDirty();
	_attackPeak->ClearDirty();
	_sustainPeak->ClearDirty();
}

bool EnvelopeUI::HasPendingAction() const
{
	return false;
}

void EnvelopeUI::ClearPendingAction()
{
}

#endif