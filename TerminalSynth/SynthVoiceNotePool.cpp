#include "Constant.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"
#include "SynthVoiceFactory.h"
#include "SynthVoiceNotePool.h"
#include <exception>
#include <map>
#include <stack>
#include <utility>

SynthVoiceNotePool::SynthVoiceNotePool(const SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo, int capacity)
{
	_capacity = capacity;
	_noteMode = soundSettings->GetNoteParameters()->mode;
	_playbackInfo = playbackInfo;
	_lastParameterHash = soundSettings->GetOscillatorParameters()->GetHashCode();
	_engagedNotes = new std::map<int, SynthVoiceBase*>();
	_disengagedNotes = new std::map<SynthVoiceBase*, SynthVoiceBase*>();
	_inactiveNotes = new std::stack<SynthVoiceBase*>();
	_singleNote = SynthVoiceFactory::CreateSynthVoiceDirect(soundRegistry, soundSettings, playbackInfo);
	_singleNoteEngaged = false;
	_singleNoteNumber = 0;

	// ALL SynthVoiceBase* INSTANCES!!! (for multiple note mode)
	for (int index = 0; index < capacity; index++)
	{
		// These values get updated before playback
		SynthVoiceBase* note = SynthVoiceFactory::CreateSynthVoiceDirect(soundRegistry, soundSettings, playbackInfo);

		_inactiveNotes->push(note);
	}
}

SynthVoiceNotePool::~SynthVoiceNotePool()
{
	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		// ~SynthNote
		delete iter->second;
	}

	// Disengaged
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end();)
	{
		// ~SynthNote
		delete iter->second;
	}

	// Inactive
	while (_inactiveNotes->size() > 0)
	{
		// ~SynthNote
		delete _inactiveNotes->top();

		_inactiveNotes->pop();
	}

	delete _engagedNotes;
	delete _disengagedNotes;
	delete _inactiveNotes;
	delete _singleNote;
}
void SynthVoiceNotePool::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	// Synth Note Mode
	_noteMode = soundSettings->GetNoteParameters()->mode;

	// Single Note
	_singleNote->Update(effectRegistry, soundSettings, parameters);

	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		iter->second->Update(effectRegistry, soundSettings, parameters);
	}

	// Disengaged
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end(); ++iter)
	{
		iter->second->Update(effectRegistry, soundSettings, parameters);
	}

	// Inactive
	std::stack<SynthVoiceBase*> tempStack;

	while (_inactiveNotes->size() > 0)
	{
		_inactiveNotes->top()->Update(effectRegistry, soundSettings, parameters);
		tempStack.push(_inactiveNotes->top());
		_inactiveNotes->pop();
	}

	while (tempStack.size() > 0)
	{
		_inactiveNotes->push(tempStack.top());
		tempStack.pop();
	}
}
bool SynthVoiceNotePool::HasEngagedNotes() const
{
	return _engagedNotes->size() > 0 || (_singleNoteEngaged && _noteMode != SynthNoteMode::MultipleNormal);
}
bool SynthVoiceNotePool::CanEngageNextNote() const
{
	switch (_noteMode)
	{
	case SynthNoteMode::MultipleNormal:
		return _engagedNotes->size() < _capacity;

	case SynthNoteMode::SingleNormal:
	case SynthNoteMode::SinglePortamento:
	case SynthNoteMode::SingleArpeggiator:
		return !_singleNoteEngaged;
	
	default:
		throw new std::exception("Unhandled Synth Note Mode:  SynthVoiceNotePool.cpp");
	}
}
bool SynthVoiceNotePool::IsEngaged(int midiNumber) const
{
	return _engagedNotes->contains(midiNumber) || (_singleNoteEngaged && _noteMode != SynthNoteMode::MultipleNormal);
}
bool SynthVoiceNotePool::NoteOn(int midiNumber, const PlaybackTime* playbackTime)
{
	if (_noteMode == SynthNoteMode::MultipleNormal)
	{
		// Already Engaged
		if (_engagedNotes->contains(midiNumber))
			return true;

		// Engage
		if (_inactiveNotes->size() > 0)
		{
			SynthVoiceBase* note = _inactiveNotes->top();

			note->NoteOn(midiNumber, playbackTime);

			_engagedNotes->insert(std::make_pair(midiNumber, note));
			_inactiveNotes->pop();

			return true;
		}

		// At Capacity
		else
			return false;
	}

	// Single Note Mode(s)
	else
	{
		if (_singleNoteEngaged)
			return true;

		// Engage
		else
		{
			_singleNote->NoteOn(midiNumber, playbackTime);
			_singleNoteEngaged = true;
			_singleNoteNumber = midiNumber;
			return true;
		}
	}
}

void SynthVoiceNotePool::NoteOff(int midiNumber, const PlaybackTime* playbackTime)
{
	if (_noteMode == SynthNoteMode::MultipleNormal)
	{
		// Not Engaged
		if (!_engagedNotes->contains(midiNumber))
			return;

		else
		{
			SynthVoiceBase* note = _engagedNotes->at(midiNumber);

			note->NoteOff(midiNumber, playbackTime);

			// Remove Engaged Note
			_engagedNotes->erase(midiNumber);

			// Disengaged
			_disengagedNotes->insert(std::make_pair(note, note));
		}
	}
	// Single Note Mode(s)
	else
	{
		if (!_singleNoteEngaged || _singleNoteNumber != midiNumber)
			return;

		// Engage
		else
		{
			_singleNote->NoteOff(midiNumber, playbackTime);
			_singleNoteEngaged = false;
		}
	}
}

void SynthVoiceNotePool::SetFrame(PlaybackFrame* frame, const PlaybackTime* playbackTime)
{
	if (_noteMode == SynthNoteMode::MultipleNormal)
	{
		// Engaged
		for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
		{
			if (iter->second->HasOutput(playbackTime))
				iter->second->AddFrame(frame, playbackTime);
		}

		// Disengaged (also prune collection)
		for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end();)
		{
			// HasOutput (ringing out of the note)
			if (iter->first->HasOutput(playbackTime))
			{
				iter->first->AddFrame(frame, playbackTime);
				iter++;
			}

			// Inactive
			else
			{
				// Put back on the stack
				_inactiveNotes->push(iter->first);

				// Prune Dis-Engaged Notes
				iter = _disengagedNotes->erase(iter);
			}
		}
	}

	// Single Note Mode(s)
	else
	{
		if (_singleNote->HasOutput(playbackTime))
			_singleNote->AddFrame(frame, playbackTime);
	}
}