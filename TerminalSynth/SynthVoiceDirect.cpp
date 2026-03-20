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
			case BuiltInOscillators::StkRhodey:
				sample = _core->GenerateRhodeySample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkBeeThree:
				sample = _core->GenerateBeeThreeSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkClarinet:
				sample = _core->GenerateClarinetSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkDrummer:
				sample = _core->GenerateDrummerSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkFlute:
				sample = _core->GenerateFluteSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkFMVoices:
				sample = _core->GenerateFMVoicesSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkGuitar:
				sample = _core->GenerateGuitarSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkHevyMetl:
				sample = _core->GenerateHevyMetlSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkMandolin:
				sample = _core->GenerateMandolinSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkMoog:
				sample = _core->GenerateMoogSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkSaxofony:
				sample = _core->GenerateSaxofonySample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkShakers:
				sample = _core->GenerateShakersSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkSitar:
				sample = _core->GenerateSitarSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkTubeBell:
				sample = _core->GenerateTubeBellSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkVoicForm:
				sample = _core->GenerateVoicFormSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkWhistle:
				sample = _core->GenerateWhistleSample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
				break;
			case BuiltInOscillators::StkWurley:
				sample = _core->GenerateWurleySample(this->GetFrequency(), frame->GetTimeCursor(), frame->GetStreamTime());
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