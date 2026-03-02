#pragma once

#ifndef SYNTH_NOTE_QUEUE_H
#define SYNTH_NOTE_QUEUE_H

#include "Envelope.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "SignalChainSettings.h"
#include "SoundRegistry.h"
#include "SynthNote.h"
#include "SynthNoteCache.h"
#include "SynthSettings.h"
#include "WaveTableCache.h"
#include <map>
#include <string>
#include <vector>

class SynthNotePool
{
public:

	SynthNotePool(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* settings, int capacity);
	~SynthNotePool();

	// Update Configuration
	void Update(const SoundRegistry* effectRegistry, const OscillatorParameters& parameters, const Envelope& envelope, const SignalChainSettings& signalChainSettings, unsigned int samplingRate);

	/// <summary>
	/// Sets midi note to either engaged / disengaged. Returns true if there are enough voice slots
	/// to add the next note, false otherwise.
	/// </summary>
	/// <param name="midiNumber">MIDI number of the desired note</param>
	/// <param name="pressed">Engaged / Disengaged</param>
	/// <param name="absoluteTime">Current stream time</param>
	/// <returns>True if the note was engaged properly. Disengaged notes will be kept until the output has ceased.</returns>
	bool SetNote(int midiNumber, bool pressed, double absoluteTime) const;

	/// <summary>
	/// Synthesizes a full output at the specified stream time. Returns true if there was output this call.
	/// </summary>
	bool SetFrame(PlaybackFrame* frame, double absoluteTime, double gain, double leftRight);

	/// <summary>
	/// Gets a list of sound banks (sample folders) from the WaveTableCache*
	/// </summary>
	/// <param name="destination">Destination list for the sound bank names</param>
	void GetSoundBanks(std::vector<std::string>& destination);

	/// <summary>
	/// Gets a list of sounds from the specified sound bank from the WaveTableCache*
	/// </summary>
	/// <param name="soundBankName">Name of Sound Bank</param>
	/// <param name="destination">Destination list for the sound names</param>
	void GetSounds(const std::string& soundBankName, std::vector<std::string>& destination);

	/// <summary>
	/// Returns true if there are currently no Engaged / Dis-Engaged (ringing) SynthNote* instances
	/// in the pool. This would give the synth time to evict old outdate cache.
	/// </summary>
	bool CanEvictCache() const;

	/// <summary>
	/// Tells the synth note pool to get rid of old notes from the previous synth update. This
	/// will not be possible without first checking to see if there are any ringing notes still
	/// int the pool (see CanEvictCache())
	/// </summary>
	void EvictOutdatedCache();

private:

	int _capacity;
	unsigned int _systemSamplingRate; 

	Envelope* _envelope;
	OscillatorParameters* _oscillatorParameters;
	SignalChain* _signalChain;
	bool _hasStaleParameters;

	// Capacity-sized map, will hold notes up to the user capacity (should be 10, for 10 active voices)
	std::map<int, SynthNote*>* _engagedNotes;

	// N-sized map, will hold notes until they've dissipated
	std::map<SynthNote*, SynthNote*>* _disengagedNotes;

	// Wave Table Cache:  This will lazy load WaveTable* instances for sound banks and oscillators
	WaveTableCache* _waveTableCache;

	// Synth Note Cache:  This will also lazy load SynthNote* instances using OscillatorParameters&
	SynthNoteCache* _synthNoteCache;

};

#endif