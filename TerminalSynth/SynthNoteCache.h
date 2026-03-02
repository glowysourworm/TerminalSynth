#pragma once

#ifndef SYNTH_NOTE_CACHE_H
#define SYNTH_NOTE_CACHE_H

#include "Envelope.h"
#include "OscillatorParameters.h"
#include "SignalChain.h"
#include "SynthNote.h"
#include "WaveTable.h"
#include <map>
#include <vector>

class SynthNoteCache
{
public:

	SynthNoteCache();
	~SynthNoteCache();

	/// <summary>
	/// Sets synth note in the cache using the provided parameters. The wave table should NOT BE DELETED!
	/// </summary>
	SynthNote* Add(const OscillatorParameters& parameters, const Envelope& envelope, SignalChain* signalChain, WaveTable* waveTable, unsigned int midiNumber);

	/// <summary>
	/// Returns true if the synth note is in the cache
	/// </summary>
	bool Has(const OscillatorParameters& parameters);

	/// <summary>
	/// Retrieves synth note from the cache
	/// </summary>
	SynthNote* Get(const OscillatorParameters& parameters);

	/// <summary>
	/// Evicts cache and deletes all allocated memory!
	/// </summary>
	void Clear();

private:

	std::map <size_t, std::vector<SynthNote*>*>* _cache;
};

#endif