#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "Synth.h"
#include "SynthNotePool.h"
#include "SynthSettings.h"

Synth::Synth(const SynthSettings* configuration, unsigned int numberOfChannels, unsigned int samplingRate)
{
	_numberOfChannels = numberOfChannels;
	_samplingRate = samplingRate;
	_postProcessing = new SignalChain();
	_pianoNotes = nullptr;	
}

Synth::~Synth()
{
	delete _pianoNotes;
	delete _postProcessing;
}

void Synth::Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const PlaybackInfo* parameters)
{
	_pianoNotes = new SynthNotePool(effectRegistry, configuration, parameters, 10);
	_postProcessing->Initialize(effectRegistry, configuration->GetDefaultSoundSettings()->GetPostProcessing(), parameters);
}

void Synth::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings)
{
	_postProcessing->Update(effectRegistry, soundSettings->GetPostProcessing());
	_pianoNotes->Update(effectRegistry, 
						soundSettings->GetOscillatorParameters(), 
						soundSettings->GetOscillatorEnvelope(),
						soundSettings->GetSignalChain(),
						_samplingRate);
}

void Synth::Set(int midiNumber, bool pressed, double absoluteTime)
{
	_pianoNotes->SetNote(midiNumber, pressed, absoluteTime);
}
bool Synth::GetSample(PlaybackFrame* frame, float gain, float leftRightBalance)
{
	// Primary Synth Voice(s)
	bool hasOutput = _pianoNotes->SetFrame(frame, gain, leftRightBalance);

	// Post Processing
	hasOutput |= _postProcessing->HasOutput();

	//if (_postProcessing->HasOutput(absoluteTime))
		_postProcessing->SetFrame(frame);

	return hasOutput;
}

void Synth::PruneNotePool()
{
	if (_pianoNotes->CanEvictCache())
		_pianoNotes->EvictOutdatedCache();
}
