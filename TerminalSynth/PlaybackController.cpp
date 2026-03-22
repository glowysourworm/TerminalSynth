#include "AtomicLock.h"
#include "BaseController.h"
#include "Constant.h"
#include "EqualizerOutput.h"
#include "IntervalTimer.h"
#include "LoopTimer.h"
#include "MidiPlaybackDevice.h"
#include "PlaybackClock.h"
#include "PlaybackController.h"
#include "PlaybackFormatTransformer.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
#include "PlaybackUserData.h"
#include "SoundRegistry.h"
#include "SynthPlaybackDevice.h"
#include "SynthSettings.h"
#include <exception>
#include <string>

PlaybackController::PlaybackController(AtomicLock* playbackLock) : BaseController(playbackLock)
{
	_initialized = false;
	_midiMode = false;

	_synthDevice = new SynthPlaybackDevice();
	_midiDevice = new MidiPlaybackDevice();
	_streamClock = new PlaybackClock();
	_audioTimer = new LoopTimer(0.001);
	_audioSampleTimer = new IntervalTimer();
	_audioLockAcquireTimer = new IntervalTimer();
	_playbackTime = new PlaybackTime();
}

PlaybackController::~PlaybackController()
{
	if (_initialized)
		this->Dispose();
}

bool PlaybackController::Initialize(PlaybackUserData* playbackData)
{
	if (_initialized)
		throw new std::exception("Audio Controller already initialized!");

	_synthDevice->Initialize(playbackData->GetEffectRegistry(), playbackData->GetSynthSettings(), playbackData->GetPlaybackInfo());
	_midiDevice->Initialize(playbackData->GetEffectRegistry(), playbackData->GetSynthSettings(), playbackData->GetPlaybackInfo());

	_initialized = true;

	return _initialized;
}

int PlaybackController::ProcessAudioCallback(void* outputBuffer, AudioStreamFormat streamFormat, unsigned int numberOfFrames, double streamTime, double streamLatency, PlaybackUserData* userData)
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
	PlaybackInfo* outputSettings = userData->GetPlaybackInfo();
	EqualizerOutput* equalizer = userData->GetEqualizer();

	// Some RT Updates
	float avgAudioMilli = _audioTimer->GetAvgMilli();
	float avgAudioSampleMicro = _audioSampleTimer->AvgMicro();
	float avgAudioLockAcquireNano = _audioLockAcquireTimer->AvgNano();

	// std::atomic wait loop (timing the lock acquire)
	_audioLockAcquireTimer->Reset();
	this->PlaybackLock->AcquireLock();
	_audioLockAcquireTimer->Mark();

	// Update Synth Device (DIRTY FLAG IS IN REAL TIME! WE NEED TO AVOID IT UNTIL THE USER HAS CHANGED A SYNTH SETTING!)
	if (configuration->IsDirty())
	{
		if (_midiMode)
			_midiDevice->Update(effectRegistry, configuration, outputSettings);
		else
			_synthDevice->Update(effectRegistry, configuration, outputSettings);

		configuration->ClearDirty();
	}

	// Write Output Buffer:  The PlaybackDevice* is handled per frame. Since the SynthPlaybackDevice* sets all notes at
	//						 once, the call should only be made on the first frame.
	//
	//						 The PlaybackTime* is updated each iteration. THE STREAM TIME WILL ONLY BE APPROXIMATE! There
	//						 have been issues using the stream time to do sampling. So, the sample time is calculated using
	//						 the frame cursor.
	//
	
	// Synth Device:  Pressed notes, or check Midi Device each frame
	bool hasOutput = !_midiMode ? _synthDevice->SetForFrame(*_playbackTime, configuration) : false;
	bool sampleSuccess = true;

	float gain = configuration->GetGain();
	float leftRight = configuration->GetLeftRightBalance();

	PlaybackFrame playbackFrame(0, 0);

	// Output || Midi Mode
	for (int frameIndex = 0; frameIndex < numberOfFrames && sampleSuccess; frameIndex++)
	{
		if (_midiMode)
			hasOutput = _midiDevice->SetForFrame(*_playbackTime, configuration);

		//if (hasOutput)
		//{
			// Audio Sample Timer
			_audioSampleTimer->Reset();

			sampleSuccess = _midiMode ? _midiDevice->WriteSample(playbackFrame, *_playbackTime, gain, leftRight) :
										_synthDevice->WriteSample(playbackFrame, *_playbackTime, gain, leftRight);

			_audioSampleTimer->Mark();

			// Apply Sample Frame
			WriteBufferWithTransform(outputBuffer, streamFormat, playbackFrame, frameIndex);

			// Apply Sample to Equalizer
			equalizer->AddSample(playbackFrame.GetLeft(), playbackFrame.GetRight());

			// Clear Sample Frame (any reference to the frame has been copied by the backend; and will be stored separately)
			playbackFrame.ClearSample();

			// Stream Time:  PRIMARY STREAM TIME SOURCE (Incrementing, instead of querying the stream source). There could be
			//				 real time audio forums about how to do this. It may be more accurate to query; but there could
			//				 be a problem getting the latest stream time (perhaps a mutex, but not likely). It's better to 
			//				 use the frame cursor to get the stream time; but this will set an equivalent, anyway.
			//
			_playbackTime->streamTime += 1 / outputSettings->GetStreamInfo()->streamSampleRate;
			_playbackTime->frameCursor++;
		//}
	}

	// RT Update (Audio)
	outputSettings->UpdateRT_Audio(streamTime, avgAudioMilli, avgAudioSampleMicro, avgAudioLockAcquireNano, streamLatency);

	// std::atomic end loop
	this->PlaybackLock->Release();

	// NEED ERROR CODE ENUMS
	return sampleSuccess ? 0 : -1;
}

void PlaybackController::WriteBufferWithTransform(void* outputBuffer, AudioStreamFormat streamFormat, const PlaybackFrame& frame, int frameIndex)
{
	char* buffer = (char*)outputBuffer;

	// Transform buffers (two floats)
	int frameSize = 4;
	char leftBuffer[4];
	char rightBuffer[4];

	// TRANSFORM STREAM:  The byte stream must match the output format
	PlaybackFormatTransformer::Transform(streamFormat, frame.GetLeft(), leftBuffer, frameSize);
	PlaybackFormatTransformer::Transform(streamFormat, frame.GetRight(), rightBuffer, frameSize);

	// Write Transformed Buffer
	for (int index = 0; index < frameSize; index++)
	{
		// Interleved frames
		buffer[(2 * frameIndex * frameSize) + index] = leftBuffer[index];
		buffer[(2 * frameIndex * frameSize) + frameSize + index] = rightBuffer[index];
	}
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
	delete _playbackTime;

	_midiDevice = nullptr;
	_synthDevice = nullptr;
	_streamClock = nullptr;
	_audioTimer = nullptr;
	_audioSampleTimer = nullptr;
	_audioLockAcquireTimer = nullptr;
	_playbackTime = nullptr;

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
