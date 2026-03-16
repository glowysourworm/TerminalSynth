#pragma once

#ifndef SIGNAL_PARAMETER_H
#define SIGNAL_PARAMETER_H

#include "Constant.h"
#include "Envelope.h"
#include <exception>
#include <istream>
#include <ostream>
#include <string>

class SignalParameter
{
public:

	SignalParameter() : SignalParameter("No Name", 0,0,0)
	{}
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

		_automationEnvelope = new Envelope();
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

		_automationEnvelope = new Envelope(*copy.GetAutomationEnvelope());
	}
	~SignalParameter()
	{
		delete _name;
		delete _automationEnvelope;
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
	Envelope* GetAutomationEnvelope() const { return _automationEnvelope; }

	void SetValue(float newValue) { _value = newValue; }
	void SetAutomationEnabled(bool value) { _automationEnabled = value; }
	void SetAutomationType(ParameterAutomationType value) { _automationType = value; }
	void SetAutomationOscillator(ParameterAutomationOscillator value) { _automationOscillator = value; }
	void SetAutomationLow(float value) { _automationLow = value; }
	void SetAutomationHigh(float value) { _automationHigh = value; }
	void SetAutomationFrequency(float value) { _automationOscillatorFrequency = value; }
	void SetAutomationEnvelope(const Envelope& envelope) { _automationEnvelope->Update(&envelope); }

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
		isDirty |= !_automationEnvelope->IsEqual(parameter->GetAutomationEnvelope());

		_automationEnabled = parameter->GetAutomationEnabled();
		_automationType = parameter->GetAutomationType();
		_automationOscillator = parameter->GetAutomationOscillator();
		_automationOscillatorFrequency = parameter->GetAutomationFrequency();
		_automationLow = parameter->GetAutomationLow();
		_automationHigh = parameter->GetAutomationHigh();

		_automationEnvelope->Update(parameter->GetAutomationEnvelope());

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

public:

	void Save(std::ostream& stream)
	{
		stream << *_name;
		stream << _value;
		stream << _min;
		stream << _max;
		stream << _automationEnabled;
		stream << (int)_automationType;
		stream << (int)_automationOscillator;
		stream << _automationOscillatorFrequency;
		stream << _automationLow;
		stream << _automationHigh;
		
		_automationEnvelope->Save(stream);
	}
	void Read(std::istream& stream)
	{
		int automationType, automationOscillator;

		stream >> *_name;
		stream >> _value;
		stream >> _min;
		stream >> _max;
		stream >> _automationEnabled;
		stream >> automationType;
		stream >> automationOscillator;
		stream >> _automationOscillatorFrequency;
		stream >> _automationLow;
		stream >> _automationHigh;

		_automationType = (ParameterAutomationType)automationType;
		_automationOscillator = (ParameterAutomationOscillator)automationOscillator;

		_automationEnvelope->Read(stream);
	}

	bool IsEqual(const SignalParameter* other)
	{
		return *_name == other->GetName() &&
			_value == other->GetValue() &&
			_min == other->GetMin() &&
			_max == other->GetMax() &&
			_automationEnabled == other->GetAutomationEnabled() &&
			_automationType == other->GetAutomationType() &&
			_automationOscillator == other->GetAutomationOscillator() &&
			_automationOscillatorFrequency == other->GetAutomationFrequency() &&
			_automationLow == other->GetAutomationLow() &&
			_automationHigh == other->GetAutomationHigh() &&
			_automationEnvelope->IsEqual(other->GetAutomationEnvelope());
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

	Envelope* _automationEnvelope;
};

#endif