#pragma once

#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include "PlaybackBuffer.h"
#include <cmath>
#include <cstdlib>

template<SignalValue TSignal>
class Accumulator
{
public:

	/// <summary>
	/// Creates accumulator with infinite window length
	/// </summary>
	/// <param name="useAbsoluteValue">Use absolute value of input signal for accumulator</param>
	Accumulator(bool useAbsoluteValue);

	/// <summary>
	/// Creates accumulator with finite window length
	/// </summary>
	/// <param name="useAbsoluteValue">Use absolute value of input signal for accumulator</param>
	/// <param name="windowLength">Window length for the signal (samples)</param>
	Accumulator(bool useAbsoluteValue, int windowLength);
	~Accumulator();

	void Add(TSignal value);

	TSignal GetTotal() const;
	TSignal GetAvg() const;

	void Reset();

private:

	TSignal _last;
	TSignal _lastWindowed;
	TSignal _average;
	TSignal _total;
	int _counter;
	int _windowLength;
	bool _useAbsoluteValue;

};

template<SignalValue TSignal>
Accumulator<TSignal>::Accumulator(bool useAbsoluteValue)
{
	_last = 0;
	_lastWindowed = 0;
	_average = 0;
	_total = 0;
	_counter = 0;
	_windowLength = -1;
	_useAbsoluteValue = useAbsoluteValue;
}

template<SignalValue TSignal>
Accumulator<TSignal>::Accumulator(bool useAbsoluteValue, int windowLength)
{
	_last = 0;
	_lastWindowed = 0;
	_average = 0;
	_total = 0;
	_counter = 0;
	_windowLength = windowLength;
	_useAbsoluteValue = useAbsoluteValue;
}

template<SignalValue TSignal>
Accumulator<TSignal>::~Accumulator()
{
}

template<SignalValue TSignal>
void Accumulator<TSignal>::Add(TSignal value)
{
	TSignal useValue = _useAbsoluteValue ? abs(value) : value;

	// Windowed
	if (_windowLength > 0 && _counter >= _windowLength)
	{
		_average += (1 / (double)(_counter)) * (useValue - _lastWindowed);
		_lastWindowed = useValue;
	}

	// Cummulative
	else
	{
		_average += (useValue - _average) / (_counter++ + 1);
	}


	_last = value;
	_total += value;
}

template<SignalValue TSignal>
TSignal Accumulator<TSignal>::GetTotal() const
{
	return _total;
}

template<SignalValue TSignal>
TSignal Accumulator<TSignal>::GetAvg() const
{
	return _average;
}

template<SignalValue TSignal>
void Accumulator<TSignal>::Reset()
{
	_last = 0;
	_average = 0;
	_total = 0;
}

#endif