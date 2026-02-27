#pragma once

#ifndef ACTIVE_EDITOR_H
#define ACTIVE_EDITOR_H

#include "EffectUI.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

class ActiveEditorUI : public UIBase<bool>
{
public:

	ActiveEditorUI();
	~ActiveEditorUI();

	void Initialize(const bool& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const bool& source) override;
	void FromUI(bool& destination, bool clearDirty) override;

	bool GetDirty() const override;

public:

	void SetEffect(EffectUI* activeEffect);
	EffectUI* GetEffect() const;

private:

	EffectUI* _activeEffect;
};

ActiveEditorUI::ActiveEditorUI() : UIBase("Active Editor", "Active Editor", ftxui::Color::White)
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

void ActiveEditorUI::UpdateComponent(bool clearDirty)
{
}

void ActiveEditorUI::ToUI(const bool& source)
{
}

void ActiveEditorUI::FromUI(bool& destination, bool clearDirty)
{
}

bool ActiveEditorUI::GetDirty() const
{
	return false;
}

void ActiveEditorUI::SetEffect(EffectUI* activeEffect)
{
}

EffectUI* ActiveEditorUI::GetEffect() const
{
	return _activeEffect;
}

#endif