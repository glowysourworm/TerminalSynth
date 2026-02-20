#pragma once

#ifndef SIGNAL_CHAIN_UI_H
#define SIGNAL_CHAIN_UI_H

#include "EffectUI.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>

class SignalChainUI : public UIBase<SignalChainSettings>
{
public:

	SignalChainUI();
	~SignalChainUI();

	void Initialize(const SignalChainSettings& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const SignalChainSettings& source) override;
	void FromUI(SignalChainSettings& destination, bool clearDirty) override;

	bool GetDirty() const override;

private:

	ftxui::Component _component;

	// Mapped by pointer value (may be ok to use the name)
	std::map<std::string, EffectUI*>* _effectUIs;

};

SignalChainUI::SignalChainUI() : UIBase("No Title", "No Title", ftxui::Color::White)
{
	_effectUIs = new std::map<std::string, EffectUI*>();
}
SignalChainUI::~SignalChainUI()
{
	// MEMORY! (EffectUI*)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		delete iter->second;
	}

	delete _effectUIs;
}

void SignalChainUI::Initialize(const SignalChainSettings& initialValue)
{
	UIBase::Initialize(initialValue);

	this->ToUI(initialValue);
	
	_component = ftxui::Container::Horizontal({});

	this->UpdateComponent(true);
}

ftxui::Component SignalChainUI::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render();
	});
}

void SignalChainUI::UpdateComponent(bool clearDirty)
{
	// Effects Included (currently)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		iter->second->UpdateComponent(clearDirty);
	}	

	// TODO: Prevent extra work in RT loop here...
	_component->DetachAllChildren();

	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		_component->Add(iter->second->GetComponent());
	}


	if (clearDirty)
		this->ClearDirty();
}

void SignalChainUI::ToUI(const SignalChainSettings& source)
{
	// Add
	for (int index = 0; index < source.GetCount(); index++)
	{
		auto element = source.Get(index);

		if (_effectUIs->contains(element.GetName()))
			continue;

		// MEMORY! (see ~SignalChainUI)
		EffectUI* effectUI = new EffectUI(element.GetName(), element.GetName(), ftxui::Color::White);

		effectUI->Initialize(element);

		_effectUIs->insert(std::make_pair(element.GetName(), effectUI));
	}

	// Remove
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end();)
	{
		if (!source.SignalContains(iter->first))
		{
			// (MEMORY!) ~EffectUI*
			delete iter->second;

			iter = _effectUIs->erase(iter);
		}
		else
			++iter;
	}
}

void SignalChainUI::FromUI(SignalChainSettings& destination, bool clearDirty)
{
	// Effects Included (currently)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		// Copy settings from the UI
		SignalSettings settings(iter->first, true);
		iter->second->FromUI(settings, clearDirty);

		// Add settings to the output
		destination.SignalAdd(settings);
	}

	if (clearDirty)
		this->ClearDirty();
}

bool SignalChainUI::GetDirty() const
{
	bool isDirty = false;

	// Effects Included (currently)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		isDirty |= iter->second->GetDirty();
	}

	return isDirty;
}

#endif