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
#include "SoundSettings.h"
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

class SynthTabUI : public UIBase<SoundSettings>
{
public:

	SynthTabUI(const SynthSettings& synthSettings);
	~SynthTabUI();

	void Initialize(const SoundSettings& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent() override;

	void ToUI(const SoundSettings& source) override;
	void FromUI(SoundSettings& destination) override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	bool _isDirty;

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

SynthTabUI::SynthTabUI(const SynthSettings& synthSettings)
{
	_isDirty = false;
	_activeEditorUI = new ActiveEditorUI();

	SignalNodeModelUI envelopeModel("Envelope", true, false, false, false, 0);
	SignalNodeModelUI oscillatorModel("Oscillator", true, false, false, false, 0);

	_pluginListUI = new ScrollViewerUI<CheckboxModelUI, CheckboxUI>("Airwin Plugins (airwin@github.com)", ftxui::Color::BlueLight, 0.005);
	_effectsSignalChainUI = new ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>("Signal Effects", ftxui::Color::GreenYellow);

	_envelopeUI = new EnvelopeUI();
	_oscillatorUI = new OscillatorUI(synthSettings.GetSoundBankSettings(), ftxui::Color::White);
	_envelopeSignalUI = new SignalNodeUI(envelopeModel);
	_oscillatorSignalUI = new SignalNodeUI(oscillatorModel);

	_signalModels = new std::map<std::string, SignalNodeModelUI*>();
	_pluginModels = new std::map<std::string, CheckboxModelUI*>();
	_effectUIs = new std::map<std::string, EffectUI*>();

	_oscillatorUI->Initialize(*synthSettings.GetSoundSettings()->GetOscillatorParameters());
	_envelopeUI->Initialize(*synthSettings.GetSoundSettings()->GetOscillatorEnvelope());
	_oscillatorSignalUI->Initialize(oscillatorModel);
	_envelopeSignalUI->Initialize(envelopeModel);

	SignalChainSettings* effectRegistry = synthSettings.GetSoundSettings()->GetEffectRegistry();

	// Effect Registry
	for (int index = 0; index < effectRegistry->GetCount(); index++)
	{
		auto element = effectRegistry->Get(index);

		// (MEMORY!) ~SynthTabUI
		EffectUI* effectUI = new EffectUI(element.GetName(), element.GetCategory(), element.GetInfoText(), ftxui::Color::White);

		// (MEMORY!) ~SynthTabUI
		SignalNodeModelUI* signalModelUI = new SignalNodeModelUI(element.GetName(), true, true, true, true, index);

		// (MEMORY!) ~SynthTabUI
		CheckboxModelUI* checkboxModelUI = new CheckboxModelUI(element.GetName(), false, index);

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

void SynthTabUI::Initialize(const SoundSettings& initialValue)
{
	_editorContainer = ftxui::Container::Vertical({});

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
		_editorContainer | ftxui::xflex_grow
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

	// Envelope / Oscillator Signal Nodes
	if (_oscillatorSignalUI->GetDirty())
	{
		_editorContainer->DetachAllChildren();
		_editorContainer->Add(_oscillatorUI->GetComponent());
	}
	if (_envelopeSignalUI->GetDirty())
	{
		_editorContainer->DetachAllChildren();
		_editorContainer->Add(_envelopeUI->GetComponent());
	}

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

				// Check Active Editor
				if (_activeEditorUI->GetEffect() == _effectUIs->at(modelName) && !model.GetIsChecked())
				{
					_activeEditorUI->SetEffect(nullptr);
					_editorContainer->DetachAllChildren();
				}
			}
		}

		// Notify Parent (this may not be cleared for several 100 iterations)
		_isDirty = true;
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
					_editorContainer->DetachAllChildren();
					_editorContainer->Add(_activeEditorUI->GetComponent());
					break;
				case SignalNodeUI::UIAction::Remove:
				{
					// Plugin List (may change)
					CheckboxModelUI* model = _pluginModels->at(signalNodeModelUI.GetName());

					// -> RemoveUI
					_effectsSignalChainUI->RemoveUI(signalNodeModelUI);

					model->SetIsChecked(false);

					// Update Plugin List
					_pluginListUI->ToUI(model->GetName(), *model);

					// Check Active Editor
					if (_activeEditorUI->GetEffect() == _effectUIs->at(modelName))
					{
						_activeEditorUI->SetEffect(nullptr);
						_editorContainer->DetachAllChildren();
					}
				}					
				break;

				// These are tricky to do in real time. We need to make sure to handle one thing
				// at a time in this loop. Come back when the rest is finished.
				case SignalNodeUI::UIAction::MoveDown:
					//_effectsSignalChainUI->MoveDown(modelName);
					_effectsSignalChainUI->ClearDirty(modelName);
					break;
				case SignalNodeUI::UIAction::MoveUp:
					//_effectsSignalChainUI->MoveUp(modelName);
					_effectsSignalChainUI->ClearDirty(modelName);
					break;
				default:
					throw new std::exception("Unhandled UI Action:  SynthTabUI.h");
				}
			}
		}

		// Notify Parent (this may not be cleared for several 100 iterations)
		_isDirty = true;
	}

	// Signal Node UI's (Dirty Flag seems two-purposed. We might need two flags, one for needs update)
	_oscillatorSignalUI->ClearDirty();
	_envelopeSignalUI->ClearDirty();

	_pluginListUI->ClearDirty();
	_effectsSignalChainUI->ClearDirty();
}

void SynthTabUI::ToUI(const SoundSettings& source)
{
	
}

void SynthTabUI::FromUI(SoundSettings& destination)
{
	// OPTIMIZE!
	SignalChainSettings signalChainSettings;

	// Effects (Enable / Disable)
	// 
	for (int index = 0; index < _effectsSignalChainUI->GetUICount(); index++)
	{
		// Effect Name
		std::string modelName = _effectsSignalChainUI->GetName(index);

		// Model (local)
		SignalNodeModelUI* model = _signalModels->at(modelName);

		// -> FromUI
		_effectsSignalChainUI->FromUI(modelName, *model);

		// Effect Settings
		SignalSettings settings;

		if (model->GetEnabled())
		{
			_effectUIs->at(modelName)->FromUI(settings);
		}

		// Add to signal
		signalChainSettings.Add(settings);
	}

	// OPTIMIZE!
	destination.GetSignalChain()->Update(signalChainSettings, true);

	// Signal Input (settings)
	OscillatorParameters oscillatorParameters = *destination.GetOscillatorParameters();
	Envelope envelope = *destination.GetOscillatorEnvelope();	

	_oscillatorUI->FromUI(oscillatorParameters);
	_envelopeUI->FromUI(envelope);

	destination.GetOscillatorParameters()->Update(oscillatorParameters);
	destination.GetOscillatorEnvelope()->Update(envelope);
}

bool SynthTabUI::GetDirty() const
{
	// This is left over from our UpdateComponent() function - which should have
	// a much higher frequency than the ToUI / FromUI functions. This is controlled
	// by the UIController.
	//
	bool isDirty = _isDirty;

	// Plugin List
	isDirty |= _pluginListUI->GetDirty();

	// Signal Nodes (enabled / disabled)
	isDirty |= _effectsSignalChainUI->GetDirty();
	isDirty |= _oscillatorSignalUI->GetDirty();
	isDirty |= _envelopeSignalUI->GetDirty();

	// Active Effect Editor
	isDirty |= _activeEditorUI->GetDirty();
	
	// Signal Input Editors
	isDirty |= _oscillatorUI->GetDirty();
	isDirty |= _envelopeUI->GetDirty();

	return isDirty;
}

void SynthTabUI::ClearDirty()
{
	// Plugin List (enabled / disabled)
	_pluginListUI->ClearDirty();

	// Signal Nodes (enabled / disabled)
	_effectsSignalChainUI->ClearDirty();
	_oscillatorSignalUI->ClearDirty();
	_envelopeSignalUI->ClearDirty();

	// Effects (settings)
	_activeEditorUI->ClearDirty();
	_oscillatorUI->ClearDirty();
	_envelopeUI->ClearDirty();
}

#endif