#include "PlaybackInfo.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include "SynthEffect.h"
#include <string>

SynthEffect::SynthEffect(const std::string& name, const SignalSettings& parameters) : SignalBase(parameters)
{
}
SynthEffect::~SynthEffect()
{
}
void SynthEffect::Initialize(const PlaybackInfo* parameters)
{
	SignalBase::Initialize(parameters);
}

void SynthEffect::SetFrame(PlaybackFrame* frame, float absoluteTime)
{

}

bool SynthEffect::HasOutput(float absoluteTime) const
{
	return false;
}

void SynthEffect::UpdateParameter(int index, float value)
{
	// Currently no automation
}
