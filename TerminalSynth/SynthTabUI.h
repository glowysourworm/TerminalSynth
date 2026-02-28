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
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>

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

	SignalNodeModelUI envelopeModel("Envelope", true, false, false, false);
	SignalNodeModelUI oscillatorModel("Oscillator", true, false, false, false);

	_pluginListUI = new ScrollViewerUI<CheckboxModelUI, CheckboxUI>("Airwin Plugins (airwin@github.com)", ftxui::Color::BlueLight, 0.005);
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
		SignalNodeModelUI* signalModelUI = new SignalNodeModelUI(element.GetName(), true, true, true, true);

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
		_signalEffectsContainer | ftxui::yflex_shrink,
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
		for (int index = _pluginListUI->GetUICount() - 1; index >= 0; index--)
		{
			// -> FromUI
			std::string modelName = _pluginListUI->GetName(index);
			CheckboxModelUI model;
			_pluginListUI->FromUI(modelName, model);

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

			// Clear Dirty (Indexed)
			_pluginListUI->ClearDirty(modelName);
		}
	}

	// Signal Chain (Changes)
	if (_effectsSignalChainUI->GetDirty())
	{
		for (int index = _effectsSignalChainUI->GetUICount() - 1; index >= 0; index--)
		{
			std::string modelName = _effectsSignalChainUI->GetName(index);

			// Get UI Component (for the real-time captures)
			SignalNodeUI* signalNodeUI = _effectsSignalChainUI->GetUI(modelName);

			// UI Action (these could be put into the model classes)
			bool hasUIAction = signalNodeUI->GetHasUIAction();
			SignalNodeUI::UIAction action = signalNodeUI->GetUIAction();

			// UI Action Pending
			if (hasUIAction)
			{
				// -> FromUI
				SignalNodeModelUI signalNodeModelUI;
				_effectsSignalChainUI->FromUI(modelName, signalNodeModelUI);

				switch (action)
				{
				case SignalNodeUI::UIAction::Edit:
					_activeEditorUI->SetEffect(_effectUIs->at(signalNodeModelUI.GetName()));
					break;
				case SignalNodeUI::UIAction::Remove:
				{
					// Plugin List (may change)
					CheckboxModelUI model = *_pluginModels->at(signalNodeModelUI.GetName());

					// -> RemoveUI
					_effectsSignalChainUI->RemoveUI(signalNodeModelUI);

					model.SetIsChecked(false);

					// Update Plugin List
					//_pluginListUI->ToUI(model);
				}					
				break;
				case SignalNodeUI::UIAction::MoveDown:
					//iter->second->SetOrder(iter->second->GetOrder() + 1);
					break;
				case SignalNodeUI::UIAction::MoveUp:
					//iter->second->SetOrder(iter->second->GetOrder() - 1);
					break;
				default:
					throw new std::exception("Unhandled UI Action:  SynthTabUI.h");
				}

				//lastAction = action;
			}
		}
	}
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