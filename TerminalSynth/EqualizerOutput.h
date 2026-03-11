#pragma once

#ifndef EQUALIZER_OUTPUT_H
#define EQUALIZER_OUTPUT_H

#include "Accumulator.h"
#include "Algorithm.h"
#include "PlaybackFrame.h"
#include <cmath>
#include <complex>
#include <exception>
#include <vector>

class EqualizerOutput
{
public:

	EqualizerOutput(unsigned int inputSizePowOf2, unsigned int outputSizePowOf2) 
	{
		if (inputSizePowOf2 < outputSizePowOf2)
			throw new std::exception("Must have a smaller output size than input:  EqualizerOutput.h");

		//_windowLength = (int)(samplingRate / 1000.0f);
		_windowLength = 10;

		_leftAccumulators = new std::vector<Accumulator<double>*>();
		_rightAccumulators = new std::vector<Accumulator<double>*>();
		_leftFFT = new std::vector<std::complex<double>>();
		_rightFFT = new std::vector<std::complex<double>>();
		_output = new std::vector<PlaybackFrame>();

		_cursor = 0;

		// Input
		for (int index = 0; index < inputSizePowOf2; index++)
		{
			_leftFFT->push_back(std::complex<double>(0, 0));
			_rightFFT->push_back(std::complex<double>(0, 0));
		}

		// Output
		for (int index = 0; index < outputSizePowOf2; index++)
		{
			// Sampling rate is ~1s relaxation period
			_leftAccumulators->push_back(new Accumulator<double>(true, _windowLength));
			_rightAccumulators->push_back(new Accumulator<double>(true, _windowLength));

			_output->push_back(PlaybackFrame(0,0,0));
		}
	};
	EqualizerOutput(const EqualizerOutput& copy)
	{
		_windowLength = copy.GetIntegrationWindowLength();

		_leftAccumulators = new std::vector<Accumulator<double>*>();
		_rightAccumulators = new std::vector<Accumulator<double>*>();
		_leftFFT = new std::vector<std::complex<double>>();
		_rightFFT = new std::vector<std::complex<double>>();
		_output = new std::vector<PlaybackFrame>();

		_cursor = 0;

		// Input
		for (int index = 0; index < copy.GetInputLength(); index++)
		{
			_leftFFT->push_back(std::complex<double>(0, 0));
			_rightFFT->push_back(std::complex<double>(0, 0));
		}

		// Output
		for (int index = 0; index < copy.GetOutputLength(); index++)
		{
			// Sampling rate is ~1s relaxation period
			_leftAccumulators->push_back(new Accumulator<double>(true, _windowLength));
			_rightAccumulators->push_back(new Accumulator<double>(true, _windowLength));

			_output->push_back(PlaybackFrame(0, 0, 0));
		}
	}
	~EqualizerOutput() 
	{
		for (int index = 0; index < _leftAccumulators->size(); index++)
		{
			delete _leftAccumulators->at(index);
			delete _rightAccumulators->at(index);
		}

		delete _leftAccumulators;
		delete _rightAccumulators;
		delete _leftFFT;
		delete _rightFFT;
		delete _output;
	};

	void AddSample(double left, double right)
	{
		double windowValue = Algorithm::GaussianWindow(0.4, _cursor, _leftFFT->size());

		_leftFFT->at(_cursor) = std::complex<double>(windowValue * left, 0);
		_rightFFT->at(_cursor) = std::complex<double>(windowValue * right, 0);

		_cursor++;

		// Ready to perform FFT
		if (_cursor == _leftFFT->size())
		{
			Algorithm::FFT(_leftFFT);
			Algorithm::FFT(_rightFFT);

			_cursor = 0;

			int outputIndex = 0;
			int bucketSize =  _leftFFT->size() / _leftAccumulators->size();
			double leftSum = 0;
			double rightSum = 0;
			double leftTotalSum = 0;
			double rightTotalSum = 0;

			// Set Output Accumulators
			for (int inputIndex = 0; inputIndex < _leftFFT->size(); inputIndex++)
			{
				// Increment to next bucket
				if (inputIndex > 0 && inputIndex % bucketSize == 0)
				{
					_leftAccumulators->at(outputIndex)->Add(leftSum / bucketSize);
					_rightAccumulators->at(outputIndex)->Add(rightSum / bucketSize);

					leftTotalSum += _leftAccumulators->at(outputIndex)->GetAvg();
					rightTotalSum += _rightAccumulators->at(outputIndex)->GetAvg();

					leftSum = 0;
					rightSum = 0;

					outputIndex++;
				}
					

				auto leftComplex = _leftFFT->at(inputIndex);
				auto rightComplex = _rightFFT->at(inputIndex);
				
				leftSum += std::sqrt((leftComplex * std::conj(leftComplex)).real());
				rightSum += std::sqrt((rightComplex * std::conj(rightComplex)).real());
			}

			// Set Output
			for (int outputIndex = 0; outputIndex < _leftAccumulators->size(); outputIndex++)
			{
				// Set output with accumulator windowed average
				_output->at(outputIndex).SetFrame(_leftAccumulators->at(outputIndex)->GetAvg() /* / fmax(leftTotalSum, 1) */,
												  _rightAccumulators->at(outputIndex)->GetAvg() /* / fmax(rightTotalSum, 1) */ , 0);
			}
		}
	}

	void GetEQ(std::vector<PlaybackFrame>* destination) const
	{
		// The Output is Symmetrical
		if (destination->size() != (_output->size() / 2))
			throw new std::exception("Destination vector for equalizer samples is not the size of the output");

		for (int index = 0; index < _output->size() / 2; index++)
		{
			destination->at(index).SetFrame(_output->at(index).GetLeft(), _output->at(index).GetRight(), 0);
		}
	}
	
	/// <summary>
	/// (MEMORY!) Creates new std::vector<PlaybackFrame>* prepared to receive sample output (contains
	///			  the current sample output)
	/// </summary>
	std::vector<PlaybackFrame>* GetEQCopy() const
	{
		std::vector<PlaybackFrame>* result = new std::vector<PlaybackFrame>();

		for (int index = 0; index < _output->size() / 2; index++)
		{
			result->push_back(PlaybackFrame(_output->at(index).GetLeft(), _output->at(index).GetRight(), 0));
		}

		return result;
	}

	int GetOutputLength() const { return _output->size(); }
	int GetInputLength() const { return _leftAccumulators->size(); }
	int GetEQLength() const { return _output->size() / 2; }

protected:

	int GetIntegrationWindowLength() const { return _windowLength; }

private:

	// Integrated output
	std::vector<Accumulator<double>*>* _leftAccumulators;
	std::vector<Accumulator<double>*>* _rightAccumulators;

	// FFT output
	std::vector<std::complex<double>>* _leftFFT;
	std::vector<std::complex<double>>* _rightFFT;

	// Output
	std::vector<PlaybackFrame>* _output;

	int _cursor;
	int _windowLength;
};

#endif