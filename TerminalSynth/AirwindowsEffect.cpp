#include "AirwindowsEffect.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <airwin_consolidated_base.h>
#include <cctype>
#include <exception>
#include <string>

AirwindowsEffect::AirwindowsEffect(AudioEffectX* plugin, const std::string& name) : SignalBase(name)
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

	// Parameters (Copy Airwindows Parameter Defaults)
	int parameterCount = 0;

	// Needed way to count parameters
	while (plugin->canConvertParameterTextToValue(parameterCount++)) {}

	// Set SignalBase* parameters (use these for reading, and write both copies)
	//
	for (int index = 0; index < parameterCount - 1; index++)
	{
		char paramName[100];
		plugin->getParameterName(index, paramName);

		// VALIDATE STRING! THESE HAD PROBLEMS WITH NULL TERMINATORS!
		for (int index = 0; index < 100; index++)
		{
			if (!std::isalnum(paramName[index]))
			{
				paramName[index] = '\0';
				break;
			}
		}

		this->AddParameter(paramName, 0.0f, 1.0f, plugin->getParameter(index));
	}
}

AirwindowsEffect::~AirwindowsEffect()
{
	delete[] _input[0];
	delete[] _input[1];
	delete[] _input;

	delete[] _output[0];
	delete[] _output[1];
	delete[] _output;
}

void AirwindowsEffect::Initialize(const SignalSettings* configuration, const OutputSettings* parameters)
{
	throw new std::exception("Airwindows effects are not initialized from the Terminal Synth libraries");
}

void AirwindowsEffect::UpdateParameter(int index, float value)
{
	// Synth Side
	SignalBase::UpdateParameter(index, value);

	// Airwindows Plugin Side
	_effect->setParameter(index, value);
}

void AirwindowsEffect::SetFrame(PlaybackFrame* frame, float absoluteTime)
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

	frame->SetFrame(_output[0][0], _output[1][0]);
}

bool AirwindowsEffect::HasOutput(float absoluteTime) const
{
	return true;
}