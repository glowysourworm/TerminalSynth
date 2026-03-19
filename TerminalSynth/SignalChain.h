#pragma once

#ifndef SIGNAL_CHAIN_H
#define SIGNAL_CHAIN_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
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
	~SignalChain();

	void Initialize(const SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings, const PlaybackInfo* parameters);

	void Update(SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings);

	void SetFrame(PlaybackFrame* frame);
	bool HasOutput(double absoluteTime) const;

	void Engage(double absoluteTime);
	void DisEngage(double absoluteTime);

private:

	// SHARED POINTERS!  These effects are not created here! They are created and stored by the SoundRegistry*
	std::vector<SignalParameterizedBase*>* _chain;
};

#endif