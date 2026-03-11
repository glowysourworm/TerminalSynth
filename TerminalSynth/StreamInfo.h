#pragma once

#ifndef STREAM_INFO_H
#define STREAM_INFO_H

#include "Constant.h"

/// <summary>
/// Real time metrics for UI handling
/// </summary>
struct StreamInfo
{
public:

	StreamInfo()
	{
		this->streamSampleRate = 0;
		this->streamChannels = 0;
		this->streamBitFormat = AudioStreamFormat::Float32;
		this->streamSuggestedLatency = 0;
		this->streamActualLatency = 0;
		this->streamOpenStatus = false;
		this->streamRunningStatus = false;
	}
	StreamInfo(const StreamInfo& copy)
	{
		this->streamSampleRate = copy.streamSampleRate;
		this->streamChannels = copy.streamChannels;
		this->streamBitFormat = copy.streamBitFormat;
		this->streamSuggestedLatency = copy.streamSuggestedLatency;
		this->streamActualLatency = copy.streamActualLatency;
		this->streamOpenStatus = copy.streamOpenStatus;
		this->streamRunningStatus = copy.streamRunningStatus;
	}

	float streamSampleRate;
	unsigned int streamChannels;
	AudioStreamFormat streamBitFormat;
	float streamSuggestedLatency;
	float streamActualLatency;
	bool streamOpenStatus;
	bool streamRunningStatus;	
};

#endif