#pragma once

#ifndef SIGNAL_CHAIN_H
#define SIGNAL_CHAIN_H

#include "OutputSettings.h"
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

	void Initialize(const SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings, const OutputSettings* parameters);
	void Update(const SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings);
	void SetFrame(PlaybackFrame* frame, float absoluteTime);
	bool HasOutput(float absoluteTime) const;

private:

	// SHARED POINTERS!  These effects are not created here! They are created and stored by the SoundRegistry*
	std::vector<SignalBase*>* _chain;
};

#endif