#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalBase.h"
#include "SignalChain.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include "SoundRegistry.h"
#include <vector>

SignalChain::SignalChain()
{
	_chain = new std::vector<SignalBase*>();
}

SignalChain::~SignalChain()
{
	// DO NOT DELETE: SignalBase* (these are handled by the SoundRegistry*)
	//

	delete _chain;
}

void SignalChain::Initialize(const SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings, const PlaybackInfo* parameters)
{
	// Add
	for (int index = 0; index < signalChainSettings->GetCount(); index++)
	{
		SignalSettings* settings = signalChainSettings->Get(index);

		// Not Enabled
		if (!settings->GetIsEnabled())
			continue;

		// Get an instance from the SoundRegistry* cache (DO NOT DELETE!)
		SignalBase* effect = effectRegistry->Checkout(settings->GetName());

		_chain->push_back(effect);
	}
}
void SignalChain::Update(SoundRegistry* effectRegistry, const SignalChainSettings* signalChainSettings)
{
	// Checkin (preserve memory cache)
	for (int index = _chain->size() - 1; index >= 0; index--)
	{
		effectRegistry->Checkin(_chain->at(index));

		_chain->pop_back();
	}

	// Checkout (from registry)
	for (int index = 0; index < signalChainSettings->GetCount(); index++)
	{
		SignalSettings* settings = signalChainSettings->Get(index);

		// Not Enabled
		if (!settings->GetIsEnabled())
			continue;

		// DO NOT DELETE! (these are all handled by the SoundRegistry*)
		SignalBase* effect = effectRegistry->Checkout(settings->GetName());

		// OPTIMIZE!
		effect->Update(settings);

		_chain->push_back(effect);
	}
}

void SignalChain::SetFrame(PlaybackFrame* frame)
{
	for (int index = 0; index < _chain->size(); index++)
	{
		_chain->at(index)->SetFrame(frame);
	}
}

bool SignalChain::HasOutput() const
{
	bool hasOutput = false;

	for (int index = 0; index < _chain->size() && !hasOutput; index++)
	{
		hasOutput |= _chain->at(index)->HasOutput();
	}

	return hasOutput;
}