#pragma once

#ifndef PLAYBACK_PARAMETERS_H
#define PLAYBACK_PARAMETERS_H

#include <string>

class OutputSettings
{
public:

	OutputSettings(
		const char* hostApi, 
		const char* deviceFormat, 
		const char* deviceName, 
		unsigned int samplingRate, 
		unsigned int numberOfChannels, 
		unsigned int outputBufferFrameSize)
	{
		_hostApi = new std::string(hostApi);
		_deviceFormat = new std::string();
		_deviceName = new std::string(deviceName);
		_samplingRate = samplingRate;
		_numberOfChannels = numberOfChannels;
		_outputBufferFrameSize = outputBufferFrameSize;

		// RT Update (Audio)
		_streamTime = 0;
		_avgAudioMilli = 0;
		_avgAudioSampleMicro = 0;
		_avgAudioLockAcquireNano = 0;
		_streamLatency = 0;
		_leftChannel = 0;
		_rightChannel = 0;

		// RT Update (UI)
		_avgUIMilli = 0;
		_avgUIDataFetchMicro = 0;
		_avgUILockAcqcuireNano = 0;
		_avgUIRenderingMilli = 0;
		_avgUISleepMilli = 0;

		// Output
		_gain = 1.0f;
		_leftRightBalance = 0.5f;
	}
	OutputSettings(const OutputSettings& copy)
	{
		_hostApi = new std::string(copy.GetHostApi());
		_deviceFormat = new std::string(copy.GetDeviceFormat());
		_deviceName = new std::string(copy.GetDeviceName());
		_samplingRate = copy.GetSamplingRate();
		_numberOfChannels = copy.GetNumberOfChannels();
		_outputBufferFrameSize = copy.GetOutputBufferFrameSize();

		// RT Update (Audio)
		_streamTime = copy.GetStreamTime();
		_avgAudioMilli = copy.GetAvgAudioMilli();
		_avgAudioSampleMicro = copy.GetAvgAudioSampleMicro();
		_avgAudioLockAcquireNano = copy.GetAvgAudioLockAcquireNano();
		_streamLatency = copy.GetStreamLatency();
		_leftChannel = copy.GetLeftChannel();
		_rightChannel = copy.GetRightChannel();

		// RT Update (UI)
		_avgUIMilli = copy.GetAvgUIMilli();
		_avgUIDataFetchMicro = copy.GetAvgUIDataFetchMicro();
		_avgUILockAcqcuireNano = copy.GetAvgUILockAqcuireNano();
		_avgUIRenderingMilli = copy.GetAvgUIRenderingMilli();
		_avgUISleepMilli = copy.GetAvgUISleepMilli();

		// Output
		_gain = copy.GetGain();
		_leftRightBalance = copy.GetLeftRightBalance();
	}
	~OutputSettings()
	{
		delete _hostApi;
		delete _deviceFormat;
		delete _deviceName;
	}

	std::string& GetHostApi() const { return *_hostApi; }
	std::string& GetDeviceFormat() const { return *_deviceFormat; }
	std::string& GetDeviceName() const { return *_deviceName; }

	unsigned int GetSamplingRate() const { return _samplingRate; }
	unsigned int GetNumberOfChannels() const { return _numberOfChannels; }
	unsigned int GetOutputBufferFrameSize() const { return _outputBufferFrameSize; }

	float GetStreamTime() const { return _streamTime; }
	float GetAvgAudioMilli() const { return _avgAudioMilli; }
	float GetAvgAudioSampleMicro() const { return _avgAudioSampleMicro; }
	float GetAvgAudioLockAcquireNano() const { return _avgAudioLockAcquireNano; }
	float GetStreamLatency() const { return _streamLatency; }

	float GetAvgUIMilli() const { return _avgUIMilli; }
	float GetAvgUIDataFetchMicro() const { return _avgUIDataFetchMicro; }
	float GetAvgUILockAqcuireNano() const { return _avgUILockAcqcuireNano; }
	float GetAvgUIRenderingMilli() const { return _avgUIRenderingMilli; }
	float GetAvgUISleepMilli() const { return _avgUISleepMilli; }

	float GetGain() const { return _gain; }
	float GetLeftChannel() const { return _leftChannel; }
	float GetRightChannel() const { return _rightChannel; }
	float GetLeftRightBalance() const { return _leftRightBalance; }

	void SetGain(float value) { _gain = value; }
	void SetLeftRightBalance(float value) { _leftRightBalance = value; }

	void UpdateDevice(const std::string& hostApi, 
					  const std::string& deviceFormat, 
					  const std::string& deviceName, 
					  unsigned int samplingRate, 
					  unsigned int numberChannels, 
					  unsigned int bufferFrameSize)
	{
		_hostApi = new std::string(hostApi);
		_deviceFormat = new std::string(deviceFormat);
		_deviceName = new std::string(deviceName);
		_samplingRate = samplingRate;
		_numberOfChannels = numberChannels;
		_outputBufferFrameSize = bufferFrameSize;
	}

	/// <summary>
	/// This should get its updates from the audio controller during its lock
	/// </summary>
	void UpdateRT_Audio(float streamTime,
						float avgAudioMilli,
						float avgAudioSampleMicro,
						float avgAudioLockAcquireNano,
						long latency,
						float leftChannel,
						float rightChannel)
	{
		_streamTime = streamTime;
		_avgAudioMilli = avgAudioMilli;
		_avgAudioSampleMicro = avgAudioSampleMicro;
		_avgAudioLockAcquireNano = avgAudioLockAcquireNano;
		_streamLatency = latency;
		_leftChannel = leftChannel;
		_rightChannel = rightChannel;
	}

	/// <summary>
	/// This should get its updates from the main controller
	/// </summary>
	void UpdateRT_UI(float avgUIMilli,
					 float avgUIDataFetchMicro,
					 float avgUILockAcqcuireNano,
					 float avgUIRenderingMilli,
					 float avgUISleepMilli)
	{
		_avgUIMilli = avgUIMilli;
		_avgUIDataFetchMicro = avgUIDataFetchMicro;
		_avgUILockAcqcuireNano = avgUILockAcqcuireNano;
		_avgUIRenderingMilli = avgUIRenderingMilli;
		_avgUISleepMilli = avgUISleepMilli;
	}

private:

	// Device Settings
	std::string* _hostApi;
	std::string* _deviceFormat;
	std::string* _deviceName;
	unsigned int _samplingRate;
	unsigned int _numberOfChannels;
	unsigned int _outputBufferFrameSize;

	// RT Values (Audio)
	float _streamTime;
	float _avgAudioMilli;
	float _avgAudioSampleMicro;
	float _avgAudioLockAcquireNano;
	long _streamLatency;

	// RT Values (UI)
	float _avgUIMilli;
	float _avgUIDataFetchMicro;
	float _avgUILockAcqcuireNano;
	float _avgUIRenderingMilli;
	float _avgUISleepMilli;

	// Output Parameters
	float _gain;
	float _leftRightBalance;

	// Output
	float _leftChannel;
	float _rightChannel;
};

#endif