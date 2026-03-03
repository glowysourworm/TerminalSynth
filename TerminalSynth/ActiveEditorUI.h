#pragma once

#ifndef ACTIVE_EDITOR_H
#define ACTIVE_EDITOR_H

#include "EffectUI.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

class ActiveEditorUI : public UIBase<bool>
{
public:

	ActiveEditorUI();
	~ActiveEditorUI();

	void Initialize(const bool& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const bool& source) override;
	void ToUI(const bool* source) override;
	void FromUI(bool& destination) override;
	void FromUI(bool* destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

public:

	void SetEffect(EffectUI* activeEffect);
	EffectUI* GetEffect() const;

private:

	EffectUI* _activeEffect;
};

ActiveEditorUI::ActiveEditorUI()
{
	_activeEffect = nullptr;
}

ActiveEditorUI::~ActiveEditorUI()
{
	// DO NOT DELETE ACTIVE EFFECT!
}

void ActiveEditorUI::Initialize(const bool& initialValue)
{

}

ftxui::Component ActiveEditorUI::GetComponent()
{
	// Empty
	if (_activeEffect == nullptr)
		return ftxui::Renderer([&] { return ftxui::text(""); });

	else
		return _activeEffect->GetComponent();
}

void ActiveEditorUI::ServicePendingAction()
{
}

void ActiveEditorUI::UpdateComponent()
{
	if (_activeEffect != nullptr)
		_activeEffect->UpdateComponent();
}

void ActiveEditorUI::Tick()
{
}

void ActiveEditorUI::ToUI(const bool& source)
{
}

void ActiveEditorUI::ToUI(const bool* source)
{
}

void ActiveEditorUI::FromUI(bool& destination)
{
}

void ActiveEditorUI::FromUI(bool* destination)
{
}

bool ActiveEditorUI::GetDirty() const
{
	return _activeEffect != nullptr ? _activeEffect->GetDirty() : false;
}

void ActiveEditorUI::ClearDirty()
{
	if (_activeEffect != nullptr)
		_activeEffect->ClearDirty();
}

bool ActiveEditorUI::HasPendingAction() const
{
	return false;
}

void ActiveEditorUI::ClearPendingAction()
{
}

void ActiveEditorUI::SetEffect(EffectUI* activeEffect)
{
	_activeEffect = activeEffect;
}

EffectUI* ActiveEditorUI::GetEffect() const
{
	return _activeEffect;
}

#endif