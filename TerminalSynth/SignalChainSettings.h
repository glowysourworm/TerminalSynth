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
		_chain = new std::vector<SignalSettings*>();
	}
	SignalChainSettings(const SignalChainSettings& copy)
	{
		_chain = new std::vector<SignalSettings*>();
		
		for (int index = 0; index < copy.GetCount(); index++)
		{
			_chain->push_back(new SignalSettings(copy.Get(index)));
		}
	}
	~SignalChainSettings()
	{
		this->Clear();

		delete _chain;
	}

	void Initialize(const std::vector<SignalSettings>& signalChain)
	{
		for (int index = 0; index < signalChain.size(); index++)
		{
			_chain->push_back(new SignalSettings(signalChain.at(index)));
		}
	}

	int GetCount() const { return _chain->size(); }

	SignalSettings Get(int index) const
	{
		return *_chain->at(index);
	}
	void GetList(std::vector<std::string>& destination) const
	{
		for (int index = 0; index < _chain->size(); index++)
		{
			destination.push_back(_chain->at(index)->GetName());
		}
	}
	
	void Add(const SignalSettings& settings)
	{
		// MEMORY! Clear(), ~SignalChainSettings
		_chain->push_back(new SignalSettings(settings));
	}
	bool Contains(const std::string& name) const
	{
		for (int index = 0; index < _chain->size(); index++)
		{
			if (_chain->at(index)->GetName() == name)
				return true;
		}

		return false;
	}
	bool Update(const SignalChainSettings& settings, bool overwrite)
	{
		if (settings.GetCount() != _chain->size() && !overwrite)
			throw new std::exception("Trying to update SignalChainSettings* with mismatching parameter sets");

		// Must rebuild the chain
		if (settings.GetCount() != _chain->size())
		{
			// MEMORY!
			this->Clear();

			// Rebuild
			for (int index = 0; index < settings.GetCount(); index++)
			{
				// MEMORY! Clear(), ~SignalChainSettings
				_chain->push_back(new SignalSettings(settings.Get(index)));
			}

			// Go ahead and return (isDirty) since we're done updating
			return true;
		}

		bool isDirty = false;

		for (int index = 0; index < _chain->size(); index++)
		{
			isDirty |= _chain->at(index)->Update(settings.Get(index));
		}

		return isDirty;
	}
	void Clear()
	{
		//MEMORY! ~SignalSettings
		for (int index = 0; index < _chain->size(); index++)
		{
			delete _chain->at(index);
		}

		_chain->clear();
	}

private:

	// Signal chain effects settings in post processing, synth note signal chain, or as part of the registry
	std::vector<SignalSettings*>* _chain;
};

#endif