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

	bool Update(const SignalParameter& parameter, bool overwrite)
	{
		if (parameter.GetName() != *_name && !overwrite)
			throw new std::exception("Trying to update SignalParameter* with mismatching names");

		bool isDirty = _value != parameter.GetValue();

		// Complete Overwrite
		if (overwrite && *_name != parameter.GetName())
		{
			_name->clear();
			_name->append(parameter.GetName());

			isDirty = true;
		}		

		_value = parameter.GetValue();

		return isDirty;
	}

private:

	std::string* _name;
	float _value;
	float _min;
	float _max;
};

#endif