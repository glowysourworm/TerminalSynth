#pragma once

#ifndef SIGNAL_PARAMETER_H
#define SIGNAL_PARAMETER_H

#include "Constant.h"
#include <exception>
#include <string>

class SignalParameter
{
public:

	SignalParameter(const std::string& name, float initialValue, float min, float max)
	{
		_name = new std::string(name);
		_value = initialValue;
		_min = min;
		_max = max;

		_automationEnabled = false;
		_automationType = ParameterAutomationType::EnvelopeSweep;
		_automationOscillator = ParameterAutomationOscillator::Sine;
		_automationOscillatorFrequency = 1.0f;
		_automationLow = 0.0f;
		_automationHigh = 1.0f;
	}
	SignalParameter(const SignalParameter& copy)
	{
		_name = new std::string(copy.GetName());
		_value = copy.GetValue();
		_min = copy.GetMin();
		_max = copy.GetMax();

		_automationEnabled = copy.GetAutomationEnabled();
		_automationType = copy.GetAutomationType();
		_automationOscillator = copy.GetAutomationOscillator();
		_automationOscillatorFrequency = copy.GetAutomationFrequency();
		_automationLow = copy.GetAutomationLow();
		_automationHigh = copy.GetAutomationHigh();
	}
	~SignalParameter()
	{
		delete _name;
	}

	std::string GetName() const { return *_name; }
	float GetValue() const { return _value; }
	float GetMin() const { return _min; }
	float GetMax() const { return _max; }
	bool GetAutomationEnabled() const { return _automationEnabled; }
	ParameterAutomationType GetAutomationType() const { return _automationType; }
	ParameterAutomationOscillator GetAutomationOscillator() const { return _automationOscillator; }
	float GetAutomationFrequency() const { return _automationOscillatorFrequency; }
	float GetAutomationLow() const { return _automationLow; }
	float GetAutomationHigh() const { return _automationHigh; }

	void SetValue(float newValue) { _value = newValue; }
	void SetAutomationEnabled(bool value) { _automationEnabled = value; }
	void SetAutomationType(ParameterAutomationType value) { _automationType = value; }
	void SetAutomationOscillator(ParameterAutomationOscillator value) { _automationOscillator = value; }
	void SetAutomationLow(float value) { _automationLow = value; }
	void SetAutomationHigh(float value) { _automationHigh = value; }
	void SetAutomationFrequency(float value) { _automationOscillatorFrequency = value; }

	bool Update(const SignalParameter* parameter, bool overwrite)
	{
		if (parameter->GetName() != *_name && !overwrite)
			throw new std::exception("Trying to update SignalParameter* with mismatching names");

		bool isDirty = _value != parameter->GetValue();

		isDirty |= _automationEnabled != parameter->GetAutomationEnabled();
		isDirty |= _automationType != parameter->GetAutomationType();
		isDirty |= _automationOscillator != parameter->GetAutomationOscillator();
		isDirty |= _automationOscillatorFrequency != parameter->GetAutomationFrequency();
		isDirty |= _automationLow != parameter->GetAutomationLow();
		isDirty |= _automationHigh != parameter->GetAutomationHigh();

		_automationEnabled = parameter->GetAutomationEnabled();
		_automationType = parameter->GetAutomationType();
		_automationOscillator = parameter->GetAutomationOscillator();
		_automationOscillatorFrequency = parameter->GetAutomationFrequency();
		_automationLow = parameter->GetAutomationLow();
		_automationHigh = parameter->GetAutomationHigh();

		// Complete Overwrite
		if (overwrite && *_name != parameter->GetName())
		{
			_name->clear();
			_name->append(parameter->GetName());

			isDirty = true;
		}		

		_value = parameter->GetValue();

		return isDirty;
	}

private:

	std::string* _name;
	float _value;
	float _min;
	float _max;

	bool _automationEnabled;
	ParameterAutomationType _automationType;
	ParameterAutomationOscillator _automationOscillator;
	float _automationOscillatorFrequency;
	float _automationLow;
	float _automationHigh;
};

#endif