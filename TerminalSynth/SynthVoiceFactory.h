#pragma once

#ifndef SYNTH_VOICE_FACTORY_H
#define SYNTH_VOICE_FACTORY_H

#include "Constant.h"
#include "PlaybackInfo.h"
#include "SoundRegistry.h"
#include "SoundSettings.h"
#include "SynthVoiceDirect.h"
#include "SynthVoicePluckedString.h"
#include "SynthVoicePrimitiveSawtooth.h"
#include "SynthVoicePrimitiveSine.h"
#include "SynthVoicePrimitiveSquare.h"
#include "SynthVoicePrimitiveTriangle.h"
#include <exception>

class SynthVoiceFactory
{
public:

	static SynthVoiceDirect* CreateSynthVoiceDirect(SoundRegistry* soundRegistry, const SoundSettings* soundSettings, const PlaybackInfo* playbackInfo)
	{
		SynthVoiceDirect* result = nullptr;

		switch (soundSettings->GetOscillatorParameters()->GetVoiceType())
		{
		case SynthVoiceType::Primitive:
		{
			switch (soundSettings->GetOscillatorParameters()->GetPrimitiveVoiceType())
			{
			case PrimitiveSynthVoices::Sine:
				result = new SynthVoicePrimitiveSine(soundRegistry, soundSettings, playbackInfo);
				break;
			case PrimitiveSynthVoices::Square:
				result = new SynthVoicePrimitiveSquare(soundRegistry, soundSettings, playbackInfo);
				break;
			case PrimitiveSynthVoices::Triangle:
				result = new SynthVoicePrimitiveTriangle(soundRegistry, soundSettings, playbackInfo);
				break;
			case PrimitiveSynthVoices::Sawtooth:
				result = new SynthVoicePrimitiveSawtooth(soundRegistry, soundSettings, playbackInfo);
				break;
			default:
				throw new std::exception("Unhandled Primitive Voice Type:  SynthVoiceFactory.h");
			}
		}
		break;
		case SynthVoiceType::TerminalSynth:
		{
			switch (soundSettings->GetOscillatorParameters()->GetTerminalVoiceType())
			{
			case TerminalSynthVoices::SynthesizedStringPluck:
				result = new SynthVoicePluckedString(soundRegistry, soundSettings, playbackInfo);
				break;

			default:
				throw new std::exception("Unhandled Terminal Synth Voice Type:  SynthVoiceFactory.h");
			}
		}
		break;
		case SynthVoiceType::Stk:
		{
			switch (soundSettings->GetOscillatorParameters()->GetStkVoiceType())
			{
			case StkSynthVoices::StkRhodey:
				break;
			case StkSynthVoices::StkBeeThree:
				break;
			case StkSynthVoices::StkClarinet:
				break;
			case StkSynthVoices::StkDrummer:
				break;
			case StkSynthVoices::StkFlute:
				break;
			case StkSynthVoices::StkFMVoices:
				break;
			case StkSynthVoices::StkGuitar:
				break;
			case StkSynthVoices::StkHevyMetl:
				break;
			case StkSynthVoices::StkMandolin:
				break;
			case StkSynthVoices::StkMoog:
				break;
			case StkSynthVoices::StkSaxofony:
				break;
			case StkSynthVoices::StkShakers:
				break;
			case StkSynthVoices::StkSitar:
				break;
			case StkSynthVoices::StkTubeBell:
				break;
			case StkSynthVoices::StkVoicForm:
				break;
			case StkSynthVoices::StkWhistle:
				break;
			case StkSynthVoices::StkWurley:
				break;
			default:
				throw new std::exception("Unhandled Stk Voice Type:  SynthVoiceFactory.h");
			}
		}
			break;
		case SynthVoiceType::SoundBank:
			throw new std::exception("Invalid SynthVoiceDirect Type (sound banks are wave table type):  SynthVoiceFactory.h");

		case SynthVoiceType::HarmonicShaper:
			throw new std::exception("Invalid SynthVoiceDirect Type (harmonic shapers are wave table type):  SynthVoiceFactory.h");

		default:
			throw new std::exception("Unhandled Synth Voice Type:  SynthVoiceFactory.h");
		}

		if (result != nullptr)
			result->Initialize(playbackInfo);

		return result;
	}
};

#endif