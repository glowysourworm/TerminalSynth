#pragma once

#ifndef SYNTH_NOTE_QUEUE_H
#define SYNTH_NOTE_QUEUE_H

#include "Envelope.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"
#include <map>
#include <stack>

class SynthVoicePool
{
public:

	SynthVoicePool(const SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo, int capacity);
	~SynthVoicePool();

	/// <summary>
	/// Updates synth voices with new settings
	/// </summary>
	void Update(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters);

	/// <summary>
	/// Sets midi note to either engaged; and resets the note. Returns true if there are enough voice slots
	/// to add the next note, false otherwise.
	/// </summary>
	/// <param name="midiNumber">MIDI number of the desired note</param>
	/// <param name="absoluteTime">Current stream time</param>
	/// <returns>True if the note was engaged properly. Returns false only if the pool is at capacity.</returns>
	bool NoteOn(int midiNumber, double absoluteTime);

	/// <summary>
	/// Sets midi note to dis-engaged. The note will continue to be part of the playback stream in the dis-engaged
	/// pool until it is finished ringing (HasOutput -> false)
	void NoteOff(int midiNumber, double absoluteTime);

	/// <summary>
	/// Returns true if the note is already engaged
	/// </summary>
	bool IsEngaged(int midiNumber) const;

	/// <summary>
	/// Returns true if there are any notes pressed
	/// </summary>
	bool HasEngagedNotes() const;

	/// <summary>
	/// Provides a callback for iterating engaged notes. Also, prunes the collection! This should be
	/// called once per sample frame.
	/// </summary>
	void SetFrame(PlaybackFrame* frame);

private:

	int _capacity;
	const PlaybackInfo* _playbackInfo;

	// Capacity-sized map, will hold notes up to the user capacity (should be 10, for 10 active voices)
	std::map<int, SynthVoiceBase*>* _engagedNotes;

	// N-sized map, will hold notes until they've dissipated
	std::map<SynthVoiceBase*, SynthVoiceBase*>* _disengagedNotes;

	// M-sized map, will hold notes after they've dissipated
	std::stack<SynthVoiceBase*>* _inactiveNotes;
};

#endif