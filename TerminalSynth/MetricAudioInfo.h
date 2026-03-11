#pragma once

#ifndef METRIC_AUDIO_INFO_H
#define METRIC_AUDIO_INFO_H

/// <summary>
/// Real time metrics for audio stream playback
/// </summary>
struct MetricAudioInfo
{
public:

	MetricAudioInfo() 
	{
		this->avgAudioMilli = 0;
		this->avgAudioSampleMicro = 0;
		this->avgAudioLockAcquireNano = 0;
		this->streamLatencySeconds = 0;
	}
	MetricAudioInfo(const MetricAudioInfo& copy)
	{
		this->avgAudioMilli = copy.avgAudioMilli;
		this->avgAudioSampleMicro = copy.avgAudioSampleMicro;
		this->avgAudioLockAcquireNano = copy.avgAudioLockAcquireNano;
		this->streamLatencySeconds = copy.streamLatencySeconds;
	}

	float avgAudioMilli;
	float avgAudioSampleMicro;
	float avgAudioLockAcquireNano;
	float streamLatencySeconds;
};

#endif