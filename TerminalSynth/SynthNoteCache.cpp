#include "OscillatorParameters.h"
#include "SynthNote.h"
#include "SynthNoteCache.h"
#include "WaveTable.h"
#include <exception>
#include <map>
#include <utility>

SynthNoteCache::SynthNoteCache()
{
	_cache = new std::map<size_t, SynthNote*>();
}

SynthNoteCache::~SynthNoteCache()
{
	for (auto iter = _cache->begin(); iter != _cache->end(); ++iter)
	{
		// ~SynthNote
		delete iter->second;
	}

	delete _cache;
}

SynthNote* SynthNoteCache::Add(const OscillatorParameters& parameters, WaveTable* waveTable, unsigned int midiNumber)
{
	size_t hashCode = parameters.GetHashCode();

	if (_cache->contains(hashCode))
		throw new std::exception("SynthNoteCache already contains the note!");

	// (MEMORY!) ~SynthNoteCache (for the WaveTable*, see ~WaveTableCache)
	SynthNote* note = new SynthNote(parameters, waveTable, midiNumber);

	_cache->insert(std::make_pair(hashCode, note));

	return note;
}

bool SynthNoteCache::Has(const OscillatorParameters& parameters)
{
	return _cache->contains(parameters.GetHashCode());
}

SynthNote* SynthNoteCache::Get(const OscillatorParameters& parameters)
{
	size_t hashCode = parameters.GetHashCode();

	if (!_cache->contains(hashCode))
		throw new std::exception("SynthNoteCache does not contain the note!");

	return _cache->at(hashCode);
}
