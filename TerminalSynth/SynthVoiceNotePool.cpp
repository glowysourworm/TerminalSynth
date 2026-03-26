#pragma once

#include "Constant.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceNote.h"
#include "SynthVoiceNotePool.h"
#include <exception>
#include <map>
#include <stack>
#include <utility>
#include <vector>

SynthVoiceNotePool::SynthVoiceNotePool(SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo, int capacity)
{
	_capacity = capacity;
	_noteMode = soundSettings->GetNoteParameters()->mode;
	_playbackInfo = playbackInfo;
	_engagedNotes = new std::map<int, SynthVoiceNote*>();
	_disengagedNotes = new std::vector<SynthVoiceNote*>();
	_inactiveNotes = new std::stack<SynthVoiceNote*>();

	for (int index = 0; index < capacity; index++)
	{
		_inactiveNotes->push(new SynthVoiceNote(soundRegistry, soundSettings, playbackInfo, 0));
	}
}

SynthVoiceNotePool::~SynthVoiceNotePool()
{
	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		// MEMORY! ~SynthVoiceNote
		delete iter->second;
	}

	// Disengaged
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end(); ++iter)
	{
		// MEMORY! ~SynthVoiceNote
		delete (*iter);
	}

	// Inactive
	while (_inactiveNotes->size() > 0)
	{
		// MEMORY! ~SynthVoiceNote
		delete _inactiveNotes->top();

		_inactiveNotes->pop();
	}

	delete _engagedNotes;
	delete _disengagedNotes;
	delete _inactiveNotes;
}
void SynthVoiceNotePool::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	// Synth Note Mode
	_noteMode = soundSettings->GetNoteParameters()->mode;

	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		iter->second->Update(effectRegistry, soundSettings, parameters);
	}

	// Disengaged
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end(); ++iter)
	{
		(*iter)->Update(effectRegistry, soundSettings, parameters);
	}

	// Inactive
	std::stack<SynthVoiceNote*> tempStack;

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
bool SynthVoiceNotePool::HasOutput(const PlaybackTime* playbackTime)
{
	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		if (iter->second->HasOutput(playbackTime))
			return true;
	}

	// Disengaged (also prune collection)
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end(); ++iter)
	{
		// HasOutput (ringing out of the note)
		if ((*iter)->HasOutput(playbackTime))
			return true;
	}
}
bool SynthVoiceNotePool::HasEngagedNotes() const
{
	return _engagedNotes->size() > 0;
}
bool SynthVoiceNotePool::CanEngageNextNote() const
{
	switch (_noteMode)
	{
	case SynthNoteMode::Normal:
		return _engagedNotes->size() < _capacity;

	case SynthNoteMode::Portamento:
	case SynthNoteMode::Arpeggiator:
		return _engagedNotes->size() == 0;
	
	default:
		throw new std::exception("Unhandled Synth Note Mode:  SynthVoiceNotePool.cpp");
	}
}
bool SynthVoiceNotePool::IsEngaged(int midiNumber) const
{
	return _engagedNotes->contains(midiNumber);
}
bool SynthVoiceNotePool::NoteOn(int midiNumber, const PlaybackTime* playbackTime)
{
	// Already Engaged
	if (_engagedNotes->contains(midiNumber))
		return true;

	// Engage
	if (_inactiveNotes->size() > 0)
	{
		_inactiveNotes->top()->NoteOn(midiNumber, playbackTime);

		_engagedNotes->insert(std::make_pair(midiNumber, _inactiveNotes->top()));
		_inactiveNotes->pop();

		return true;
	}

	// At Capacity
	else
		return false;
}

void SynthVoiceNotePool::NoteOff(int midiNumber, const PlaybackTime* playbackTime)
{
	// Not Engaged
	if (!_engagedNotes->contains(midiNumber))
		return;

	else
	{
		_engagedNotes->at(midiNumber)->NoteOff(midiNumber, playbackTime);

		// Disengaged
		_disengagedNotes->push_back(_engagedNotes->at(midiNumber));

		// Remove Engaged Note
		_engagedNotes->erase(midiNumber);
	}
}

void SynthVoiceNotePool::IterateNotes(const PlaybackTime* playbackTime, const SynthVoiceNotePoolIterator& callback)
{
	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		if (iter->second->HasOutput(playbackTime))
			callback(iter->second, true);
	}

	// Disengaged (also prune collection)
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end();)
	{
		// HasOutput (ringing out of the note)
		if ((*iter)->HasOutput(playbackTime))
		{
			callback(*iter, false);
			iter++;
		}

		// Inactive
		else
		{
			_inactiveNotes->push(*iter);

			// Prune Dis-Engaged Notes
			iter = _disengagedNotes->erase(iter);
		}
	}
}