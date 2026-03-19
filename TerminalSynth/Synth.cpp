#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "Synth.h"
#include "SynthSettings.h"
#include "SynthVoicePool.h"

Synth::Synth(const SynthSettings* configuration, unsigned int numberOfChannels, unsigned int samplingRate)
{
	_numberOfChannels = numberOfChannels;
	_samplingRate = samplingRate;
	_postProcessing = new SignalChain();
	_notePool = nullptr;
}

Synth::~Synth()
{
	delete _postProcessing;

	if (_notePool != nullptr)
		delete _notePool;
}

void Synth::Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters)
{
	_notePool = new SynthVoicePool(effectRegistry, configuration->GetCurrentSoundSettings(), parameters, 10);
	_postProcessing->Initialize(effectRegistry, configuration->GetDefaultSoundSettings()->GetPostProcessing(), parameters);
}

void Synth::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	_postProcessing->Update(effectRegistry, soundSettings->GetPostProcessing());
	_notePool->Update(effectRegistry, soundSettings, parameters);
}

void Synth::SetNote(int midiNumber, bool pressed, double absoluteTime)
{
	// THIS WHOLE LOOP NEEDS TO BE EVENT BASED (w/ the frontend)

	bool isEngaged = _notePool->IsEngaged(midiNumber);

	if (isEngaged && pressed)
		return;

	// Note Off
	else if (isEngaged && !pressed)
		_notePool->NoteOff(midiNumber, absoluteTime);

	// Note On
	else if (!isEngaged && pressed)
		_notePool->NoteOn(midiNumber, absoluteTime);

	// Post-Processing (All Notes)
	if (_notePool->HasEngagedNotes())
		_postProcessing->Engage(absoluteTime);
	else
		_postProcessing->DisEngage(absoluteTime);
}
bool Synth::GetSample(PlaybackFrame* frame, float gain, float leftRightBalance)
{
	bool hasOutput = false;

	// Primary Synth Voice(s) (Also, prunes note pool)
	_notePool->SetFrame(frame);

	// Post Processing
	hasOutput |= _postProcessing->HasOutput(frame->GetStreamTime());

	//if (_postProcessing->HasOutput(absoluteTime))
		_postProcessing->SetFrame(frame);

	return hasOutput;
}
