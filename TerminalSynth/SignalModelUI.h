#pragma once

#ifndef SIGNAL_MODEL_UI_H
#define SIGNAL_MODEL_UI_H

#include <string>

class SignalModelUI
{
public:

	SignalModelUI(const std::string& name, bool enabled) 
	{
		_name = new std::string(name);
		_enabled = enabled;
	}
	SignalModelUI(const SignalModelUI& model)
	{
		_name = new std::string(model.GetName());
		_enabled = model.GetEnabled();
	}
	~SignalModelUI()
	{
		delete _name;
	}

	std::string GetName() const { return *_name; }
	bool GetEnabled() const { return _enabled; }

	void SetEnabled(bool value) { _enabled = value; }

	void Update(const SignalModelUI& model)
	{
		_name->clear();
		_name->append(model.GetName());
		_enabled = model.GetEnabled();
	}

private:

	std::string* _name;
	bool _enabled;
};

#endif