#include "Matrix.h"
#include "PlaybackFrame.h"
#include "Vector.h"
#include "WaveBase.h"
#include "WaveTable.h"
#include <cmath>
#include <exception>

WaveTable::WaveTable(Mode mode, unsigned int frameLength, unsigned int samplingRate, unsigned int systemSamplingRate)
	: WaveBase(samplingRate)
{
	_mode = mode;

	_samplingRate = samplingRate;
	_systemSamplingRate = systemSamplingRate;
	_frameLength = frameLength;
	_frames = new PlaybackFrame[_frameLength];

	_splineA = new Matrix<double>(3, 3);
	_splineAInverse = new Matrix<double>(3, 3);
	_splineB = new Vector<double>(3);
	_splineK = new Vector<double>(3);
}

WaveTable::~WaveTable()
{
	delete[] _frames;
	delete _splineA;
	delete _splineAInverse;
	delete _splineB;
	delete _splineK;
}

void WaveTable::CreateSamplesByTime(WaveTableSampleGenerateSecondCallback callback)
{
	// Oversampling
	for (int index = 0; index < _frameLength; index++)
	{
		float left = 0;
		float right = 0;
		float sampleTime = (index / (float)_samplingRate);		// Frame length and sampling rate are both in the scaled domain
		callback(sampleTime, left, right);

		_frames[index].SetFrame(left, right, 1.0f);
	}
}

void WaveTable::CreateSamplesByFrame(WaveTableSampleGenerateFrameCallback callback)
{
	// Oversampling (Caller must know the frame length)
	for (int index = 0; index < _frameLength; index++)
	{
		float left = 0;
		float right = 0;
		callback(index, left, right);

		_frames[index].SetFrame(left, right, 1.0f);
	}
}

bool WaveTable::HasOutput(double zeroTime, double absoluteTime) const
{
	switch (_mode)
	{
	case WaveTable::Mode::Periodic:
		return true;
	case WaveTable::Mode::SoundSample:
		return (absoluteTime - zeroTime) < (_frameLength / (double)_samplingRate);			// Checks for more samples
	default:
		throw new std::exception("Unhandled wave table mode:  WaveTable.cpp");
	}
}

void WaveTable::Clear(double zeroTime, double absoluteTime)
{
	WaveBase::Clear(zeroTime, absoluteTime);
}

int WaveTable::GetFrameLength() const
{
	return _frameLength;
}

float WaveTable::GetSamplingRate() const
{
	return _samplingRate;
}

void WaveTable::SetFrameImpl(PlaybackFrame* frame, double zeroTime, double absoluteTime)
{
	float left = GetLinearSpline(zeroTime, absoluteTime, true);
	float right = GetLinearSpline(zeroTime, absoluteTime, false);

	frame->SetFrame(left, right, frame->GetEnvelopeLevel());
}

float WaveTable::GetLinearSpline(double zeroTime, double absoluteTime, bool channelLeft)
{
	// The zero time is called by the SynthNote* when it is engaged
	double sampleTime = absoluteTime - zeroTime;

	// First, take our oversampled array, and get the necessary points to match it to the spline
	int bigIndex = sampleTime * _systemSamplingRate /* *(_samplingRate / (float)_systemSamplingRate)*/;			// Expanded to Frame Length (which is oversampled)

	// Oversampled frame domain
	int frame0Index = bigIndex % _frameLength;
	int frame1Index = (bigIndex + 1) % _frameLength;

	// These signal coordaintes are from our oversampled domain
	double y0 = channelLeft ? _frames[frame0Index].GetLeft() : _frames[frame0Index].GetRight();
	double y1 = channelLeft ? _frames[frame1Index].GetLeft() : _frames[frame1Index].GetRight();

	// Anti-Aliasing (restart the wave period)
	//if (frame1Index < frame0Index)
	//	return y0;

	// Can't interpolate (no need to interpolate)
	if (y0 == y1)
		return y0;

	// These time coordinates are calculated in system time (but just for the duration of one buffer length!)
	double x0 = frame0Index / (double)_samplingRate;
	double x1 = frame1Index < frame0Index ? (x0 + _frameLength / (double)_samplingRate) : frame1Index / (double)_samplingRate;	// Wrap-around check
	
	// y = m * x + b
	//
	// y0 = m * x0 + b
	// y1 = m * x1 + b
	//
	// y1 - y0 = m * (x1 - x0)
	//
	// b = y0 - m * x0
	//
	double m = (x1 - x0) / (y1 - y0);
	double b = y0 - (m * x0);

	// We need to get the closest wave time (in system time coordinates).
	double waveTime = fmod(sampleTime, _frameLength / (double)_samplingRate);

	return (m * waveTime) + b;
}
//
//float WaveTable::GetLinearSplineBuffered(double absoluteTime, bool channelLeft)
//{
//	// First, take our oversampled array, and get the necessary points to match it to the spline
//	int bigIndex = absoluteTime * _systemSamplingRate * (_samplingRate / (float)_systemSamplingRate);			// Expanded to Frame Length (which is oversampled)
//
//	// Oversampled frame domain
//	int frameIndex = bigIndex % _frameLength;
//
//	// These signal coordaintes are from our oversampled domain:  y0 = last output, y1 = next output
//	double y0 = channelLeft ? this->GetOutput().GetLeft() : this->GetOutput().GetRight();
//	double y1 = channelLeft ? _frames[frameIndex].GetLeft() : _frames[frameIndex].GetRight();
//
//	// These time coordinates are buffered (stored)
//	//
//	double x0 = this->GetLastTime();					// System time coordinate
//	double x1 = absoluteTime;
//
//	// y = m * x + b
//	//
//	// y0 = m * x0 + b
//	// y1 = m * x1 + b
//	//
//	// y1 - y0 = m * (x1 - x0)
//	//
//	// b = y0 - m * x0
//	//
//	double m = (x1 - x0) / (y1 - y0);
//	double b = y0 - (m * x0);
//
//	// We need to get the closest wave time (in system time coordinates).
//	double waveTime = fmod(absoluteTime, _frameLength / (double)_samplingRate);
//
//	return (m * waveTime) + b;
//}

float WaveTable::GetCubicSpline(double absoluteTime, bool channelLeft)
{
	// https://en.wikipedia.org/wiki/Spline_interpolation
	//
	// The example shows how the cubic spline problem breaks
	// down into solving a simple 3x3 linear system for the 
	// parameter values. Matching the first and second derivatives
	// guarantees a smooth function at the spline "knots". 

	// First, take our oversampled array, and get the necessary points to match it to the spline
	int bigIndex = absoluteTime * _systemSamplingRate * (_samplingRate / (float)_systemSamplingRate);			// Expanded to Frame Length (which is oversampled)

	// NOTE:  The domain "x", in our program, is the time. Wikipedia's time parameter "t" was used to
	//		  parameterize the domain from [0,1].

	// x_0, x_1, x_2 (our periodic function will be mapped to the continuous domain, here)
	int frameIndex1 = bigIndex % _frameLength;
	int frameIndex0 = frameIndex1 - 1 < 0 ? _frameLength - 1 : frameIndex1 - 1;
	int frameIndex2 = frameIndex1 + 1 >= _frameLength ? 0 : frameIndex1 + 1;

	// y(x_0) = y_1, y(x_1) = y_2, y(x_3) = y_3
	double y0 = channelLeft ? _frames[frameIndex0].GetLeft() : _frames[frameIndex0].GetRight();
	double y1 = channelLeft ? _frames[frameIndex1].GetLeft() : _frames[frameIndex1].GetRight();
	double y2 = channelLeft ? _frames[frameIndex2].GetLeft() : _frames[frameIndex2].GetRight();

	double deltaTime = 1.0 / (double)_systemSamplingRate;														// Our dt is in the normal time domain

	// The x_0, x_1, x_2, knots weren't needed other than the constant deltaTime
	//
	_splineA->Set(0, 0, 2.0 / deltaTime);		// a_11
	_splineA->Set(0, 1, 1.0 / deltaTime);		// a_12
	_splineA->Set(0, 2, 0);						// a_13

	_splineA->Set(1, 0, 1.0 / deltaTime);		// a_21
	_splineA->Set(1, 1, 4.0 / deltaTime);		// a_22
	_splineA->Set(1, 2, 1.0 / deltaTime);		// a_23

	_splineA->Set(1, 2, 0);						// a_31
	_splineA->Set(2, 1, 1.0 / deltaTime);		// a_32
	_splineA->Set(2, 2, 2.0 / deltaTime);		// a_33

	_splineB->Set(0, 3.0 * ((y1 - y0) / powf(deltaTime, 2.0f)));
	_splineB->Set(1, 3.0 * (((y1 - y0) / powf(deltaTime, 2.0f)) + ((y2 - y1) / powf(deltaTime, 2.0f))));
	_splineB->Set(2, 3.0 * ((y2 - y1) / powf(deltaTime, 2.0f)));

	// Linear System:
	//
	// k0 * a11 + k1 * a12			  = b1
	// k0 * a21 + k1 * a22 + k2 * a23 = b2
	//            k1 * a32 + k2 * a33 = b3
	//
	// Substitute the result k_i, back into the equations for the spline q(x)
	//
	// a1 =  k0 * dt - (y1 - y0)
	// a2 =  k1 * dt - (y2 - y1)
	// b1 = -k1 * dt + (y1 - y0)
	// b2 = -k2 * dt + (y2 - y1)

	// Ak = b  (solve for k)
	// k = A_inv * b

	// Initialize A_inv
	_splineAInverse->SetFrom(*_splineA);

	// Calculate Inverse
	if (_splineAInverse->Invert())
	{
		// k = A_inv * b -> (stored to) k
		_splineAInverse->Mult(*_splineB, *_splineK);
	}
	else
	{
		throw new std::exception("Unable to invert audio matrix:  WaveTable.cpp");
	}

	double a1 = (_splineK->Get(0) * deltaTime) - (y1 - y0);
	double a2 = (_splineK->Get(1) * deltaTime) - (y2 - y1);
	double b1 = (-1 * _splineK->Get(1) * deltaTime) + (y1 - y0);
	double b2 = (-1 * _splineK->Get(2) * deltaTime) + (y2 - y1);

	// Substitute Backwards:  t(x) = x - x_1 / x_2 - x_1
	// 
	// Time Coordinates:  We're going to use the absolute time, here, unless there
	//					  is a miscalculation that is audible. I would expect a 
	//					  treatment based on frequency analysis would be better suited
	//					  to interpolating this periodic signal.
	//
	double period = _frameLength / (double)_samplingRate;			// Period calculated from oversampled frame length and sampling rate
	double periodTime = fmod(absoluteTime, period);					// Normal time domain
	double x1 = frameIndex1 / (double)_samplingRate;				// this gives us x_1, for y(x_1) = y_1
	double t = (periodTime - x1) / deltaTime;						// Normal time domain

	// Finally:  q_i(x) = (1 - t)y_i-1 + (t * y_i) + ((t * (1 - t)) * ((1 - t)a_i + (t * b_i)))
	//
	// We'll be using i = 1, for our spline choice, out of the family of spline results
	//
	return ((1 - t) * y0) + (t * y1) + ((t * (1 - t)) * ((((1 - t) * a1) + (t * b1))));
}
