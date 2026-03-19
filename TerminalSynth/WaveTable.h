//#pragma once
//
//#ifndef WAVETABLE_H
//#define WAVETABLE_H
//
//#include "Matrix.h"
//#include "PlaybackFrame.h"
//#include "Vector.h"
//#include "WaveBase.h"
//#include <functional>
//
//class WaveTable : public WaveBase
//{
//public:
//
//	enum class Mode
//	{
//		/// <summary>
//		/// The wave table will hold a single period of the oscillator; and repeat it.
//		/// </summary>
//		Periodic,
//
//		/// <summary>
//		/// The wave table will hold a sound sample that is not likely a single period of the
//		/// periodic wave form. It should be treated as such; and triggered a single time.
//		/// </summary>
//		SoundSample
//	};
//
//	/// <summary>
//	/// Delegate to create L/R channel samples for the wave table. The sampleTime [0, period] will go until the period
//	/// has been reached. The WaveTable's responsibility is to provide time points for its oversampled range. Each sample
//	/// will be set into the local frame buffer.
//	/// </summary>
//	using WaveTableSampleGenerateSecondCallback = std::function<void(float sampleTime, float& leftSample, float& rightSample)>;
//
//	/// <summary>
//	/// Delegate to create L/R channel samples for the wave table. The frameIndex [0, period] will go until the period
//	/// has been reached. The WaveTable's responsability is to provide time points for its oversampled range. Each sample
//	/// will be set into the local frame buffer.
//	/// </summary>
//	using WaveTableSampleGenerateFrameCallback = std::function<void(int frameIndex, float& leftSample, float& rightSample)>;
//
//	WaveTable(Mode mode, unsigned int frameLength, unsigned int samplingRate, unsigned int systemSamplingRate);
//	~WaveTable();
//
//	/// <summary>
//	/// Sets samples into the wave table (using stream time)
//	/// </summary>
//	void CreateSamplesByTime(WaveTableSampleGenerateSecondCallback callback);
//
//	/// <summary>
//	/// Sets samples into the wave table (per frame)
//	/// </summary>
//	void CreateSamplesByFrame(WaveTableSampleGenerateFrameCallback callback);
//
//	bool HasOutput(double zeroTime, double absoluteTime) const override;
//	void Clear(double zeroTime, double absoluteTime) override;
//
//	/// <summary>
//	/// Gets (oversampled) frame length of the wave table
//	/// </summary>
//	int GetFrameLength() const;
//
//	/// <summary>
//	/// Gets (oversampled) sampling rate of the wave table
//	/// </summary>
//	float GetSamplingRate() const;
//
//protected:
//
//	void SetFrameImpl(PlaybackFrame* frame, double zeroTime, double absoluteTime) override;
//
//private:
//
//	float GetLinearSpline(size_t timeCursor, double zeroTime, double absoluteTime, bool channelLeft);
//	//float GetLinearSplineBuffered(double absoluteTime, bool channelLeft);
//	//float SetFrameCubicSpline(PlaybackFrame* frame, double absoluteTime, bool channelLeft);
//
//private:
//
//	Mode _mode;
//
//	unsigned int _samplingRate;					// These two sampling rates may differ depending on how the table was built.
//	unsigned int _systemSamplingRate;			// The ratio of the two will show the scale factor for providing accurate samples.
//	PlaybackFrame* _frames;
//	unsigned int _frameLength;
//
//	// A-Matrix from our cubic spline interpolation
//	Matrix<double>* _splineA;
//	Matrix<double>* _splineAInverse;
//	Vector<double>* _splineB;
//	Vector<double>* _splineK;
//};
//
//#endif