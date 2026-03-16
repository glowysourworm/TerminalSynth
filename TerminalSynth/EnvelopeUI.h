#pragma once

#ifndef ENVELOPE_UI_H
#define ENVELOPE_UI_H

#include "Envelope.h"
#include "SliderUI.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <exception>
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <string_view>
#include <vector>

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
	std::vector<SliderUI*>* _attackValues;
	std::vector<SliderUI*>* _releaseValues;
	SliderUI* _attack;
	SliderUI* _release;
};

EnvelopeUI::EnvelopeUI(const Envelope& initialValue)
{
	_attack = new SliderUI(initialValue.GetAttackTime(), 0.0f, 1.0f, 0.01f, "Attack", "Attack   (s) {:.2f}", ftxui::Color::White, ftxui::Color::GreenYellow);
	_release = new SliderUI(initialValue.GetReleaseTime(), 0.0f, 3.0f, 0.01f, "Release", "Release  (s) {:.2f}", ftxui::Color::White, ftxui::Color::GreenYellow);

	_attackValues = new std::vector<SliderUI*>();
	_releaseValues = new std::vector<SliderUI*>();

	float attackTimeDelta = initialValue.GetAttackTime() / initialValue.GetAttackLength();
	float releaseTimeDelta = initialValue.GetReleaseTime() / initialValue.GetReleaseLength();

	for (int index = 0; index < initialValue.GetAttackLength(); index++)
	{
		float attackTimeStart = attackTimeDelta * index;
		float attackTimeEnd = attackTimeDelta * (index + 1);

		auto sliderUI = new SliderUI(initialValue.GetAttackValue(index), 0.0f, 1.0f, 0.01f, "Attack", ftxui::Color::Blue, ftxui::Color::BlueLight);

		sliderUI->Initialize(initialValue.GetAttackValue(index));

		_attackValues->push_back(sliderUI);
	}

	for (int index = 0; index < initialValue.GetReleaseLength(); index++)
	{
		float releaseTimeStart = releaseTimeDelta * index;
		float releaseTimeEnd = releaseTimeDelta * (index + 1);

		auto sliderUI = new SliderUI(initialValue.GetReleaseValue(index), 0.0f, 1.0f, 0.01f, "Release", ftxui::Color::Magenta, ftxui::Color::MagentaLight);

		sliderUI->Initialize(initialValue.GetReleaseValue(index));

		_releaseValues->push_back(sliderUI);
	}
}

EnvelopeUI::~EnvelopeUI()
{
	UIBase::~UIBase();

	delete _attack;
	delete _release;

	for (int index = 0; index < _attackValues->size(); index++)
	{
		delete _attackValues->at(index);
	}

	for (int index = 0; index < _releaseValues->size(); index++)
	{
		delete _releaseValues->at(index);
	}

	delete _attackValues;
	delete _releaseValues;
}

void EnvelopeUI::Initialize(const Envelope& envelope)
{
	_attack->Initialize(envelope.GetAttackTime());
	_release->Initialize(envelope.GetReleaseTime());

	auto envelopeStack = ftxui::Container::Vertical({});

	for (int index = 0; index < envelope.GetAttackLength(); index++)
	{
		_attackValues->at(index)->ToUI(envelope.GetAttackValue(index));

		envelopeStack->Add(_attackValues->at(index)->GetComponent());
	}

	for (int index = 0; index < envelope.GetReleaseLength(); index++)
	{
		_releaseValues->at(index)->ToUI(envelope.GetReleaseValue(index));

		envelopeStack->Add(_releaseValues->at(index)->GetComponent());
	}

	_component = ftxui::Container::Vertical(
	{
		ftxui::Renderer([&] { return ftxui::text("Envelope") | ftxui::color(ftxui::Color::GreenYellow); }),
		ftxui::Renderer([&] { return ftxui::separator(); }),

		envelopeStack,

		ftxui::Renderer([&] { return ftxui::separator(); }),

		_attack->GetComponent(),
		_release->GetComponent()

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
	_release->UpdateComponent();

	// Go ahead and update attack / release times
	float attackTime, releaseTime;

	_attack->FromUI(attackTime);
	_release->FromUI(releaseTime);

	float attackTimeDelta = attackTime / _attackValues->size();
	float releaseTimeDelta = releaseTime / _releaseValues->size();

	for (int index = 0; index < _attackValues->size(); index++)
	{
		float attackTimeStart = attackTimeDelta * index;
		float attackTimeEnd = attackTimeDelta * (index + 1);
		float attack;

		_attackValues->at(index)->FromUI(attack);

		std::string attackStartStr = std::format("{:.2f}", attackTimeStart);
		std::string attackEndStr = std::format("{:.2f}", attackTimeEnd);
		std::string attackStr = std::format("{:.2f}", attack);

		_attackValues->at(index)->SetLabel("Attack  [" + attackStartStr + "-" + attackEndStr + "] (" + attackStr + ")");
	}

	for (int index = 0; index < _releaseValues->size(); index++)
	{
		float releaseTimeStart = releaseTimeDelta * index;
		float releaseTimeEnd = releaseTimeDelta * (index + 1);
		float release;

		_releaseValues->at(index)->FromUI(release);

		std::string releaseStartStr = std::format("{:.2f}", releaseTimeStart);
		std::string releaseEndStr = std::format("{:.2f}", releaseTimeEnd);
		std::string releaseStr = std::format("{:.2f}", release);

		
		_releaseValues->at(index)->SetLabel("Release [" + releaseStartStr + "-" + releaseEndStr + "] (" + releaseStr + ")");
	}
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
	float attack, release;

	_attack->FromUI(attack);
	_release->FromUI(release);

	destination->SetAttackTime(attack);
	destination->SetReleaseTime(release);

	for (int index = 0; index < destination->GetAttackLength(); index++)
	{
		float attackValue;
		_attackValues->at(index)->FromUI(attackValue);

		destination->SetAttackValue(index, attackValue);
	}

	for (int index = 0; index < destination->GetReleaseLength(); index++)
	{
		float releaseValue;
		_releaseValues->at(index)->FromUI(releaseValue);

		destination->SetReleaseValue(index, releaseValue);
	}
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

	for (int index = 0; index < _attackValues->size(); index++)
	{
		isDirty |= _attackValues->at(index)->GetDirty();
	}

	for (int index = 0; index < _releaseValues->size(); index++)
	{
		isDirty |= _releaseValues->at(index)->GetDirty();
	}

	isDirty |= _attack->GetDirty();
	isDirty |= _release->GetDirty();

	return isDirty;
}

void EnvelopeUI::ClearDirty()
{
	_attack->ClearDirty();
	_release->ClearDirty();

	for (int index = 0; index < _attackValues->size(); index++)
	{
		_attackValues->at(index)->ClearDirty();
	}

	for (int index = 0; index < _releaseValues->size(); index++)
	{
		_releaseValues->at(index)->ClearDirty();
	}
}

bool EnvelopeUI::HasPendingAction() const
{
	return false;
}

void EnvelopeUI::ClearPendingAction()
{
}

#endif