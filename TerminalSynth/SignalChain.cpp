#include "EffectRegistry.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalChain.h"
#include "SignalChainSettings.h"
#include "SignalSettings.h"
#include <vector>

SignalChain::SignalChain()
{
	_chain = new std::vector<SignalBase*>();
}

SignalChain::~SignalChain()
{
	delete _chain;
}

void SignalChain::Initialize(const EffectRegistry* effectRegistry, const SignalChainSettings* signalChainSettings, const OutputSettings* parameters)
{
	// Add
	for (int index = 0; index < signalChainSettings->GetCount(); index++)
	{
		SignalSettings settings = signalChainSettings->Get(index);
		SignalBase* effect = effectRegistry->GetEffect(settings.GetName());

		_chain->push_back(effect);
	}
}

void SignalChain::Update(const EffectRegistry* effectRegistry, const SignalChainSettings* signalChainSettings)
{
	_chain->clear();

	// Add
	for (int index = 0; index < signalChainSettings->GetCount(); index++)
	{
		SignalSettings settings = signalChainSettings->Get(index);
		SignalBase* effect = effectRegistry->GetEffect(settings.GetName());

		// THIS UPDATE PORTION OF THE METHOD COULD BE MANAGED BY THE CONTROLLERS!
		effect->Update(settings);

		_chain->push_back(effect);
	}
}

void SignalChain::SetFrame(PlaybackFrame* frame, float absoluteTime)
{
	for (int index = 0; index < _chain->size(); index++)
	{
		_chain->at(index)->SetFrame(frame, absoluteTime);
	}
}

bool SignalChain::HasOutput(float absoluteTime) const
{
	bool hasOutput = false;

	for (int index = 0; index < _chain->size() && !hasOutput; index++)
	{
		hasOutput |= _chain->at(index)->HasOutput(absoluteTime);
	}

	return hasOutput;
}