#pragma once

#ifndef PLAYBACK_PARAMETERS_H
#define PLAYBACK_PARAMETERS_H

#include "EqualizerOutput.h"
#include "OutputDeviceInfo.h"
#include "PlaybackFrame.h"
#include <exception>
#include <string>
#include <vector>

class OutputSettings
{
private:

	const int FFT_INPUT_SIZE = 1024;
	const int FFT_OUTPUT_SIZE = 32;

public:

	OutputSettings()
	{
		_hostApi = new std::string("");		
		_devices = new std::vector<OutputDeviceInfo*>();

		// Initialize!
		_selectedDevice = nullptr;
		_equalizer = nullptr;

		// RT Update (Audio)
		_streamTime = 0;
		_avgAudioMilli = 0;
		_avgAudioSampleMicro = 0;
		_avgAudioLockAcquireNano = 0;
		_streamLatency = 0;

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
		_devices = new std::vector<OutputDeviceInfo*>();

		for (int index = 0; index < copy.GetDeviceList()->size(); index++)
		{
			_devices->push_back(new OutputDeviceInfo(*copy.GetDeviceList()->at(index)));
		}

		// Initialize!
		_selectedDevice = copy.GetSelectedDevice() == nullptr ? nullptr : new OutputDeviceInfo(*copy.GetSelectedDevice());
		_equalizer = copy.GetEqualizer() == nullptr ? nullptr : new EqualizerOutput(*copy.GetEqualizer());

		// RT Update (Audio)
		_streamTime = copy.GetStreamTime();
		_avgAudioMilli = copy.GetAvgAudioMilli();
		_avgAudioSampleMicro = copy.GetAvgAudioSampleMicro();
		_avgAudioLockAcquireNano = copy.GetAvgAudioLockAcquireNano();
		_streamLatency = copy.GetStreamLatency();

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

		for (int index = 0; index < _devices->size(); index++)
		{
			delete _devices->at(index);
		}

		delete _devices;

		if (_selectedDevice != nullptr)
			delete _selectedDevice;

		if (_equalizer != nullptr)
			delete _equalizer;
	}

	std::string GetHostApi() const { return *_hostApi; }
	std::vector<OutputDeviceInfo*>* GetDeviceList() const { return _devices; }
	OutputDeviceInfo* GetSelectedDevice() const { return _selectedDevice; }

	unsigned int GetSamplingRate() const { return _selectedDevice->GetSamplingRate(); }
	unsigned int GetNumberOfChannels() const { return _selectedDevice->GetNumberOfChannels(); }
	unsigned int GetOutputBufferFrameSize() const { return _selectedDevice->GetBufferFrameSize(); }
	std::string GetDeviceName() const { return _selectedDevice->GetDeviceName(); }
	std::string GetDeviceFormat() const { return _selectedDevice->GetDeviceFormat(); }

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
	float GetLeftRightBalance() const { return _leftRightBalance; }

	EqualizerOutput* GetEqualizer() const { return _equalizer; }

	void SetHostApi(const std::string& hostApi) 
	{ 
		_hostApi->clear();
		_hostApi->append(hostApi);
	}
	void SetStreamLatency(float value) { _streamLatency = value; }
	void SetGain(float value) { _gain = value; }
	void SetLeftRightBalance(float value) { _leftRightBalance = value; }

	void AddDevice(unsigned int deviceId,
				   const std::string& deviceFormat,
				   const std::string& deviceFormatParagraph,
				   const std::string& deviceName,
				   unsigned int samplingRate,
				   unsigned int numberChannels,
				   unsigned int bufferFrameSize,
				   bool selectDevice)
	{
		// MEMORY! ~OutputSettings
		OutputDeviceInfo* info = new OutputDeviceInfo(deviceId, deviceName, deviceFormat, deviceFormatParagraph, samplingRate, numberChannels, bufferFrameSize);

		for (int index = 0; index < _devices->size(); index++)
		{
			if (*_devices->at(index) == *info)
				throw new std::exception("Output device already exists:  OutputSettings.h");
		}

		_devices->push_back(info);
		
		if (selectDevice)
		{
			if (_equalizer != nullptr)
				delete _equalizer;

			_selectedDevice = info;
			_equalizer = new EqualizerOutput(FFT_INPUT_SIZE, FFT_OUTPUT_SIZE, info->GetSamplingRate());
		}
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
				if (_equalizer != nullptr)
					delete _equalizer;

				_selectedDevice = _devices->at(index);
				_equalizer = new EqualizerOutput(FFT_INPUT_SIZE, FFT_OUTPUT_SIZE, _selectedDevice->GetSamplingRate());
				return;
			}
		}

		throw new std::exception("Output device not found:  OutputSettings.h");
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
				_devices->at(index)->Update(currentSamplingRate, outputBufferSize);

				if (selectDevice)
				{
					if (_equalizer != nullptr)
						delete _equalizer;

					_selectedDevice = _devices->at(index);
					_equalizer = new EqualizerOutput(FFT_INPUT_SIZE, FFT_OUTPUT_SIZE, _selectedDevice->GetSamplingRate());
				}
				return;
			}				
		}

		throw new std::exception("Output device not found:  OutputSettings.h");
	}

	/// <summary>
	/// This should get its updates from the audio controller during its lock
	/// </summary>
	void UpdateRT_Audio(float streamTime,
						float avgAudioMilli,
						float avgAudioSampleMicro,
						float avgAudioLockAcquireNano,
						long latency)
	{
		_streamTime = streamTime;
		_avgAudioMilli = avgAudioMilli;
		_avgAudioSampleMicro = avgAudioSampleMicro;
		_avgAudioLockAcquireNano = avgAudioLockAcquireNano;
		_streamLatency = latency;
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

	/// <summary>
	/// This function does not apply the next sample to the equalizer; and will forcedly set
	/// all variables. This should be used for straight copying without memory allocation.
	/// </summary>
	void UpdateAllForUI(float streamTime,
		float avgAudioMilli,
		float avgAudioSampleMicro,
		float avgAudioLockAcquireNano,
		long latency,
		float avgUIMilli,
		float avgUIDataFetchMicro,
		float avgUILockAcqcuireNano,
		float avgUIRenderingMilli,
		float avgUISleepMilli)
	{
		_streamTime = streamTime;
		_avgAudioMilli = avgAudioMilli;
		_avgAudioSampleMicro = avgAudioSampleMicro;
		_avgAudioLockAcquireNano = avgAudioLockAcquireNano;
		_streamLatency = latency;

		_avgUIMilli = avgUIMilli;
		_avgUIDataFetchMicro = avgUIDataFetchMicro;
		_avgUILockAcqcuireNano = avgUILockAcqcuireNano;
		_avgUIRenderingMilli = avgUIRenderingMilli;
		_avgUISleepMilli = avgUISleepMilli;
	}

private:

	// Device Settings
	std::string* _hostApi;
	OutputDeviceInfo* _selectedDevice;
	std::vector<OutputDeviceInfo*>* _devices;

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
	EqualizerOutput* _equalizer;
};

#endif