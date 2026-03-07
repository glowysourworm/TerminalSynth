#pragma once

#ifndef SIGNAL_SETTINGS_H
#define SIGNAL_SETTINGS_H

#include "SignalParameter.h"
#include <exception>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

class SignalSettings
{
public:

	SignalSettings()
	{
		_parameters = new std::vector<SignalParameter*>();
		_name = new std::string("");
		_category = new std::string("");
		_infoText = new std::string("");
		_isAirwinEffect = false;
		_isEnabled = false;
	}
	SignalSettings(const std::string& name, const std::string& category, const std::string& infoText, bool isAirwinEffect)
	{
		_parameters = new std::vector<SignalParameter*>();
		_name = new std::string(name);
		_category = new std::string(category);
		_infoText = new std::string(infoText);
		_isAirwinEffect = isAirwinEffect;
		_isEnabled = false;
	}
	SignalSettings(const SignalSettings& copy)
	{
		_parameters = new std::vector<SignalParameter*>();

		for (int index = 0; index < copy.GetParameterCount(); index++)
		{
			_parameters->push_back(new SignalParameter(*copy.GetParameter(index)));
		}

		_name = new std::string(copy.GetName());
		_category = new std::string(copy.GetCategory());
		_infoText = new std::string(copy.GetInfoText());
		_isAirwinEffect = copy.GetIsAirwinEffect();
		_isEnabled = copy.GetIsEnabled();
	}
	~SignalSettings()
	{
		for (int index = 0; index < _parameters->size(); index++)
		{
			delete _parameters->at(index);
		}

		_parameters->clear();

		delete _parameters;
		delete _name;
		delete _category;
		delete _infoText;
	}

	std::string GetName() const { return *_name; }
	std::string GetCategory() const { return *_category; }
	std::string GetInfoText() const { return *_infoText; }
	bool GetIsAirwinEffect() const { return _isAirwinEffect; }
	bool GetIsEnabled() const { return _isEnabled; }
	SignalParameter* GetParameter(int index) const { return _parameters->at(index); }
	float GetParameterValue(int index) const { return _parameters->at(index)->GetValue(); }
	float GetParameterMin(int index) const { return _parameters->at(index)->GetMin(); }
	float GetParameterMax(int index) const { return _parameters->at(index)->GetMax(); }
	std::string GetParameterName(int index) const { return _parameters->at(index)->GetName(); }
	int GetParameterCount() const { return _parameters->size(); }

	void AddParameter(const SignalParameter& parameter)
	{
		_parameters->push_back(new SignalParameter(parameter));
	}
	void UpdateParameter(int index, const SignalParameter* value)
	{
		_parameters->at(index)->Update(value, false);				// Preserve name
	}
	void SetInfoText(const std::string& value)
	{
		_infoText->clear();
		_infoText->append(value);
	}
	void SetCategory(const std::string& value)
	{
		_category->clear();
		_category->append(value);
	}
	void SetName(const std::string& value)
	{
		_name->clear();
		_name->append(value);
	}
	void SetIsEnabled(bool value)
	{
		_isEnabled = value;
	}

	bool Update(const SignalSettings* parameters, bool overwrite)
	{
		if (parameters->GetName() != *_name && !overwrite)
			throw new std::exception("Trying to update SignalParameters* with mismatching names");

		if (parameters->GetParameterCount() != _parameters->size() && !overwrite)
			throw new std::exception("Trying to update SignalParameters* with mismatching parameter sets");

		bool isDirty = false;

		if (parameters->GetName() != *_name && overwrite)
		{
			_name->clear();
			_name->append(parameters->GetName());

			isDirty = true;
		}

		isDirty |= *_category != parameters->GetCategory();
		isDirty |= *_infoText != parameters->GetInfoText();
		isDirty |= _isAirwinEffect != parameters->GetIsAirwinEffect();
		isDirty |= _isEnabled != parameters->GetIsEnabled();

		_category->clear();
		_infoText->clear();

		_category->append(parameters->GetCategory());
		_infoText->append(parameters->GetInfoText());
		
		_isAirwinEffect = parameters->GetIsAirwinEffect();
		_isEnabled = parameters->GetIsEnabled();

		// Update
		for (int index = 0; index < parameters->GetParameterCount(); index++)
		{
			// Update (larger in size)
			if (index >= _parameters->size())
			{
				_parameters->push_back(new SignalParameter(*parameters->GetParameter(index)));			// MEMORY!
				isDirty = true;
			}

			// Update (entries not aligned or matched)
			else if (_parameters->at(index)->GetName() != parameters->GetParameter(index)->GetName())
			{
				if (!overwrite)
					throw new std::exception("Trying to overwrite protected parameter:  SignalSettings.h");

				isDirty |= _parameters->at(index)->Update(parameters->GetParameter(index), overwrite);
			}

			// Update
			else
				isDirty |= _parameters->at(index)->Update(parameters->GetParameter(index), false);
		}

		// Remove (excess elements)
		for (int index = _parameters->size() - 1; index >= parameters->GetParameterCount(); index--)
		{
			delete _parameters->at(index);		// MEMORY! ~SignalParameter

			_parameters->pop_back();

			isDirty = true;
		}

		return isDirty;
	}

public:

	void Save(std::ostream& stream)
	{
		stream << *_name;
		stream << *_category;
		stream << *_infoText;
		stream << _isEnabled;
		stream << _isAirwinEffect;
		stream << _parameters->size();

		for (int index = 0; index < _parameters->size(); index++)
		{
			_parameters->at(index)->Save(stream);
		}
	}
	void Read(std::istream& stream)
	{
		for (int index = 0; index < _parameters->size(); index++)
		{
			delete _parameters->at(index);
		}

		_parameters->clear();

		size_t length = 0;

		stream >> *_name;
		stream >> *_category;
		stream >> *_infoText;
		stream >> _isEnabled;
		stream >> _isAirwinEffect;
		stream >> length;

		for (int index = 0; index < length; index++)
		{
			SignalParameter parameter;
			parameter.Read(stream);

			_parameters->push_back(new SignalParameter(parameter));
		}
	}

	bool IsEqual(const SignalSettings* other)
	{
		if (_parameters->size() != other->GetParameterCount())
			return false;

		for (int index = 0; index < _parameters->size(); index++)
		{
			if (!_parameters->at(index)->IsEqual(other->GetParameter(index)))
				return false;
		}

		return *_name == other->GetName() &&
			*_category == other->GetCategory() &&
			*_infoText == other->GetInfoText() &&
			_isEnabled == other->GetIsEnabled() &&
			_isAirwinEffect == other->GetIsAirwinEffect();
	}

private:

	std::string* _name;
	std::string* _category;
	std::string* _infoText;
	bool _isEnabled;
	bool _isAirwinEffect;
	std::vector<SignalParameter*>* _parameters;
};

#endif