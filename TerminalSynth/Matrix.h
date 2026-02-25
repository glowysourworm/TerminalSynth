#pragma once

#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"
#include <cfloat>
#include <exception>
#include <functional>
#include <iostream>
#include <string>

template<typename T>
class Matrix : public NumberSpan<T>
{
public:

	Matrix(int rows, int columns);
	Matrix(T** data, int rows, int columns);
	Matrix(const Matrix<T>& copy);
	~Matrix();

	void SetFrom(const Matrix<T>& matrix);
	void SetRow(int rowIndex, const T* dataRow);

	int GetDimension(int index) const override;

	void Add(const Matrix<T>& matrix);
	void Add(int row, int column, T value);
	void Subtract(const Matrix<T>& matrix);
	void Mult(const Matrix<T>& matrix);
	void Mult(const Vector<T>& vector, Vector<T>& destination);

	/// <summary>
	/// Uses Gaussian Elimination to invert the (square) matrix. Returns false if
	/// the operation was unsuccessful due to in improper determinant (non-invertability)
	/// </summary>
	bool Invert();

	/// <summary>
	/// Checks that matrix is the identity
	/// </summary>
	bool IsIdentity() const;

public:

	void AddRows(int sourceRowIndex, int destinationRowIndex);
	void MultRow(int rowIndex, T factor);

public:

	T Get(int row, int column) const
	{
		return _data[row][column];
	}

	void Set(int row, int column, T element)
	{
		_data[row][column] = element;
	}

public:

	using MatrixIteratorByValue = std::function<void(int row, int column, T currentValue)>;

	void IterateByValue(MatrixIteratorByValue callback) const;

private:
	
	using MatrixIteratorByReference = std::function<void(int row, int column, T* currentRef)>;

	void IterateByRef(MatrixIteratorByReference callback) const;

public:

	using MatrixIteratorRef = std::function<void(T* currentRef)>;
	using MatrixIteratorPredicate = std::function<bool(int row, int column, T* currentRef)>;
	
	/// <summary>
	/// Iterates matrix providing a set callback with no indices
	/// </summary>
	void IterateSet(MatrixIteratorRef callback);

	/// <summary>
	/// Sets the value into the matrix when the predicate returns true
	/// </summary>
	void IterateSet(T value, MatrixIteratorPredicate predicate);

private:

	void Output() const;

private:

	int _rows;
	int _columns;
	T** _data;

	// Did user allocate the data vector?
	bool _localMemory;
};

template<typename T>
Matrix<T>::Matrix(int rows, int columns)
{
	_data = new T*[rows];
	_rows = rows;
	_columns = columns;
	_localMemory = true;

	for (int index = 0; index < _rows; index++)
	{
		_data[index] = new T[_columns];
	}		
}

template<typename T>
Matrix<T>::Matrix(T** data, int rows, int columns)
{
	_data = data;
	_rows = rows;
	_columns = columns;
	_localMemory = false;
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& copy)
{
	_rows = copy.GetDimension(0);
	_columns = copy.GetDimension(1);
	_data = new T*[_rows];
	_localMemory = true;

	for (int row = 0; row < _rows; row++)
	{
		_data[row] = new T[_columns];

		for (int column = 0; column < _columns; column++)
		{
			_data[row][column] = copy.Get(row, column);
		}
	}

}

template<typename T>
Matrix<T>::~Matrix()
{
	if (_localMemory)
	{
		for (int index = 0; index < _rows; index++)
		{
			delete[] _data[index];
		}

		delete[] _data;
	}		
}

template<typename T>
void Matrix<T>::SetFrom(const Matrix<T>& matrix)
{
	if (_rows != matrix.GetDimension(0) ||
		_columns != matrix.GetDimension(1))
		throw new std::exception("Matrix dimensions do not match:  Matrix::Add");

	Matrix<T>* that = this;

	matrix.IterateByValue([&that](int row, int column, T value) {
		that->Set(row, column, value);
	});
}

template<typename T>
void Matrix<T>::SetRow(int rowIndex, const T* dataRow)
{
	for (int column = 0; column < _columns; column++)
	{
		this->Set(rowIndex, column, dataRow[column]);
	}
}

template<typename T>
int Matrix<T>::GetDimension(int index) const
{
	if (index == 0)
		return _rows;

	else if (index == 1)
		return _columns;

	else
		throw new std::exception("Invalid dimension index:  Matrix.h");
}

template<typename T>
void Matrix<T>::Add(const Matrix<T>& matrix)
{
	if (_rows != matrix.GetDimension(0) ||
		_columns != matrix.GetDimension(1))
		throw new std::exception("Matrix dimensions do not match:  Matrix::Add");

	Matrix<T>* that = this;

	matrix.IterateByValue([&that](int row, int column, T value) {
		that->Set(row, column, that->Get(row, column) + value);
	});
}

template<typename T>
void Matrix<T>::Add(int row, int column, T value)
{
	this->Set(row, column, this->Get(row, column) + value);
}

template<typename T>
void Matrix<T>::Subtract(const Matrix<T>& matrix)
{
	if (_rows != matrix.GetDimension(0) ||
		_columns != matrix.GetDimension(1))
		throw new std::exception("Matrix dimensions do not match:  Matrix::Subtract");

	Matrix<T>* that = this;

	matrix.IterateByValue([](int row, int column, T value) {
		that->Set(row, column, that->Get(row, column) - value);
	});
}

template<typename T>
void Matrix<T>::Mult(const Matrix<T>& matrix)
{
	if (_rows != matrix.GetDimension(1) ||
		_columns != matrix.GetDimension(0))
		throw new std::exception("Matrix dimensions do not allow multiplication:  Matrix::Mult");

	Matrix<T> result(_rows, _columns);

	for (int row = 0; row < _rows; row++)
	{
		for (int column = 0; column < _columns; column++)
		{
			result.Set(row, column, 0);

			for (int rowK = 0; rowK < _rows; rowK++)
			{
				result.Add(row, column, this->Get(row, rowK) * matrix.Get(rowK, column));
			}
		}
	}

	Matrix<T>* that = this;

	// Transfer the result
	result.IterateByValue([&that](int row, int column, T value) {
		that->Set(row, column, value);
	});
}

template<typename T>
void Matrix<T>::Mult(const Vector<T>& vector, Vector<T>& destination)
{
	if (_columns != vector.GetDimension(0) ||
		_columns != destination.GetDimension(0))
		throw new std::exception("Matrix / Vector dimensions do not allow multiplication:  Matrix::Mult");

	for (int row = 0; row < _rows; row++)
	{
		destination.Set(row, 0);

		for (int column = 0; column < _columns; column++)
		{
			destination.Add(row, this->Get(row, column) * vector.Get(column));
		}
	}
}

template<typename T>
bool Matrix<T>::Invert()
{
	if (_rows != _columns ||
		_rows < 2)
		throw new std::exception("Matrix dimensions do not match:  Matrix::Invert");

	// Gaussian Elimination:  https://en.wikipedia.org/wiki/Gaussian_elimination
	//
	// Procedure:
	//
	// 0) Augment the matrix with a square identity matrix
	// 1) Build an upper triangular matrix by using elementary operations
	// 2) Finish the left hand side identity matrix by reducing the upper 
	//	  portion
	// 3) Remove the right hand side as the inverse result
	//

	Matrix<T>* that = this;
	Matrix<T> augmentedMatrix(_rows, _columns * 2);

	// Augmented Matrix: Initialize 
	for (int row = 0; row < _rows; row++)
	{
		for (int column = 0; column < _columns * 2; column++)
		{
			if (column < _columns)
			{
				augmentedMatrix.Set(row, column, this->Get(row, column));
			}
				
			else
			{
				// Right hand side diagonal
				if ((column - _columns) == row)
					augmentedMatrix.Set(row, column, 1);

				else
					augmentedMatrix.Set(row, column, 0);
			}
		}
	}

	// Output: DEBUG
	//augmentedMatrix.Output();

	// Create Upper Triangular Matrix: LHS
	for (int column = 0; column < _columns - 1; column++)
	{
		for (int row = _rows - 1; row > column; row--)
		{
			if (augmentedMatrix.Get(row, column) == 0)
				continue;

			// (A[i, j] * factor) + A[i - 1, j] = 0
			//
			// factor = -1 * A[i - 1, j] / A[i, j]
			//
			T factor = -1 * (augmentedMatrix.Get(row - 1, column) / augmentedMatrix.Get(row, column));

			// Row Multiply, then add to the bottom most row
			augmentedMatrix.MultRow(row, factor);
			augmentedMatrix.AddRows(row - 1, row);
		}
	}

	// Output: DEBUG
	//augmentedMatrix.Output();

	// Complete the Diagonal Matrix: LHS
	for (int column = _columns - 1; column >= 0; column--)
	{
		for (int row = 0; row <= column; row++)
		{
			// Diagonal
			if (row == column)
			{
				// This might indicate a Det = 0 matrix! Checking for this requires
				// a determinant calculation, which can be done using LU-decomposition, 
				// which is essentially what we're computing here!
				if (augmentedMatrix.Get(row, column) == 0)
					return false;

				else
				{
					// A[i, j] * factor = 1
					// 
					T factor = 1 / augmentedMatrix.Get(row, column);

					augmentedMatrix.MultRow(row, factor);
				}				
			}
			else if (augmentedMatrix.Get(row, column) == 0)
				continue;

			else
			{
				// (A[i + 1, j] * factor) + A[i, j] = 0
				//
				// factor = -1 * A[i, j] / A[i + 1, j]
				//
				T factor = -1 * (augmentedMatrix.Get(row, column) / augmentedMatrix.Get(row + 1, column));

				// Row Multiply, then add to the current row
				augmentedMatrix.MultRow(row + 1, factor);
				augmentedMatrix.AddRows(row + 1, row);
			}
		}
	}

	// Output: DEBUG
	//augmentedMatrix.Output();


	// This could be checked using matrix mutliplication:  A * A_inv = I
	//
	// There's a couple of problems:  1) Noise near the double threshold; and 2) 
	// Extra memory allocation during the audio callback.
	//
	// Both of these could use some investigation; but it's going to take more
	// control over our matrix class. This is a good first start.
	//
	// Result!
	for (int row = 0; row < _rows; row++)
	{
		for (int column = _columns; column < _columns * 2; column++)
		{
			this->Set(row, column - _columns, augmentedMatrix.Get(row, column));
		}
	}

	return true;
}

template<typename T>
bool Matrix<T>::IsIdentity() const
{
	for (int row = 0; row < _rows; row++)
	{
		for (int column = 0; column < _columns; column++)
		{
			// Off Diagonal (NOISE ALSO!)
			if (row != column &&
				this->Get(row, column) > DBL_EPSILON)
				return false;

			// Diagonal
			else if (row == column &&
					 this->Get(row, column) - 1 > DBL_EPSILON)
					 return false;
		}
	}

	return true;
}

template<typename T>
void Matrix<T>::AddRows(int sourceRowIndex, int destinationRowIndex)
{
	if (sourceRowIndex < 0 ||
		sourceRowIndex >= _rows ||
		destinationRowIndex < 0 ||
		destinationRowIndex >= _rows)
		throw new std::exception("Row indices out of range:  Matrix.h");

	for (int column = 0; column < _columns; column++)
	{
		T value = this->Get(sourceRowIndex, column) + this->Get(destinationRowIndex, column);

		this->Set(destinationRowIndex, column, value);
	}
}

template<typename T>
void Matrix<T>::MultRow(int rowIndex, T factor)
{
	if (rowIndex < 0 ||
		rowIndex >= _rows)
		throw new std::exception("Row index out of range:  Matrix.h");

	for (int column = 0; column < _columns; column++)
	{
		T value = this->Get(rowIndex, column);

		this->Set(rowIndex, column, value * factor);
	}
}

template<typename T>
void Matrix<T>::IterateByValue(MatrixIteratorByValue callback) const
{
	for (int row = 0; row < _rows; row++)
	{
		for (int column = 0; column < _columns; column++)
		{
			// Passing by value to user code
			callback(row, column, this->Get(row, column));
		}
	}
}

template<typename T>
void Matrix<T>::IterateByRef(MatrixIteratorByReference callback) const
{
	for (int row = 0; row < _rows; row++)
	{
		for (int column = 0; column < _columns; column++)
		{
			// Passing address to the memory location
			callback(row, column, &_data[row][column]);
		}			
	}
}

template<typename T>
void Matrix<T>::IterateSet(MatrixIteratorRef callback)
{
	IterateByRef([&](int row, int column, T* currentRef) {
		callback(currentRef);
	});
}

template<typename T>
void Matrix<T>::IterateSet(T value, MatrixIteratorPredicate predicate)
{
	IterateByRef([&](int row, int column, T* currentRef) {
		if (predicate(row, column, currentRef))
			*currentRef = value;
	});
}

template<typename T>
void Matrix<T>::Output() const
{
	std::cout << std::endl;

	IterateByValue([&](int row, int column, double value) {

		if (column == 0)
			std::cout << "| ";

		std::cout << std::to_string(value) + " ";

		if (column == (_columns - 1))
			std::cout << " |" << std::endl;
	});

	std::cout << std::endl;
}

#endif