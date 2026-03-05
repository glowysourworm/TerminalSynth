#pragma once

#ifndef SIGNAL_CHAIN_UI_H
#define SIGNAL_CHAIN_UI_H

#include "ActiveEditorUI.h"
#include "CheckboxModelUI.h"
#include "CheckboxUI.h"
#include "EffectUI.h"
#include "EnvelopeUI.h"
#include "OscillatorUI.h"
#include "ScrollViewerUI.h"
#include "SignalChainSettings.h"
#include "SignalNodeModelUI.h"
#include "SignalNodeUI.h"
#include "SignalSettings.h"
#include "SoundSettings.h"
#include "SynthTabModelUI.h"
#include "UIBase.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>

class SynthTabUI : public UIBase<SynthTabModelUI>
{
public:

	SynthTabUI(const SynthTabModelUI& synthSettings);
	~SynthTabUI();

	void Initialize(const SynthTabModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const SynthTabModelUI& source) override;
	void ToUI(const SynthTabModelUI* source) override;
	void FromUI(SynthTabModelUI& destination) override;
	void FromUI(SynthTabModelUI* destination) override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	ftxui::Component _component;
	ftxui::Component _signalInputContainer;
	ftxui::Component _signalEffectsContainer;
	ftxui::Component _signalChainContainer;
	ftxui::Component _editorContainer;

	SynthTabModelUI* _model;

	// Editors
	OscillatorUI* _oscillatorUI;
	EnvelopeUI* _envelopeUI;
	ActiveEditorUI* _activeEditorUI;

	// Scroll Viewers
	ScrollViewerUI<CheckboxModelUI, CheckboxUI>* _pluginListUI;
	ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>* _postProcessingUI;

	std::map<std::string, SignalNodeModelUI*>* _postProcessingModels;
	std::map<std::string, CheckboxModelUI*>* _pluginModels;

	// Static Signal Chain Elements
	SignalNodeUI* _oscillatorSignalUI;
	SignalNodeUI* _envelopeSignalUI;

	// EffectUI instances by name
	std::map<std::string, EffectUI*>* _effectUIs;

	// Extra local dirty status for enabling / disabling effects
	bool _isDirty;
};

SynthTabUI::SynthTabUI(const SynthTabModelUI& model)
{
	_isDirty = false;
	_model = new SynthTabModelUI(model);
	_activeEditorUI = new ActiveEditorUI();

	SignalNodeModelUI envelopeModel("Envelope", true, false, false, false, 0);
	SignalNodeModelUI oscillatorModel("Oscillator", true, false, false, false, 0);

	_pluginListUI = new ScrollViewerUI<CheckboxModelUI, CheckboxUI>("Airwin Plugins (airwin@github.com)", ftxui::Color::BlueLight, 0.005);
	_postProcessingUI = new ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>("Signal Effects", ftxui::Color::GreenYellow);

	_envelopeUI = new EnvelopeUI();
	_oscillatorUI = new OscillatorUI(_model->GetSoundBankSettings(), ftxui::Color::White);
	_envelopeSignalUI = new SignalNodeUI(envelopeModel);
	_oscillatorSignalUI = new SignalNodeUI(oscillatorModel);

	_postProcessingModels = new std::map<std::string, SignalNodeModelUI*>();
	_pluginModels = new std::map<std::string, CheckboxModelUI*>();
	_effectUIs = new std::map<std::string, EffectUI*>();

	_oscillatorUI->Initialize(*_model->GetSoundSettings()->GetOscillatorParameters());
	_envelopeUI->Initialize(*_model->GetSoundSettings()->GetOscillatorEnvelope());
	_oscillatorSignalUI->Initialize(oscillatorModel);
	_envelopeSignalUI->Initialize(envelopeModel);

	SignalChainSettings* effectRegistry = _model->GetSoundSettings()->GetEffectRegistry();

	// Effect Registry
	for (int index = 0; index < effectRegistry->GetCount(); index++)
	{
		auto element = effectRegistry->Get(index);

		// (MEMORY!) ~SynthTabUI
		EffectUI* effectUI = new EffectUI(element->GetName(), element->GetCategory(), element->GetInfoText(), ftxui::Color::White);

		// (MEMORY!) ~SynthTabUI
		SignalNodeModelUI* signalModelUI = new SignalNodeModelUI(element->GetName(), true, true, true, true, index);

		// (MEMORY!) ~SynthTabUI
		CheckboxModelUI* checkboxModelUI = new CheckboxModelUI(element->GetName(), false, index);

		// Plugin List UI
		_pluginListUI->AddUI(*checkboxModelUI);

		effectUI->Initialize(*element);

		_effectUIs->insert(std::make_pair(element->GetName(), effectUI));
		_postProcessingModels->insert(std::make_pair(element->GetName(), signalModelUI));
		_pluginModels->insert(std::make_pair(element->GetName(), checkboxModelUI));
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
	for (auto iter = _postProcessingModels->begin(); iter != _postProcessingModels->end(); ++iter)
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
	delete _postProcessingModels;
	delete _pluginModels;

	// Editors
	delete _oscillatorUI;
	delete _envelopeUI;
	delete _activeEditorUI;

	// Scroll Viewers
	delete _pluginListUI;
	delete _postProcessingUI;

	// Signal Input
	delete _oscillatorSignalUI;
	delete _envelopeSignalUI;

	// Model
	delete _model;
}

void SynthTabUI::Initialize(const SynthTabModelUI& model)
{	
	_editorContainer = ftxui::Container::Vertical({});

	_signalEffectsContainer = ftxui::Container::Vertical({

		// Signal Effects (ScrollViewerUI)
		_postProcessingUI->GetComponent() | ftxui::yflex_shrink

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

void SynthTabUI::ServicePendingAction()
{
	// Envelope / Oscillator Signal Nodes
	if (_oscillatorSignalUI->HasPendingAction())
	{
		_editorContainer->DetachAllChildren();
		_editorContainer->Add(_oscillatorUI->GetComponent());

		// Clear Pending
		_oscillatorSignalUI->ClearPendingAction();
	}
	if (_envelopeSignalUI->HasPendingAction())
	{
		_editorContainer->DetachAllChildren();
		_editorContainer->Add(_envelopeUI->GetComponent());

		// Clear Pending
		_envelopeSignalUI->ClearPendingAction();
	}

	// Plugin List (Changes)
	if (_pluginListUI->HasPendingAction())
	{
		for (int index = _pluginListUI->GetUICount() - 1; index >= 0; index--)
		{
			// -> FromUI (we can go ahead and clear dirty flags here, if there are any)
			std::string modelName = _pluginListUI->GetName(index);
			CheckboxModelUI* model = _pluginModels->at(modelName);

			// Previous Value
			bool previousValue = model->GetIsChecked();

			// Updated Value
			_pluginListUI->FromUI(modelName, model);

			// Has Changed
			if (model->GetIsChecked() != previousValue)
			{
				// Add
				if (model->GetIsChecked())
					_postProcessingUI->AddUI(*_postProcessingModels->at(modelName));

				// Remove
				else
					_postProcessingUI->RemoveUI(*_postProcessingModels->at(modelName));

				// Check Active Editor
				if (_activeEditorUI->GetEffect() == _effectUIs->at(modelName) && !model->GetIsChecked())
				{
					_activeEditorUI->SetEffect(nullptr);
					_editorContainer->DetachAllChildren();
				}
			}
		}

		// Notify Parent (this may not be cleared for several 100 iterations)
		_pluginListUI->ClearPendingAction();

		// Go ahead and notify parent to collect UI data
		_isDirty = true;
	}

	// Signal Chain (Changes)
	if (_postProcessingUI->HasPendingAction())
	{
		for (int index = _postProcessingUI->GetUICount() - 1; index >= 0; index--)
		{
			std::string modelName = _postProcessingUI->GetName(index);

			// Get UI Component (for the real-time captures)
			SignalNodeUI* signalNodeUI = _postProcessingUI->GetUI(modelName);

			// UI Action (these could be put into the model classes)
			bool hasUIAction = signalNodeUI->GetHasUIAction();
			SignalNodeUI::UIAction action = signalNodeUI->GetUIAction();

			// UI Action Pending
			if (hasUIAction)
			{
				// -> FromUI
				SignalNodeModelUI* signalNodeModelUI = _postProcessingModels->at(modelName);
				_postProcessingUI->FromUI(modelName, signalNodeModelUI);

				switch (action)
				{
				case SignalNodeUI::UIAction::Edit:
					_activeEditorUI->SetEffect(_effectUIs->at(modelName));
					_editorContainer->DetachAllChildren();
					_editorContainer->Add(_activeEditorUI->GetComponent());
					break;
				case SignalNodeUI::UIAction::Remove:
				{
					// Plugin List (may change)
					CheckboxModelUI* model = _pluginModels->at(modelName);

					// -> RemoveUI
					_postProcessingUI->RemoveUI(*signalNodeModelUI);

					model->SetIsChecked(false);

					// Update Plugin List
					_pluginListUI->ToUI(model->GetName(), model);

					// Check Active Editor
					if (_activeEditorUI->GetEffect() == _effectUIs->at(modelName))
					{
						_activeEditorUI->SetEffect(nullptr);
						_editorContainer->DetachAllChildren();
					}

					// Notify parent to collect UI data
					_isDirty = true;
				}
				break;

				// These are tricky to do in real time. We need to make sure to handle one thing
				// at a time in this loop. Come back when the rest is finished.
				case SignalNodeUI::UIAction::MoveDown:
					//_effectsSignalChainUI->MoveDown(modelName);
					//_effectsSignalChainUI->ClearDirty(modelName);
					break;
				case SignalNodeUI::UIAction::MoveUp:
					//_effectsSignalChainUI->MoveUp(modelName);
					//_effectsSignalChainUI->ClearDirty(modelName);
					break;
				default:
					throw new std::exception("Unhandled UI Action:  SynthTabUI.h");
				}
			}
		}

		// Notify Parent (this may not be cleared for several 100 iterations)
		_postProcessingUI->ClearPendingAction();
	}
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
	_postProcessingUI->UpdateComponent();
	_pluginListUI->UpdateComponent();
}

void SynthTabUI::Tick()
{
	_oscillatorUI->Tick();
	_envelopeUI->Tick();

	// This will contain dummy text if there is no active editor
	_activeEditorUI->Tick();

	// Static Signal Chain Elements
	_oscillatorSignalUI->Tick();
	_envelopeSignalUI->Tick();

	// Scroll Viewers
	_postProcessingUI->Tick();
	_pluginListUI->Tick();
}

void SynthTabUI::ToUI(const SynthTabModelUI& source)
{
	
}

void SynthTabUI::ToUI(const SynthTabModelUI* source)
{
}

void SynthTabUI::FromUI(SynthTabModelUI& destination)
{
	throw new std::exception("Pleaes use the pointer version of this function FromUI(..)");
}

void SynthTabUI::FromUI(SynthTabModelUI* destination)
{
	// Effects (check for changes)
	// 
	for (int index = 0; index < _postProcessingUI->GetUICount(); index++)
	{
		// Effect Name
		std::string modelName = _postProcessingUI->GetName(index);

		// Model (local)
		SignalNodeModelUI* model = _postProcessingModels->at(modelName);

		// -> FromUI (update local)
		_postProcessingUI->FromUI(modelName, model);

		// Add (if enabled)
		if (!_model->GetSoundSettings()->GetPostProcessing()->Contains(modelName))
		{
			// Effect Settings 
			SignalSettings settings;
			settings.SetIsEnabled(model->GetEnabled());
			_effectUIs->at(modelName)->FromUI(settings);
			_model->GetSoundSettings()->GetPostProcessing()->Add(settings);
		}

		// Update
		else
		{
			// Effect Settings (already heaped)
			SignalSettings* settings = _model->GetSoundSettings()->GetPostProcessing()->Get(modelName);
			settings->SetIsEnabled(model->GetEnabled());
			_effectUIs->at(modelName)->FromUI(settings);
		}
	}

	// Remove SignalChainSettings*
	for (int index = _model->GetSoundSettings()->GetPostProcessing()->GetCount() - 1; index >= 0; index--)
	{
		// Remove
		if (index >= _postProcessingUI->GetUICount())
		{
			_model->GetSoundSettings()->GetPostProcessing()->RemoveAt(index);
		}
	}

	// Signal Input (settings)
	_oscillatorUI->FromUI(_model->GetSoundSettings()->GetOscillatorParameters());
	_envelopeUI->FromUI(_model->GetSoundSettings()->GetOscillatorEnvelope());

	// -> Update
	_model->Update(destination->GetSoundSettings(), destination->GetSoundBankSettings());
}

bool SynthTabUI::HasPendingAction() const
{
	// This is left over from our Tick() function - which should have
	// a much higher frequency than the UpdateComponent / ToUI / FromUI 
	// functions. This is controlled by the UIController.
	//
	bool hasPendingAction = false;

	// Plugin List
	hasPendingAction |= _pluginListUI->HasPendingAction();

	// Signal Nodes (enabled / disabled)
	hasPendingAction |= _postProcessingUI->HasPendingAction();
	hasPendingAction |= _oscillatorSignalUI->HasPendingAction();
	hasPendingAction |= _envelopeSignalUI->HasPendingAction();

	// Active Effect Editor
	hasPendingAction |= _activeEditorUI->HasPendingAction();

	// Signal Input Editors
	hasPendingAction |= _oscillatorUI->HasPendingAction();
	hasPendingAction |= _envelopeUI->HasPendingAction();

	return hasPendingAction;
}

void SynthTabUI::ClearPendingAction()
{
	// Plugin List (checkbox change)
	_pluginListUI->ClearPendingAction();

	// Signal Nodes (hover, edit, remove, move up, move down)
	_postProcessingUI->ClearPendingAction();
	_oscillatorSignalUI->ClearPendingAction();
	_envelopeSignalUI->ClearPendingAction();

	// Effects (settings) (should not be any actions)
	_activeEditorUI->ClearPendingAction();
	_oscillatorUI->ClearPendingAction();
	_envelopeUI->ClearPendingAction();
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
	isDirty |= _postProcessingUI->GetDirty();
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
	_postProcessingUI->ClearDirty();
	_oscillatorSignalUI->ClearDirty();
	_envelopeSignalUI->ClearDirty();

	// Effects (settings)
	_activeEditorUI->ClearDirty();
	_oscillatorUI->ClearDirty();
	_envelopeUI->ClearDirty();

	_isDirty = false;
}

#endif