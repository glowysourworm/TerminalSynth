#include "Oscillator.h"
#include "PlaybackFrame.h"
#include "SynthNote.h"

SynthNote::SynthNote(Oscillator* oscillator, unsigned int midiNumber, unsigned int samplingRate)
{
	_midiNumber = midiNumber;
	_oscillator = oscillator;
}

SynthNote::~SynthNote()
{
	delete _oscillator;
}

unsigned int SynthNote::GetMidiNumber() const
{
	return _midiNumber;
}

void SynthNote::GetSample(PlaybackFrame* frame, float absoluteTime)
{
	// Generate Oscillator
	_oscillator->SetFrame(frame, absoluteTime);
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
	return _oscillator->HasOutput(absoluteTime);
}

void SynthNote::Engage(float absoluteTime)
{
	_oscillator->Engage(absoluteTime);
}

void SynthNote::DisEngage(float absoluteTime)
{
	_oscillator->DisEngage(absoluteTime);
}
