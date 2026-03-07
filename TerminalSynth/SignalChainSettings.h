#pragma once

#ifndef SIGNAL_CHAIN_SETTINGS_H
#define SIGNAL_CHAIN_SETTINGS_H

#include "SignalSettings.h"
#include <exception>
#include <istream>
#include <iterator>
#include <ostream>
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
			_chain->push_back(new SignalSettings(*copy.Get(index)));
		}
	}
	~SignalChainSettings()
	{
		//MEMORY! ~SignalSettings
		for (int index = 0; index < _chain->size(); index++)
		{
			delete _chain->at(index);
		}

		_chain->clear();

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

	SignalSettings* Get(int index) const
	{
		return _chain->at(index);
	}
	SignalSettings* Get(const std::string& name) const
	{
		for (int index = 0; index < _chain->size(); index++)
		{
			if (_chain->at(index)->GetName() == name)
				return _chain->at(index);
		}

		throw new std::exception("Signal chain settings not found:  SignalChainSettings.h");
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
	void RemoveAt(int index)
	{
		if (index >= _chain->size())
			throw new std::exception("Index outside the bounds of an array:  SignalChainSettings.h");

		auto iter = _chain->begin();
		std::advance(iter, index);

		// MEMORY! ~SignalSettings
		delete* iter;

		_chain->erase(iter);
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
	bool Update(const SignalChainSettings* settings, bool overwrite)
	{
		if (settings->GetCount() != _chain->size() && !overwrite)
			throw new std::exception("Trying to update SignalChainSettings* with mismatching parameter sets");

		bool isDirty = false;

		// Update
		for (int index = 0; index < settings->GetCount(); index++)
		{
			// Update (larger in size)
			if (index >= _chain->size())
			{
				// MEMORY! (local)
				_chain->push_back(new SignalSettings(*settings->Get(index)));
				isDirty = true;
			}

			// Update (entries not aligned or matched)
			else if (_chain->at(index)->GetName() != settings->Get(index)->GetName())
			{
				if (!overwrite)
					throw new std::exception("Trying to overwrite protected parameter:  SignalChainSettings.h");

				isDirty |= _chain->at(index)->Update(settings->Get(index), overwrite);
			}

			// Update
			else
				isDirty |= _chain->at(index)->Update(settings->Get(index), false);
		}

		// Remove (excess elements)
		for (int index = _chain->size() - 1; index >= settings->GetCount(); index--)
		{
			delete _chain->at(index);		// MEMORY! ~SignalSettings

			_chain->pop_back();

			isDirty = true;
		}

		return isDirty;
	}

public:

	void Save(std::ostream& stream)
	{
		// Length
		stream << _chain->size();

		for (int index = 0; index < _chain->size(); index++)
		{
			// SignalSettings -> Save
			_chain->at(index)->Save(stream);
		}
	}
	void Read(std::istream& stream)
	{
		for (int index = 0; index < _chain->size(); index++)
		{
			delete _chain->at(index);
		}

		_chain->clear();

		size_t length = 0;

		// Length
		stream >> length;

		for (int index = 0; index < _chain->size(); index++)
		{
			// Signal Settings -> Read
			SignalSettings settings;
			settings.Read(stream);
			
			_chain->push_back(new SignalSettings(settings));
		}
	}

	bool IsEqual(const SignalChainSettings* other)
	{
		if (_chain->size() != other->GetCount())
			return false;

		for (int index = 0; index < _chain->size(); index++)
		{
			if (!_chain->at(index)->IsEqual(other->Get(index)))
				return false;
		}

		return true;
	}

private:

	// Signal chain effects settings in post processing, synth note signal chain, or as part of the registry
	std::vector<SignalSettings*>* _chain;
};

#endif