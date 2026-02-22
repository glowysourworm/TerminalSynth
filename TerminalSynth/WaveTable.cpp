#include "PlaybackFrame.h"
#include "WaveTable.h"

WaveTable::WaveTable(unsigned int frameLength, unsigned int samplingRate, unsigned int systemSamplingRate)
{
	_samplingRate = samplingRate;
	_systemSamplingRate = systemSamplingRate;
	_frameLength = frameLength;
	_frames = new PlaybackFrame[_frameLength];
}

WaveTable::~WaveTable()
{
	delete[] _frames;
}

void WaveTable::CreateSamplesByTime(WaveTableSampleGenerateSecondCallback callback)
{
	// Oversampling
	for (int index = 0; index < _frameLength; index++)
	{
		float left = 0;
		float right = 0;
		float sampleTime = (index / (float)_samplingRate) * (_samplingRate / (float)_systemSamplingRate);				// Frame Length is scaled
		callback(sampleTime, left, right);

		_frames[index].SetFrame(left, right);
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

		_frames[index].SetFrame(left, right);
	}
}

float WaveTable::GetSampleL(double absoluteTime)
{
	int bigIndex = absoluteTime * _systemSamplingRate * (_samplingRate / (float)_systemSamplingRate);			// Expanded to Frame Length
	int frameIndex = bigIndex % _frameLength;

	return _frames[frameIndex].GetLeft();
}

float WaveTable::GetSampleR(double absoluteTime)
{
	int bigIndex = absoluteTime * _systemSamplingRate * (_samplingRate / (float)_systemSamplingRate);			// Expanded to Frame Length (which is oversampled)
	int frameIndex = bigIndex % _frameLength;

	return _frames[frameIndex].GetRight();
}