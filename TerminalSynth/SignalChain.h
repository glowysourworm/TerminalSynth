#pragma once

#ifndef SIGNAL_CHAIN_H
#define SIGNAL_CHAIN_H

#include "PlaybackInfo.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalChainSettings.h"
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
	bool HasOutput() const;

	void Engage(double absoluteTime);
	void DisEngage(double absoluteTime);

private:

	// SHARED POINTERS!  These effects are not created here! They are created and stored by the SoundRegistry*
	std::vector<SignalBase*>* _chain;
};

#endif