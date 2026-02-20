#pragma once

#ifndef WAVETABLE_H
#define WAVETABLE_H

#include "PlaybackFrame.h"
#include <functional>

class WaveTable
{
public:

	/// <summary>
	/// Delegate to create L/R channel samples for a given midi note. The absoluteTime [0, period] will go until the period
	/// has been reached. The WaveTable's responsability is to provide time points for its oversampled range. Each sample
	/// will be set into the local frame buffer.
	/// </summary>
	using WaveTableSampleGenerateCallback = std::function<void(float sampleTime, float& leftSample, float& rightSample)>;

	WaveTable(unsigned int midiNote, unsigned int frequency, unsigned int samplingRate);
	~WaveTable();

	/// <summary>
	/// Adds sample array to the wave table
	/// </summary>
	/// <param name="midiNote">Midi note to key the sample array</param>
	/// <param name="samples">Samples created for the wave table for this MIDI note</param>
	/// <param name="callback">Callback for creating the next sample</param>
	void CreateSamples(WaveTableSampleGenerateCallback callback);

	/// <summary>
	/// Gets a sample value for playback given the input stream time
	/// </summary>
	float GetSampleL(double absoluteTime);
	float GetSampleR(double absoluteTime);

private:

	unsigned int _midiNote;
	unsigned int _samplingRate;
	PlaybackFrame* _frames;
	unsigned int _scaleFactor;				// Oversampling:  Factor is applied to the length of the 
											// buffer relative to it's otherwise natural period.
	unsigned int _frameLength;

	float _frequency;
	float _period;
};

#endif