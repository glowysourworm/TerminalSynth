#pragma once

#include "Constant.h"
#include <string>

class OutputDeviceInfo
{
public:

	OutputDeviceInfo(int id,
					const std::string& deviceName,
					const std::string& deviceFormatString,
					const std::string& deviceFormatParagraph,
					unsigned int samplingRate,
					unsigned int numberChannels,
					unsigned int bufferFrameSize,
					AudioStreamFormat deviceFormat,
					float suggestedLatencySeconds)
	{
		_id = id;
		_deviceFormat = deviceFormat;
		_deviceName = new std::string(deviceName);
		_deviceFormatString = new std::string(deviceFormatString);
		_deviceFormatParagraph = new std::string(deviceFormatParagraph);
		_samplingRate = samplingRate;
		_numberOfChannels = numberChannels;
		_bufferFrameSize = bufferFrameSize;
		_suggestedLatencySeconds = suggestedLatencySeconds;
	}
	OutputDeviceInfo(const OutputDeviceInfo& copy)
	{
		_id = copy.GetId();
		_deviceName = new std::string(copy.GetDeviceName());
		_deviceFormat = copy.GetDeviceFormat();
		_deviceFormatString = new std::string(copy.GetDeviceFormatString());
		_deviceFormatParagraph = new std::string(copy.GetDeviceFormatParagraph());
		_samplingRate = copy.GetSamplingRate();
		_numberOfChannels = copy.GetNumberOfChannels();
		_bufferFrameSize = copy.GetBufferFrameSize();
		_suggestedLatencySeconds = copy.GetSuggestedLatencySeconds();
	}
	~OutputDeviceInfo()
	{
		delete _deviceName;
		delete _deviceFormatString;
		delete _deviceFormatParagraph;
	}

	int GetId() const { return _id; }
	AudioStreamFormat GetDeviceFormat() const { return _deviceFormat; }
	std::string GetDeviceFormatString() const { return *_deviceFormatString; }
	std::string GetDeviceFormatParagraph() const { return *_deviceFormatParagraph; }
	std::string GetDeviceName() const { return *_deviceName; }
	unsigned int GetSamplingRate() const { return _samplingRate; }
	unsigned int GetNumberOfChannels() const { return _numberOfChannels; }
	unsigned int GetBufferFrameSize() const { return _bufferFrameSize; }
	float GetSuggestedLatencySeconds() const { return _suggestedLatencySeconds; }

	// These were needed for RT Audio initialization
	void SetSamplingRate(unsigned int value) { _samplingRate = value; }
	void SetBufferFrameSize(unsigned int value) { _bufferFrameSize = value; }

	bool operator==(const OutputDeviceInfo& info) { return IsEqual(info); }
	bool operator!=(const OutputDeviceInfo& info) { return !IsEqual(info); }

private:

	bool IsEqual(const OutputDeviceInfo& info)
	{
		return this->GetId() == info.GetId() &&
			this->GetDeviceName() == info.GetDeviceName() &&
			this->GetDeviceFormat() == info.GetDeviceFormat() &&
			this->GetDeviceFormatParagraph() == info.GetDeviceFormatParagraph() &&
			this->GetDeviceFormat() == info.GetDeviceFormat() &&
			this->GetSamplingRate() == info.GetSamplingRate() &&
			this->GetNumberOfChannels() == info.GetNumberOfChannels() &&
			this->GetBufferFrameSize() == info.GetBufferFrameSize() &&
			this->GetSuggestedLatencySeconds() == info.GetSuggestedLatencySeconds();
	}

private:

	int _id;
	std::string* _deviceFormatString;
	std::string* _deviceFormatParagraph;
	std::string* _deviceName;
	AudioStreamFormat _deviceFormat;
	unsigned int _samplingRate;
	unsigned int _numberOfChannels;
	unsigned int _bufferFrameSize;
	float _suggestedLatencySeconds;
};