#pragma once

#ifndef SIGNAL_PARAMETER_H
#define SIGNAL_PARAMETER_H

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
	}
	SignalParameter(const SignalParameter& copy)
	{
		_name = new std::string(copy.GetName());
		_value = copy.GetValue();
		_min = copy.GetMin();
		_max = copy.GetMax();
	}
	~SignalParameter()
	{
		delete _name;
	}

	std::string GetName() const { return *_name; }
	float GetValue() const { return _value; }
	float GetMin() const { return _min; }
	float GetMax() const { return _max; }

	void SetValue(float newValue) { _value = newValue; }

	void Update(const SignalParameter& parameter)
	{
		if (parameter.GetName() != *_name)
			throw new std::exception("Trying to update SignalParameter* with mismatching names");

		_value = parameter.GetValue();
	}

private:

	std::string* _name;
	float _value;
	float _min;
	float _max;
};

#endif