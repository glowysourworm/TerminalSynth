#pragma once

#ifndef SIGNAL_MODEL_UI_H
#define SIGNAL_MODEL_UI_H

#include "ModelUI.h"
#include <string>

class SignalNodeModelUI : public ModelUI
{
public:

	SignalNodeModelUI()
	{
		_name = new std::string("");
		_enabled = false;
		_canEnable = false;
		_canReorder = false;
		_canRemove = false;
	}
	SignalNodeModelUI(const std::string& name, bool enabled, bool canEnable, bool canReorder, bool canRemove)
	{
		_name = new std::string(name);
		_enabled = enabled;
		_canEnable = canEnable;
		_canReorder = canReorder;
		_canRemove = canRemove;
	}
	SignalNodeModelUI(const SignalNodeModelUI& model)
	{
		_name = new std::string(model.GetName());
		_enabled = model.GetEnabled();
		_canEnable = model.GetCanEnable();
		_canReorder = model.GetCanReorder();
		_canRemove = model.GetCanRemove();
	}
	~SignalNodeModelUI()
	{
		delete _name;
	}

	std::string GetName() const override { return *_name; }
	bool GetEnabled() const { return _enabled; }
	bool GetCanEnable() const { return _canEnable; }
	bool GetCanReorder() const { return _canReorder; }
	bool GetCanRemove() const { return _canRemove; }

	void SetEnabled(bool value) { _enabled = value; }
	void SetCanEnable(bool value) { _canEnable = value; }
	void SetCanReorder(bool value) { _canReorder = value; }
	void SetCanRemove(bool value) { _canRemove = value; }

	void Update(const SignalNodeModelUI& model)
	{
		_name->clear();
		_name->append(model.GetName());
		_enabled = model.GetEnabled();
		_canEnable = model.GetCanEnable();
		_canReorder = model.GetCanReorder();
		_canRemove = model.GetCanRemove();
	}

	bool operator==(const SignalNodeModelUI& model) const { return IsEqual(model); }
	bool operator!=(const SignalNodeModelUI& model) const { return !IsEqual(model); }

private:

	bool IsEqual(const SignalNodeModelUI& model) const
	{
		return model.GetName() == *_name &&
			model.GetEnabled() == _enabled &&
			model.GetCanEnable() == _canEnable &&
			model.GetCanReorder() == _canReorder &&
			model.GetCanRemove() == _canRemove;
	}

private:

	std::string* _name;
	bool _enabled;
	bool _canEnable;
	bool _canReorder;
	bool _canRemove;
};

#endif