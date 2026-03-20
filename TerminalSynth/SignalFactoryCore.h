#pragma once

#ifndef SIGNAL_FACTORY_CORE_H
#define SIGNAL_FACTORY_CORE_H

#include "OscillatorParameters.h"
#include <BeeThree.h>
#include <Clarinet.h>
#include <Drummer.h>
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
#include <vector>

/// <summary>
/// Core of SignalFactory* that does not depend on SignalBase*. There can be no filters or complex
/// SignalBase* effects because there will be a circular dependency.
/// </summary>
class SignalFactoryCore
{
public:

	const int RANDOM_OSCILLATOR_LENGTH = 8;

public:

	SignalFactoryCore(float samplingRate);
	~SignalFactoryCore();

	float GetFrequency(unsigned int midiNote);

	/// <summary>
	/// Sets the signal factory for the specified oscillator parameters, and resets all filters
	/// </summary>
	void Reset(const OscillatorParameters* parameters);

	float GenerateTriangleSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSquareSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSawtoothSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSineSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateRandomSample(float frequency, size_t timeCursor, double streamTime);

	float GenerateRhodeySample(float frequency, size_t timeCursor, double streamTime);
	float GenerateBeeThreeSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateClarinetSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateDrummerSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateFluteSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateGuitarSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateHevyMetlSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateMandolinSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateMoogSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSaxofonySample(float frequency, size_t timeCursor, double streamTime);
	float GenerateShakersSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateSitarSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateTubeBellSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateVoicFormSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateWhistleSample(float frequency, size_t timeCursor, double streamTime);
	float GenerateWurleySample(float frequency, size_t timeCursor, double streamTime);

private:

	float _samplingRate;
	float _signalHigh;
	float _signalLow;

	std::vector<float>* _randomQuadrantValues;

	stk::Rhodey* _stkRhodey;
	stk::BeeThree* _stkB3;
	stk::Clarinet* _stkClarinet;
	stk::Drummer* _stkDrummer;
	stk::Flute* _stkFlute;
	stk::Guitar* _stkGuitar;
	stk::HevyMetl* _stkHevyMetl;
	stk::Mandolin* _stkMandolin;
	stk::Moog* _stkMoog;
	stk::Saxofony* _stkSaxofony;
	stk::Shakers* _stkShakers;
	stk::Sitar* _stkSitar;
	stk::TubeBell* _stkTubeBell;
	stk::VoicForm* _stkVoicForm;
	stk::Whistle* _stkWhistle;
	stk::Wurley* _stkWurley;
};

#endif