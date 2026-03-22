#pragma once

#ifndef SIGNAL_CHAIN_H
#define SIGNAL_CHAIN_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalChainSettings.h"
#include "SignalParameterizedBase.h"
#include "SoundRegistry.h"
#include <vector>

/// <summary>
/// Container class for a chain of SignalBase* instances
/// </summary>
class SignalChain
{
public:

	SignalChain();
	SignalChain(const SignalChain& copy);
	~SignalChain();

	void Initialize(const SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings, const PlaybackInfo* parameters);

	void Update(SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings);

	void SetFrame(PlaybackFrame* frame, const PlaybackTime* playbackTime);
	bool HasOutput(const PlaybackTime* playbackTime) const;

	void Engage(const PlaybackTime* playbackTime);
	void DisEngage(const PlaybackTime* playbackTime);

protected:

	//SHARED POINTERS!  These effects are held by the SoundRegistry*
	std::vector<SignalParameterizedBase*>* GetChain() const { return _chain; }

private:

	// SHARED POINTERS!  These effects are not created here! They are created and stored by the SoundRegistry*
	std::vector<SignalParameterizedBase*>* _chain;
};

#endif