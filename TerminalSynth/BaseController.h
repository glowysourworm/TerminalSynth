#pragma once

#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H

#include "AtomicLock.h"
#include "PlaybackUserData.h"

/// <summary>
/// Simple base class to enforce Initialize / Dispose pattern on the controllers. The Synth, SynthPlaybackDevice, and
/// the signal chain all depend on real time updates. These two pointers have to be shared appropriately in order to 
/// avoid context switching between threads.
/// </summary>
class BaseController
{
public:

	BaseController(AtomicLock* playbackLock)
	{
		this->PlaybackLock = playbackLock;
	};
	~BaseController() {};

	/// <summary>
	/// Initialization function for the synth backend. This must be called before starting the player!
	/// </summary>
	virtual bool Initialize(PlaybackUserData* playbackData) = 0;

	/// <summary>
	/// Starts any threads associated with the controller, after initialization.
	/// </summary>
	virtual void Start() = 0;

	/// <summary>
	/// Disposes of backend, and controller resources
	/// </summary>
	virtual bool Dispose() = 0;

protected:

	/// <summary>
	/// Lock required for the playback thread since we do not have dispatch control over
	/// it, we have to force synchronization to wait on the SynthSettings* shared pointer.
	/// </summary>
	AtomicLock* PlaybackLock;
};

#endif