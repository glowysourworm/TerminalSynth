#pragma once

#ifndef SYNTH_VOICE_BASE_H
#define SYNTH_VOICE_BASE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SignalBase.h"
#include "SignalParameterizedBase.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceNote.h"
#include "SynthVoiceNotePool.h"

class SynthVoiceBase : public SignalParameterizedBase
{

public:

	/// <summary>
	/// Creates a synth voice; and stores private variables for the parameters.
	/// </summary>
	SynthVoiceBase(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo) 
		: SignalParameterizedBase(*settings->GetSynthVoiceSettings())
	{
		_notePool = new SynthVoiceNotePool(soundRegistry, settings, playbackInfo, 10);
	}
	~SynthVoiceBase()
	{
		delete _notePool;
	}

	bool IsEngaged(int midiNumber) const { return _notePool->IsEngaged(midiNumber); }
	bool CanEngageNextNote() const { return _notePool->CanEngageNextNote(); }
	bool HasEngagedNotes() const { return _notePool->HasEngagedNotes(); }

	virtual bool HasOutput(const PlaybackTime* playbackTime) const
	{
		return _notePool->HasOutput(playbackTime);
	}
	virtual void NoteOn(int midiNumber, const PlaybackTime* playbackTime)
	{
		SignalBase::Engage(playbackTime);
		
		_notePool->NoteOn(midiNumber, playbackTime);
	}
	virtual void NoteOff(int midiNumber, const PlaybackTime* playbackTime)
	{
		SignalBase::DisEngage(playbackTime);

		_notePool->NoteOff(midiNumber, playbackTime);
	}
	virtual void Clear()
	{

	}

	virtual void Update(SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
	{
		_notePool->Update(soundRegistry, settings, playbackInfo);
	}

protected:

	void SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime) override
	{
		// Synth Note(s):  Mix notes during iteration - calling sub-class GetSample functions.
		//
		_notePool->IterateNotes(playbackTime, [&](const SynthVoiceNote* note, bool isEngaged) {
			
			float sample = this->GetSample(note, playbackTime);

			frame->AddFrame(sample, sample);
		});
	}

	/// <summary>
	/// Gets a sample for the synth voice for the provided note
	/// </summary>
	virtual float GetSample(const SynthVoiceNote* note, const PlaybackTime* playbackTime) = 0;

private:

	SynthVoiceNotePool* _notePool;
};

#endif