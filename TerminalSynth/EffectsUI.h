#pragma once

#ifndef SIGNAL_CHAIN_UI_H
#define SIGNAL_CHAIN_UI_H

#include "ActiveEditorUI.h"
#include "CheckboxModelUI.h"
#include "CheckboxUI.h"
#include "ControlPanelUI.h"
#include "EffectUI.h"
#include "EffectsModelUI.h"
#include "ScrollViewerUI.h"
#include "SignalChainSettings.h"
#include "SignalNodeModelUI.h"
#include "SignalNodeUI.h"
#include "SignalSettings.h"
#include "SoundSettings.h"
#include "UIBase.h"
#include "ValueCapture.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

class EffectsUI : public UIBase<EffectsModelUI>
{
public:

	EffectsUI(const EffectsModelUI& initialValue);
	~EffectsUI();

	void Initialize(const EffectsModelUI& initialValue) override;
	ftxui::Component GetComponent() override;

	void ServicePendingAction() override;
	void UpdateComponent() override;
	void Tick() override;

	void ToUI(const EffectsModelUI& source) override;
	void ToUI(const EffectsModelUI* source) override;
	void FromUI(EffectsModelUI& destination) override;
	void FromUI(EffectsModelUI* destination) override;

	bool HasPendingAction() const override;
	void ClearPendingAction() override;

	bool GetDirty() const override;
	void ClearDirty() override;

private:

	void OnChangeCategory();

private:

	ftxui::Component _component;
	ftxui::Component _signalEffectsContainer;
	ftxui::Component _signalChainContainer;
	ftxui::Component _editorContainer;

	// Editors
	ActiveEditorUI* _activeEditorUI;

	// Scroll Viewers
	ScrollViewerUI<CheckboxModelUI, CheckboxUI>* _pluginListUI;
	ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>* _postProcessingUI;

	std::map<std::string, SignalNodeModelUI*>* _postProcessingModels;
	std::map<std::string, CheckboxModelUI*>* _pluginModels;

	// Effect Cateogies
	std::vector<std::string>* _effectCategories;
	std::map<std::string, std::vector<std::string>*>* _effectsByCategory;

	ValueCapture<int>* _effectCategorySelectedIndex;

	// EffectUI instances by name
	std::map<std::string, EffectUI*>* _effectUIs;

	// Extra local dirty status for enabling / disabling effects
	bool _isDirty;
};

EffectsUI::EffectsUI(const EffectsModelUI& model)
{
	_isDirty = false;
	_activeEditorUI = new ActiveEditorUI();

	SignalNodeModelUI envelopeModel("Envelope", true, false, false, false, 0);
	SignalNodeModelUI oscillatorModel("Oscillator", true, false, false, false, 0);

	_pluginListUI = new ScrollViewerUI<CheckboxModelUI, CheckboxUI>(0.005);
	_postProcessingUI = new ScrollViewerUI<SignalNodeModelUI, SignalNodeUI>();

	_effectCategories = new std::vector<std::string>();
	_effectsByCategory = new std::map<std::string, std::vector<std::string>*>();

	_effectCategorySelectedIndex = new ValueCapture<int>(0);

	_postProcessingModels = new std::map<std::string, SignalNodeModelUI*>();
	_pluginModels = new std::map<std::string, CheckboxModelUI*>();
	_effectUIs = new std::map<std::string, EffectUI*>();

	auto effectRegistry = model.GetEffectRegistryList();

	// Effect Registry
	for (int index = 0; index < effectRegistry->size(); index++)
	{
		auto element = effectRegistry->at(index);

		// New Category
		if (!_effectsByCategory->contains(element->GetCategory()))
		{
			_effectCategories->push_back(element->GetCategory());
			_effectsByCategory->insert(std::make_pair(element->GetCategory(), new std::vector<std::string>()));		// MEMORY! ~EffectsUI
		}

		// List By Category
		_effectsByCategory->at(element->GetCategory())->push_back(element->GetName());

		// (MEMORY!) ~EffectsUI
		EffectUI* effectUI = new EffectUI(element->GetName(), element->GetCategory(), element->GetInfoText(), ftxui::Color::White);

		// (MEMORY!) ~EffectsUI
		SignalNodeModelUI* signalModelUI = new SignalNodeModelUI(element->GetName(), true, true, true, true, index);

		// (MEMORY!) ~EffectsUI
		CheckboxModelUI* checkboxModelUI = new CheckboxModelUI(element->GetName(), false, index);

		// Plugin List UI (FIRST CATEGORY)
		if (_effectCategories->at(0) == element->GetCategory())
			_pluginListUI->AddUI(*checkboxModelUI);

		effectUI->Initialize(*element);

		_effectUIs->insert(std::make_pair(element->GetName(), effectUI));
		_postProcessingModels->insert(std::make_pair(element->GetName(), signalModelUI));
		_pluginModels->insert(std::make_pair(element->GetName(), checkboxModelUI));
	}
}
EffectsUI::~EffectsUI()
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

	for (auto iter = _effectsByCategory->begin(); iter != _effectsByCategory->end(); ++iter)
	{
		// std::vector*
		delete iter->second;
	}

	// Collections
	delete _effectUIs;
	delete _postProcessingModels;
	delete _pluginModels;

	delete _effectCategories;
	delete _effectsByCategory;

	delete _effectCategorySelectedIndex;

	// Editors
	delete _activeEditorUI;

	// Scroll Viewers
	delete _pluginListUI;
	delete _postProcessingUI;
}

void EffectsUI::Initialize(const EffectsModelUI& model)
{	
	auto categoryDropdown = ftxui::Dropdown(_effectCategories, _effectCategorySelectedIndex->GetRef());

	_editorContainer = ftxui::Container::Vertical({});

	_signalEffectsContainer = ftxui::Container::Vertical({

		ftxui::Renderer([&] {return ftxui::text("Effects Chain") | ftxui::color(ftxui::Color::GreenYellow); }),
		ftxui::Renderer([&] {return ftxui::separator(); }),

		// Signal Effects (ScrollViewerUI)
		_postProcessingUI->GetComponent() | ftxui::yflex_shrink

	}) | ftxui::border;

	_signalChainContainer = ftxui::Container::Vertical({

		// Effects Chain (ScrollViewerUI)
		_signalEffectsContainer | ftxui::yflex_shrink,
	});

	// Plugin List | Signal Chain (Vertical) | Effect Editor
	_component = ftxui::Container::Vertical({

		ftxui::Container::Horizontal({

			// Airwin Plugin Effects
			ftxui::Container::Vertical({

				ftxui::Renderer([] { return ftxui::text("Airwin Plugins (airwin@github.com)") | ftxui::color(ftxui::Color::BlueLight); }),
				ftxui::Renderer([] {return ftxui::separator(); }),

				categoryDropdown,

				ftxui::Renderer([] {return ftxui::separator(); }),

				_pluginListUI->GetComponent() | ftxui::yflex_shrink

			}) | ftxui::border,

			_signalChainContainer | ftxui::yflex_grow,

			// Active Editor
			_editorContainer | ftxui::xflex_grow
		})
	});
}

ftxui::Component EffectsUI::GetComponent()
{
	return _component;
}

void EffectsUI::ServicePendingAction()
{
	// Category Selector
	if (_effectCategorySelectedIndex->HasChanged())
	{
		OnChangeCategory();
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
				if (model->GetIsChecked() && !_postProcessingUI->Contains(modelName))
					_postProcessingUI->AddUI(*_postProcessingModels->at(modelName));

				// Remove
				else if (_postProcessingUI->Contains(modelName))
					_postProcessingUI->RemoveUI(*_postProcessingModels->at(modelName));

				// Check Active Editor
				if (_activeEditorUI->GetEffect() == _effectUIs->at(modelName) && !model->GetIsChecked())
				{
					_activeEditorUI->SetEffect(nullptr);
					_editorContainer->DetachAllChildren();
				}
			}
		}

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
					if (_postProcessingUI->Contains(modelName))
						_postProcessingUI->RemoveUI(*signalNodeModelUI);

					model->SetIsChecked(false);

					// Update Plugin List
					if (_pluginListUI->Contains(modelName))
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
	}
}

void EffectsUI::OnChangeCategory()
{
	std::string currentCategory = _effectCategories->at(_effectCategorySelectedIndex->GetValue());

	// Set Plugin List (for current category)
	//
	for (auto iter = _pluginModels->begin(); iter != _pluginModels->end(); ++iter)
	{
		SignalSettings effectSettings;
		CheckboxModelUI* model = iter->second;
		std::string modelName = iter->first;
		
		// Get Effect Category
		_effectUIs->at(modelName)->FromUI(effectSettings);
		
		// Remove
		if (currentCategory != effectSettings.GetCategory() && _pluginListUI->Contains(modelName))
			_pluginListUI->RemoveUI(*model);

		// Add
		else if (currentCategory == effectSettings.GetCategory())
			_pluginListUI->AddUI(*model);

		//model->SetIsChecked(false);

		// Check Active Editor
		//if (_activeEditorUI->GetEffect() == _effectUIs->at(modelName))
		//{
		//	_activeEditorUI->SetEffect(nullptr);
		//	_editorContainer->DetachAllChildren();
		//}
	}
}

void EffectsUI::UpdateComponent()
{
	// This will contain dummy text if there is no active editor
	_activeEditorUI->UpdateComponent();

	// Scroll Viewers
	_postProcessingUI->UpdateComponent();
	_pluginListUI->UpdateComponent();
}

void EffectsUI::Tick()
{
	// This will contain dummy text if there is no active editor
	_activeEditorUI->Tick();

	// Scroll Viewers
	_postProcessingUI->Tick();
	_pluginListUI->Tick();
}

void EffectsUI::ToUI(const EffectsModelUI& source)
{
	
}

void EffectsUI::ToUI(const EffectsModelUI* source)
{
}

void EffectsUI::FromUI(EffectsModelUI& destination)
{
	throw new std::exception("Pleaes use the pointer version of this function FromUI(..)");
}

void EffectsUI::FromUI(EffectsModelUI* destination)
{
	// Post Processing (for update)
	SignalChainSettings* postProcessing = destination->GetSoundSettings()->GetPostProcessing();

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
		if (!postProcessing->Contains(modelName))
		{
			// Effect Settings 
			SignalSettings settings;
			settings.SetIsEnabled(model->GetEnabled());
			_effectUIs->at(modelName)->FromUI(settings);
			postProcessing->Add(settings);
		}

		// Update
		else
		{
			// Effect Settings (already heaped)
			SignalSettings* settings = postProcessing->Get(modelName);
			settings->SetIsEnabled(model->GetEnabled());
			_effectUIs->at(modelName)->FromUI(settings);
		}
	}

	// Remove SignalChainSettings*
	for (int index = postProcessing->GetCount() - 1; index >= 0; index--)
	{
		// Remove
		if (index >= _postProcessingUI->GetUICount())
		{
			postProcessing->RemoveAt(index);
		}
	}
}

bool EffectsUI::HasPendingAction() const
{
	// This is left over from our Tick() function - which should have
	// a much higher frequency than the UpdateComponent / ToUI / FromUI 
	// functions. This is controlled by the UIController.
	//
	bool hasPendingAction = false;

	// Category Chooser
	hasPendingAction |= _effectCategorySelectedIndex->HasChanged();

	// Plugin List
	hasPendingAction |= _pluginListUI->HasPendingAction();

	// Signal Nodes (enabled / disabled)
	hasPendingAction |= _postProcessingUI->HasPendingAction();

	// Active Effect Editor
	hasPendingAction |= _activeEditorUI->HasPendingAction();

	return hasPendingAction;
}

void EffectsUI::ClearPendingAction()
{
	// Category Chooser
	_effectCategorySelectedIndex->Clear();

	// Plugin List (checkbox change)
	_pluginListUI->ClearPendingAction();

	// Signal Nodes (hover, edit, remove, move up, move down)
	_postProcessingUI->ClearPendingAction();

	// Effects (settings) (should not be any actions)
	_activeEditorUI->ClearPendingAction();
}

bool EffectsUI::GetDirty() const
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

	// Active Effect Editor
	isDirty |= _activeEditorUI->GetDirty();
	
	return isDirty;
}

void EffectsUI::ClearDirty()
{
	// Plugin List (enabled / disabled)
	_pluginListUI->ClearDirty();

	// Signal Nodes (enabled / disabled)
	_postProcessingUI->ClearDirty();

	// Effects (settings)
	_activeEditorUI->ClearDirty();

	_isDirty = false;
}

#endif