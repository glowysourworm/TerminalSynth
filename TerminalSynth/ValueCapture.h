#pragma once

#ifndef VALUE_TRIGGER_H
#define VAlUE_TRIGGER_H

/// <summary>
/// Trigger that provides value indicators for a real time situation. Typical uses are for checking
/// changed status of a variable.
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class ValueCapture
{
public:
	ValueCapture(T initialValue) 
	{ 
		_refValue = new T(initialValue); 
		_value = *_refValue;
	}
	~ValueCapture() 
	{
		delete _refValue;
	};

	/// <summary>
	/// Returns the original pointer
	/// </summary>
	T* GetRef() const { return _refValue; }

	/// <summary>
	/// Returns the value of the capture via dereferencing
	/// </summary>
	T GetValue() const { return *_refValue; }

	/// <summary>
	/// Sets the underlying value
	/// </summary>
	void SetValue(T value)
	{
		*_refValue = value;
	}

	/// <summary>
	/// Check the current value via dereferencing
	/// </summary>
	bool HasChanged()
	{
		return _value != *_refValue;
	}

	/// <summary>
	/// Updates the current value via dereferencing the stored pointer
	/// </summary>
	void Clear()
	{
		_value = *_refValue;
	}

private:

	T _value;
	T* _refValue;
};

#endif