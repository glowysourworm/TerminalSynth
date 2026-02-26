#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "SynthNote.h"
#include "WaveTable.h"

SynthNote::SynthNote(const OscillatorParameters& parameters, WaveTable* waveTable, unsigned int midiNumber)
{
	_midiNumber = midiNumber;
	_waveTable = waveTable;
	_parameters = new OscillatorParameters(parameters);
}

SynthNote::~SynthNote()
{
	// DO NOTE DELETE:  WaveTable*  
	//
	// These are sound sources, and are handled by the wave table cache
	//

	delete _parameters;
}

unsigned int SynthNote::GetMidiNumber() const
{
	return _midiNumber;
}

void SynthNote::GetSample(PlaybackFrame* frame, float absoluteTime)
{
	_waveTable->SetFrame(frame, absoluteTime);

	float envelopeLevel = _parameters->GetEnvelope()->GetEnvelopeLevel(absoluteTime);

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
	return _parameters->GetEnvelope()->HasOutput(absoluteTime) && _waveTable->HasOutput(absoluteTime);
}

void SynthNote::Engage(float absoluteTime)
{
	_waveTable->Clear(absoluteTime);
	_parameters->GetEnvelope()->Engage(absoluteTime);
}

void SynthNote::DisEngage(float absoluteTime)
{
	_parameters->GetEnvelope()->DisEngage(absoluteTime);
}
