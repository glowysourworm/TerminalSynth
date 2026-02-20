#include "EffectRegistry.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "Synth.h"
#include "SynthNoteQueue.h"
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

void Synth::Initialize(const EffectRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* parameters)
{
	_pianoNotes = new SynthNoteQueue(configuration, parameters, 10);
	_postProcessing->Initialize(effectRegistry, configuration->GetSignalChainRegistry(), parameters);
}

void Synth::Update(const EffectRegistry* effectRegistry, const SynthSettings* configuration)
{
	_postProcessing->Update(effectRegistry, configuration->GetSignalChainRegistry());
}

void Synth::Set(int midiNumber, bool pressed, double absoluteTime, const SynthSettings* configuration)
{
	_pianoNotes->SetNote(midiNumber, pressed, absoluteTime, configuration, _samplingRate);
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