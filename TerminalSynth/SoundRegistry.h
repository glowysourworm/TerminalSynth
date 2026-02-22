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

/// <summary>
/// Registry for AirwinEffect* instances, SignalSettings* for the signal chain, and also sound bank
/// data for the UI. (This was separated from the configuration because of a circular dependency with SignalBase*)
/// </summary>
class SoundRegistry
{
public:

	SoundRegistry(const std::vector<std::string>& soundBanks, const std::map<std::string, std::vector<std::string>>& soundBankMap);
	~SoundRegistry();

	bool Initialize(float samplingRate);

	int GetCount() const;
	std::string GetName(int index) const;
	SignalSettings GetSettings(const std::string& name) const;
	SignalBase* GetEffect(const std::string& name) const;

	std::vector<std::string> GetSoundBanks() const;
	std::vector<std::string> GetSoundNames(const std::string& soundBank) const;

private:

	// Loaded from airwindows-plugins.lib 
	AirwinRegistry* _airwinEffectRegistry;

	// Created from Airwin / Local effects 
	std::map<std::string, SignalBase*>* _effectsByName;

	// SignalBase* -> Signal Chain Settings (ALL EFFECTS)
	std::map<SignalBase*, SignalSettings*>* _signalChainAll;

	// Sound Banks / Sound Bank Entries
	std::vector<std::string>* _soundBanks;
	std::map<std::string, std::vector<std::string>*>* _soundBankMap;
};

SoundRegistry::SoundRegistry(const std::vector<std::string>& soundBanks, const std::map<std::string, std::vector<std::string>>& soundBankMap)
{
	_airwinEffectRegistry = new AirwinRegistry();
	_effectsByName = new std::map<std::string, SignalBase*>();
	_signalChainAll = new std::map<SignalBase*, SignalSettings*>();
	_soundBanks = new std::vector<std::string>(soundBanks);
	_soundBankMap = new std::map<std::string, std::vector<std::string>*>();

	for (auto iter = soundBankMap.begin(); iter != soundBankMap.end(); ++iter)
	{
		// (MEMORY!) ~SoundRegistry
		std::vector<std::string>* soundBankList = new std::vector<std::string>(iter->second);

		_soundBankMap->insert(std::make_pair(iter->first, soundBankList));
	}
}

SoundRegistry::~SoundRegistry()
{
	for (auto iter = _effectsByName->begin(); iter != _effectsByName->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _signalChainAll->begin(); iter != _signalChainAll->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _soundBankMap->begin(); iter != _soundBankMap->end(); ++iter)
	{
		delete iter->second;
	}

	delete _airwinEffectRegistry;
	delete _effectsByName;
	delete _signalChainAll;
}

bool SoundRegistry::Initialize(float samplingRate)
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

int SoundRegistry::GetCount() const
{
	return _signalChainAll->size();
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

SignalSettings SoundRegistry::GetSettings(const std::string& name) const
{
	SignalBase* effect = _effectsByName->at(name);

	return *_signalChainAll->at(effect);
}

SignalBase* SoundRegistry::GetEffect(const std::string& name) const
{
	return _effectsByName->at(name);
}

std::vector<std::string> SoundRegistry::GetSoundBanks() const
{
	return *_soundBanks;
}

std::vector<std::string> SoundRegistry::GetSoundNames(const std::string& soundBank) const
{
	return *_soundBankMap->at(soundBank);
}

#endif