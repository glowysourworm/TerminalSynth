#include "AtomicLock.h"
#include "AudioController.h"
#include "BaseController.h"
#include "EffectRegistry.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MidiPlaybackDevice.h"
#include "OutputSettings.h"
#include "PlaybackClock.h"
#include "RtAudioUserData.h"
#include "SynthPlaybackDevice.h"
#include "SynthSettings.h"
#include <exception>
#include <string>

AudioController::AudioController(AtomicLock* atomicLock) : BaseController(atomicLock)
{
	_initialized = false;
	_midiMode = false;

	_synthDevice = new SynthPlaybackDevice<float>();
	_midiDevice = new MidiPlaybackDevice<float>();
	_audioTimer = new LoopTimer(0.001);
	_streamClock = new PlaybackClock();
	_synthIntervalTimer = new IntervalTimer();

	_outputL = 0;
	_outputR = 0;
}

AudioController::~AudioController()
{
	if (_initialized)
		this->Dispose();
}

bool AudioController::Initialize(SynthSettings* configuration, OutputSettings* parameters, EffectRegistry* effectRegistry)
{
	if (_initialized)
		throw new std::exception("Audio Controller already initialized!");

	_synthDevice->Initialize(effectRegistry, configuration, parameters);
	_midiDevice->Initialize(effectRegistry, configuration, parameters);

	_initialized = true;

	return _initialized;
}

int AudioController::ProcessAudioCallback(float* outputBuffer, unsigned int numberOfFrames, double streamTime, RtAudioUserData* userData)
{
	// Main Controller Initialization
	if (!userData->IsInitialized())
		return 0;

	// Audio Controller Initialization
	if (!_initialized)
		return 0;

	// Full Audio Loop Timer
	_audioTimer->Mark();

	// Frontend Processing Time (Start!)
	_synthIntervalTimer->Reset();

	SynthSettings* configuration = userData->GetSynthSettings();
	EffectRegistry* effectRegistry = userData->GetEffectRegistry();

	// std::atomic wait loop
	this->Lock->AcquireLock();

	// Update Synth Device (DIRTY FLAG IS IN REAL TIME! WE NEED TO AVOID IT UNTIL THE USER HAS CHANGED A SYNTH SETTING!)
	//if (configuration->IsDirty())
	//{
		if (_midiMode)
			_midiDevice->Update(effectRegistry, configuration);
		else
			_synthDevice->Update(effectRegistry, configuration);
	//}

	int rtAudioReturnValue = 0;

	// Last Output
	bool lastOutput = _midiMode ? _midiDevice->GetLastOutput() : _synthDevice->GetLastOutput();

	// Windows API, SynthSettings*, SynthPlaybackDevice* (be aware of usage)
	//
	bool pressedKeys = _midiMode ? _midiDevice->SetForPlayback(numberOfFrames, streamTime, configuration) :
								   _synthDevice->SetForPlayback(numberOfFrames, streamTime, configuration);

	// Optimize CPU
	if (lastOutput || pressedKeys)
	{
		// Write playback buffer from synth device
		rtAudioReturnValue = _midiMode ? _midiDevice->WritePlaybackBuffer(outputBuffer, numberOfFrames, streamTime, configuration) :
										 _synthDevice->WritePlaybackBuffer(outputBuffer, numberOfFrames, streamTime, configuration);

		// Get output for the UI
		_outputL = _synthDevice->GetOutputLeft();
		_outputR = _synthDevice->GetOutputRight();
	}
	else
	{
		_outputL = 0;
		_outputR = 0;
	}

	// std::atomic end loop
	this->Lock->Release();

	// Frontend Processing Time (Mark.)
	_synthIntervalTimer->Mark();

	return rtAudioReturnValue;
}

void AudioController::Start()
{
	_streamClock->Reset();
	_streamClock->Start();
	_audioTimer->Reset();
}

bool AudioController::Dispose()
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	try
	{
		delete _synthDevice;
		delete _midiDevice;
		delete _streamClock;
		delete _audioTimer;

		_midiDevice = NULL;
		_synthDevice = NULL;
		_streamClock = NULL;
		_audioTimer = NULL;
		_synthIntervalTimer = NULL;

		_initialized = false;

		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}

	return false;
}

void AudioController::SetMidiMode(const std::string& midiFile)
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	_midiMode = true;

	// Prepare MIDI Playback
	_midiDevice->Load(midiFile);
}

void AudioController::SetSynthMode()
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	_midiMode = false;

	// Nothing else to do..
}

void AudioController::GetUpdate(float& streamTime, float& audioTime, float& frontendTime, float& latency, float& left, float& right)
{
	if (!_initialized)
		throw new std::exception("Audio Controller not yet initialized!");

	// These timers appear thread-safe (there may be un-ordered memory usage, but we do not write them)
	//
	streamTime = _streamClock->GetTime();
	audioTime = _audioTimer->GetAvgMilli();
	frontendTime = _synthIntervalTimer->AvgMilli();
	latency = 0;
	left = _outputL;
	right = _outputR;
}
