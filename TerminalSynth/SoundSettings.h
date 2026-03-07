#pragma once

#ifndef SOUND_SETTINGS_H
#define SOUND_SETTINGS_H

#include "Envelope.h"
#include "OscillatorParameters.h"
#include "SignalChainSettings.h"
#include <istream>
#include <ostream>
#include <string>

class SoundSettings
{
public:

	SoundSettings() : SoundSettings("No Name")
	{ }
	SoundSettings(const std::string& name)
	{
		_name = new std::string(name);
		_postProcessing = new SignalChainSettings();
		_signalProcessing = new SignalChainSettings();
		_oscillatorParameters = new OscillatorParameters();
		_oscillatorEnvelope = new Envelope();
	}
	SoundSettings(const SoundSettings& copy)
	{
		_name = new std::string(copy.GetName());
		_postProcessing = new SignalChainSettings(*copy.GetPostProcessing());
		_signalProcessing = new SignalChainSettings(*copy.GetSignalChain());
		_oscillatorParameters = new OscillatorParameters(*copy.GetOscillatorParameters());
		_oscillatorEnvelope = new Envelope(*copy.GetOscillatorEnvelope());
	}
	~SoundSettings()
	{
		delete _name;
		delete _signalProcessing;
		delete _postProcessing;
		delete _oscillatorParameters;
		delete _oscillatorEnvelope;
	}

	std::string GetName() const { return *_name; }

	OscillatorParameters* GetOscillatorParameters() const { return _oscillatorParameters; }
	Envelope* GetOscillatorEnvelope() const { return _oscillatorEnvelope; }

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
		isDirty |= _postProcessing->Update(settings->GetPostProcessing(), true);			// overwrite
		isDirty |= _signalProcessing->Update(settings->GetSignalChain(), true);				// overwrite

		return isDirty;
	}

public:

	bool IsEqual(const SoundSettings* other) const
	{
		return *_name == other->GetName() &&
			_oscillatorParameters->IsEqual(other->GetOscillatorParameters()) &&
			_oscillatorEnvelope->IsEqual(other->GetOscillatorEnvelope()) &&
			_postProcessing->IsEqual(other->GetPostProcessing()) &&
			_signalProcessing->IsEqual(other->GetSignalChain());
	}

public:

	void Save(std::ostream& stream)
	{
		// Name
		stream << *_name;

		// Oscillator Parameters
		_oscillatorParameters->Save(stream);

		// Envelope
		_oscillatorEnvelope->Save(stream);

		// Post Processing
		_postProcessing->Save(stream);

		// Signal Processing
		_signalProcessing->Save(stream);
	}
	void Read(std::istream& stream)
	{
		// Name
		stream >> *_name;

		// Oscillator Parameters
		_oscillatorParameters->Read(stream);

		// Envelope
		_oscillatorEnvelope->Read(stream);

		// Post Processing
		_postProcessing->Read(stream);

		// Signal Processing
		_signalProcessing->Read(stream);
	}

private:

	std::string* _name;

	// Oscillator
	OscillatorParameters* _oscillatorParameters;

	// Envelope
	Envelope* _oscillatorEnvelope;

	// Current included SignalBase* effects in post processing
	SignalChainSettings* _postProcessing;

	// Current included SignalBase* effects in the synth note signal chain
	SignalChainSettings* _signalProcessing;
};

#endif