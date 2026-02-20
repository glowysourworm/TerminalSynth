#pragma once

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "OscillatorParameters.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include "WaveTable.h"
#include <string>

class Oscillator : public SignalBase
{
public:

	/// <summary>
	/// Creates Oscillator with pre-allocated wave table (PLEASE DELETE IN YOUR CLASS!)
	/// </summary>
	Oscillator(const std::string& name, const OscillatorParameters& parameters, WaveTable* waveTable);
	~Oscillator() override;

	void SetParameters(const OscillatorParameters& parameters);

	void Initialize(const SignalSettings* configuration, const OutputSettings* parameters) override;
	void SetFrame(PlaybackFrame* frame, float absoluteTime) override;
	bool HasOutput(float absoluteTime) const override;

	void Engage(double absoluteTime);
	void DisEngage(double absoluteTime);

private:

	OscillatorParameters* _parameters;
	WaveTable* _waveTable;

	bool _initialized;
};

#endif