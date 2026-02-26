#include "Constant.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SynthNote.h"
#include "SynthNoteCache.h"
#include "SynthSettings.h"
#include "SynthSoundMap.h"
#include "WaveTable.h"
#include "WaveTableCache.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

SynthSoundMap::SynthSoundMap(const SynthSettings* configuration, const OutputSettings* settings, int capacity)
{
	_capacity = capacity;
	_systemSamplingRate = settings->GetSamplingRate();
	_engagedNotes = new std::map<int, SynthNote*>();
	_disengagedNotes = new std::vector<SynthNote*>();
	_waveTableCache = new WaveTableCache();
	_synthNoteCache = new SynthNoteCache();
	_oscillatorParameters = new OscillatorParameters(*configuration->GetOscillator());

	// Initialize (CHECK SOUND BANKS!) (NO LOGGING!)
	_waveTableCache->Initialize(configuration, settings);
}

SynthSoundMap::~SynthSoundMap()
{
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		delete iter->second;
	}
	for (int index = 0; index < _disengagedNotes->size(); index++)
	{
		delete _disengagedNotes->at(index);
	}
	delete _engagedNotes;
	delete _disengagedNotes;
	delete _waveTableCache;
	delete _synthNoteCache;
	delete _oscillatorParameters;
}

void SynthSoundMap::Update(const OscillatorParameters& parameters, unsigned int samplingRate)
{
	_systemSamplingRate = samplingRate;
	_oscillatorParameters->Update(parameters);
}

bool SynthSoundMap::SetNote(int midiNumber, bool pressed, double absoluteTime) const
{
	if (pressed)
	{
		// Already Engaged
		if (_engagedNotes->contains(midiNumber))
			return true;

		// BLOCKING RE-ENGAGED NOTES:  The WaveTable* has to be shared across instances, so this
		//							   is a "blocking feature" for now.. We have to be able to 
		//							   have a state-less design for the wave table (state-less, but
		//							   single-threaded)
		//
		for (int index = 0; index < _disengagedNotes->size(); index++)
		{
			if (_disengagedNotes->at(index)->GetMidiNumber() == midiNumber)
				return true;
		}

		// Engage
		if (_engagedNotes->size() < _capacity)
		{
			// Calculate frequency from MIDI number
			float frequency = TerminalSynth::GetMidiFrequency(midiNumber);

			// Oscillator Parameters (copy, and set next frequency)
			OscillatorParameters parameters = *_oscillatorParameters;
			parameters.SetFrequency(frequency);

			SynthNote* note = nullptr;

			// Check the cache for the note
			if (_synthNoteCache->Has(parameters))
			{
				note = _synthNoteCache->Get(parameters);
			}
			else
			{
				// Cache handles memory for WaveTable*
				WaveTable* waveTable = _waveTableCache->Get(parameters, midiNumber);

				// Synth note cache will handle these
				note = _synthNoteCache->Add(parameters, waveTable, midiNumber);
			}

			note->Engage(absoluteTime);

			_engagedNotes->insert(std::make_pair(midiNumber, note));

			return true;
		}

		// Queue is full
		else
			return false;
	}
	else
	{
		// Not Engaged
		if (!_engagedNotes->contains(midiNumber))
			return true;

		else
		{
			SynthNote* note = _engagedNotes->at(midiNumber);

			note->DisEngage(absoluteTime);

			// Remove Engaged Note
			_engagedNotes->erase(midiNumber);

			// Disengaged
			_disengagedNotes->push_back(note);

			return true;
		}
	}
}

bool SynthSoundMap::SetFrame(PlaybackFrame* frame, double absoluteTime, double gain, double leftRight)
{
	// Use local frame to mix the note output
	PlaybackFrame noteFrame;

	// Engaged
	for (auto iter = _engagedNotes->begin(); iter != _engagedNotes->end(); ++iter)
	{
		// HasOutput() -> Add / Mix Sample
		if (iter->second->HasOutput(absoluteTime))
			iter->second->AddSample(&noteFrame, absoluteTime);
	}

	// Disengaged (also prune collection)
	for (int index = _disengagedNotes->size() - 1; index >= 0; index--)
	{
		// HasOutput (ringing out of the note)
		if (_disengagedNotes->at(index)->HasOutput(absoluteTime))
			_disengagedNotes->at(index)->AddSample(&noteFrame, absoluteTime);

		// ...Empty
		else
		{
			_disengagedNotes->erase(_disengagedNotes->begin() + index);
		}
			
	}

	// MIXING THIS RIGHT AWAY FOR NOW
	// 
	noteFrame.SetFrame(noteFrame.GetLeft() * gain * (1 - leftRight), noteFrame.GetRight() * gain * leftRight);

	frame->SetFrame(&noteFrame);

	return _engagedNotes->size() > 0 || _disengagedNotes->size() > 0;
}

void SynthSoundMap::GetSoundBanks(std::vector<std::string>& destination)
{
	_waveTableCache->GetSoundBanks(destination);
}

void SynthSoundMap::GetSounds(const std::string& soundBankName, std::vector<std::string>& destination)
{
	_waveTableCache->GetSoundNames(soundBankName, destination);
}
