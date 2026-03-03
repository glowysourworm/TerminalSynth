#pragma once

#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

#include "AirwindowsEffect.h"
#include "OutputSettings.h"
#include "SignalBase.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include <AirwinRegistry.h>
#include <AirwinRegistryEntry.h>
#include <airwin_consolidated_base.h>
#include <exception>
#include <iterator>
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

	/// <summary>
	/// Creates an initial collection of effects plugins from the Airwin .lib, and creates a basic
	/// cache of instances. The destination list may be used to initialize the names of the plugins.
	/// </summary>
	bool Initialize(float samplingRate, std::vector<SignalSettings>& destinationList);

	/// <summary>
	/// Gets the count of unique-by-name instances of signal effects. Multiple instances of
	/// each effect may be held; but that does not change the count of the "registry" - which
	/// is just a hash map of the effects by name. Each instances may be "checked out", creating
	/// sub-collections of pointers to memory for those effects. THEY SHOULD NOT BE DELETED! All
	/// memory for the effects is managed by the SoundRegistry*.
	/// </summary>
	int GetCount() const;

	/// <summary>
	/// Gets name of i-th effect in the registry. The registry will hold a collection, that is
	/// created during initialization, of the complete list of sound effects. There will be at
	/// least one copy of each effect; and copies are held in a sub-collection. So, the count
	/// in the registry should remain constant after initialization.
	/// </summary>
	std::string GetName(int index) const;

	/// <summary>
	/// Reserves a copy of the effect, which should NOT BE DELETED BY USER CODE! These will
	/// be managed by the SoundRegistry so we don't allocate memory in real time. Multiple
	/// instances are allowed for the same effect name based on SignalBase* pointer uniqueness.
	/// </summary>
	SignalBase* Checkout(const std::string& name) const;

	/// <summary>
	/// Effect use has been terminated by the user, so the memory may be freed at some 
	/// idle time from the application controllers; or on exit.
	/// </summary>
	void Checkin(SignalBase* effect);

private:

	// Loaded from airwindows-plugins.lib 
	AirwinRegistry* _airwinEffectRegistry;

	// Created from Airwin / Local effects (DO NOT DELETE AIRWIN DATA HERE!)
	std::map<std::string, AirwinRegistryEntry*>* _registryEntries;

	// Instances of our SignalBase* effects
	std::map<std::string, std::vector<SignalBase*>*>* _effectInstances;

	// Instances that are being used
	std::map<std::string, std::vector<SignalBase*>*>* _effectInstancesCheckedOut;

	float _samplingRate;
};

SoundRegistry::SoundRegistry()
{
	_airwinEffectRegistry = new AirwinRegistry();
	_registryEntries = new std::map<std::string, AirwinRegistryEntry*>();
	_effectInstances = new std::map<std::string, std::vector<SignalBase*>*>();
	_effectInstancesCheckedOut = new std::map<std::string, std::vector<SignalBase*>*>();
}

SoundRegistry::~SoundRegistry()
{
	// MEMORY! ~SignalBase* (derived class)
	for (auto iter = _effectInstances->begin(); iter != _effectInstances->end(); ++iter)
	{
		// std::vector<SignalBase*>*
		//
		for (int index = 0; index < iter->second->size(); index++)
			delete iter->second->at(index);

		delete iter->second;
	}
	for (auto iter = _effectInstancesCheckedOut->begin(); iter != _effectInstancesCheckedOut->end(); ++iter)
	{
		// std::vector<SignalBase*>*
		//
		for (int index = 0; index < iter->second->size(); index++)
			delete iter->second->at(index);

		delete iter->second;
	}

	// MEMORY! ~SignalSettings*
	for (auto iter = _effectInstances->begin(); iter != _effectInstances->end(); ++iter)
	{
		delete iter->second;
	}

	delete _airwinEffectRegistry;
	delete _registryEntries;				// AirwinRegistryEntry* instances are handled in the other .lib
	delete _effectInstances;
	delete _effectInstancesCheckedOut;
}

bool SoundRegistry::Initialize(float samplingRate, std::vector<SignalSettings>& destinationList)
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
		_registryEntries->insert(std::make_pair(pluginList.at(index), pluginEntry));
		_effectInstances->insert(std::make_pair(pluginList.at(index), new std::vector<SignalBase*>()));					// MEMORY! ~SoundRegistry
		_effectInstancesCheckedOut->insert(std::make_pair(pluginList.at(index), new std::vector<SignalBase*>()));		// MEMORY! ~SoundRegistry

		// Airwindows Instance!
		AudioEffectX* effect = pluginEntry->CreateEffect(samplingRate);

		// MEMORY! ~SoundRegistry
		AirwindowsEffect* wrappedEffect = new AirwindowsEffect(effect, pluginList[index], pluginEntry->GetCategory(), pluginEntry->GetWhatText());

		destinationList.push_back(wrappedEffect->GetSettings());
	}

	return true;
}

int SoundRegistry::GetCount() const
{
	return _registryEntries->size();
}

std::string SoundRegistry::GetName(int index) const
{
	if (index >= _registryEntries->size() ||
		index < 0)
		throw new std::exception("Index outside the bounds of the registry:  SoundRegistry.h");

	auto iter = _registryEntries->begin();
	std::advance(iter, index);
	return iter->first;
}

SignalBase* SoundRegistry::Checkout(const std::string& name) const
{
	if (!_registryEntries->contains(name))
		throw new std::exception("Effect name not found in SoundRegistry*");

	// Available Instances
	if (_effectInstances->at(name)->size() > 0)
	{
		SignalBase* instance = _effectInstances->at(name)->back();

		_effectInstancesCheckedOut->at(name)->push_back(instance);
		_effectInstances->at(name)->pop_back();

		return instance;
	}

	AirwinRegistryEntry* entry = _registryEntries->at(name);

	// Airwindows Instance!
	AudioEffectX* effect = entry->CreateEffect(_samplingRate);

	// MEMORY! ~SoundRegistry
	AirwindowsEffect* wrappedEffect = new AirwindowsEffect(effect, name, entry->GetCategory(), entry->GetWhatText());

	// Checked Out
	_effectInstancesCheckedOut->at(name)->push_back(wrappedEffect);

	return wrappedEffect;
}

void SoundRegistry::Checkin(SignalBase* effect)
{
	if (!_effectInstancesCheckedOut->contains(effect->GetName()))
		throw new std::exception("Effect name not found (Checked Out) in SoundRegistry*");

	// Effect Instances:  This should be a very small list (1, or 2 at the most)
	//
	for (auto iter = _effectInstancesCheckedOut->at(effect->GetName())->begin(); 
			  iter != _effectInstancesCheckedOut->at(effect->GetName())->end(); ++iter)
	{
		// Pointer Equality
		if (*iter == effect)
		{
			// Remove via iterator
			_effectInstancesCheckedOut->at(effect->GetName())->erase(iter);

			// Put back on original list
			_effectInstances->at(effect->GetName())->push_back(effect);
			
			return;
		}
	}
}


#endif