#pragma once

#ifndef SIGNAL_CHAIN_UI_H
#define SIGNAL_CHAIN_UI_H

#include "ActiveEditorUI.h"
#include "CheckboxModelUI.h"
#include "CheckboxUI.h"
#include "EffectUI.h"
#include "Envelope.h"
#include "EnvelopeUI.h"
#include "OscillatorParameters.h"
#include "OscillatorUI.h"
#include "ScrollViewerUI.h"
#include "SignalChainSettings.h"
#include "SignalNodeModelUI.h"
#include "SignalNodeUI.h"
#include "SignalSettings.h"
#include "SynthSettings.h"
#include "UIBase.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

class SynthTabUI : public UIBase<SignalChainSettings>
{
public:

	SynthTabUI(const SynthSettings& synthSettings, const SignalChainSettings& settings);
	~SynthTabUI();

	void Initialize(const SignalChainSettings& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent() override;

	void ToUI(const SignalChainSettings& source) override;
	void FromUI(SignalChainSettings& destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	ftxui::Component _component;
	ftxui::Component _signalInputContainer;
	ftxui::Component _signalEffectsContainer;
	ftxui::Component _signalChainContainer;
	ftxui::Component _editorContainer;

	// Editors
	OscillatorUI* _oscillatorUI;
	EnvelopeUI* _envelopeUI;
	ActiveEditorUI* _activeEditorUI;

	// Scroll Viewers
	ScrollViewerUI<CheckboxModelUI, CheckboxUI>* _pluginListUI;
	ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>* _effectsSignalChainUI;

	std::map<std::string, SignalNodeModelUI*>* _signalModels;
	std::map<std::string, CheckboxModelUI*>* _pluginModels;

	// Static Signal Chain Elements
	SignalNodeUI* _oscillatorSignalUI;
	SignalNodeUI* _envelopeSignalUI;

	// EffectUI instances by name
	std::map<std::string, EffectUI*>* _effectUIs;
};

SynthTabUI::SynthTabUI(const SynthSettings& synthSettings, const SignalChainSettings& settings)
{
	_activeEditorUI = new ActiveEditorUI();

	SignalNodeModelUI envelopeModel("Envelope", true, false, false);
	SignalNodeModelUI oscillatorModel("Oscillator", true, false, false);

	_pluginListUI = new ScrollViewerUI<CheckboxModelUI, CheckboxUI>("Airwin Plugins (airwin@github.com)", ftxui::Color::BlueLight);
	_effectsSignalChainUI = new ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>("Signal Effects", ftxui::Color::GreenYellow);

	_envelopeUI = new EnvelopeUI();
	_oscillatorUI = new OscillatorUI(synthSettings.GetSoundBankSettings(), ftxui::Color::White);
	_envelopeSignalUI = new SignalNodeUI(envelopeModel);
	_oscillatorSignalUI = new SignalNodeUI(oscillatorModel);

	_signalModels = new std::map<std::string, SignalNodeModelUI*>();
	_pluginModels = new std::map<std::string, CheckboxModelUI*>();
	_effectUIs = new std::map<std::string, EffectUI*>();

	_oscillatorUI->Initialize(*settings.GetOscillatorParameters());
	_envelopeUI->Initialize(*settings.GetOscillatorEnvelope());
	_oscillatorSignalUI->Initialize(oscillatorModel);
	_envelopeSignalUI->Initialize(envelopeModel);

	// Effect Registry
	for (int index = 0; index < settings.GetRegistryCount(); index++)
	{
		auto element = settings.GetFromRegistry(index);

		// (MEMORY!) ~SynthTabUI
		EffectUI* effectUI = new EffectUI(element.GetName(), element.GetCategory(), element.GetInfoText(), ftxui::Color::White);

		// (MEMORY!) ~SynthTabUI
		SignalNodeModelUI* signalModelUI = new SignalNodeModelUI(element.GetName(), true, true, true);

		// (MEMORY!) ~SynthTabUI
		CheckboxModelUI* checkboxModelUI = new CheckboxModelUI(element.GetName(), false);

		// Plugin List UI
		_pluginListUI->AddUI(*checkboxModelUI);

		effectUI->Initialize(element);

		_effectUIs->insert(std::make_pair(element.GetName(), effectUI));
		_signalModels->insert(std::make_pair(element.GetName(), signalModelUI));
		_pluginModels->insert(std::make_pair(element.GetName(), checkboxModelUI));
	}
}
SynthTabUI::~SynthTabUI()
{
	// MEMORY! (EffectUI*)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		delete iter->second;
	}
	// MEMORY! (SignalModelUI*)
	for (auto iter = _signalModels->begin(); iter != _signalModels->end(); ++iter)
	{
		delete iter->second;
	}

	// MEMORY! (CheckboxModelUI*)
	for (auto iter = _pluginModels->begin(); iter != _pluginModels->end(); ++iter)
	{
		delete iter->second;
	}

	// Collections
	delete _effectUIs;
	delete _signalModels;
	delete _pluginModels;

	// Editors
	delete _oscillatorUI;
	delete _envelopeUI;
	delete _activeEditorUI;

	// Scroll Viewers
	delete _pluginListUI;
	delete _effectsSignalChainUI;

	// Signal Input
	delete _oscillatorSignalUI;
	delete _envelopeSignalUI;
}

void SynthTabUI::Initialize(const SignalChainSettings& initialValue)
{
	_signalEffectsContainer = ftxui::Container::Vertical({

		// Signal Effects (ScrollViewerUI)
		_effectsSignalChainUI->GetComponent() | ftxui::yflex_shrink

	});

	// Signal Chain
	_signalInputContainer = ftxui::Container::Vertical({

		ftxui::Renderer([&] {return ftxui::text("Signal Input") | ftxui::color(ftxui::Color::GreenYellow); }),
		ftxui::Renderer([&] {return ftxui::separator(); }),
		_oscillatorSignalUI->GetComponent() | ftxui::xflex_grow,
		_envelopeSignalUI->GetComponent() | ftxui::xflex_grow
	});

	_signalChainContainer = ftxui::Container::Vertical({

		// Signal Input
		_signalInputContainer | ftxui::border,

		// Signal Effects (ScrollViewerUI)
		_signalEffectsContainer | ftxui::yflex_grow,
	});

	// Plugin List | Signal Chain (Vertical) | Effect Editor
	_component = ftxui::Container::Horizontal({

		// Airwin Plugin Effects
		_pluginListUI->GetComponent(),

		_signalChainContainer | ftxui::yflex_grow,

		// Active Editor
		_activeEditorUI->GetComponent()
	});
}

ftxui::Component SynthTabUI::GetComponent()
{

	return ftxui::Renderer(_component, [&] {
		return _component->Render();
	});
}

void SynthTabUI::UpdateComponent()
{
	_oscillatorUI->UpdateComponent();
	_envelopeUI->UpdateComponent();
	
	// This will contain dummy text if there is no active editor
	_activeEditorUI->UpdateComponent();

	// Static Signal Chain Elements
	_oscillatorSignalUI->UpdateComponent();
	_envelopeSignalUI->UpdateComponent();

	// Scroll Viewers
	_effectsSignalChainUI->UpdateComponent();
	_pluginListUI->UpdateComponent();

	bool signalLayoutDirty = false;

	// Plugin List (Changes)
	if (_pluginListUI->GetDirty())
	{
		for (int index = 0; index < _pluginListUI->GetUICount(); index++)
		{
			// -> FromUI
			CheckboxModelUI model;
			_pluginListUI->FromUI(index, model);

			// Has Changed
			if (model.GetIsChecked() != _pluginModels->at(model.GetName())->GetIsChecked())
			{
				// Add
				if (model.GetIsChecked())
					_effectsSignalChainUI->AddUI(*_signalModels->at(model.GetName()));

				// Remove
				else
					_effectsSignalChainUI->RemoveUI(*_signalModels->at(model.GetName()));

				// Model Update (we must update our copy)
				_pluginModels->at(model.GetName())->SetIsChecked(model.GetIsChecked());
			}
		}

		_pluginListUI->ClearDirty();
	}

	//// Reset Signal Effects Layout (when there has been a change)
	//if (_pluginListUI->GetDirty() || _effectsSignalChainUI->GetDirty())
	//{
	//	std::vector<SignalNodeUI*> signalsIncluded;
	//	SignalNodeUI::UIAction lastAction = SignalNodeUI::UIAction::None;
	//	int orderIndex = 0;

	//	// SignalUIs
	//	for (auto iter = _signalUIs->begin(); iter != _signalUIs->end(); ++iter)
	//	{
	//		// Enabled
	//		if (_pluginListUI->IsSelected(iter->first))
	//		{
	//			// Add / Remove (reset order)
	//			if (_pluginListUI->GetDirty())
	//				iter->second->SetOrder(orderIndex++);

	//			// UI Action
	//			SignalNodeUI::UIAction action = iter->second->GetUIAction();

	//			// UI Action Pending
	//			if (iter->second->GetHasUIAction() && lastAction == SignalNodeUI::UIAction::None)
	//			{
	//				switch (action)
	//				{
	//				case SignalNodeUI::UIAction::Edit:
	//					_activeEditorUI->SetEffect(_effectUIs->at(iter->first));
	//					break;
	//				case SignalNodeUI::UIAction::Remove:
	//					_pluginListUI->Toggle(iter->first);
	//					break;
	//				case SignalNodeUI::UIAction::MoveDown:
	//					iter->second->SetOrder(iter->second->GetOrder() + 1);
	//					break;
	//				case SignalNodeUI::UIAction::MoveUp:
	//					iter->second->SetOrder(iter->second->GetOrder() - 1);
	//					break;
	//				default:
	//					throw new std::exception("Unhandled UI Action:  SynthTabUI.h");
	//				}

	//				lastAction = action;
	//			}

	//			// Re-Order
	//			else if (lastAction == SignalNodeUI::UIAction::MoveDown)
	//			{
	//				iter->second->SetOrder(iter->second->GetOrder() - 1);
	//				lastAction = SignalNodeUI::UIAction::None;
	//			}

	//			// Re-Order
	//			else if (lastAction == SignalNodeUI::UIAction::MoveUp)
	//			{
	//				iter->second->SetOrder(iter->second->GetOrder() + 1);
	//				lastAction = SignalNodeUI::UIAction::None;
	//			}

	//			// -> UpdateComponent()
	//			_signalUIs->at(iter->first)->UpdateComponent();

	//			// Include in component stack
	//			signalsIncluded.push_back(iter->second);
	//		}
	//	}

	//	// Sort SignalUI(s)
	//	std::sort(signalsIncluded.begin(), signalsIncluded.end(), [](SignalNodeUI* signalUI1, SignalNodeUI* signalUI2) {
	//		return signalUI1->GetOrder() < signalUI2->GetOrder();
	//	});

	//	// Add to component stack
	//	for (int index = 0; index < signalsIncluded.size(); index++)
	//	{
	//		_signalEffectsList->Add(signalsIncluded[index]->GetComponent());
	//	}
	//}
}

void SynthTabUI::ToUI(const SignalChainSettings& source)
{
	
}

void SynthTabUI::FromUI(SignalChainSettings& destination)
{
	destination.SignalClear();

	// Effects (Enable / Disable)
	//for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	//{
	//	//// Enabled Status
	//	//SignalNodeModelUI model("", false, 0);
	//	//_signalModels->at(iter->first)->FromUI(model);

	//	//// Effect Settings
	//	//SignalSettings effectSettings("", "", "", true);
	//	//iter->second->FromUI(effectSettings);

	//	//// Enabled (part of the signal chain)
	//	//if (model.GetEnabled())
	//	//	destination.SignalAdd(effectSettings);
	//}

	//OscillatorParameters oscillatorParameters = *destination.GetOscillatorParameters();
	//Envelope envelope = *destination.GetOscillatorEnvelope();
	//std::vector<std::string> pluginList;
	//bool dummy;
	//SignalNodeModelUI dummySignal("", true, 0);

	//_oscillatorUI->FromUI(oscillatorParameters);
	////_pluginListUI->FromUI(pluginList);
	//_envelopeUI->FromUI(envelope);
	//_activeEditorUI->FromUI(dummy);
	//_oscillatorSignalUI->FromUI(dummySignal);
	//_envelopeSignalUI->FromUI(dummySignal);
}

bool SynthTabUI::GetDirty() const
{
	bool isDirty = false;

	isDirty |= _pluginListUI->GetDirty();

	// Active Effect Editor
	isDirty |= _activeEditorUI->GetDirty();

	// Signals Included (currently)
	//for (auto iter = _signalUIs->begin(); iter != _signalUIs->end(); ++iter)
	//{
	//	if (iter->second)
	//		isDirty = _signalUIs->at(iter->first)->GetDirty();
	//}

	return isDirty;
}

void SynthTabUI::ClearDirty()
{
}

#endif