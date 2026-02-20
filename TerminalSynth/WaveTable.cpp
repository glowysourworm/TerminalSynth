#include "PlaybackFrame.h"
#include "WaveTable.h"

WaveTable::WaveTable(unsigned int midiNote, unsigned int frequency, unsigned int samplingRate)
{
	_scaleFactor = 100;
	_midiNote = midiNote;
	_samplingRate = samplingRate;
	_frequency = frequency;
	_period = 1 / (float)_frequency;
	_frameLength = _period * samplingRate * _scaleFactor;					// Set for oversampling
	_frames = new PlaybackFrame[_frameLength];
}

WaveTable::~WaveTable()
{
	delete[] _frames;
}

void WaveTable::CreateSamples(WaveTableSampleGenerateCallback callback)
{
	// Oversampling
	for (int index = 0; index < _frameLength; index++)
	{
		float left = 0;
		float right = 0;
		float sampleTime = index * (_period / (float)_frameLength);		// Frame Length is scaled
		callback(sampleTime, left, right);

		_frames[index].SetFrame(left, right);
	}
}

float WaveTable::GetSampleL(double absoluteTime)
{
	int bigIndex = absoluteTime * _samplingRate * _scaleFactor;			// Expanded to Frame Length
	int frameIndex = bigIndex % _frameLength;

	return _frames[frameIndex].GetLeft();
}

float WaveTable::GetSampleR(double absoluteTime)
{
	int bigIndex = absoluteTime * _samplingRate * _scaleFactor;			// Expanded to Frame Length (which is oversampled)
	int frameIndex = bigIndex % _frameLength;


	return _frames[frameIndex].GetRight();
}