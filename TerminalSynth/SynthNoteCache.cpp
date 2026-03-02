#include "Envelope.h"
#include "OscillatorParameters.h"
#include "SignalChain.h"
#include "SynthNote.h"
#include "SynthNoteCache.h"
#include "WaveTable.h"
#include <exception>
#include <map>
#include <utility>
#include <vector>

SynthNoteCache::SynthNoteCache()
{
	_cache = new std::map<size_t, std::vector<SynthNote*>*>();
}

SynthNoteCache::~SynthNoteCache()
{
	for (auto iter = _cache->begin(); iter != _cache->end(); ++iter)
	{
		for (int index = 0; index < iter->second->size(); index++)
		{
			// ~SynthNote
			delete iter->second->at(index);
		}

		// ~std::vector
		delete iter->second;
	}

	delete _cache;
}

SynthNote* SynthNoteCache::Add(const OscillatorParameters& parameters, const Envelope& envelope, SignalChain* signalChain, WaveTable* waveTable, unsigned int midiNumber)
{
	size_t hashCode = parameters.GetHashCode();

	// (MEMORY!) ~SynthNoteCache (for the WaveTable*, see ~WaveTableCache)
	SynthNote* note = new SynthNote(parameters, envelope, signalChain, waveTable, midiNumber);

	if (_cache->contains(hashCode))
	{
		_cache->at(hashCode)->push_back(note);
	}
	else
	{
		// (MEMORY!) ~SynthNoteCache
		std::vector<SynthNote*>* list = new std::vector<SynthNote*>();

		// Multiple Notes (used for note ringing + next engaged note)
		list->push_back(note);

		_cache->insert(std::make_pair(hashCode, list));
	}

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

	// Has at least one of these SynthNote* instances! Doesn't matter which
	return _cache->at(hashCode)->at(0);
}

void SynthNoteCache::Clear()
{
	// Clear due to a significant configuration change (synth settings, or oscillator parameters)

	for (auto iter = _cache->begin(); iter != _cache->end(); ++iter)
	{
		for (int index = 0; index < iter->second->size(); index++)
		{
			// ~SynthNote
			delete iter->second->at(index);
		}

		// ~std::vector
		delete iter->second;
	}

	_cache->clear();
}
