#include "Envelope.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "SynthNote.h"
#include "WaveTable.h"

SynthNote::SynthNote(const OscillatorParameters& parameters, const Envelope& envelope, WaveTable* waveTable, unsigned int midiNumber)
{
	_midiNumber = midiNumber;
	_waveTable = waveTable;
	_envelope = new Envelope(envelope);
	_parameters = new OscillatorParameters(parameters);
}

SynthNote::~SynthNote()
{
	// DO NOTE DELETE:  WaveTable*  
	//
	// These are sound sources, and are handled by the wave table cache
	//

	delete _envelope;
	delete _parameters;
}

unsigned int SynthNote::GetMidiNumber() const
{
	return _midiNumber;
}

void SynthNote::GetSample(PlaybackFrame* frame, float absoluteTime)
{
	_waveTable->SetFrame(frame, _envelope->GetEngageTime(), absoluteTime);

	float envelopeLevel = _envelope->GetEnvelopeLevel(absoluteTime);

	frame->SetFrame(envelopeLevel * frame->GetLeft(), envelopeLevel * frame->GetRight());
}

void SynthNote::AddSample(PlaybackFrame* frame, float absoluteTime)
{
	PlaybackFrame noteFrame(*frame);

	// Create Sample
	this->GetSample(&noteFrame, absoluteTime);

	// Add Output
	frame->AddFrame(&noteFrame);
}

bool SynthNote::HasOutput(float absoluteTime)
{
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
