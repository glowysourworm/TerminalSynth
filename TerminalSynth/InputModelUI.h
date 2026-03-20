#pragma once

#ifndef INPUT_MODEL_UI_H
#define INPUT_MODEL_UI_H

#include "Envelope.h"
#include "ModelUI.h"
#include "OscillatorParameters.h"
#include "PlaybackUserData.h"
#include "SoundBankSettings.h"
#include <string>

class InputModelUI : public ModelUI
{
public:

	InputModelUI(const PlaybackUserData* playbackData);
	InputModelUI(const InputModelUI& copy);
	~InputModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void From(OscillatorParameters* parameters, Envelope* envelope);
	void To(const OscillatorParameters* parameters, const Envelope* envelope);

public:

	OscillatorParameters* GetOscillatorParameters() const { return _oscillatorParameters; }
	Envelope* GetEnvelope() const { return _envelope; }

	const SoundBankSettings* GetSoundBankSettings() const { return _soundBankSettings; }

private:

	OscillatorParameters* _oscillatorParameters;
	Envelope* _envelope;

	// Read Only:  DO NOT DELETE!
	const SoundBankSettings* _soundBankSettings;

	std::string* _name;
};

InputModelUI::InputModelUI(const PlaybackUserData* playbackData)
{
	_name = new std::string("Input");
	_envelope = new Envelope(*playbackData->GetSynthSettings()->GetCurrentSoundSettings()->GetOscillatorEnvelope());
	_oscillatorParameters = new OscillatorParameters(*playbackData->GetSynthSettings()->GetCurrentSoundSettings()->GetOscillatorParameters());
	_soundBankSettings = playbackData->GetSynthSettings()->GetSoundBankSettings();
}

InputModelUI::InputModelUI(const InputModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_envelope = new Envelope(*copy.GetEnvelope());
	_oscillatorParameters = new OscillatorParameters(*copy.GetOscillatorParameters());
	_soundBankSettings = copy.GetSoundBankSettings();
}

InputModelUI::~InputModelUI()
{
	delete _name;
	delete _envelope;
	delete _oscillatorParameters;
}

std::string InputModelUI::GetName() const
{
	return *_name;
}

int InputModelUI::GetOrder() const
{
	return 0;
}

void InputModelUI::From(OscillatorParameters* parameters, Envelope* envelope)
{
	parameters->Update(_oscillatorParameters);
	envelope->Update(_envelope);
}
void InputModelUI::To(const OscillatorParameters* parameters, const Envelope* envelope)
{
	//_oscillatorParameters->Update(parameters);
	//_envelope->Update(envelope);
}

#endif