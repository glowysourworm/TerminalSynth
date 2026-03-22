#pragma once

#ifndef INPUT_MODEL_UI_H
#define INPUT_MODEL_UI_H

#include "Envelope.h"
#include "ModelUI.h"
#include "OscillatorParameters.h"
#include "PlaybackUserData.h"
#include "SoundBankSettings.h"
#include "SynthNoteParameters.h"
#include <string>

class InputModelUI : public ModelUI
{
public:

	InputModelUI(const PlaybackUserData* playbackData);
	InputModelUI(const InputModelUI& copy);
	~InputModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void From(OscillatorParameters* parameters, Envelope* envelope, SynthNoteParameters* noteParameters);
	void To(const OscillatorParameters* parameters, const Envelope* envelope, const SynthNoteParameters* noteParameters);

public:

	OscillatorParameters* GetOscillatorParameters() const { return _oscillatorParameters; }
	Envelope* GetEnvelope() const { return _envelope; }
	SynthNoteParameters* GetSynthNoteParamters() const { return _synthNoteParameters; }

	const SoundBankSettings* GetSoundBankSettings() const { return _soundBankSettings; }

private:

	OscillatorParameters* _oscillatorParameters;
	Envelope* _envelope;
	SynthNoteParameters* _synthNoteParameters;

	// Read Only:  DO NOT DELETE!
	const SoundBankSettings* _soundBankSettings;

	std::string* _name;
};

InputModelUI::InputModelUI(const PlaybackUserData* playbackData)
{
	_name = new std::string("Input");
	_envelope = new Envelope(*playbackData->GetSynthSettings()->GetCurrentSoundSettings()->GetOscillatorEnvelope());
	_oscillatorParameters = new OscillatorParameters(*playbackData->GetSynthSettings()->GetCurrentSoundSettings()->GetOscillatorParameters());
	_synthNoteParameters = new SynthNoteParameters(*playbackData->GetSynthSettings()->GetCurrentSoundSettings()->GetNoteParameters());
	_soundBankSettings = playbackData->GetSynthSettings()->GetSoundBankSettings();
}

InputModelUI::InputModelUI(const InputModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_envelope = new Envelope(*copy.GetEnvelope());
	_oscillatorParameters = new OscillatorParameters(*copy.GetOscillatorParameters());
	_synthNoteParameters = new SynthNoteParameters(*copy.GetSynthNoteParamters());
	_soundBankSettings = copy.GetSoundBankSettings();
}

InputModelUI::~InputModelUI()
{
	delete _name;
	delete _envelope;
	delete _oscillatorParameters;
	delete _synthNoteParameters;
}

std::string InputModelUI::GetName() const
{
	return *_name;
}

int InputModelUI::GetOrder() const
{
	return 0;
}

void InputModelUI::From(OscillatorParameters* parameters, Envelope* envelope, SynthNoteParameters* noteParameters)
{
	parameters->Update(_oscillatorParameters);
	envelope->Update(_envelope);
	noteParameters->arpeggioBPM = _synthNoteParameters->arpeggioBPM;
	noteParameters->chord = _synthNoteParameters->chord;
	noteParameters->mode = _synthNoteParameters->mode;
	noteParameters->pornamentoSeconds = _synthNoteParameters->pornamentoSeconds;
}
void InputModelUI::To(const OscillatorParameters* parameters, const Envelope* envelope, const SynthNoteParameters* noteParameters)
{
	//_oscillatorParameters->Update(parameters);
	//_envelope->Update(envelope);
}

#endif