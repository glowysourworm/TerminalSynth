#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactory.h"
#include "SoundFileReader.h"
#include "SynthSettings.h"
#include "WaveTable.h"
#include "WaveTableCache.h"
#include "WaveTableCacheKey.h"
#include <exception>
#include <map>
#include <string>
#include <utility>
#include <vector>

WaveTableCache::WaveTableCache()
{
	_soundBankList = new std::vector<WTCacheKey_SoundBank*>();
	_oscillatorList = new std::vector<WTCacheKey_Oscillator*>();

	// Lazy loaded during playback
	_cacheSoundBank = new std::map<size_t, WaveTable*>();
	_cacheOscillator = new std::map<size_t, WaveTable*>();
}

WaveTableCache::~WaveTableCache()
{
	for (int index = 0; index < _soundBankList->size(); index++)
	{
		delete _soundBankList->at(index);
	}
	for (int index = 0; index < _oscillatorList->size(); index++)
	{
		delete _oscillatorList->at(index);
	}
	for (auto iter = _cacheSoundBank->begin(); iter != _cacheSoundBank->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _cacheOscillator->begin(); iter != _cacheOscillator->end(); ++iter)
	{
		delete iter->second;
	}

	delete _soundBankList;
	delete _oscillatorList;
	delete _cacheSoundBank;
	delete _cacheOscillator;
	delete _signalFactory;
}

bool WaveTableCache::Initialize(const SynthSettings* synthSettings, const OutputSettings* outputSettings)
{
	// Oscillator Sampling Rate:  This may be set differently to oversample the oscillator
	//
	_systemSamplingRate = outputSettings->GetSamplingRate();
	_oscillatorSamplingRate = outputSettings->GetSamplingRate();

	_signalFactory = new SignalFactory(outputSettings);
	
	bool success = Initialize_SoundBanks(synthSettings, outputSettings);
	success &= Initialize_Oscillators(synthSettings, outputSettings);

	return success;
}

bool WaveTableCache::Initialize_SoundBanks(const SynthSettings* synthSettings, const OutputSettings* outputSettings)
{
	try
	{
		std::vector<std::string> soundBanks = synthSettings->GetSoundBankSettings()->GetSoundBanks();

		// Sound Bank(s)
		for (int index = 0; index < soundBanks.size(); index++)
		{
			std::vector<std::string> soundNames = synthSettings->GetSoundBankSettings()->GetSoundNames(soundBanks[index]);

			// Sound Name(s)
			for (int nameIndex = 0; nameIndex < soundNames.size(); nameIndex++)
			{
				// MIDI Frequencies
				//
				for (int midiNumber = MIDI_PIANO_LOW_NUMBER; midiNumber <= MIDI_PIANO_HIGH_NUMBER; midiNumber++)
				{
					std::string soundFileName = synthSettings->GetSoundBankSettings()->GetSoundFileName(soundBanks[index], soundNames[nameIndex]);

					// This will be over / undersampled at this "fundamental" frequency to create the sample table
					float frequency = TerminalSynth::GetMidiFrequency(midiNumber);	

					// (MEMORY!) ~WaveTableCache
					WTCacheKey_SoundBank* cacheKey = new WTCacheKey_SoundBank(frequency, soundBanks[index], soundNames[nameIndex], soundFileName);

					_soundBankList->push_back(cacheKey);
				}
			}
		}
	}
	catch (std::exception ex)
	{
		return false;
	}

	return true;
}

bool WaveTableCache::Initialize_Oscillators(const SynthSettings* synthSettings, const OutputSettings* outputSettings)
{
	try
	{
		// Create cache entries: Per Midi Number * Per Oscillator Type
		for (int midiNumber = synthSettings->GetMidiLow(); midiNumber <= synthSettings->GetMidiHigh(); midiNumber++)
		{
			// Oscillator Types
			for (int oscillatorType = 0; oscillatorType <= (int)BuiltInOscillators::SynthesizedStringPluck; oscillatorType++)
			{
				float frequency = TerminalSynth::GetMidiFrequency(midiNumber);
				OscillatorParameters parameters(OscillatorType::BuiltIn, (BuiltInOscillators)oscillatorType, "", "", frequency, SIGNAL_LOW, SIGNAL_HIGH);

				// (MEMORY!) ~WaveTableCache (Also, note oversampling factor!) (this is propagated using the second sampling rate in WaveTable*)
				WTCacheKey_Oscillator* cacheKey = new WTCacheKey_Oscillator(parameters, midiNumber, outputSettings->GetSamplingRate() * synthSettings->GetOversamplingFactor());

				_oscillatorList->push_back(cacheKey);
			}
		}
	}
	catch (std::exception ex)
	{
		return false;
	}

	return true;
}

bool WaveTableCache::CreateWaveTable(WTCacheKey_SoundBank* cacheKey)
{
	try
	{
		SoundFileReader reader(cacheKey->GetFileName());

		if (!reader.Open())
			return false;

		int numberFrames = reader.GetNumberFrames();
		int numberChannels = reader.GetNumberChannels();
		int sampleRate = reader.GetSampleRate();

		// (MEMORY!) Delete locally below
		PlaybackFrame* frames = reader.Read();

		// Store sound information
		cacheKey->SetSoundFileData(sampleRate, numberChannels, numberFrames);

		// Calculate number of frames: # Frames * (Sample Rate / System Sampling Rate) * (frequency / fundamental) (USING A FUNDAMENTAL OF C4 = MIDI(60))
		unsigned int adjustedNumberFrames = numberFrames * (sampleRate / (double)_systemSamplingRate) * (cacheKey->GetDesiredFrequency() / TerminalSynth::GetMidiFrequency(60));
		float adjustedSamplingRate = sampleRate * (cacheKey->GetDesiredFrequency() / TerminalSynth::GetMidiFrequency(60));
		//float adjustedSamplingRate = sampleRate;

		// (MEMORY!) ~WaveTableCache
		WaveTable* waveTable = new WaveTable(WaveTable::Mode::SoundSample, adjustedNumberFrames, adjustedSamplingRate, _systemSamplingRate);

		waveTable->CreateSamplesByFrame(WaveTable::WaveTableSampleGenerateFrameCallback([&frames, &adjustedNumberFrames, &numberFrames]
		(int frameIndex, float& leftSample, float& rightSample)
		{
			// Calculate frame index:  Adjusted number was used based on the above parameters. We need to figure out from where
			//						   in the file frames it should draw samples.
			//
			int fileFrameIndex = (int)(frameIndex * (numberFrames / (double)adjustedNumberFrames));

			if (fileFrameIndex > numberFrames)
				throw new std::exception("Outside the bounds of the sound bank file");

			leftSample = frames[fileFrameIndex].GetLeft();
			rightSample = frames[fileFrameIndex].GetRight();
		}));

		// Set Cache
		_cacheSoundBank->insert(std::make_pair(cacheKey->GetHashCode(), waveTable));

		delete[] frames;
	}
	catch (std::exception ex)
	{
		return false;
	}

	return true;
}

bool WaveTableCache::CreateWaveTable(WTCacheKey_Oscillator* cacheKey)
{
	try
	{
		// (MEMORY!) ~WaveTableCache
		WaveTable* waveTable = new WaveTable(WaveTable::Mode::Periodic, 
											 cacheKey->GetNumberOfFrames(), 
										     cacheKey->GetSampleRate(), 
											 _systemSamplingRate);
		SignalFactory* signalFactory = _signalFactory;
		OscillatorParameters parameters = cacheKey->GetParameters();
		PlaybackFrame frame;

		// Sets SignalFactory* for next oscillator
		signalFactory->Reset(cacheKey->GetParameters());

		// Create Samples
		waveTable->CreateSamplesByTime(WaveTable::WaveTableSampleGenerateSecondCallback([&signalFactory, &parameters, &frame](float sampleTime, float& leftSample, float& rightSample)
		{
			signalFactory->GenerateSample(parameters, frame, sampleTime);
			leftSample = frame.GetLeft();
			rightSample = frame.GetRight();
		}));


		_cacheOscillator->insert(std::make_pair(cacheKey->GetHashCode(), waveTable));
	}
	catch (std::exception ex)
	{
		return false;
	}

	return true;
}

void WaveTableCache::GetSoundBanks(std::vector<std::string>& destination)
{
	std::map<std::string, bool> soundBankMap;

	for (int index = 0; index < _soundBankList->size(); index++)
	{
		if (!soundBankMap.contains(_soundBankList->at(index)->GetSoundBank()))
		{
			destination.push_back(_soundBankList->at(index)->GetSoundBank());
			soundBankMap.insert(std::make_pair(_soundBankList->at(index)->GetName(), true));
		}
	}
}

void WaveTableCache::GetSoundNames(const std::string& soundBank, std::vector<std::string>& destination)
{
	for (int index = 0; index < _soundBankList->size(); index++)
	{
		if (_soundBankList->at(index)->GetSoundBank() == soundBank)
			destination.push_back(_soundBankList->at(index)->GetName());
	}
}

WaveTable* WaveTableCache::Get(const OscillatorParameters& parameters, int midiNumber)
{
	// Other overload...
	if (parameters.GetType() == OscillatorType::SampleBased)
		return Get(parameters.GetSoundBank(), parameters.GetSoundName(), parameters.GetFrequency());

	WTCacheKey_Oscillator* cacheKey = nullptr;
	WTCacheKey_Oscillator cacheMatch(parameters, midiNumber, _oscillatorSamplingRate);

	for (int index = 0; index < _oscillatorList->size(); index++)
	{
		if (_oscillatorList->at(index)->GetHashCode() == cacheMatch.GetHashCode())
		{
			cacheKey = _oscillatorList->at(index);
			break;
		}
	}

	if (cacheKey == nullptr)
		throw new std::exception("Sound bank not found:  WaveTableCache.cpp");

	// New WaveTable*
	if (!_cacheOscillator->contains(cacheKey->GetHashCode()))
	{
		if (!CreateWaveTable(cacheKey))
			throw new std::exception("Unable to create wave table for oscillator:  WaveTableCache.cpp");
	}

	WaveTable* result = _cacheOscillator->at(cacheKey->GetHashCode());

	// Set for frequency:  Not required for oscillator-based pre-built wave tables. Just for sound banks...

	return result;
}

void WaveTableCache::Clear()
{
	// Just clear the cached WaveTable* instances, not the lists
	//

	for (auto iter = _cacheSoundBank->begin(); iter != _cacheSoundBank->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _cacheOscillator->begin(); iter != _cacheOscillator->end(); ++iter)
	{
		delete iter->second;
	}

	_cacheSoundBank->clear();
	_cacheOscillator->clear();
}

WaveTable* WaveTableCache::Get(const std::string& soundBankName, const std::string& soundName, float desiredFrequency)
{
	WTCacheKey_SoundBank* cacheKey = nullptr;

	for (int index = 0; index < _soundBankList->size(); index++)
	{
		// This should match the correct file
		if (_soundBankList->at(index)->GetSoundBank() == soundBankName &&
			_soundBankList->at(index)->GetName() == soundName &&
			_soundBankList->at(index)->GetDesiredFrequency() == desiredFrequency)
		{
			cacheKey = _soundBankList->at(index);
			break;
		}
	}

	if (cacheKey == nullptr)
		throw new std::exception("Sound bank not found:  WaveTableCache.cpp");

	// New WaveTable*
	if (!_cacheSoundBank->contains(cacheKey->GetHashCode()))
	{
		if (!CreateWaveTable(cacheKey))
			throw new std::exception("Unable to create wave table for oscillator:  WaveTableCache.cpp");
	}

	WaveTable* result = _cacheSoundBank->at(cacheKey->GetHashCode());

	// Set for frequency

	return result;
}