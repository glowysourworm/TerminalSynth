#pragma once

#include <string>

class OutputDeviceInfo
{
public:

	OutputDeviceInfo(unsigned int id,
					const std::string& deviceName,
					const std::string& deviceFormat,
					const std::string& deviceFormatParagraph,
					unsigned int samplingRate,
					unsigned int numberChannels,
					unsigned int bufferFrameSize)
	{
		_id = id;
		_deviceName = new std::string(deviceName);
		_deviceFormat = new std::string(deviceFormat);
		_deviceFormatParagraph = new std::string(deviceFormatParagraph);
		_samplingRate = samplingRate;
		_numberOfChannels = numberChannels;
		_bufferFrameSize = bufferFrameSize;
	}
	OutputDeviceInfo(const OutputDeviceInfo& copy)
	{
		_id = copy.GetId();
		_deviceName = new std::string(copy.GetDeviceName());
		_deviceFormat = new std::string(copy.GetDeviceFormat());
		_deviceFormatParagraph = new std::string(copy.GetDeviceFormatParagraph());
		_samplingRate = copy.GetSamplingRate();
		_numberOfChannels = copy.GetNumberOfChannels();
		_bufferFrameSize = copy.GetBufferFrameSize();
	}
	~OutputDeviceInfo()
	{
		delete _deviceName;
		delete _deviceFormat;
		delete _deviceFormatParagraph;
	}

	unsigned int GetId() const { return _id; }
	std::string GetDeviceFormat() const { return *_deviceFormat; }
	std::string GetDeviceFormatParagraph() const { return *_deviceFormatParagraph; }
	std::string GetDeviceName() const { return *_deviceName; }
	unsigned int GetSamplingRate() const { return _samplingRate; }
	unsigned int GetNumberOfChannels() const { return _numberOfChannels; }
	unsigned int GetBufferFrameSize() const { return _bufferFrameSize; }

	void Update(unsigned int samplingRate, unsigned int bufferFrameSize)
	{
		_samplingRate = samplingRate;
		_bufferFrameSize = bufferFrameSize;
	}

	bool operator==(const OutputDeviceInfo& info) { return IsEqual(info); }
	bool operator!=(const OutputDeviceInfo& info) { return !IsEqual(info); }

private:

	bool IsEqual(const OutputDeviceInfo& info)
	{
		return this->GetId() == info.GetId() &&
			this->GetDeviceName() == info.GetDeviceName() &&
			this->GetDeviceFormat() == info.GetDeviceFormat() &&
			this->GetDeviceFormatParagraph() == info.GetDeviceFormatParagraph() &&
			this->GetSamplingRate() == info.GetSamplingRate() &&
			this->GetNumberOfChannels() == info.GetNumberOfChannels() &&
			this->GetBufferFrameSize() == info.GetBufferFrameSize();
	}

private:

	unsigned int _id;
	std::string* _deviceFormat;
	std::string* _deviceFormatParagraph;
	std::string* _deviceName;
	unsigned int _samplingRate;
	unsigned int _numberOfChannels;
	unsigned int _bufferFrameSize;
};