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
#include <vector>

class SignalChainUI : public UIBase<SignalChainSettings>
{
public:

	SignalChainUI(const SignalChainSettings& settings);
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

	// Category Name, Effect Name(s)
	std::map<std::string, std::vector<std::string>*>* _categoryLists;

	// Enabled flag for effects
	std::map<std::string, bool>* _effectsEnabled;

};

SignalChainUI::SignalChainUI(const SignalChainSettings& settings) : UIBase("No Title", "No Title", ftxui::Color::White)
{
	_effectUIs = new std::map<std::string, EffectUI*>();
	_categoryLists = new std::map<std::string, std::vector<std::string>*>();
	_effectsEnabled = new std::map<std::string, bool>();

	// Effect Registry
	for (int index = 0; index < settings.GetRegistryCount(); index++)
	{
		auto element = settings.GetFromRegistry(index);

		// MEMORY! (see ~SignalChainUI)
		EffectUI* effectUI = new EffectUI(element.GetName(), element.GetCategory(), element.GetInfoText(), element.GetName(), ftxui::Color::White);

		effectUI->Initialize(element);

		_effectUIs->insert(std::make_pair(element.GetName(), effectUI));

		// Category
		if (!_categoryLists->contains(element.GetCategory()))
		{
			// (MEMORY!) ~SignalChainUI
			_categoryLists->insert(std::make_pair(element.GetCategory(), new std::vector<std::string>()));
		}

		_categoryLists->at(element.GetCategory())->push_back(element.GetName());
		_effectsEnabled->insert(std::make_pair(element.GetName(), false));
	}

	// -> ToUI (display the current signal chain)
	this->ToUI(settings);
}
SignalChainUI::~SignalChainUI()
{
	// MEMORY! (EffectUI*)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _categoryLists->begin(); iter != _categoryLists->end(); ++iter)
	{
		delete iter->second;
	}

	delete _effectUIs;
	delete _categoryLists;
	delete _effectsEnabled;
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
	_component->DetachAllChildren();

	// Effects Enabled
	for (auto iter = _effectsEnabled->begin(); iter != _effectsEnabled->end(); ++iter)
	{
		// Show UI
		if (iter->second)
		{
			// -> UpdateComponent()
			_effectUIs->at(iter->first)->UpdateComponent(clearDirty);

			// Add to component stack
			_component->Add(_effectUIs->at(iter->first)->GetComponent());
		}
			
	}

	if (clearDirty)
		this->ClearDirty();
}

void SignalChainUI::ToUI(const SignalChainSettings& source)
{
	// Effects Enabled
	for (auto iter = _effectsEnabled->begin(); iter != _effectsEnabled->end(); ++iter)
	{
		// Set for UpdateComponent() pass
		iter->second = source.SignalContains(iter->first);
	}
}

void SignalChainUI::FromUI(SignalChainSettings& destination, bool clearDirty)
{
	// Effects Included (currently)
	for (auto iter = _effectsEnabled->begin(); iter != _effectsEnabled->end(); ++iter)
	{
		// Enabled
		if (iter->second)
		{
			auto effect = _effectUIs->at(iter->first);

			// Copy settings from the UI
			SignalSettings settings(iter->first, "", "", true);
			effect->FromUI(settings, clearDirty);

			// Add settings to the output
			destination.SignalAdd(settings);
		}
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