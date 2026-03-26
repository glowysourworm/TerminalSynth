#pragma once

#ifndef SIGNAL_PARAMETER_AUTOMATER_H
#define SIGNAL_PARAMETER_AUTOMATER_H

#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalParameter.h"

/// <summary>
/// Container class for a chain of SignalBase* instances
/// </summary>
class SignalParameterAutomater
{
public:

	SignalParameterAutomater();
	~SignalParameterAutomater();

	/// <summary>
	/// Initializes components with output settings
	/// </summary>
	void Initialize(const PlaybackInfo* parameters);

	/// <summary>
	/// Updates from SignalParameter*
	/// </summary>
	void Update(const SignalParameter* parameter);

	/// <summary>
	/// Gets the current value of the parameter, which may depend on the envelope level. NOTE:  THE
	/// PLAYBACK FRAME IS NOT ALTERED! There should be a const, here, but there's still a problem in 
	/// the SignalFactory*
	/// </summary>
	float GetValue(const PlaybackFrame* frame, const PlaybackTime* playbackTime) const;

	/// <summary>
	/// Engages the parameter automater's envelope (if set)
	/// </summary>
	void Engage(const PlaybackTime* playbackTime);

	/// <summary>
	/// Dis-engages the parameter automater's envelope (if set)
	/// </summary>
	void DisEngage(const PlaybackTime* playbackTime);

private:

	float _samplingRate;

	ParameterAutomationType _type;
	ParameterAutomationOscillator _oscillatorType;

	PlaybackFrame* _frame;									// Our local frame for using the SignalFactory*
	OscillatorParameters* _oscillatorParameters;			// These will not be the system values

	float _oscillatorFrequency;

	Envelope* _envelope;
};

#endif