#pragma once

#ifndef SIGNAL_MODEL_UI_H
#define SIGNAL_MODEL_UI_H

#include <string>

class SignalNodeModelUI
{
public:

	SignalNodeModelUI(const std::string& name, bool enabled, bool canEnable, bool canReorder)
	{
		_name = new std::string(name);
		_enabled = enabled;
		_canEnable = canEnable;
		_canReorder = canReorder;
	}
	SignalNodeModelUI(const SignalNodeModelUI& model)
	{
		_name = new std::string(model.GetName());
		_enabled = model.GetEnabled();
		_canEnable = model.GetCanEnable();
		_canReorder = model.GetCanReorder();
	}
	~SignalNodeModelUI()
	{
		delete _name;
	}

	std::string GetName() const { return *_name; }
	bool GetEnabled() const { return _enabled; }
	bool GetCanEnable() const { return _canEnable; }
	bool GetCanReorder() const { return _canReorder; }

	void SetEnabled(bool value) { _enabled = value; }
	void SetCanEnable(bool value) { _canEnable = value; }
	void SetCanReorder(bool value) { _canReorder = value; }

	void Update(const SignalNodeModelUI& model)
	{
		_name->clear();
		_name->append(model.GetName());
		_enabled = model.GetEnabled();
		_canEnable = model.GetCanEnable();
		_canReorder = model.GetCanReorder();
	}

	bool operator==(const SignalNodeModelUI& model) const { return IsEqual(model); }
	bool operator!=(const SignalNodeModelUI& model) const { return !IsEqual(model); }

private:

	bool IsEqual(const SignalNodeModelUI& model) const
	{
		return model.GetName() == *_name &&
			   model.GetEnabled() == _enabled;
	}

private:

	std::string* _name;
	bool _enabled;
	bool _canEnable;
	bool _canReorder;
};

#endif