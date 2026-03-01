#pragma once

#ifndef CHECKBOX_MODEL_UI_H
#define CHECKBOX_MODEL_UI_H

#include "ModelUI.h"
#include <string>

class CheckboxModelUI : public ModelUI
{
public:

	CheckboxModelUI()
	{
		_name = new std::string("");
		_isChecked = false;
		_order = 0;
	}
	CheckboxModelUI(const std::string& name, bool isChecked, int order)
	{
		_name = new std::string(name);
		_isChecked = isChecked;
		_order = order;
	}
	CheckboxModelUI(const CheckboxModelUI& copy)
	{
		_name = new std::string(copy.GetName());
		_isChecked = copy.GetIsChecked();
		_order = copy.GetOrder();
	}
	~CheckboxModelUI()
	{
		delete _name;
	}

	std::string GetName() const override { return *_name; }
	int GetOrder() const override { return _order; }
	bool GetIsChecked() const { return _isChecked; }

	void SetIsChecked(bool value) { _isChecked = value; }
	void SetName(const std::string& name)
	{
		_name->clear();
		_name->append(name);
	}
	void SetOrder(int value) { _order = value; }

	bool operator==(const CheckboxModelUI& model) const { return IsEqual(model); }
	bool operator!=(const CheckboxModelUI& model) const { return !IsEqual(model); }

private:

	bool IsEqual(const CheckboxModelUI& model) const
	{
		return model.GetName() == *_name &&
			   model.GetIsChecked() == _isChecked &&
			   model.GetOrder() == _order;
	}

private:

	bool _isChecked;
	std::string* _name;
	int _order;
};

#endif