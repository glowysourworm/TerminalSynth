#pragma once
#include "Constant.h"
#include "OscillatorParameters.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "PlaybackTime.h"
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

void SynthVoiceDirect::SetFrameImpl(PlaybackFrame* frame, const PlaybackTime* playbackTime)
{
	const OscillatorParameters* parameters = this->GetOscillatorParameters();

	float sample = 0;
	float envelope = this->GetEnvelope()->GetEnvelopeLevel(playbackTime);
	float frequency = this->GetNoteProcessor()->GetNextFrequency(frame, playbackTime);

	switch (parameters->GetType())
	{
		case OscillatorType::BuiltIn:
		{
			switch (parameters->GetBuiltInType())
			{
			case BuiltInOscillators::Sine:
				sample = _core->GenerateSineSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::Square:
				sample = _core->GenerateSquareSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::Sawtooth:
				sample = _core->GenerateSawtoothSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::Triangle:
				sample = _core->GenerateTriangleSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::SynthesizedStringPluck:
				sample = _core->GenerateSawtoothSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkRhodey:
				sample = _core->GenerateRhodeySample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkBeeThree:
				sample = _core->GenerateBeeThreeSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkClarinet:
				sample = _core->GenerateClarinetSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkDrummer:
				sample = _core->GenerateDrummerSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkFlute:
				sample = _core->GenerateFluteSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkFMVoices:
				sample = _core->GenerateFMVoicesSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkGuitar:
				sample = _core->GenerateGuitarSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkHevyMetl:
				sample = _core->GenerateHevyMetlSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkMandolin:
				sample = _core->GenerateMandolinSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkMoog:
				sample = _core->GenerateMoogSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkSaxofony:
				sample = _core->GenerateSaxofonySample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkShakers:
				sample = _core->GenerateShakersSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkSitar:
				sample = _core->GenerateSitarSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkTubeBell:
				sample = _core->GenerateTubeBellSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkVoicForm:
				sample = _core->GenerateVoicFormSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkWhistle:
				sample = _core->GenerateWhistleSample(frequency, playbackTime);
				break;
			case BuiltInOscillators::StkWurley:
				sample = _core->GenerateWurleySample(frequency, playbackTime);
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