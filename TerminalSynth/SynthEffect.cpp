#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include "SynthEffect.h"
#include <string>

SynthEffect::SynthEffect(const std::string& name, const SignalSettings& parameters) : SignalBase(name)
{
	_parameters = new SignalSettings(parameters);
}
SynthEffect::~SynthEffect()
{
	delete _parameters;
}
void SynthEffect::Initialize(const SignalSettings* settings, const OutputSettings* parameters)
{
	SignalBase::Initialize(settings, parameters);
}

void SynthEffect::SetFrame(PlaybackFrame* frame, float absoluteTime)
{

}

bool SynthEffect::HasOutput(float absoluteTime) const
{
	return false;
}
