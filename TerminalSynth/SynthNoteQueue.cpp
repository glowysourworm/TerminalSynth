#include "Oscillator.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactory.h"
#include "SynthNote.h"
#include "SynthNoteQueue.h"
#include "SynthSettings.h"
#include <map>
#include <utility>
#include <vector>

SynthNoteQueue::SynthNoteQueue(const SynthSettings* configuration, const OutputSettings* parameters, int capacity)
{
	_capacity = capacity;
	_signalFactory = new SignalFactory(parameters);
	_engagedNotes = new std::map<int, SynthNote*>();
	_disengagedNotes = new std::vector<SynthNote*>();
}

SynthNoteQueue::~SynthNoteQueue()
{
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		delete iter->second;
	}
	for (int index = 0; index < _disengagedNotes->size(); index++)
	{
		delete _disengagedNotes->at(index);
	}
	delete _engagedNotes;
	delete _disengagedNotes;
	delete _signalFactory;
}

bool SynthNoteQueue::SetNote(int midiNumber, bool pressed, double absoluteTime, const SynthSettings* configuration, unsigned int samplingRate) const
{
	if (pressed)
	{
		// Already Engaged
		if (_engagedNotes->contains(midiNumber))
			return true;

		// Engage
		else if (_engagedNotes->size() < _capacity)
		{
			// Calculate frequency from MIDI number
			unsigned int frequency = _signalFactory->GetFrequency(midiNumber);

			// Oscillator Parameters
			OscillatorParameters currentParameters = *configuration->GetOscillator();
			OscillatorParameters parameters(
				currentParameters.GetBuiltInType(),
				frequency,
				currentParameters.GetSignalLow(),
				currentParameters.GetSignalHigh(),
				*currentParameters.GetEnvelope());

			// (MEMORY!) These may be cached, but, for now we're going to delete these with the synth note
			Oscillator* oscillator = _signalFactory->Generate(midiNumber, parameters);

			// (MEMORY!) Delete notes when they're removed from the dis-engaged list
			SynthNote* note = new SynthNote(oscillator, midiNumber, samplingRate);

			note->Engage(absoluteTime);

			_engagedNotes->insert(std::make_pair(midiNumber, note));

			return true;
		}

		// Queue is full
		else
			return false;
	}
	else
	{
		// Not Engaged
		if (!_engagedNotes->contains(midiNumber))
			return true;

		else
		{
			SynthNote* note = _engagedNotes->at(midiNumber);

			note->DisEngage(absoluteTime);

			// Remove Engaged Note
			_engagedNotes->erase(midiNumber);

			// Disengaged
			_disengagedNotes->push_back(note);

			return true;
		}
	}
}

bool SynthNoteQueue::SetFrame(PlaybackFrame* frame, double absoluteTime, double gain, double leftRight)
{
	// Use local frame to mix the note output
	PlaybackFrame noteFrame;

	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		iter->second->AddSample(&noteFrame, absoluteTime);
	}

	// Disengaged (also prune collection)
	for (int index = _disengagedNotes->size() - 1; index >= 0; index--)
	{
		// HasOutput (ringing out of the note)
		if (_disengagedNotes->at(index)->HasOutput(absoluteTime))
			_disengagedNotes->at(index)->AddSample(&noteFrame, absoluteTime);

		// ...Empty
		else
		{
			// (MEMORY!) These are completed notes (Oscillator*, SynthNote*)
			delete _disengagedNotes->at(index);

			_disengagedNotes->erase(_disengagedNotes->begin() + index);
		}
			
	}

	// MIXING THIS RIGHT AWAY FOR NOW
	// 
	noteFrame.SetFrame(noteFrame.GetLeft() * gain * (1 - leftRight), noteFrame.GetRight() * gain * leftRight);

	frame->SetFrame(&noteFrame);

	return _engagedNotes->size() > 0 || _disengagedNotes->size() > 0;
}
