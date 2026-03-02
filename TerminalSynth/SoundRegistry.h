#pragma once

#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

#include "AirwindowsEffect.h"
#include "OutputSettings.h"
#include "SignalBase.h"
#include "SignalParameter.h"
#include "SignalSettings.h"
#include <AirwinRegistry.h>
#include <AirwinRegistryEntry.h>
#include <airwin_consolidated_base.h>
#include <exception>
#include <map>
#include <string>
#include <utility>
#include <vector>

/// <summary>
/// Registry for AirwinEffect* instances, SignalSettings* for the signal chain, and also sound bank
/// data for the UI. (This was separated from the configuration because of a circular dependency with SignalBase*)
/// </summary>
class SoundRegistry
{
public:

	SoundRegistry();
	~SoundRegistry();

	bool Initialize(float samplingRate);

	int GetCount() const;
	std::string GetName(int index) const;
	void GetEntry(const std::string& name, SignalSettings& destination) const;

	// MEMORY! (user must manage this one!)
	SignalBase* CreateEffect(const std::string& name) const;

private:

	// Loaded from airwindows-plugins.lib 
	AirwinRegistry* _airwinEffectRegistry;

	// Created from Airwin / Local effects 
	std::map<std::string, AirwinRegistryEntry*>* _effectsByName;

	float _samplingRate;
};

SoundRegistry::SoundRegistry()
{
	_airwinEffectRegistry = new AirwinRegistry();
	_effectsByName = new std::map<std::string, AirwinRegistryEntry*>();
}

SoundRegistry::~SoundRegistry()
{
	for (auto iter = _effectsByName->begin(); iter != _effectsByName->end(); ++iter)
	{
		delete iter->second;
	}

	delete _airwinEffectRegistry;
	delete _effectsByName;
}

bool SoundRegistry::Initialize(float samplingRate)
{
	_samplingRate = samplingRate;

	// LOAD AIRWIN PLUGINS!
	bool success = _airwinEffectRegistry->Load(samplingRate);

	std::vector<std::string> pluginList;

	// Get Plugin Name List
	_airwinEffectRegistry->GetPlugins(pluginList);

	for (int index = 0; index < pluginList.size(); index++)
	{
		// Airwin Plugin
		AirwinRegistryEntry* pluginEntry = _airwinEffectRegistry->GetPlugin(pluginList[index]);

		// Store to registry maps
		_effectsByName->insert(std::make_pair(pluginList.at(index), pluginEntry));
	}

	return true;
}

int SoundRegistry::GetCount() const
{
	return _effectsByName->size();
}

std::string SoundRegistry::GetName(int index) const
{
	int counter = 0;

	for (auto iter = _effectsByName->begin(); iter != _effectsByName->end(); ++iter)
	{
		if (counter++ == index)
			return iter->first;
	}

	throw new std::exception("Effect not found by index:  SoundRegistry.h");
}

void SoundRegistry::GetEntry(const std::string& name, SignalSettings& destination) const
{
	AirwinRegistryEntry* entry = _effectsByName->at(name);

	destination.SetName(name);
	destination.SetCategory(entry->GetCategory());
	destination.SetInfoText(entry->GetWhatText());

	// Have to create an instance here to initialize the parameters
	AudioEffectX* effect = entry->CreateEffect(_samplingRate);
	AirwindowsEffect* wrappedEffect = new AirwindowsEffect(effect, name, entry->GetCategory(), entry->GetWhatText());

	for (int paramIndex = 0; paramIndex < wrappedEffect->GetParameterCount(); paramIndex++)
	{
		SignalParameter parameter(
			wrappedEffect->GetParameterName(paramIndex),
			wrappedEffect->GetParameterValue(paramIndex),
			wrappedEffect->GetParameterMin(paramIndex),
			wrappedEffect->GetParameterMax(paramIndex));

		destination.AddParameter(parameter);
	}

	// MEMORY! ~AirwindowsEffect (deletes AudioEffectX*)
	delete wrappedEffect;
}

SignalBase* SoundRegistry::CreateEffect(const std::string& name) const
{
	AirwinRegistryEntry* regEntry = _effectsByName->at(name);

	// MEMORY! (these are user managed!)
	AudioEffectX* pluginInstance = regEntry->CreateEffect(_samplingRate);

	// -> AirwindowsEffect (wrapper)
	AirwindowsEffect* effect = new AirwindowsEffect(pluginInstance, name, regEntry->GetCategory(), regEntry->GetWhatText());

	// -> Signal Chain
	SignalSettings* parameters = new SignalSettings(name,regEntry->GetCategory(), regEntry->GetWhatText(), true);

	for (int paramIndex = 0; paramIndex < effect->GetParameterCount(); paramIndex++)
	{
		SignalParameter parameter(
			effect->GetParameterName(paramIndex),
			effect->GetParameterValue(paramIndex),
			effect->GetParameterMin(paramIndex),
			effect->GetParameterMax(paramIndex));

		parameters->AddParameter(parameter);
	}

	return effect;
}

#endif