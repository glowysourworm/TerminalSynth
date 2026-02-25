#pragma once

#ifndef WAVETABLE_H
#define WAVETABLE_H

#include "Matrix.h"
#include "PlaybackFrame.h"
#include "Vector.h"
#include "WaveBase.h"
#include <functional>

class WaveTable : public WaveBase
{
public:

	/// <summary>
	/// Delegate to create L/R channel samples for the wave table. The sampleTime [0, period] will go until the period
	/// has been reached. The WaveTable's responsibility is to provide time points for its oversampled range. Each sample
	/// will be set into the local frame buffer.
	/// </summary>
	using WaveTableSampleGenerateSecondCallback = std::function<void(float sampleTime, float& leftSample, float& rightSample)>;

	/// <summary>
	/// Delegate to create L/R channel samples for the wave table. The frameIndex [0, period] will go until the period
	/// has been reached. The WaveTable's responsability is to provide time points for its oversampled range. Each sample
	/// will be set into the local frame buffer.
	/// </summary>
	using WaveTableSampleGenerateFrameCallback = std::function<void(int frameIndex, float& leftSample, float& rightSample)>;

	WaveTable(unsigned int frameLength, unsigned int samplingRate, unsigned int systemSamplingRate);
	~WaveTable() override;

	/// <summary>
	/// Sets samples into the wave table (using stream time)
	/// </summary>
	void CreateSamplesByTime(WaveTableSampleGenerateSecondCallback callback);

	/// <summary>
	/// Sets samples into the wave table (per frame)
	/// </summary>
	void CreateSamplesByFrame(WaveTableSampleGenerateFrameCallback callback);

	bool HasOutput(float absoluteTime) const override;

protected:

	void SetFrameImpl(PlaybackFrame* frame, double absoluteTime) override;

private:

	float GetLinearSpline(double absoluteTime, bool channelLeft);
	//float GetLinearSplineBuffered(double absoluteTime, bool channelLeft);
	float GetCubicSpline(double absoluteTime, bool channelLeft);

private:

	unsigned int _samplingRate;					// These two sampling rates may differ depending on how the table was built.
	unsigned int _systemSamplingRate;			// The ratio of the two will show the scale factor for providing accurate samples.
	PlaybackFrame* _frames;
	unsigned int _frameLength;
	double _lastPeriodTime;

	// A-Matrix from our cubic spline interpolation
	Matrix<double>* _splineA;
	Matrix<double>* _splineAInverse;
	Vector<double>* _splineB;
	Vector<double>* _splineK;
};

#endif