#pragma once

#ifndef SIGNAL_PARAMETER_AUTOMATER_H
#define SIGNAL_PARAMETER_AUTOMATER_H

#include "Constant.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactoryCore.h"
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
	void Initialize(const OutputSettings* parameters);

	/// <summary>
	/// Updates from SignalParameter*
	/// </summary>
	void Update(const SignalParameter* parameter);

	/// <summary>
	/// Gets the current value of the parameter, which may depend on the envelope level. NOTE:  THE
	/// PLAYBACK FRAME IS NOT ALTERED! There should be a const, here, but there's still a problem in 
	/// the SignalFactory*
	/// </summary>
	float GetValue(const PlaybackFrame* frame, double absoluteTime) const;

private:

	ParameterAutomationType _type;
	ParameterAutomationOscillator _oscillatorType;

	// NOTE:  SignalFactoryCore* does not depend on SignalBase*

	PlaybackFrame* _frame;									// Our local frame for using the SignalFactory*
	SignalFactoryCore* _signalFactory;						// Our local (non-system) version
	SignalFactoryCore* _signalFactoryRandom;				// Our local (non-system) version, set up for random value generation
	OscillatorParameters* _oscillatorParameters;			// These will not be the system values
	OscillatorParameters* _oscillatorParametersRandom;		// These will not be the system values, set up for random value generation
};

#endif