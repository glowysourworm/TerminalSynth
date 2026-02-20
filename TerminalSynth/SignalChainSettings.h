#pragma once

#ifndef SIGNAL_CHAIN_SETTINGS_H
#define SIGNAL_CHAIN_SETTINGS_H

#include "SignalSettings.h"
#include <exception>
#include <string>
#include <vector>

class SignalChainSettings
{
public:

	SignalChainSettings()
	{
		_completeList = new std::vector<SignalSettings*>();
		_signalChain = new std::vector<SignalSettings*>();
	}
	SignalChainSettings(const SignalChainSettings& copy)
	{
		_completeList = new std::vector<SignalSettings*>();
		_signalChain = new std::vector<SignalSettings*>();

		for (int index = 0; index < copy.GetRegistryCount(); index++)
		{
			_completeList->push_back(new SignalSettings(copy.GetFromRegistry(index)));
		}
		for (int index = 0; index < copy.GetCount(); index++)
		{
			_signalChain->push_back(new SignalSettings(copy.Get(index)));
		}
	}
	~SignalChainSettings()
	{
		for (int index = 0; index < _signalChain->size(); index++)
		{
			delete _signalChain->at(index);
		}
		for (int index = 0; index < _completeList->size(); index++)
		{
			delete _completeList->at(index);
		}

		delete _signalChain;
		delete _completeList;
	}

	void Initialize(const std::vector<SignalSettings>& signalChainRegistry)
	{
		for (int index = 0; index < signalChainRegistry.size(); index++)
		{
			_completeList->push_back(new SignalSettings(signalChainRegistry.at(index)));
		}
	}

	int GetCount() const { return _signalChain->size(); }
	int GetRegistryCount() const { return _completeList->size(); }

	/// <summary>
	/// Add entry to signal chain, not the registry
	/// </summary>
	void SignalAdd(const SignalSettings& settings)
	{
		_signalChain->push_back(new SignalSettings(settings));
	}
	bool SignalContains(const std::string& name) const
	{
		for (int index = 0; index < _signalChain->size(); index++)
		{
			if (_signalChain->at(index)->GetName() == name)
				return true;
		}

		return false;
	}
	SignalSettings Get(int index) const
	{
		return *_signalChain->at(index);
	}
	SignalSettings GetFromRegistry(int index) const
	{
		return *_completeList->at(index);
	}
	SignalSettings GetFromRegistry(const std::string& name) const
	{
		for (int index = 0; index < _completeList->size(); index++)
		{
			if (_completeList->at(index)->GetName() == name)
				return *_completeList->at(index);
		}

		throw new std::exception("Unable to find name of effect in registry:  SignalChainSettings.h");
	}
	void GetRegistryList(std::vector<std::string>& destination) const
	{
		for (int index = 0; index < _completeList->size(); index++)
		{
			destination.push_back(_completeList->at(index)->GetName());
		}
	}
	void SignalRemove(int index)
	{
		_signalChain->erase(_signalChain->begin() + index);
	}
	void SignalUpdate(int index, const SignalSettings& settings)
	{
		if (settings.GetParameterCount() != _signalChain->size())
			throw new std::exception("Trying to update SignalParameters* with mismatching parameter sets");

		_signalChain->at(index)->Update(settings);
	}
	void SignalClear()
	{
		for (int index = 0; index < _signalChain->size(); index++)
		{
			delete _signalChain->at(index);
		}

		_signalChain->clear();
	}

private:

	// Effect Registry:  There was a circular reference coupling SignalBase* to SynthSettings*. So, this list is just 
	//					 loose strings for having a lookup. This is the complete list of SignalSettings*.
	std::vector<SignalSettings*>* _completeList;

	// Current included SignalBase* effects in post processing
	std::vector<SignalSettings*>* _signalChain;
};

#endif