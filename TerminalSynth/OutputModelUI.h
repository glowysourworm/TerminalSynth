#pragma once

#ifndef OUTPUT_MODEL_UI_H
#define OUTPUT_MODEL_UI_H

#include "ModelUI.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include <string>
#include <vector>

class OutputModelUI : public ModelUI
{
public:

	OutputModelUI(const OutputSettings* outputSettings);
	OutputModelUI(const OutputModelUI& copy);
	~OutputModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void FromUI(OutputSettings* destination);
	void ToUI(const OutputSettings* source);

	OutputSettings* GetOutputSettings() const;
	std::vector<PlaybackFrame>* GetEqualizerOutput() const;

private:

	std::string* _name;

	OutputSettings* _outputSettings;

	std::vector<PlaybackFrame>* _equalizerOutput;
};

OutputModelUI::OutputModelUI(const OutputSettings* outputSettings)
{
	_name = new std::string("Output");
	_outputSettings = new OutputSettings(*outputSettings);
	_equalizerOutput = new std::vector<PlaybackFrame>();
}

OutputModelUI::OutputModelUI(const OutputModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_outputSettings = new OutputSettings(*copy.GetOutputSettings());
	_equalizerOutput = new std::vector<PlaybackFrame>();
}

OutputModelUI::~OutputModelUI()
{
	delete _name;
	delete _outputSettings;
	delete _equalizerOutput;
}

OutputSettings* OutputModelUI::GetOutputSettings() const
{
	return _outputSettings;
}

std::vector<PlaybackFrame>* OutputModelUI::GetEqualizerOutput() const
{
	return _equalizerOutput;
}

std::string OutputModelUI::GetName() const
{
	return *_name;
}

int OutputModelUI::GetOrder() const
{
	return 0;
}

void OutputModelUI::FromUI(OutputSettings* destination)
{
	
}
void OutputModelUI::ToUI(const OutputSettings* source)
{
	if (_equalizerOutput->size() != source->GetEqualizer()->GetEQLength())
	{
		_equalizerOutput->clear();
		for (int index = 0; index < source->GetEqualizer()->GetEQLength(); index++)
		{
			_equalizerOutput->push_back(PlaybackFrame(0, 0, 0));
		}
	}

	// Does not apply equalizer output
	_outputSettings->UpdateAllForUI(
		source->GetStreamTime(),
		source->GetAvgAudioMilli(),
		source->GetAvgAudioSampleMicro(),
		source->GetAvgAudioLockAcquireNano(),
		source->GetStreamLatency(),
		source->GetAvgUIMilli(),
		source->GetAvgUIDataFetchMicro(),
		source->GetAvgUILockAqcuireNano(),
		source->GetAvgUIRenderingMilli(),
		source->GetAvgUISleepMilli());

	// Equalizer Update
	source->GetEqualizer()->GetEQ(_equalizerOutput);
}

#endif