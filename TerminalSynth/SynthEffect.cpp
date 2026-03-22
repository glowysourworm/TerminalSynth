#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalParameterizedBase.h"
#include "SignalSettings.h"
#include "SynthEffect.h"
#include <string>

SynthEffect::SynthEffect(const std::string& name, const SignalSettings& parameters) : SignalParameterizedBase(parameters)
{
}
SynthEffect::~SynthEffect()
{
}
void SynthEffect::Initialize(const PlaybackInfo* parameters)
{
	//SignalParameterizedBase::Initialize(parameters);
}

void SynthEffect::SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime)
{

}

bool SynthEffect::HasOutput(const PlaybackTime* playbackTime) const
{
	return false;
}

void SynthEffect::UpdateParameter(int index, float value)
{
	// Currently no automation
}
