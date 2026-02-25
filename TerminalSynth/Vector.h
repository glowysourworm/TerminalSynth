#pragma once
#include <exception>

#ifndef VECTOR_H
#define VECTOR_H

template<typename T>
class NumberSpan
{
	virtual int GetDimension(int index) const = 0;
};

template<typename T>
class Vector : public NumberSpan<T>
{
public:
	Vector(int size);
	Vector(T* data, int size);
	~Vector();

	int GetDimension(int index) const override;

	void Add(const Vector<T>& vector);
	void Add(T constant);
	void Add(int index, T constant);
	void Subtract(const Vector<T>& vector);
	void Subtract(T constant);
	void Subtract(int index, T constant);
	T Dot(const Vector<T>& vector);
	void Cross(const Vector<T>& vector);

public:

	T Get(int index) const
	{
		return _data[index];
	}
	void Set(int index, T value)
	{
		_data[index] = value;
	}

private:

	int _size;
	T* _data;

	// Did user allocate the data vector?
	bool _localMemory;
};

template<typename T>
Vector<T>::Vector(int size)
{
	_data = new T[size];
	_size = size;
	_localMemory = true;
}

template<typename T>
Vector<T>::Vector(T* data, int size)
{
	_data = data;
	_size = size;
	_localMemory = false;
}

template<typename T>
Vector<T>::~Vector()
{
	if (_localMemory)
		delete[] _data;
}

template<typename T>
int Vector<T>::GetDimension(int index) const
{
	if (index != 0)
		throw new std::exception("Invalid vector dimension index:  Vector::GetDimension");

	return _size;
}

template<typename T>
void Vector<T>::Add(const Vector<T>& vector)
{
	if (_size != vector.GetDimension(0))
		throw new std::exception("Mismatching vector sizes:  Vector::Add");

	for (int index = 0; index < _size; index++)
		_data[index] += vector[index];
}

template<typename T>
void Vector<T>::Add(T constant)
{
	for (int index = 0; index < _size; index++)
		_data[index] += constant;
}

template<typename T>
void Vector<T>::Add(int index, T constant)
{
	_data[index] += constant;
}

template<typename T>
void Vector<T>::Subtract(const Vector<T>& vector)
{
	if (_size != vector.GetDimension(0))
		throw new std::exception("Mismatching vector sizes:  Vector::Subtract");

	for (int index = 0; index < _size; index++)
		_data[index] -= vector[index];
}

template<typename T>
void Vector<T>::Subtract(T constant)
{
	for (int index = 0; index < _size; index++)
		_data[index] -= constant;
}

template<typename T>
void Vector<T>::Subtract(int index, T constant)
{
	_data[index] -= constant;
}

template<typename T>
T Vector<T>::Dot(const Vector<T>& vector)
{
	if (_size != vector.GetDimension(0))
		throw new std::exception("Mismatching vector sizes:  Vector::Dot");

	T result = 0;

	for (int index = 0; index < _size; index++)
		result += _data[index] * vector[index];
}

template<typename T>
void Vector<T>::Cross(const Vector<T>& vector)
{
	throw new std::exception("Not implemented:  Vector::Cross");
}


#endif