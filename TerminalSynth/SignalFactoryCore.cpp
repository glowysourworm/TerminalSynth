#include "Constant.h"
#include "OscillatorParameters.h"
#include "PlaybackTime.h"
#include "SignalFactoryCore.h"
#include <BeeThree.h>
#include <Clarinet.h>
#include <Drummer.h>
#include <FMVoices.h>
#include <Flute.h>
#include <Guitar.h>
#include <HevyMetl.h>
#include <Mandolin.h>
#include <Moog.h>
#include <Rhodey.h>
#include <Saxofony.h>
#include <Shakers.h>
#include <Sitar.h>
#include <TubeBell.h>
#include <VoicForm.h>
#include <Whistle.h>
#include <Wurley.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <vector>

SignalFactoryCore::SignalFactoryCore(float samplingRate)
{
	_randomQuadrantValues = new std::vector<float>();
	_stkRhodey = new stk::Rhodey();
	_stkB3 = new stk::BeeThree();
	_stkClarinet = new stk::Clarinet();
	_stkDrummer = new stk::Drummer();
	_stkFMVoices = new stk::FMVoices();
	_stkFlute = new stk::Flute(440);
	_stkGuitar = new stk::Guitar();
	_stkHevyMetl = new stk::HevyMetl();
	_stkMandolin = new stk::Mandolin(440);
	_stkMoog = new stk::Moog();
	_stkSaxofony = new stk::Saxofony(440);
	_stkShakers = new stk::Shakers(10);
	_stkSitar = new stk::Sitar();
	_stkTubeBell = new stk::TubeBell();
	_stkVoicForm = new stk::VoicForm();
	_stkWhistle = new stk::Whistle();
	_stkWurley = new stk::Wurley();
	_samplingRate = samplingRate;
	_signalHigh = SIGNAL_HIGH;
	_signalLow = SIGNAL_LOW;
}

SignalFactoryCore::~SignalFactoryCore()
{
	delete _randomQuadrantValues;
	delete _stkRhodey;
	delete _stkB3;
	delete _stkClarinet;
	delete _stkDrummer;
	delete _stkFlute;
	delete _stkFMVoices;
	delete _stkGuitar;
	delete _stkHevyMetl;
	delete _stkMandolin;
	delete _stkMoog;
	delete _stkSaxofony;
	delete _stkShakers;
	delete _stkSitar;
	delete _stkTubeBell;
	delete _stkVoicForm;
	delete _stkWhistle;
	delete _stkWurley;
}

void SignalFactoryCore::Reset(const OscillatorParameters* parameters)
{
	_signalHigh = parameters->GetSignalHigh();
	_signalLow = parameters->GetSignalLow();
	_randomQuadrantValues->clear();

	_stkRhodey->clear();
	_stkRhodey->setFrequency(parameters->GetFrequency());
	_stkRhodey->setGain(0, 1);
	_stkRhodey->setGain(1, 1);
	_stkRhodey->setGain(2, 1);
	_stkRhodey->setGain(3, 1);
	_stkRhodey->noteOn(parameters->GetFrequency(), 1);

	_stkB3->clear();
	_stkB3->setFrequency(parameters->GetFrequency());
	_stkB3->setGain(0, 1);
	_stkB3->setGain(1, 1);
	_stkB3->setGain(2, 1);
	_stkB3->setGain(3, 1);
	_stkB3->setModulationDepth(0.05);
	_stkB3->setModulationSpeed(6.0);
	_stkB3->noteOn(parameters->GetFrequency(), 1);

	_stkClarinet->clear();
	_stkClarinet->noteOn(parameters->GetFrequency(), 1);

	_stkDrummer->clear();
	_stkDrummer->noteOn(parameters->GetFrequency(), 1);
	
	_stkFlute->clear();
	_stkFlute->noteOn(parameters->GetFrequency(), 1);

	_stkFMVoices->clear();
	_stkFMVoices->setFrequency(parameters->GetFrequency());
	_stkFMVoices->setRatio(0, 0.1);
	_stkFMVoices->setRatio(1, 0.15);
	_stkFMVoices->setRatio(2, 0.2);
	_stkFMVoices->setRatio(3, 0.25);
	_stkFMVoices->setModulationDepth(0.05);
	_stkFMVoices->setModulationSpeed(6);
	_stkFMVoices->noteOn(parameters->GetFrequency(), 1);

	_stkGuitar->clear();
	_stkGuitar->noteOn(parameters->GetFrequency(), 1);

	_stkHevyMetl->clear();
	_stkHevyMetl->noteOn(parameters->GetFrequency(), 1);

	_stkMandolin->clear();
	_stkMandolin->noteOn(parameters->GetFrequency(), 1);

	_stkMoog->clear();
	_stkMoog->noteOn(parameters->GetFrequency(), 1);

	_stkSaxofony->clear();
	_stkSaxofony->noteOn(parameters->GetFrequency(), 1);

	_stkShakers->clear();
	_stkShakers->noteOn(parameters->GetFrequency(), 100);

	_stkSitar->clear();
	_stkSitar->noteOn(parameters->GetFrequency(), 1);

	_stkTubeBell->clear();
	_stkTubeBell->noteOn(parameters->GetFrequency(), 1);

	_stkVoicForm->clear();
	_stkVoicForm->setPhoneme("uuu");
	_stkVoicForm->setPitchSweepRate(1);
	_stkVoicForm->setVoiced(1);
	_stkVoicForm->setFilterSweepRate(0, 1);
	_stkVoicForm->setFilterSweepRate(1, 0.5);
	_stkVoicForm->setFilterSweepRate(2, 0.3);
	_stkVoicForm->setFilterSweepRate(3, 0.1);
	_stkVoicForm->noteOn(parameters->GetFrequency(), 1);

	_stkWhistle->clear();
	_stkWhistle->noteOn(parameters->GetFrequency(), 1);

	_stkWurley->clear();
	_stkWurley->setRatio(0, 1);
	_stkWurley->setRatio(1, 1);
	_stkWurley->setRatio(2, 1);
	_stkWurley->setRatio(3, 1);
	_stkWurley->noteOn(parameters->GetFrequency(), 1);

	for (int index = 0; index < RANDOM_OSCILLATOR_LENGTH; index++)
	{
		_randomQuadrantValues->push_back(((_signalHigh - _signalLow) * (rand() / (float)RAND_MAX)) + _signalLow);
	}
}

float SignalFactoryCore::GetFrequency(unsigned int midiNote)
{
	return TerminalSynth::GetMidiFrequency(midiNote);		// We need a namespace!
}

float SignalFactoryCore::GenerateTriangleSample(float frequency, const PlaybackTime* playbackTime)
{
	float period = 1 / frequency;
	float periodQuarter = 0.25f * period;
	float high = _signalHigh;
	float low = _signalLow;
	float sample = 0;

	// Using modulo arithmetic to get the relative period time
	float periodTime = fmod(playbackTime->FromCursor(_samplingRate), period);

	// First Quadrant
	if (periodTime < periodQuarter)
	{
		sample = (2.0 * (high - low) / period) * periodTime;
	}

	// Second Quadrant
	else if (periodTime < (2.0 * periodQuarter))
	{
		sample = ((-2.0 * (high - low) / period) * (periodTime - (float)periodQuarter)) - low;
	}

	// Third Quadrant
	else if (periodTime < 3.0 * periodQuarter)
	{
		sample = (-2.0 * (high - low) / period) * (periodTime - (2.0 * periodQuarter));
	}

	// Fourth Quadrant
	else
	{
		sample = ((2.0 * (high - low) / period) * (periodTime - (3.0 * periodQuarter))) + low;
	}

	return sample;
}

float SignalFactoryCore::GenerateSquareSample(float frequency, const PlaybackTime* playbackTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmod(playbackTime->FromCursor(_samplingRate), period);
	float sample = 0;

	if (periodTime < period / 2.0)
		sample = _signalHigh;

	else
		sample = _signalLow;

	return sample;
}

float SignalFactoryCore::GenerateSawtoothSample(float frequency, const PlaybackTime* playbackTime)
{
	// Using modulo arithmetic to get the relative period time
	float period = 1 / frequency;
	float periodTime = fmod(playbackTime->FromCursor(_samplingRate), period);

	return (((_signalHigh - _signalLow) / period) * periodTime) + _signalLow;
}

float SignalFactoryCore::GenerateSineSample(float frequency, const PlaybackTime* playbackTime)
{
	return (0.5f * (_signalHigh - _signalLow) * sinf(2.0 * std::numbers::pi * frequency * playbackTime->FromCursor(_samplingRate))) + (0.5f * (_signalHigh + _signalLow));
}

float SignalFactoryCore::GenerateRandomSample(float frequency, const PlaybackTime* playbackTime)
{
	float period = 1 / frequency;

	// Using modulo arithmetic to get the relative period time
	float periodTime = fmod(playbackTime->FromCursor(_samplingRate), period);

	float periodDiv = (period / _randomQuadrantValues->size());
	float periodBucket = periodTime / periodDiv;

	float periodBucketIndex = 0;
	float remainder = std::modf(periodBucket, &periodBucketIndex);
	int periodIndex = std::min<int>(periodBucketIndex, _randomQuadrantValues->size() - 1);
	periodIndex = std::max<int>(periodIndex, 0);

	return _randomQuadrantValues->at(periodIndex);
}
float SignalFactoryCore::GenerateRhodeySample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkRhodey->tick();
}
float SignalFactoryCore::GenerateBeeThreeSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkB3->tick();
}
float SignalFactoryCore::GenerateClarinetSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkClarinet->tick();
}
float SignalFactoryCore::GenerateDrummerSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkDrummer->tick();
}
float SignalFactoryCore::GenerateFMVoicesSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkFMVoices->tick();
}
float SignalFactoryCore::GenerateFluteSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkFlute->tick();
}
float SignalFactoryCore::GenerateGuitarSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkGuitar->tick();
}
float SignalFactoryCore::GenerateHevyMetlSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkHevyMetl->tick();
}
float SignalFactoryCore::GenerateMandolinSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkMandolin->tick();
}
float SignalFactoryCore::GenerateMoogSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkMoog->tick();
}
float SignalFactoryCore::GenerateSaxofonySample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkSaxofony->tick();
}
float SignalFactoryCore::GenerateShakersSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkShakers->tick();
}
float SignalFactoryCore::GenerateSitarSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkSitar->tick();
}
float SignalFactoryCore::GenerateTubeBellSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkTubeBell->tick();
}
float SignalFactoryCore::GenerateVoicFormSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkVoicForm->tick();
}
float SignalFactoryCore::GenerateWhistleSample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkWhistle->tick();
}
float SignalFactoryCore::GenerateWurleySample(float frequency, const PlaybackTime* playbackTime)
{
	return _stkWurley->tick();
}