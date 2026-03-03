#pragma once

#ifndef SOUND_SETTINGS_H
#define SOUND_SETTINGS_H

#include "Envelope.h"
#include "OscillatorParameters.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include <vector>

class SoundSettings
{
public:

	SoundSettings()
	{
		_completeList = new SignalChainSettings();
		_postProcessing = new SignalChainSettings();
		_signalProcessing = new SignalChainSettings();
		_oscillatorParameters = new OscillatorParameters();
		_oscillatorEnvelope = new Envelope();
	}
	SoundSettings(const SoundSettings& copy)
	{
		_completeList = new SignalChainSettings(*copy.GetEffectRegistry());
		_postProcessing = new SignalChainSettings(*copy.GetPostProcessing());
		_signalProcessing = new SignalChainSettings(*copy.GetSignalChain());
		_oscillatorParameters = new OscillatorParameters(*copy.GetOscillatorParameters());
		_oscillatorEnvelope = new Envelope(*copy.GetOscillatorEnvelope());
	}
	~SoundSettings()
	{
		delete _signalProcessing;
		delete _postProcessing;
		delete _completeList;
		delete _oscillatorParameters;
		delete _oscillatorEnvelope;
	}

	void Initialize(const std::vector<SignalSettings>& signalChainRegistry)
	{
		for (int index = 0; index < signalChainRegistry.size(); index++)
		{
			_completeList->Add(signalChainRegistry.at(index));
		}
	}

	OscillatorParameters* GetOscillatorParameters() const { return _oscillatorParameters; }
	Envelope* GetOscillatorEnvelope() const { return _oscillatorEnvelope; }

	SignalChainSettings* GetEffectRegistry() const { return _completeList; }
	SignalChainSettings* GetSignalChain() const { return _signalProcessing; }
	SignalChainSettings* GetPostProcessing() const { return _postProcessing; }

	/// <summary>
	/// Updates the sound settings data. Returns true if there were changes to the settings.
	/// </summary>
	bool Update(const SoundSettings* settings)
	{
		bool isDirty = false;

		isDirty |= _oscillatorParameters->Update(settings->GetOscillatorParameters());
		isDirty |= _oscillatorEnvelope->Update(settings->GetOscillatorEnvelope());
		isDirty |= _completeList->Update(settings->GetEffectRegistry(), false);
		isDirty |= _postProcessing->Update(settings->GetPostProcessing(), true);			// overwrite
		isDirty |= _signalProcessing->Update(settings->GetSignalChain(), true);				// overwrite

		return isDirty;
	}

private:

	// Oscillator
	OscillatorParameters* _oscillatorParameters;

	// Envelope
	Envelope* _oscillatorEnvelope;

	// Effect Registry:  There was a circular reference coupling SignalBase* to SynthSettings*. So, this list is just 
	//					 loose strings for having a lookup. This is the complete list of SignalSettings*.
	SignalChainSettings* _completeList;

	// Current included SignalBase* effects in post processing
	SignalChainSettings* _postProcessing;

	// Current included SignalBase* effects in the synth note signal chain
	SignalChainSettings* _signalProcessing;
};

#endif