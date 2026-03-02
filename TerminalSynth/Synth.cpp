#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "SoundRegistry.h"
#include "Synth.h"
#include "SynthNotePool.h"
#include "SynthSettings.h"

Synth::Synth(const SynthSettings* configuration, unsigned int numberOfChannels, unsigned int samplingRate)
{
	_numberOfChannels = numberOfChannels;
	_samplingRate = samplingRate;
	_postProcessing = new SignalChain();
}

Synth::~Synth()
{
	delete _pianoNotes;
	delete _postProcessing;
}

void Synth::Initialize(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters)
{
	_pianoNotes = new SynthNotePool(effectRegistry, configuration, parameters, 10);
	_postProcessing->Initialize(effectRegistry, configuration->GetSoundSettings()->GetPostProcessing(), parameters);
}

void Synth::Update(const SoundRegistry* effectRegistry, const SynthSettings* configuration)
{
	_postProcessing->Update(effectRegistry, *configuration->GetSoundSettings()->GetPostProcessing());
	_pianoNotes->Update(effectRegistry, 
						*configuration->GetSoundSettings()->GetOscillatorParameters(), 
						*configuration->GetSoundSettings()->GetOscillatorEnvelope(),
						*configuration->GetSoundSettings()->GetSignalChain(),
						_samplingRate);
}

void Synth::Set(int midiNumber, bool pressed, double absoluteTime, const SynthSettings* configuration)
{
	_pianoNotes->SetNote(midiNumber, pressed, absoluteTime);
}
bool Synth::GetSample(PlaybackFrame* frame, double absoluteTime, const SynthSettings* configuration)
{
	float gain = configuration->GetOutputGain();
	float leftRight = configuration->GetOutputLeftRight();

	// Primary Synth Voice(s)
	bool hasOutput = _pianoNotes->SetFrame(frame, absoluteTime, gain, leftRight);

	// Post Processing
	hasOutput |= _postProcessing->HasOutput(absoluteTime);

	//if (_postProcessing->HasOutput(absoluteTime))
		_postProcessing->SetFrame(frame, absoluteTime);

	//// Effect Chain
	//bool reverbHasOutput = _reverb->HasOutput(absoluteTime) && configuration->GetHasReverb();
	//bool delayHasOutput = _delay->HasOutput(absoluteTime) && configuration->GetHasDelay();

	//// OUTPUT EFFECTS	
	//if (reverbHasOutput)
	//	_reverb->GetSample(frame, absoluteTime);

	//if (delayHasOutput)
	//	_delay->GetSample(frame, absoluteTime);

	return hasOutput;
}

void Synth::PruneNotePool()
{
	if (_pianoNotes->CanEvictCache())
		_pianoNotes->EvictOutdatedCache();
}
