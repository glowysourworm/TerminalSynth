#pragma once

#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

#include "AirwindowsEffect.h"
#include "SignalBase.h"
#include "SignalParameter.h"
#include "SignalSettings.h"
#include <AirwinRegistry.h>
#include <airwin_consolidated_base.h>
#include <exception>
#include <map>
#include <string>
#include <utility>
#include <vector>

class EffectRegistry
{
public:

	EffectRegistry();
	~EffectRegistry();

	bool Initialize(float samplingRate);

	int GetCount() const;
	std::string GetName(int index) const;
	SignalSettings GetSettings(const std::string& name) const;
	SignalBase* GetEffect(const std::string& name) const;

private:

	// Loaded from airwindows-plugins.lib 
	AirwinRegistry* _airwinEffectRegistry;

	// Created from Airwin / Local effects 
	std::map<std::string, SignalBase*>* _effectsByName;

	// SignalBase* -> Signal Chain Settings (ALL EFFECTS)
	std::map<SignalBase*, SignalSettings*>* _signalChainAll;
};

EffectRegistry::EffectRegistry()
{
	_airwinEffectRegistry = new AirwinRegistry();
	_effectsByName = new std::map<std::string, SignalBase*>();
	_signalChainAll = new std::map<SignalBase*, SignalSettings*>();
}

EffectRegistry::~EffectRegistry()
{
	for (auto iter = _effectsByName->begin(); iter != _effectsByName->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _signalChainAll->begin(); iter != _signalChainAll->end(); ++iter)
	{
		delete iter->second;
	}

	delete _airwinEffectRegistry;
	delete _effectsByName;
	delete _signalChainAll;
}

bool EffectRegistry::Initialize(float samplingRate)
{
	// LOAD AIRWIN PLUGINS! (This may take a couple seconds)
	bool success = _airwinEffectRegistry->Load(samplingRate);

	std::vector<std::string> pluginList;

	// Get Plugin Name List
	_airwinEffectRegistry->GetPlugins(pluginList);

	for (int index = 0; index < pluginList.size(); index++)
	{
		// Airwin Plugin
		AudioEffectX* plugin = _airwinEffectRegistry->GetPlugin(pluginList[index])->GetEffect();

		// -> AirwindowsEffect (wrapper)
		AirwindowsEffect* effect = new AirwindowsEffect(plugin, pluginList[index]);

		// -> Signal Chain
		SignalSettings* parameters = new SignalSettings(pluginList.at(index), true);

		for (int paramIndex = 0; paramIndex < effect->GetParameterCount(); paramIndex++)
		{
			SignalParameter parameter(
				effect->GetParameterName(paramIndex),
				effect->GetParameterValue(paramIndex),
				effect->GetParameterMin(paramIndex),
				effect->GetParameterMax(paramIndex));

			parameters->AddParameter(parameter);
		}

		// Store to registry maps
		_effectsByName->insert(std::make_pair(pluginList.at(index), effect));
		_signalChainAll->insert(std::make_pair(effect, parameters));
	}

	return true;
}

int EffectRegistry::GetCount() const
{
	return _signalChainAll->size();
}

std::string EffectRegistry::GetName(int index) const
{
	int counter = 0;

	for (auto iter = _effectsByName->begin(); iter != _effectsByName->end(); ++iter)
	{
		if (counter++ == index)
			return iter->first;
	}

	throw new std::exception("Effect not found by index:  EffectRegistry.h");
}

SignalSettings EffectRegistry::GetSettings(const std::string& name) const
{
	SignalBase* effect = _effectsByName->at(name);

	return *_signalChainAll->at(effect);
}

SignalBase* EffectRegistry::GetEffect(const std::string& name) const
{
	return _effectsByName->at(name);
}

#endif