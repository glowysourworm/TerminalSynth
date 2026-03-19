#pragma once
#include "Constant.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalFactoryCore.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceBase.h"
#include "SynthVoiceDirect.h"
#include <exception>

SynthVoiceDirect::SynthVoiceDirect(const SoundRegistry* soundRegistry, const SoundSettings* settings, const PlaybackInfo* playbackInfo)
	: SynthVoiceBase(soundRegistry, settings, playbackInfo)
{
	_core = new SignalFactoryCore(playbackInfo->GetStreamInfo()->streamSampleRate);
}

SynthVoiceDirect::~SynthVoiceDirect()
{
	if (_core != nullptr)
		delete _core;
}

void SynthVoiceDirect::Initialize(const PlaybackInfo* playbackInfo)
{

}

void SynthVoiceDirect::SetFrameImpl(PlaybackFrame* frame)
{
	const OscillatorParameters* parameters = this->GetOscillatorParameters();

	float sample = 0;
	float envelope = this->GetEnvelope()->GetEnvelopeLevel(frame->GetStreamTime());

	switch (parameters->GetType())
	{
		case OscillatorType::BuiltIn:
		{
			switch (parameters->GetBuiltInType())
			{
			case BuiltInOscillators::Sine:
				sample = _core->GenerateSineSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::Square:
				sample = _core->GenerateSquareSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::Sawtooth:
				sample = _core->GenerateSawtoothSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::Triangle:
				sample = _core->GenerateTriangleSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::SynthesizedStringPluck:
				sample = _core->GenerateSawtoothSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkVoice:
				break;
			default:
				throw new std::exception("Unhandled Oscillator Type:  SynthVoiceDirect.cpp");
			}
		}
		break;

		default:
			throw new std::exception("Unhandled Oscillator Type:  SynthVoiceDirect.cpp");
	}

	frame->SetFrame(envelope * sample, envelope * sample);
}