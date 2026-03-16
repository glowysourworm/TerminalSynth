#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
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

void SynthEffect::SetFrameImpl(PlaybackFrame* frame)
{

}

bool SynthEffect::HasOutput() const
{
	return false;
}

void SynthEffect::UpdateParameter(int index, float value)
{
	// Currently no automation
}
