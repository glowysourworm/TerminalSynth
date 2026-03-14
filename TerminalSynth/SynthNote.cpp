#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "SynthNote.h"
#include "WaveTable.h"

SynthNote::SynthNote(const OscillatorParameters& parameters, 
					 const Envelope& envelope, 
					 SignalChain* signalChain, 
					 WaveTable* waveTable, 
					 unsigned int midiNumber)
{
	_midiNumber = midiNumber;
	_waveTable = waveTable;
	_effectsChain = signalChain;
	_envelope = new Envelope(envelope);
	_parameters = new OscillatorParameters(parameters);
}

SynthNote::~SynthNote()
{
	// DO NOTE DELETE:  WaveTable*, SignalChain*
	//
	// These are sound sources, and are handled by the wave table cache and effects registry
	//


	delete _envelope;
	delete _parameters;
}

unsigned int SynthNote::GetMidiNumber() const
{
	return _midiNumber;
}

void SynthNote::GetSample(PlaybackFrame* frame)
{
	_waveTable->SetFrame(frame, _envelope->GetEngageTime(), frame->GetStreamTime());

	//float envelopeLevel = _envelope->GetEnvelopeLevel(frame->GetStreamTime());
	float envelopeLevel = 1;

	frame->SetFrame(envelopeLevel * frame->GetLeft(), envelopeLevel * frame->GetRight());
}

void SynthNote::AddSample(PlaybackFrame* frame)
{
	PlaybackFrame noteFrame(*frame);

	// Create Sample
	this->GetSample(&noteFrame);

	// Add Effects Chain
	_effectsChain->SetFrame(&noteFrame);

	// Add Output
	frame->AddFrame(noteFrame.GetLeft(), noteFrame.GetRight());
}

bool SynthNote::HasOutput(float absoluteTime)
{
	// We must have a way to terminate the note! So, this must be set with the envelope! Otherwise,
	// the effect may ring out!
	//
	return _envelope->HasOutput(absoluteTime) && _waveTable->HasOutput(_envelope->GetEngageTime(), absoluteTime);
}

void SynthNote::Engage(float absoluteTime)
{
	_waveTable->Clear(_envelope->GetEngageTime(), absoluteTime);
	_envelope->Engage(absoluteTime);
}

void SynthNote::DisEngage(float absoluteTime)
{
	_envelope->DisEngage(absoluteTime);
}
