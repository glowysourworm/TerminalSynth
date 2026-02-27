#pragma once

#ifndef SIGNAL_CHAIN_UI_H
#define SIGNAL_CHAIN_UI_H

#include "ActiveEditorUI.h"
#include "CheckListUI.h"
#include "EffectUI.h"
#include "Envelope.h"
#include "EnvelopeUI.h"
#include "OscillatorParameters.h"
#include "OscillatorUI.h"
#include "SignalChainSettings.h"
#include "SignalModelUI.h"
#include "SignalSettings.h"
#include "SignalUI.h"
#include "SynthSettings.h"
#include "UIBase.h"
#include <cmath>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/direction.hpp>
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
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const SignalChainSettings& source) override;
	void FromUI(SignalChainSettings& destination, bool clearDirty) override;

	bool GetDirty() const override;

private:

	ftxui::Component _component;
	ftxui::Component _signalEffectsContainer;
	ftxui::Component _signalEffectsList;
	ftxui::Component _editorContainer;

	float* _signalEffectsListScrollY;
	bool* _signalEffectsHover;
	float _scrollDeltaY;

	// Editors
	OscillatorUI* _oscillatorUI;
	CheckListUI* _pluginListUI;
	EnvelopeUI* _envelopeUI;
	ActiveEditorUI* _activeEditorUI;

	// Static Signal Chain Elements
	SignalUI* _oscillatorSignalUI;
	SignalUI* _envelopeSignalUI;

	// SignalUI instances by name
	std::map<std::string, SignalUI*>* _signalUIs;

	// EffectUI instances by name
	std::map<std::string, EffectUI*>* _effectUIs;

	// Category Name, Effect Name(s)
	std::map<std::string, std::vector<std::string>*>* _categoryLists;
};

SynthTabUI::SynthTabUI(const SynthSettings& synthSettings, const SignalChainSettings& settings) : UIBase("No Title", "No Title", ftxui::Color::GreenYellow)
{
	_activeEditorUI = new ActiveEditorUI();
	_signalEffectsListScrollY = new float(0);
	_signalEffectsHover = new bool(false);
	_scrollDeltaY = 0.05;

	SignalModelUI envelopeModel("Envelope", true);
	SignalModelUI oscillatorModel("Oscillator", true);

	_pluginListUI = new CheckListUI(ftxui::Color::White);
	_envelopeUI = new EnvelopeUI();
	_oscillatorUI = new OscillatorUI(synthSettings.GetSoundBankSettings(), ftxui::Color::White);
	_envelopeSignalUI = new SignalUI(envelopeModel, false, false);
	_oscillatorSignalUI = new SignalUI(oscillatorModel, false, false);
	_signalUIs = new std::map<std::string, SignalUI*>();
	_effectUIs = new std::map<std::string, EffectUI*>();
	_categoryLists = new std::map<std::string, std::vector<std::string>*>();

	// Initialize some of these child UI's right away
	std::vector<std::string> pluginList;
	settings.GetRegistryList(pluginList);

	_pluginListUI->Initialize(pluginList);
	_oscillatorUI->Initialize(*settings.GetOscillatorParameters());
	_envelopeUI->Initialize(*settings.GetOscillatorEnvelope());
	_oscillatorSignalUI->Initialize(oscillatorModel);
	_envelopeSignalUI->Initialize(envelopeModel);

	// Effect Registry
	for (int index = 0; index < settings.GetRegistryCount(); index++)
	{
		auto element = settings.GetFromRegistry(index);

		// Enabled:  "True" will be the initial setting; but the signal won't be included in the chain
		//			 until you've selected it.
		//
		SignalModelUI signalModel(element.GetName(), true);

		// (MEMORY!) ~SignalChainUI
		EffectUI* effectUI = new EffectUI(element.GetName(), element.GetCategory(), element.GetInfoText(), element.GetName(), ftxui::Color::White);

		// (MEMORY!) ~SignalChainUI
		SignalUI* signalUI = new SignalUI(signalModel, true, true);

		effectUI->Initialize(element);
		signalUI->Initialize(signalModel);

		_effectUIs->insert(std::make_pair(element.GetName(), effectUI));
		_signalUIs->insert(std::make_pair(element.GetName(), signalUI));

		// Category
		if (!_categoryLists->contains(element.GetCategory()))
		{
			// (MEMORY!) ~SignalChainUI
			_categoryLists->insert(std::make_pair(element.GetCategory(), new std::vector<std::string>()));
		}

		_categoryLists->at(element.GetCategory())->push_back(element.GetName());
	}
}
SynthTabUI::~SynthTabUI()
{
	// MEMORY! (EffectUI*)
	for (auto iter = _effectUIs->begin(); iter != _effectUIs->end(); ++iter)
	{
		delete iter->second;
	}
	// MEMORY! (SignalUI*)
	for (auto iter = _signalUIs->begin(); iter != _signalUIs->end(); ++iter)
	{
		delete iter->second;
	}

	// MEMORY! ~std::vector
	for (auto iter = _categoryLists->begin(); iter != _categoryLists->end(); ++iter)
	{
		delete iter->second;
	}

	delete _activeEditorUI;
	delete _signalEffectsListScrollY;
	delete _signalEffectsHover;
	delete _oscillatorUI;
	delete _pluginListUI;
	delete _envelopeUI;
	delete _effectUIs;
	delete _signalUIs;
	delete _categoryLists;
}

void SynthTabUI::Initialize(const SignalChainSettings& initialValue)
{
	UIBase::Initialize(initialValue);

	// Slider bound to the position of the list
	ftxui::SliderOption<float> sliderOptions;
	sliderOptions.value = _signalEffectsListScrollY;
	sliderOptions.min = 0.f;
	sliderOptions.max = 1.f;
	sliderOptions.increment = 0.1f;
	sliderOptions.direction = ftxui::Direction::Down;
	sliderOptions.color_active = ftxui::Color::BlueLight;
	sliderOptions.color_inactive = ftxui::Color::White;
	auto scrollBarY = ftxui::Slider(sliderOptions);

	// Signal Chain Effects
	_signalEffectsList = ftxui::Container::Vertical({});

	// Signal Chain Effects (Container) (Scroll View)
	_signalEffectsContainer = ftxui::Container::Horizontal({

		// Must render the list to add the focused relative position offset
		ftxui::Renderer(_signalEffectsList, [&] {
			return _signalEffectsList->Render() | ftxui::focusPositionRelative(0, *_signalEffectsListScrollY) | ftxui::yframe;
		}) | ftxui::xflex_shrink,

		scrollBarY | ftxui::yframe

		}) | ftxui::CatchEvent([&](ftxui::Event event) {

			if (event.mouse().button == ftxui::Mouse::Button::WheelUp && *_signalEffectsHover)
			{
				// Set Mouse Wheel (clipped [0,1])
				*_signalEffectsListScrollY = fminf(fmaxf(*_signalEffectsListScrollY - _scrollDeltaY, 0), 1);
			}
			else if (event.mouse().button == ftxui::Mouse::Button::WheelDown && *_signalEffectsHover)
			{
				// Set Mouse Wheel (clipped [0,1])
				*_signalEffectsListScrollY = fminf(fmaxf(*_signalEffectsListScrollY + _scrollDeltaY, 0), 1);
			}

			// Pass through
			if (event.is_mouse())
				return false;

			// Cancel keyboard events
			return true;

		}) | ftxui::Hoverable(_signalEffectsHover);

	// Signal Chain
	auto signalChain = ftxui::Container::Vertical({
		ftxui::Renderer([&] {return ftxui::text("Signal Input") | ftxui::color(this->GetLabelColor()); }),
		ftxui::Renderer([&] {return ftxui::separator(); }),
		_oscillatorSignalUI->GetComponent() | ftxui::xflex_grow,
		_envelopeSignalUI->GetComponent() | ftxui::xflex_grow,
		ftxui::Renderer([&] {return ftxui::separator(); }),
		ftxui::Renderer([&] {return ftxui::text("Signal Effects") | ftxui::color(this->GetLabelColor()); }),
		ftxui::Renderer([&] {return ftxui::separator(); }),

		// Rest of effects
		_signalEffectsContainer | ftxui::yflex_shrink
	});

	// Enabled Signal Chain Elements
	for (int index = 0; index < initialValue.GetCount(); index++)
	{
		_signalEffectsContainer->Add(_signalUIs->at(initialValue.Get(index).GetName())->GetComponent() | ftxui::xflex_grow);
	}

	// Plugin List | Signal Chain (Vertical) | Effect Editor
	_component = ftxui::Container::Horizontal({

		// Airwin Plugin Effects
		_pluginListUI->GetComponent(),

		// Signal Chain
		signalChain | ftxui::border,

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

void SynthTabUI::UpdateComponent(bool clearDirty)
{
	_oscillatorUI->UpdateComponent(clearDirty);

	_pluginListUI->UpdateComponent(clearDirty);
	_envelopeUI->UpdateComponent(clearDirty);
	
	if (_activeEditorUI != nullptr)
		_activeEditorUI->UpdateComponent(clearDirty);

	// Static Signal Chain Elements
	_oscillatorSignalUI->UpdateComponent(clearDirty);
	_envelopeSignalUI->UpdateComponent(clearDirty);

	_signalEffectsList->DetachAllChildren();

	// SignalUIs
	for (auto iter = _signalUIs->begin(); iter != _signalUIs->end(); ++iter)
	{
		// Enabled
		if (_pluginListUI->IsSelected(iter->first))
		{
			// -> UpdateComponent()
			_signalUIs->at(iter->first)->UpdateComponent(clearDirty);

			// Add to component stack
			_signalEffectsList->Add(_signalUIs->at(iter->first)->GetComponent());

			// Check Active Effect Editor
			if (_signalUIs->at(iter->first)->GetEditFlag())
				_activeEditorUI->SetEffect(_effectUIs->at(iter->first));
		}
	}

	if (clearDirty)
		this->ClearDirty();
}

void SynthTabUI::ToUI(const SignalChainSettings& source)
{
	
}

void SynthTabUI::FromUI(SignalChainSettings& destination, bool clearDirty)
{
	// Effects (Enable / Disable)
	for (auto iter = _signalUIs->begin(); iter != _signalUIs->end(); ++iter)
	{
		if (_pluginListUI->IsSelected(iter->first))
		{

		}
	}

	OscillatorParameters oscillatorParameters = *destination.GetOscillatorParameters();
	Envelope envelope = *destination.GetOscillatorEnvelope();
	std::vector<std::string> pluginList;
	bool dummy;
	SignalModelUI dummySignal("", true);

	_oscillatorUI->FromUI(oscillatorParameters, clearDirty);
	_pluginListUI->FromUI(pluginList, clearDirty);
	_envelopeUI->FromUI(envelope, clearDirty);
	_activeEditorUI->FromUI(dummy, clearDirty);
	_oscillatorSignalUI->FromUI(dummySignal, clearDirty);
	_envelopeSignalUI->FromUI(dummySignal, clearDirty);

	if (clearDirty)
	{
		this->ClearDirty();
	}
		
}

bool SynthTabUI::GetDirty() const
{
	bool isDirty = false;

	isDirty |= _pluginListUI->GetDirty();

	// Active Effect Editor
	if (_activeEditorUI != nullptr)
		isDirty |= _activeEditorUI->GetDirty();

	// Signals Included (currently)
	for (auto iter = _signalUIs->begin(); iter != _signalUIs->end(); ++iter)
	{
		if (iter->second)
			isDirty = _signalUIs->at(iter->first)->GetDirty();
	}

	return isDirty;
}

#endif