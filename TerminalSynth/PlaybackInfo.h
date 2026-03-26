#pragma once

#ifndef PLAYBACK_PARAMETERS_H
#define PLAYBACK_PARAMETERS_H

#include "Constant.h"
#include "MetricAudioInfo.h"
#include "MetricUIInfo.h"
#include "StreamInfo.h"
#include <string>

class PlaybackInfo
{
public:

	PlaybackInfo(bool stkEnabaled, bool soundBankEnabled)
	{
		_hostApi = new std::string("");
		_audioMetrics = new MetricAudioInfo();
		_uiMetrics = new MetricUIInfo();
		_streamInfo = new StreamInfo();
		_stkEnabled = stkEnabaled;
		_soundBankEnabled = soundBankEnabled;
	}
	PlaybackInfo(const PlaybackInfo& copy)
	{
		_hostApi = new std::string(copy.GetHostApi());
		_audioMetrics = new MetricAudioInfo(*copy.GetAudioMetrics());
		_uiMetrics = new MetricUIInfo(*copy.GetUIMetrics());
		_streamInfo = new StreamInfo(*copy.GetStreamInfo());
		_stkEnabled = copy.GetStkEnabled();
		_soundBankEnabled = copy.GetSoundBankEnabled();
	}
	~PlaybackInfo()
	{
		delete _hostApi;
		delete _audioMetrics;
		delete _uiMetrics;
		delete _streamInfo;
	}

	std::string GetHostApi() const { return *_hostApi; }
	MetricUIInfo* GetUIMetrics() const { return _uiMetrics; }
	MetricAudioInfo* GetAudioMetrics() const { return _audioMetrics; }
	StreamInfo* GetStreamInfo() const { return _streamInfo; }

	bool GetStkEnabled() const { return _stkEnabled; }
	bool GetSoundBankEnabled() const { return _soundBankEnabled; }

	void SetForHostApi(const std::string& hostApi) 
	{ 
		_hostApi->clear();
		_hostApi->append(hostApi);
	}
	void SetForOutputDevice(AudioStreamFormat deviceFormat, unsigned int numberChannels, unsigned int samplingRate, float suggestedLatencySeconds)
	{
		_streamInfo->streamBitFormat = deviceFormat;
		_streamInfo->streamChannels = numberChannels;
		_streamInfo->streamSampleRate = samplingRate;
		_streamInfo->streamSuggestedLatency = suggestedLatencySeconds;
	}
	void UpdateStreamStatus(bool streamOpen, bool streamRunning)
	{
		_streamInfo->streamOpenStatus = streamOpen;
		_streamInfo->streamRunningStatus = streamRunning;
	}

	/// <summary>
	/// This should get its updates from the audio controller during its lock
	/// </summary>
	void UpdateRT_Audio(float streamTime,
						float avgAudioMilli,
						float avgAudioSampleMicro,
						float avgAudioLockAcquireNano,
						float streamLatencySeconds)
	{
		_audioMetrics->avgAudioMilli = avgAudioMilli;
		_audioMetrics->avgAudioSampleMicro = avgAudioSampleMicro;
		_audioMetrics->avgAudioLockAcquireNano = avgAudioLockAcquireNano;
		_audioMetrics->streamLatencySeconds = streamLatencySeconds;
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
		_uiMetrics->avgUIMilli = avgUIMilli;
		_uiMetrics->avgUIDataFetchMicro = avgUIDataFetchMicro;
		_uiMetrics->avgUILockAcqcuireNano = avgUILockAcqcuireNano;
		_uiMetrics->avgUIRenderingMilli = avgUIRenderingMilli;
		_uiMetrics->avgUISleepMilli = avgUISleepMilli;
	}

	void Update(const PlaybackInfo* source)
	{
		_hostApi->clear();
		_hostApi->append(source->GetHostApi());

		_streamInfo->streamBitFormat = source->GetStreamInfo()->streamBitFormat;
		_streamInfo->streamChannels = source->GetStreamInfo()->streamChannels;
		_streamInfo->streamOpenStatus = source->GetStreamInfo()->streamOpenStatus;
		_streamInfo->streamRunningStatus = source->GetStreamInfo()->streamRunningStatus;
		_streamInfo->streamSampleRate = source->GetStreamInfo()->streamSampleRate;
		_streamInfo->streamSuggestedLatency = source->GetStreamInfo()->streamSuggestedLatency;

		_audioMetrics->avgAudioMilli = source->GetAudioMetrics()->avgAudioMilli;
		_audioMetrics->avgAudioMilli = source->GetAudioMetrics()->avgAudioMilli;
		_audioMetrics->avgAudioSampleMicro = source->GetAudioMetrics()->avgAudioSampleMicro;
		_audioMetrics->avgAudioLockAcquireNano = source->GetAudioMetrics()->avgAudioLockAcquireNano;
		_audioMetrics->streamLatencySeconds = source->GetAudioMetrics()->streamLatencySeconds;

		_uiMetrics->avgUIMilli = source->GetUIMetrics()->avgUIMilli;
		_uiMetrics->avgUIDataFetchMicro = source->GetUIMetrics()->avgUIDataFetchMicro;
		_uiMetrics->avgUILockAcqcuireNano = source->GetUIMetrics()->avgUILockAcqcuireNano;
		_uiMetrics->avgUIRenderingMilli = source->GetUIMetrics()->avgUIRenderingMilli;
		_uiMetrics->avgUISleepMilli = source->GetUIMetrics()->avgUISleepMilli;
	}

private:

	// Host API
	std::string* _hostApi;

	// Stream Info / Status
	StreamInfo* _streamInfo;

	// RT Metrics
	MetricAudioInfo* _audioMetrics;
	MetricUIInfo* _uiMetrics;

	// Features
	bool _stkEnabled;
	bool _soundBankEnabled;
};

#endif