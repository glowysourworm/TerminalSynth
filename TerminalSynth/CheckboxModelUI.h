#pragma once

#ifndef CHECKBOX_MODEL_UI_H
#define CHECKBOX_MODEL_UI_H

#include <string>

class CheckboxModelUI
{
public:

	CheckboxModelUI()
	{
		_name = new std::string("");
		_isChecked = false;
	}
	CheckboxModelUI(const std::string& name, bool isChecked)
	{
		_name = new std::string(name);
		_isChecked = isChecked;
	}
	CheckboxModelUI(const CheckboxModelUI& copy)
	{
		_name = new std::string(copy.GetName());
		_isChecked = copy.GetIsChecked();
	}
	~CheckboxModelUI()
	{
		delete _name;
	}

	std::string GetName() const { return *_name; }
	bool GetIsChecked() const { return _isChecked; }

	void SetIsChecked(bool value)
	{
		_isChecked = value;
	}
	void SetName(const std::string& name)
	{
		_name->clear();
		_name->append(name);
	}

	bool operator==(const CheckboxModelUI& model) const { return IsEqual(model); }
	bool operator!=(const CheckboxModelUI& model) const { return !IsEqual(model); }

private:

	bool IsEqual(const CheckboxModelUI& model) const
	{
		return model.GetName() == *_name &&
			   model.GetIsChecked() == _isChecked;
	}

private:

	bool _isChecked;
	std::string* _name;
};

#endif