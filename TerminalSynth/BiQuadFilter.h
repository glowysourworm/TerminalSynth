#pragma once

#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"

/// <summary>
/// Bi-Quad Filter with multiple uses:  (please see https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html). 
/// 
/// Parameter 0:  GainDb (db) (Peaking / Shelving EQ Filters Only)
/// Parameter 1:  Corner Frequency (All Filter Types)
/// Parameter 2:  Q (Resonance, Bandwidth, or Shelf Slope depending on filter type)
/// </summary>
class BiQuadFilter : public SignalBase
{
public:

	enum class FilterType
	{
		/// <summary>
		/// Low pass filter with gain response
		/// </summary>
		LPF,

		/// <summary>
		/// High pass filter with gain response
		/// </summary>
		HPF,

		/// <summary>
		/// Band pass filter with gain response
		/// </summary>
		BPF_Gain,

		/// <summary>
		/// Band pass filter with flat 0dB gain
		/// </summary>
		BPF_Flat,

		/// <summary>
		/// Narrow band filter used for cutting noise
		/// </summary>
		Notch,

		/// <summary>
		/// All pass filter
		/// </summary>
		APF,

		/// <summary>
		/// Boosts or cuts narrow band of frequencies around the corner frequency
		/// </summary>
		PeakingEQ,

		/// <summary>
		/// Flat low pass filter (good for EQ use)
		/// </summary>
		LowShelf,

		/// <summary>
		/// Flat high pass filter (good for EQ use)
		/// </summary>
		HighShelf
	};

public:

	BiQuadFilter(FilterType filterType, unsigned int samplingRate);
	BiQuadFilter(FilterType filterType, unsigned int samplingRate, float corner, float resonance);
	BiQuadFilter(FilterType filterType, unsigned int samplingRate, float dbGain, float corner, float resonance);
	~BiQuadFilter() override;

	void Initialize(const SignalSettings* settings, const OutputSettings* parameters) override;
	void SetFrame(PlaybackFrame* frame, float absoluteTime) override;
	bool HasOutput(float absoluteTime) const override;

private:

	void Set_LPF(unsigned int samplingRate);
	void Set_HPF(unsigned int samplingRate);
	void Set_BPF_Gain(unsigned int samplingRate);
	void Set_BPF_Flat(unsigned int samplingRate);
	void Set_Notch(unsigned int samplingRate);
	void Set_APF(unsigned int samplingRate);
	void Set_PeakingEQ(unsigned int samplingRate);
	void Set_LowShelf(unsigned int samplingRate);
	void Set_HighShelf(unsigned int samplingRate);

	float GetGainDb() const { return this->GetParameterValue(0); }
	float GetCorner() const { return this->GetParameterValue(1); }
	float GetQ() const { return this->GetParameterValue(2); }

private:

	PlaybackFrame* _input1;			// [n-1] frame
	PlaybackFrame* _input2;			// [n-2] frame
	PlaybackFrame* _output1;
	PlaybackFrame* _output2;

	FilterType _type;

	float _a0;
	float _a1; 
	float _a2;

	float _b0;
	float _b1;
	float _b2;
};

#endif