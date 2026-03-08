#include "AtomicLock.h"
#include "BaseController.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MidiPlaybackDevice.h"
#include "OutputSettings.h"
#include "PlaybackClock.h"
#include "PlaybackController.h"
#include "RtAudioUserData.h"
#include "SoundRegistry.h"
#include "SynthPlaybackDevice.h"
#include "SynthSettings.h"
#include <exception>
#include <string>

PlaybackController::PlaybackController(AtomicLock* playbackLock) : BaseController(playbackLock)
{
	_initialized = false;
	_midiMode = false;

	_synthDevice = new SynthPlaybackDevice<float>();
	_midiDevice = new MidiPlaybackDevice<float>();
	_streamClock = new PlaybackClock();
	_audioTimer = new LoopTimer(0.001);
	_audioSampleTimer = new IntervalTimer();
	_audioLockAcquireTimer = new IntervalTimer();
}

PlaybackController::~PlaybackController()
{
	if (_initialized)
		this->Dispose();
}

bool PlaybackController::Initialize(SynthSettings* configuration, OutputSettings* parameters, SoundRegistry* effectRegistry)
{
	if (_initialized)
		throw new std::exception("Audio Controller already initialized!");

	_synthDevice->Initialize(effectRegistry, configuration, parameters);
	_midiDevice->Initialize(effectRegistry, configuration, parameters);

	_initialized = true;

	return _initialized;
}

int PlaybackController::ProcessAudioCallback(float* outputBuffer, unsigned int numberOfFrames, double streamTime, double streamLatency, RtAudioUserData* userData)
{
	// Main Controller Initialization
	if (!userData->IsInitialized())
		return 0;

	// Audio Controller Initialization
	if (!_initialized)
		return 0;

	// Full Audio Loop Timer
	_audioTimer->Mark();

	SynthSettings* configuration = userData->GetSynthSettings();
	SoundRegistry* effectRegistry = userData->GetEffectRegistry();
	OutputSettings* outputSettings = userData->GetOutputSettings();

	// Some RT Updates
	float avgAudioMilli = _audioTimer->GetAvgMilli();
	float avgAudioSampleMicro = _audioSampleTimer->AvgMicro();
	float avgAudioLockAcquireNano = _audioLockAcquireTimer->AvgNano();
	float leftChannel;
	float rightChannel;

	// std::atomic wait loop (timing the lock acquire)
	_audioLockAcquireTimer->Reset();
	this->PlaybackLock->AcquireLock();
	_audioLockAcquireTimer->Mark();

	// Update Synth Device (DIRTY FLAG IS IN REAL TIME! WE NEED TO AVOID IT UNTIL THE USER HAS CHANGED A SYNTH SETTING!)
	if (configuration->IsDirty())
	{
		if (_midiMode)
			_midiDevice->Update(effectRegistry, configuration);
		else
			_synthDevice->Update(effectRegistry, configuration);

		configuration->ClearDirty();
	}

	int rtAudioReturnValue = 0;

	// Last Output
	bool lastOutput = _midiMode ? _midiDevice->GetLastOutput() : _synthDevice->GetLastOutput();

	// Windows API, SynthSettings*, SynthPlaybackDevice* (be aware of usage)
	//
	bool pressedKeys = _midiMode ? _midiDevice->SetForPlayback(numberOfFrames, streamTime, configuration) :
								   _synthDevice->SetForPlayback(numberOfFrames, streamTime, configuration);

	// Optimize CPU
	//if (lastOutput || pressedKeys)
	//{
		// Audio Sample Timer
		_audioSampleTimer->Reset();

		// Write playback buffer from synth device
		rtAudioReturnValue = _midiMode ? _midiDevice->WritePlaybackBuffer(outputBuffer, numberOfFrames, streamTime, outputSettings) :
										 _synthDevice->WritePlaybackBuffer(outputBuffer, numberOfFrames, streamTime, outputSettings);

		_audioSampleTimer->Mark();
	//}

	// RT Update (Audio)
	outputSettings->UpdateRT_Audio(streamTime, avgAudioMilli, avgAudioSampleMicro, avgAudioLockAcquireNano, 0);
	outputSettings->SetStreamLatency(streamLatency);

	// std::atomic end loop
	this->PlaybackLock->Release();

	return rtAudioReturnValue;
}

void PlaybackController::Start()
{
	_streamClock->Reset();
	_streamClock->Start();
	_audioTimer->Reset();
	_audioSampleTimer->Reset();
	_audioLockAcquireTimer->Reset();
}

bool PlaybackController::Dispose()
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	delete _synthDevice;
	delete _midiDevice;
	delete _streamClock;
	delete _audioTimer;
	delete _audioSampleTimer;
	delete _audioLockAcquireTimer;

	_midiDevice = nullptr;
	_synthDevice = nullptr;
	_streamClock = nullptr;
	_audioTimer = nullptr;
	_audioSampleTimer = nullptr;
	_audioLockAcquireTimer = nullptr;

	_initialized = false;

	return true;
}

void PlaybackController::SetMidiMode(const std::string& midiFile)
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	_midiMode = true;

	// Prepare MIDI Playback
	_midiDevice->Load(midiFile);
}

void PlaybackController::SetSynthMode()
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	_midiMode = false;

	// Nothing else to do..
}
