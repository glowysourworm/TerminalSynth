#pragma once

#ifndef PLAYBACK_DEVICE_REGISTER_H
#define PLAYBACK_DEVICE_REGISTER_H

#include "Constant.h"
#include "OutputDeviceInfo.h"
#include <exception>
#include <string>
#include <vector>

class PlaybackDeviceRegister
{
public:

	PlaybackDeviceRegister()
	{
		_devices = new std::vector<OutputDeviceInfo*>();
	}
	~PlaybackDeviceRegister()
	{
		for (int index = 0; index < _devices->size(); index++)
		{
			delete _devices->at(index);
		}

		delete _devices;
	}

	unsigned int GetSamplingRate() const { return _selectedDevice->GetSamplingRate(); }
	unsigned int GetNumberOfChannels() const { return _selectedDevice->GetNumberOfChannels(); }
	unsigned int GetOutputBufferFrameSize() const { return _selectedDevice->GetBufferFrameSize(); }
	std::string GetDeviceName() const { return _selectedDevice->GetDeviceName(); }
	std::string GetDeviceFormatString() const { return _selectedDevice->GetDeviceFormatString(); }
	AudioStreamFormat GetDeviceFormat() const { return _selectedDevice->GetDeviceFormat(); }
	float GetSuggestedLatencySeconds() const { return _selectedDevice->GetSuggestedLatencySeconds(); }

	OutputDeviceInfo* GetSelectedDevice() const { return _selectedDevice; }

	void AddDevice(unsigned int deviceId,
		const std::string& deviceFormatString,
		const std::string& deviceFormatParagraph,
		const std::string& deviceName,
		AudioStreamFormat deviceFormat,
		unsigned int samplingRate,
		unsigned int numberChannels,
		unsigned int bufferFrameSize,
		float suggestedLatency,
		bool selectDevice)
	{
		// MEMORY! ~PlaybackInfo
		OutputDeviceInfo* info = new OutputDeviceInfo(deviceId, deviceName, 
														deviceFormatString, 
														deviceFormatParagraph, 
														samplingRate, numberChannels, 
														bufferFrameSize, deviceFormat,
														suggestedLatency);

		for (int index = 0; index < _devices->size(); index++)
		{
			if (*_devices->at(index) == *info)
				throw new std::exception("Output device already exists:  PlaybackDeviceRegister.h");
		}

		_devices->push_back(info);

		if (selectDevice)
			_selectedDevice = info;
	}

	bool ContainsDevice(const std::string& deviceName)
	{
		for (int index = 0; index < _devices->size(); index++)
		{
			if (_devices->at(index)->GetDeviceName() == deviceName)
				return true;
		}
		return false;
	}

	void SelectDevice(const std::string& deviceName)
	{
		for (int index = 0; index < _devices->size(); index++)
		{
			if (_devices->at(index)->GetDeviceName() == deviceName)
			{
				_selectedDevice = _devices->at(index);
				return;
			}
		}

		throw new std::exception("Output device not found:  PlaybackDeviceRegister.h");
	}

	void UpdateDevice(const std::string& deviceName,
						unsigned int currentSamplingRate,
						unsigned int outputBufferSize,
						bool selectDevice)
	{
		for (int index = 0; index < _devices->size(); index++)
		{
			if (_devices->at(index)->GetDeviceName() == deviceName)
			{
				_devices->at(index)->SetSamplingRate(currentSamplingRate);
				_devices->at(index)->SetBufferFrameSize(outputBufferSize);

				if (selectDevice)
				{
					_selectedDevice = _devices->at(index);
				}
				return;
			}
		}

		throw new std::exception("Output device not found:  PlaybackDeviceRegister.h");
	}

	void GetDeviceList(std::vector<std::string>& destination) const
	{
		for (int index = 0; index < _devices->size(); index++)
		{
			destination.push_back(_devices->at(index)->GetDeviceName());
		}
	}

private:

	OutputDeviceInfo* _selectedDevice;
	std::vector<OutputDeviceInfo*>* _devices;
};

#endif