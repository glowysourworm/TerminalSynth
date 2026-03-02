#pragma once

#ifndef SIGNAL_SETTINGS_H
#define SIGNAL_SETTINGS_H

#include "SignalParameter.h"
#include <exception>
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
	}
	SignalSettings(const std::string& name, const std::string& category, const std::string& infoText, bool isAirwinEffect)
	{
		_parameters = new std::vector<SignalParameter*>();
		_name = new std::string(name);
		_category = new std::string(category);
		_infoText = new std::string(infoText);
		_isAirwinEffect = isAirwinEffect;
	}
	SignalSettings(const SignalSettings& copy)
	{
		_parameters = new std::vector<SignalParameter*>();

		for (int index = 0; index < copy.GetParameterCount(); index++)
		{
			_parameters->push_back(new SignalParameter(copy.GetParameter(index)));
		}

		_name = new std::string(copy.GetName());
		_category = new std::string(copy.GetCategory());
		_infoText = new std::string(copy.GetInfoText());
		_isAirwinEffect = copy.GetIsAirwinEffect();
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
	SignalParameter GetParameter(int index) const { return *_parameters->at(index); }
	float GetParameterValue(int index) const { return _parameters->at(index)->GetValue(); }
	float GetParameterMin(int index) const { return _parameters->at(index)->GetMin(); }
	float GetParameterMax(int index) const { return _parameters->at(index)->GetMax(); }
	std::string GetParameterName(int index) const { return _parameters->at(index)->GetName(); }
	int GetParameterCount() const { return _parameters->size(); }

	void AddParameter(const SignalParameter& parameter)
	{
		_parameters->push_back(new SignalParameter(parameter));
	}
	void SetParameter(int index, float value)
	{
		_parameters->at(index)->SetValue(value);
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

	bool Update(const SignalSettings& parameters)
	{
		if (parameters.GetName() != *_name)
			throw new std::exception("Trying to update SignalParameters* with mismatching names");

		if (parameters.GetParameterCount() != _parameters->size())
			throw new std::exception("Trying to update SignalParameters* with mismatching parameter sets");

		bool isDirty = false;

		for (int index = 0; index < parameters.GetParameterCount(); index++)
		{
			isDirty |= _parameters->at(index)->Update(parameters.GetParameter(index));
		}

		return isDirty;
	}

private:

	std::string* _name;
	std::string* _category;
	std::string* _infoText;
	bool _isAirwinEffect;
	std::vector<SignalParameter*>* _parameters;
};

#endif