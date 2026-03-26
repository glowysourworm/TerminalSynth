#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "Synth.h"
#include "SynthSettings.h"
#include "SynthVoiceNotePool.h"

Synth::Synth(const SynthSettings* configuration, unsigned int numberOfChannels, unsigned int samplingRate)
{
	_numberOfChannels = numberOfChannels;
	_samplingRate = samplingRate;
	_postProcessing = new SignalChain();
	_notePool = nullptr; 
	_octave = configuration->GetCurrentSoundSettings()->GetOscillatorParameters()->GetOctave();
}

Synth::~Synth()
{
	delete _postProcessing;

	if (_notePool != nullptr)
		delete _notePool;
}

void Synth::Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters)
{
	_notePool = new SynthVoiceNotePool(effectRegistry, configuration->GetCurrentSoundSettings(), parameters, 10);
	_postProcessing->Initialize(effectRegistry, configuration->GetCurrentSoundSettings()->GetPostProcessing(), parameters);
	_octave = configuration->GetCurrentSoundSettings()->GetOscillatorParameters()->GetOctave();
}

void Synth::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	_postProcessing->Update(effectRegistry, soundSettings->GetPostProcessing());
	_notePool->Update(effectRegistry, soundSettings, parameters);
	_octave = soundSettings->GetOscillatorParameters()->GetOctave();
}

void Synth::SetNote(int midiNumber, bool pressed, const PlaybackTime* playbackTime)
{
	// THIS WHOLE LOOP NEEDS TO BE EVENT BASED (w/ the frontend)

	bool isEngaged = _notePool->IsEngaged(midiNumber);

	if (isEngaged && pressed)
		return;

	// Note Off
	else if (isEngaged && !pressed)
		_notePool->NoteOff(midiNumber, playbackTime);

	// Note On
	else if (!isEngaged && pressed && _notePool->CanEngageNextNote())
		_notePool->NoteOn(midiNumber, playbackTime);

	// Post-Processing (All Notes)
	if (_notePool->HasEngagedNotes())
		_postProcessing->Engage(playbackTime);
	else
		_postProcessing->DisEngage(playbackTime);
}
bool Synth::GetSample(PlaybackFrame* frame, const PlaybackTime* playbackTime, float gain, float leftRightBalance)
{
	bool hasOutput = false;

	// Primary Synth Voice(s) (Also, prunes note pool)
	_notePool->SetFrame(frame, playbackTime);

	// Post Processing
	hasOutput |= _postProcessing->HasOutput(playbackTime);

	//if (_postProcessing->HasOutput(absoluteTime))
		_postProcessing->SetFrame(frame, playbackTime);

	// This is now being used for error modes. The has output has been put on hold.. several little things.
	return true;
}
