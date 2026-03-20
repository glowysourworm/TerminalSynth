#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"
#include "SynthVoiceDirect.h"
#include "SynthVoicePool.h"
#include <map>
#include <stack>
#include <utility>

SynthVoicePool::SynthVoicePool(const SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo, int capacity)
{
	_capacity = capacity;
	_playbackInfo = playbackInfo;
	_engagedNotes = new std::map<int, SynthVoiceBase*>();
	_disengagedNotes = new std::map<SynthVoiceBase*, SynthVoiceBase*>();
	_inactiveNotes = new std::stack<SynthVoiceBase*>();

	// ALL SynthVoiceBase* INSTANCES!!!
	for (int index = 0; index < capacity; index++)
	{
		// These values get updated before playback
		SynthVoiceBase* note = new SynthVoiceDirect(soundRegistry, soundSettings, playbackInfo);

		_inactiveNotes->push(note);
	}
}

SynthVoicePool::~SynthVoicePool()
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
}
void SynthVoicePool::Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters)
{
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		iter->second->Update(effectRegistry, soundSettings, parameters);
	}
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end(); ++iter)
	{
		iter->second->Update(effectRegistry, soundSettings, parameters);
	}

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
bool SynthVoicePool::HasEngagedNotes() const
{
	return _engagedNotes->size() > 0;
}
bool SynthVoicePool::IsEngaged(int midiNumber) const
{
	return _engagedNotes->contains(midiNumber);
}
bool SynthVoicePool::NoteOn(int midiNumber, double absoluteTime)
{
	// Already Engaged
	if (_engagedNotes->contains(midiNumber))
		return true;

	// Engage
	if (_inactiveNotes->size() > 0)
	{
		SynthVoiceBase* note = _inactiveNotes->top();

		note->NoteOn(midiNumber, absoluteTime);

		_engagedNotes->insert(std::make_pair(midiNumber, note));
		_inactiveNotes->pop();

		return true;
	}

	// At Capacity
	else
		return false;
}

void SynthVoicePool::NoteOff(int midiNumber, double absoluteTime)
{
	// Not Engaged
	if (!_engagedNotes->contains(midiNumber))
		return;

	else
	{
		SynthVoiceBase* note = _engagedNotes->at(midiNumber);
		
		note->NoteOff(midiNumber, absoluteTime);

		// Remove Engaged Note
		_engagedNotes->erase(midiNumber);

		// Disengaged
		_disengagedNotes->insert(std::make_pair(note, note));
	}
}

void SynthVoicePool::SetFrame(PlaybackFrame* frame)
{
	// Use local frame to mix the note output
	//PlaybackFrame noteFrame(*frame);

	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		if (iter->second->HasOutput(frame->GetStreamTime()))
			iter->second->AddFrame(frame);
	}

	// Disengaged (also prune collection)
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end();)
	{
		// HasOutput (ringing out of the note)
		if (iter->first->HasOutput(frame->GetStreamTime()))
		{
			iter->first->AddFrame(frame);
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