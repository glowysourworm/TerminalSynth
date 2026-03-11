#pragma once

#ifndef OUTPUT_MODEL_UI_H
#define OUTPUT_MODEL_UI_H

#include "EqualizerOutput.h"
#include "ModelUI.h"
#include "OutputDeviceInfo.h"
#include "PlaybackDeviceRegister.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackUserData.h"
#include <string>
#include <vector>

class OutputModelUI : public ModelUI
{
public:

	OutputModelUI(const PlaybackUserData* playbackData);
	OutputModelUI(const OutputModelUI& copy);
	~OutputModelUI();

	std::string GetName() const override;
	int GetOrder() const override;

	void FromUI(std::string& selectedDeviceName);
	void ToUI(const PlaybackUserData* playbackData);

	PlaybackInfo* GetPlaybackInfo() const;
	std::vector<PlaybackFrame>* GetEqualizerOutput() const;

	float GetGain() const { return _gain; }
	float GetLeftRightBalance() const { return _leftRightBalance; }
	const OutputDeviceInfo* GetSelectedDevice() const { return _selectedDevice; }
	std::string GetSelectedDeviceName() const { return *_selectedDeviceName; }
	std::vector<std::string> GetDeviceNameList() const { return *_deviceNameList; }

	void SetGain(float value) { _gain = value; }
	void SetLeftRightBalance(float value) { _leftRightBalance = value; }
	void SetSelectedDeviceName(const std::string& deviceName);

private:

	std::string* _name;

	PlaybackInfo* _playbackInfo;

	float _gain;
	float _leftRightBalance;

	// DO NOT DELETE!
	const OutputDeviceInfo* _selectedDevice;

	std::string* _selectedDeviceName;
	
	std::vector<std::string>* _deviceNameList;
	std::vector<PlaybackFrame>* _equalizerOutput;
};

OutputModelUI::OutputModelUI(const PlaybackUserData* playbackData)
{
	_name = new std::string("Output");
	_playbackInfo = new PlaybackInfo(*playbackData->GetPlaybackInfo());
	_equalizerOutput = playbackData->GetEqualizer()->GetEQCopy();
	_gain = 1.0f;
	_leftRightBalance = 0.5f;

	_selectedDevice = playbackData->GetDeviceRegister()->GetSelectedDevice();
	_selectedDeviceName = new std::string(_selectedDevice->GetDeviceName());
	_deviceNameList = new std::vector<std::string>();

	playbackData->GetDeviceRegister()->GetDeviceList(*_deviceNameList);
}

OutputModelUI::OutputModelUI(const OutputModelUI& copy)
{
	_name = new std::string(copy.GetName());
	_playbackInfo = new PlaybackInfo(*copy.GetPlaybackInfo());
	_equalizerOutput = new std::vector<PlaybackFrame>(*copy.GetEqualizerOutput());
	_gain = copy.GetGain();
	_leftRightBalance = copy.GetLeftRightBalance();
	_selectedDevice = copy.GetSelectedDevice();
	_selectedDeviceName = new std::string(_selectedDevice->GetDeviceName());

	_deviceNameList = new std::vector<std::string>(copy.GetDeviceNameList());
}

OutputModelUI::~OutputModelUI()
{
	delete _name;
	delete _playbackInfo;
	delete _equalizerOutput;
	delete _deviceNameList;
	delete _selectedDeviceName;
}

PlaybackInfo* OutputModelUI::GetPlaybackInfo() const
{
	return _playbackInfo;
}

std::vector<PlaybackFrame>* OutputModelUI::GetEqualizerOutput() const
{
	return _equalizerOutput;
}

void OutputModelUI::SetSelectedDeviceName(const std::string& deviceName)
{
	_selectedDeviceName->clear();
	_selectedDeviceName->append(deviceName);
}

std::string OutputModelUI::GetName() const
{
	return *_name;
}

int OutputModelUI::GetOrder() const
{
	return 0;
}

void OutputModelUI::FromUI(std::string& selectedDeviceName)
{
	selectedDeviceName = *_selectedDeviceName;
}
void OutputModelUI::ToUI(const PlaybackUserData* playbackData)
{
	// Does not apply equalizer output
	_playbackInfo->Update(playbackData->GetPlaybackInfo());

	// Equalizer Update
	playbackData->GetEqualizer()->GetEQ(_equalizerOutput);
}

#endif