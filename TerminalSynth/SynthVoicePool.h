#pragma once

#ifndef SYNTH_NOTE_QUEUE_H
#define SYNTH_NOTE_QUEUE_H

#include "Constant.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"
#include <functional>
#include <map>
#include <stack>
#include <vector>

class SynthVoicePool
{
public:

	SynthVoicePool(SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo, int capacity);
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
	/// <param name="timeCursor">PlaybackFrame time cursor (which counts frame samples)</param>
	/// <returns>True if the note was engaged properly. Returns false only if the pool is at capacity.</returns>
	bool NoteOn(int midiNumber, const PlaybackTime* playbackTime);

	/// <summary>
	/// Sets midi note to dis-engaged. The note will continue to be part of the playback stream in the dis-engaged
	/// pool until it is finished ringing (HasOutput -> false)
	/// </summary>
	void NoteOff(int midiNumber, const PlaybackTime* playbackTime);

	/// <summary>
	/// Returns true if the note is already engaged
	/// </summary>
	bool IsEngaged(int midiNumber) const;

	/// <summary>
	/// Returns true if the pool has capacity for more engaged notes. Depends on SynthNoteMode.
	/// </summary>
	bool CanEngageNextNote() const;

	/// <summary>
	/// Returns true if there are any notes pressed
	/// </summary>
	bool HasEngagedNotes() const;

	/// <summary>
	/// Returns true if there are any note envelopes still left ringing out, or engaged
	/// </summary>
	bool HasOutput(const PlaybackTime* playbackTime);

public:

	using SynthVoiceNotePoolIterator = std::function<void(SynthVoiceBase* note, bool isEngaged)>;

	/// <summary>
	/// Iterates notes and provides a callback to process note synthesis. Also, prunes the note pool
	/// to move disengaged notes to the proper collection.
	/// </summary>
	void IterateNotes(const PlaybackTime* playbackTime, const SynthVoiceNotePoolIterator& callback);

private:

	/// <summary>
	/// Re-creates synth voice cache - all as inactive notes
	/// </summary>
	void ResetVoices(SoundRegistry* effectRegistry, const SoundSettings* soundSettings, const PlaybackInfo* parameters);

	/// <summary>
	/// Deletes all synth voice instances
	/// </summary>
	void DisposeVoices();

private:

	int _capacity;
	SynthNoteMode _noteMode;
	const PlaybackInfo* _playbackInfo;

	size_t _lastSynthVoiceHashCode;

	// Capacity-sized map, will hold notes up to the user capacity (should be 10, for 10 active voices)
	std::map<int, SynthVoiceBase*>* _engagedNotes;

	// N-sized map, will hold notes until they've dissipated
	std::vector<SynthVoiceBase*>* _disengagedNotes;

	// M-sized map, will hold notes after they've dissipated
	std::stack<SynthVoiceBase*>* _inactiveNotes;
};

#endif