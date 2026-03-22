#include "AirwindowsEffect.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalBase.h"
#include "SignalParameterizedBase.h"
#include "SignalSettings.h"
#include <airwin_consolidated_base.h>

AirwindowsEffect::AirwindowsEffect(const SignalSettings& settings, AudioEffectX* plugin) : SignalParameterizedBase(settings)
{
	//_effect = new kCathedral(0);
	_input = new float* [2];
	_output = new float* [2];

	// One sample per channel
	_input[0] = new float[1];			// Left
	_input[1] = new float[1];			// Right

	_output[0] = new float[1];			// Left (output)
	_output[1] = new float[1];			// Right (output)

	_effect = plugin;
}

AirwindowsEffect::~AirwindowsEffect()
{
	// MEMORY! (see SoundRegistry) (these effect instances are not managed by the registry)
	delete _effect;

	delete[] _input[0];
	delete[] _input[1];
	delete[] _input;

	delete[] _output[0];
	delete[] _output[1];
	delete[] _output;
}
void AirwindowsEffect::Initialize(const PlaybackInfo* parameters)
{
	SignalBase::Initialize(parameters);
}

void AirwindowsEffect::UpdateParameter(int index, float value)
{
	// Airwindows Plugin Side
	_effect->setParameter(index, value);
}

void AirwindowsEffect::SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime)
{
	// airwindows format:  The effect treats the inputs as dry input, and the outputs as the result. I think 
	//					   they may have been confused about other audio APIs, which have input as the microphone.
	//
	//					   He's also left the effects "window" as essentially however many frames you want to
	//					   process at once, but then assumed that there's only one frame! So, I'm unsure what 
	//					   he wanted.. But, I hope I've applied the effect properly!
	// 
	//					   Finally, the audio is non-interleved. So, you'll have to know to parse your signal 
	//					   before calling his plugin. We're going to spool off samples one-by-one, with a pre-allocated
	//					   single-sample buffer.
	// 
	//					   Let's see how it sounds!
	//

	_input[0][0] = frame->GetLeft();
	_input[1][0] = frame->GetRight();

	_effect->processReplacing(_input, _output, 1);

	// Need mixing parameter
	frame->SetFrame(_output[0][0], _output[1][0]);
}

bool AirwindowsEffect::HasOutput(const PlaybackTime* playbackTime) const
{
	return true;
}