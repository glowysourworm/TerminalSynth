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

	SignalSettings(const std::string& name, bool isAirwinEffect)
	{
		_parameters = new std::vector<SignalParameter*>();
		_name = new std::string(name);
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
		_isAirwinEffect = copy.GetIsAirwinEffect();
	}
	~SignalSettings()
	{
		for (int index = 0; index < _parameters->size(); index++)
		{
			delete _parameters->at(index);
		}

		delete _parameters;
		delete _name;
	}

	std::string GetName() const { return *_name; }
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

	void Update(const SignalSettings& parameters)
	{
		if (parameters.GetName() != *_name)
			throw new std::exception("Trying to update SignalParameters* with mismatching names");

		if (parameters.GetParameterCount() != _parameters->size())
			throw new std::exception("Trying to update SignalParameters* with mismatching parameter sets");

		for (int index = 0; index < parameters.GetParameterCount(); index++)
		{
			_parameters->at(index)->Update(parameters.GetParameter(index));
		}
	}

private:

	std::string* _name;
	bool _isAirwinEffect;
	std::vector<SignalParameter*>* _parameters;
};

#endif