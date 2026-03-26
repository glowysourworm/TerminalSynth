#pragma once

#include "Constant.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"
#include "SynthVoiceFactory.h"
#include "SynthVoicePool.h"
#include <exception>
#include <map>
#include <stack>
#include <utility>
#include <vector>

SynthVoicePool::SynthVoicePool(SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo, int capacity)
{
	_capacity = capacity;
	_noteMode = soundSettings->GetNoteParameters()->mode;
	_playbackInfo = playbackInfo;
	_engagedNotes = new std::map<int, SynthVoiceBase*>();
	_disengagedNotes = new std::vector<SynthVoiceBase*>();
	_inactiveNotes = new std::stack<SynthVoiceBase*>();

	ResetVoices(soundRegistry, soundSettings, playbackInfo);
}

SynthVoicePool::~SynthVoicePool()
{
	DisposeVoices();

	delete _engagedNotes;
	delete _disengagedNotes;
	delete _inactiveNotes;
}
void SynthVoicePool::DisposeVoices()
{
	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		// MEMORY! ~SynthVoiceBase
		delete iter->second;
	}

	_engagedNotes->clear();

	// Disengaged
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end(); ++iter)
	{
		// MEMORY! ~SynthVoiceBase
		delete (*iter);
	}

	_disengagedNotes->clear();

	// Inactive
	while (_inactiveNotes->size() > 0)
	{
		// MEMORY! ~SynthVoiceBase
		delete _inactiveNotes->top();

		_inactiveNotes->pop();
	}
}
void SynthVoicePool::ResetVoices(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	DisposeVoices();

	for (int index = 0; index < _capacity; index++)
	{
		// MEMORY! ~SynthVoicePool -> DisposeVoices
		_inactiveNotes->push(SynthVoiceFactory::CreateSynthVoiceDirect(effectRegistry, soundSettings, parameters));
	}

	// Signals a voice change
	_lastSynthVoiceHashCode = soundSettings->GetOscillatorParameters()->GetVoiceHashCode();
}
void SynthVoicePool::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	// Synth Voice Change
	if (_lastSynthVoiceHashCode != soundSettings->GetOscillatorParameters()->GetVoiceHashCode())
		ResetVoices(effectRegistry, soundSettings, parameters);

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
bool SynthVoicePool::HasOutput(const PlaybackTime* playbackTime)
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
bool SynthVoicePool::HasEngagedNotes() const
{
	return _engagedNotes->size() > 0;
}
bool SynthVoicePool::CanEngageNextNote() const
{
	switch (_noteMode)
	{
	case SynthNoteMode::Normal:
	case SynthNoteMode::Arpeggiator:
		return _engagedNotes->size() < _capacity;

	case SynthNoteMode::Portamento:	
		return _engagedNotes->size() == 0;
	
	default:
		throw new std::exception("Unhandled Synth Note Mode:  SynthVoiceNotePool.cpp");
	}
}
bool SynthVoicePool::IsEngaged(int midiNumber) const
{
	return _engagedNotes->contains(midiNumber);
}
bool SynthVoicePool::NoteOn(int midiNumber, const PlaybackTime* playbackTime)
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

void SynthVoicePool::NoteOff(int midiNumber, const PlaybackTime* playbackTime)
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

void SynthVoicePool::IterateNotes(const PlaybackTime* playbackTime, const SynthVoiceNotePoolIterator& callback)
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
