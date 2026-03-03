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
		_order = 0;
	}
	SignalNodeModelUI(const std::string& name, bool enabled, bool canEnable, bool canReorder, bool canRemove, int order)
	{
		_name = new std::string(name);
		_enabled = enabled;
		_canEnable = canEnable;
		_canReorder = canReorder;
		_canRemove = canRemove;
		_order = order;
	}
	SignalNodeModelUI(const SignalNodeModelUI& model)
	{
		_name = new std::string(model.GetName());
		_enabled = model.GetEnabled();
		_canEnable = model.GetCanEnable();
		_canReorder = model.GetCanReorder();
		_canRemove = model.GetCanRemove();
		_order = model.GetOrder();
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
	int GetOrder() const { return _order; }

	void SetEnabled(bool value) { _enabled = value; }
	void SetCanEnable(bool value) { _canEnable = value; }
	void SetCanReorder(bool value) { _canReorder = value; }
	void SetCanRemove(bool value) { _canRemove = value; }
	void SetOrder(int value) { _order = value; }

	void Update(const SignalNodeModelUI* model)
	{
		_name->clear();
		_name->append(model->GetName());
		_enabled = model->GetEnabled();
		_canEnable = model->GetCanEnable();
		_canReorder = model->GetCanReorder();
		_canRemove = model->GetCanRemove();
		_order = model->GetOrder();
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
			model.GetCanRemove() == _canRemove &&
			model.GetOrder() == _order;
	}

private:

	std::string* _name;
	bool _enabled;
	bool _canEnable;
	bool _canReorder;
	bool _canRemove;
	int _order;
};

#endif