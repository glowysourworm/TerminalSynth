//#pragma once
//
//#ifndef WAVEBASE_H
//#define WAVEBASE_H
//
//#include "Constant.h"
//#include "PlaybackFrame.h"
//
//class WaveBase
//{
//public:
//
//	/// <summary>
//	/// Creates an instance of the wave base
//	/// </summary>
//	/// <param name="samplingRate">System (or stream) sampling rate</param>
//	WaveBase(unsigned int samplingRate)
//	{
//		_signalLow = SIGNAL_LOW;
//		_signalHigh = SIGNAL_HIGH;
//		_samplingRate = samplingRate;
//	};
//
//	/// <summary>
//	/// Creates an instance of the wave base
//	/// </summary>
//	/// <param name="samplingRate">System (or stream) sampling rate</param>
//	/// <param name="signalLow">High limit for the signal (default is SIGNAL_HIGH)</param>
//	/// <param name="signalHigh">Low limit for the signal (default is SIGNAL_LOW)</param>
//	WaveBase(unsigned int samplingRate, double signalLow, double signalHigh)
//	{
//		_signalLow = signalLow;
//		_signalHigh = signalHigh;
//		_samplingRate = samplingRate;
//	}
//	~WaveBase()
//	{
//	}
//
//	/// <summary>
//	/// (SignalBase) Sets accumulators for signal, and stores a copy of the frame in the input / output
//	/// buffers, for the specified buffer length.
//	/// </summary>
//	void SetFrame(PlaybackFrame* frame, double zeroTime, double absoluteTime)
//	{
//		// Input (copy)
//		PlaybackFrame input = *frame;
//
//		// Call the class's implementation (modifies the frame)
//		this->SetFrameImpl(frame, zeroTime, absoluteTime);
//	}
//	virtual bool HasOutput(double zeroTime, double absoluteTime) const = 0;
//
//	/// <summary>
//	/// Function to clear the signal base of all of its internal buffers, and signal history. Any parameters
//	/// or signal settings should NOT be cleared or reset to default. This is for any of the signal buffers.
//	/// </summary>
//	virtual void Clear(double zeroTime, double absoluteTime)
//	{
//	}
//
//protected:
//
//	/// <summary>
//	/// Abstract function to implement for setting the actual frame's signal
//	/// </summary>
//	/// <param name="frame"></param>
//	/// <param name="absoluteTime"></param>
//	virtual void SetFrameImpl(PlaybackFrame* frame, double zeroTime, double absoluteTime) = 0;
//
//public:
//
//	unsigned int GetSamplingRate() const { return _samplingRate; }
//
//private:
//
//	double _signalLow;
//	double _signalHigh;	
//	unsigned int _samplingRate;
//};
//
//#endif