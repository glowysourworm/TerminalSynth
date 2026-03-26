#pragma once

#ifndef PLAYBACK_DEVICE_H
#define PLAYBACK_DEVICE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SynthSettings.h"

class PlaybackDevice
{

public:

	PlaybackDevice();
	~PlaybackDevice();

	/// <summary>
	/// Initialization of playback device is required before starting RT Audio playback.
	/// </summary>
	/// <returns>Returns true if device is ready, otherwise false for some sort of error</returns>
	virtual bool Initialize(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters) = 0;

	/// <summary>
	/// Update of playback device with current settings
	/// </summary>
	/// <returns>Returns true if device is ready, otherwise false for some sort of error</returns>
	virtual bool Update(SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters) = 0;

	/// <summary>
	/// Sets the playback device for this stream time prior to writing playback buffer. Returns
	/// true if the setup was successful; and that there is anything to play this frame.
	/// </summary>
	/// <param name="configuration">Configuration for the synth device</param>
	/// <returns>True if there are samples to process. Otherwise, there should be silence for the next playback period</returns>
	virtual bool SetForFrame(const PlaybackTime& playbackTime, const SynthSettings* configuration) = 0;

	/// <summary>
	/// Tells the playback device to write one sample to the playback frame. Returns true if the write was successful. Uses up to
	/// date parameters supplied by the PlaybackController*
	/// </summary>
	virtual bool WriteSample(PlaybackFrame& playbackFrame, const PlaybackTime& playbackTime, float gain, float leftRightBalance) = 0;
};


PlaybackDevice::PlaybackDevice()
{
}

PlaybackDevice::~PlaybackDevice()
{
}

#endif