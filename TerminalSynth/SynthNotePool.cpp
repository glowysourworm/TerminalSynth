#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalChain.h"
#include "SignalChainSettings.h"
#include "SoundRegistry.h"
#include "SynthNote.h"
#include "SynthNoteCache.h"
#include "SynthNotePool.h"
#include "SynthSettings.h"
#include "WaveTable.h"
#include "WaveTableCache.h"
#include <exception>
#include <map>
#include <string>
#include <utility>
#include <vector>

SynthNotePool::SynthNotePool(const SoundRegistry* effectRegistry, const SynthSettings* configuration, const OutputSettings* settings, int capacity)
{
	_capacity = capacity;
	_systemSamplingRate = settings->GetSamplingRate();
	_engagedNotes = new std::map<int, SynthNote*>();
	_disengagedNotes = new std::map<SynthNote*, SynthNote*>();
	_waveTableCache = new WaveTableCache();
	_synthNoteCache = new SynthNoteCache();
	_envelope = new Envelope(*configuration->GetSoundSettings()->GetOscillatorEnvelope());
	_oscillatorParameters = new OscillatorParameters(*configuration->GetSoundSettings()->GetOscillatorParameters());
	_hasStaleParameters = false;
	_signalChain = new SignalChain();

	// Initialize (CHECK SOUND BANKS!) (NO LOGGING!)
	_waveTableCache->Initialize(configuration, settings);
	_signalChain->Initialize(effectRegistry, configuration->GetSoundSettings()->GetSignalChain(), settings);
}

SynthNotePool::~SynthNotePool()
{
	delete _engagedNotes;
	delete _disengagedNotes;
	delete _waveTableCache;
	delete _synthNoteCache;
	delete _envelope;
	delete _oscillatorParameters;
}

void SynthNotePool::Update(SoundRegistry* effectRegistry, const OscillatorParameters& parameters, const Envelope& envelope, const SignalChainSettings& signalChainSettings, unsigned int samplingRate)
{
	_systemSamplingRate = samplingRate;
	_oscillatorParameters->Update(parameters);
	_envelope->Update(envelope);
	_signalChain->Update(effectRegistry, signalChainSettings);

	_hasStaleParameters = true;
}

bool SynthNotePool::CanEvictCache() const
{
	return _engagedNotes->size() == 0 && _disengagedNotes->size() == 0;
}

void SynthNotePool::EvictOutdatedCache()
{
	if (!this->CanEvictCache())
		throw new std::exception("Trying to evict synth note pool before notes have finished ringing!");

	if (!_hasStaleParameters)
		return;

	// ~SynthNote (does not delete WaveTable*)
	_synthNoteCache->Clear();

	// Reset flag
	_hasStaleParameters = false;

	// The WaveTable* instances are cached based on the OscillatorParameters + MIDI#. So, they aren't
	// affected by changes to the configuration parameters. 
}

bool SynthNotePool::SetNote(int midiNumber, bool pressed, double absoluteTime) const
{
	if (pressed)
	{
		// Already Engaged
		if (_engagedNotes->contains(midiNumber))
			return true;

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
				// Not in Dis-Engaged note pool
				if (!_disengagedNotes->contains(_synthNoteCache->Get(parameters)))
					note = _synthNoteCache->Get(parameters);

				// Dis-Engaged (still ringing): Create new (multiple instances will exist in the cache for the midi number)
				else
				{
					// Cache handles memory for WaveTable*
					WaveTable* waveTable = _waveTableCache->Get(parameters, midiNumber);

					// Synth note cache will handle these
					note = _synthNoteCache->Add(parameters, *_envelope, _signalChain, waveTable, midiNumber);
				}
			}
			else
			{
				// Cache handles memory for WaveTable*
				WaveTable* waveTable = _waveTableCache->Get(parameters, midiNumber);

				// Synth note cache will handle these
				note = _synthNoteCache->Add(parameters, *_envelope, _signalChain, waveTable, midiNumber);
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
			_disengagedNotes->insert(std::make_pair(note, note));

			return true;
		}
	}
}

bool SynthNotePool::SetFrame(PlaybackFrame* frame, double absoluteTime, double gain, double leftRight)
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
	for (auto iter = _disengagedNotes->begin(); iter != _disengagedNotes->end();)
	{
		// HasOutput (ringing out of the note)
		if (iter->first->HasOutput(absoluteTime))
		{
			iter->first->AddSample(&noteFrame, absoluteTime);
			iter++;
		}

		// ...Empty
		else
		{
			iter = _disengagedNotes->erase(iter);
		}
	}

	// MIXING THIS RIGHT AWAY FOR NOW
	// 
	noteFrame.SetFrame(noteFrame.GetLeft() * gain * (1 - leftRight), noteFrame.GetRight() * gain * leftRight);

	frame->SetFrame(&noteFrame);

	return _engagedNotes->size() > 0 || _disengagedNotes->size() > 0;
}

void SynthNotePool::GetSoundBanks(std::vector<std::string>& destination)
{
	_waveTableCache->GetSoundBanks(destination);
}

void SynthNotePool::GetSounds(const std::string& soundBankName, std::vector<std::string>& destination)
{
	_waveTableCache->GetSoundNames(soundBankName, destination);
}