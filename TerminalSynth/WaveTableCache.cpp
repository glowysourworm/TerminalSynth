#include "Constant.h"
#include "Envelope.h"
#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalFactory.h"
#include "SynthSettings.h"
#include "WaveTable.h"
#include "WaveTableCache.h"
#include "WaveTableCacheKey.h"
#include <exception>
#include <filesystem>
#include <map>
#include <sndfile.h>
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
		// Sound Library Folder:  /{base folder}/{individual library folder(s)}
		for (auto const& dirEntry : std::filesystem::directory_iterator{ synthSettings->GetSoundBankDirectory() })
		{
			// {individual library folder}
			if (dirEntry.is_directory())
			{
				// Sound Bank:  Iterate sound files in sub-directory
				for (auto const& dirSubEntry : std::filesystem::directory_iterator{ dirEntry.path() })
				{
					if (dirSubEntry.path().extension().string() == ".wav" ||
						dirSubEntry.path().extension().string() == ".aif" ||
						dirSubEntry.path().extension().string() == ".aiff")
					{
						int extensionSize = dirSubEntry.path().extension().string().size();
						std::string soundBankName = dirEntry.path().filename().string();
						std::string soundFileName = dirSubEntry.path().filename().string();
						std::string soundName = soundFileName.replace(soundFileName.size() - extensionSize, soundFileName.size() - 1, "");		// Remove Extension

						// (MEMORY!) ~WaveTableCache
						WTCacheKey_SoundBank* cacheKey = new WTCacheKey_SoundBank(soundBankName, soundName, soundFileName);

						_soundBankList->push_back(cacheKey);
					}
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
				OscillatorParameters parameters((BuiltInOscillators)oscillatorType, frequency, SIGNAL_LOW, SIGNAL_HIGH, Envelope());

				// (MEMORY!) ~WaveTableCache
				WTCacheKey_Oscillator* cacheKey = new WTCacheKey_Oscillator(parameters, midiNumber, outputSettings->GetSamplingRate());

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
		// libsndfile:  Small tutorial https://digitalsoundandmusic.com/5-3-3-reading-and-writing-formatted-audio-files-in-c/
		//
		SNDFILE* sndFile;
		SF_INFO sfinfo;

		if ((sndFile = sf_open(cacheKey->GetFileName().c_str(), SFM_READ, &sfinfo)) == NULL)
			return false;

		// Store sound information
		cacheKey->SetSoundFileData(sfinfo.samplerate, sfinfo.channels, sfinfo.frames);

		// (MEMORY!) ~WaveTableCache
		WaveTable* waveTable = new WaveTable(sfinfo.frames, sfinfo.samplerate, _systemSamplingRate);

		waveTable->CreateSamplesByFrame(WaveTable::WaveTableSampleGenerateFrameCallback([&sfinfo, &sndFile](int frameIndex, float& leftSample, float& rightSample)
		{
			// FLOAT
			if ((sfinfo.format & SF_FORMAT_FLOAT) == 0)
			{
				// MONO
				if (sfinfo.channels == 1)
				{
					float buffer[1];
					sf_read_float(sndFile, buffer, 1);
					leftSample = buffer[0];
					rightSample = buffer[0];
				}

				// STEREO
				else if (sfinfo.channels == 2)
				{
					float buffer[2];
					sf_read_float(sndFile, buffer, 2);
					leftSample = buffer[0];
					rightSample = buffer[1];
				}
				else 
					throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
			}
			else if ((sfinfo.format & SF_FORMAT_DOUBLE) == 0)
			{
				// MONO
				if (sfinfo.channels == 1)
				{
					double buffer[1];
					sf_read_double(sndFile, buffer, 1);
					leftSample = buffer[0];
					rightSample = buffer[0];
				}

				// STEREO
				else if (sfinfo.channels == 2)
				{
					double buffer[2];
					sf_read_double(sndFile, buffer, 2);
					leftSample = buffer[0];
					rightSample = buffer[1];
				}
				else
					throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
			}
			else
				throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
		}));

		// Set Cache
		_cacheSoundBank->insert(std::make_pair(cacheKey->GetHashCode(), waveTable));
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
		WaveTable* waveTable = new WaveTable(cacheKey->GetNumberOfFrames(), cacheKey->GetSampleRate(), _systemSamplingRate);
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

	// Set for frequency

	return result;
}

WaveTable* WaveTableCache::Get(const std::string& soundBankName, const std::string& soundName, float desiredFrequency)
{
	WTCacheKey_SoundBank* cacheKey = nullptr;

	for (int index = 0; index < _soundBankList->size(); index++)
	{
		// This should match the correct file
		if (_soundBankList->at(index)->GetSoundBank() == soundBankName &&
			_soundBankList->at(index)->GetName() == soundName)
		{
			cacheKey = _soundBankList->at(index);
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