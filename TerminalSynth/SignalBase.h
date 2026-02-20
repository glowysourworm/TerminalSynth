#pragma once

#ifndef SIGNALBASE_H
#define SIGNALBASE_H

#include "Accumulator.h"
#include "Constant.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalParameter.h"
#include "SignalSettings.h"
#include <string>

class SignalBase
{
public:

	SignalBase(const std::string& name) 
	{
		_name = new std::string(name);
		_low = SIGNAL_LOW;
		_high = SIGNAL_HIGH;
		_numberOfChannels = 0;
		_samplingRate = 0;
		_settings = new SignalSettings(name, false);
		_leftAccumulator = nullptr;
		_rightAccumulator = nullptr;
	};
	SignalBase(const std::string& name, float low, float high)
	{
		_name = new std::string(name);
		_low = low;
		_high = high;
		_numberOfChannels = 0;
		_samplingRate = 0;
		_settings = new SignalSettings(name, false);
		_leftAccumulator = nullptr;
		_rightAccumulator = nullptr;
	}
	virtual ~SignalBase() 
	{
		delete _name;
		delete _settings;

		if (_leftAccumulator != nullptr)
			delete _leftAccumulator;

		if (_rightAccumulator != nullptr)
			delete _rightAccumulator;
	}

	virtual void Initialize(const SignalSettings* settings, const OutputSettings* parameters)
	{
		_numberOfChannels = parameters->GetNumberOfChannels();
		_samplingRate = parameters->GetSamplingRate();

		// Track clipping
		_leftAccumulator = new Accumulator<float>(true, parameters->GetSamplingRate());
		_rightAccumulator = new Accumulator<float>(true, parameters->GetSamplingRate());
	}

	/// <summary>
	/// Updates SignalBase* by calling UpdateParameter(..)
	/// </summary>
	/// <param name="settings">New SignalSettings object - used for parameter values only!</param>
	void Update(const SignalSettings& settings)
	{
		// Update Parameter (override)
		for (int index = 0; index < settings.GetParameterCount(); index++)
		{
			// -> override
			this->UpdateParameter(index, settings.GetParameterValue(index));
		}
	}

	/// <summary>
	/// (SignalBase) Sets accumulators for signal. In derived class, it should be used to produce the 
	/// output.
	/// </summary>
	virtual void SetFrame(PlaybackFrame* frame, float absoluteTime)
	{
		_leftAccumulator->Add(frame->GetLeft());
		_rightAccumulator->Add(frame->GetRight());
	}
	virtual bool HasOutput(float absoluteTime) const = 0;

	/// <summary>
	/// Function to clear the signal base of all of its internal buffers, and signal history. Any parameters
	/// or signal settings should NOT be cleared or reset to default. This is for any of the signal buffers.
	/// </summary>
	virtual void Clear()
	{
		_leftAccumulator->Reset();
		_rightAccumulator->Reset();
	}

	bool HasClipped() const { return _leftAccumulator->GetAvg() > _high || _rightAccumulator->GetAvg() > _high; }
	bool HasClippedLeft() const { return _leftAccumulator->GetAvg() > _high; }
	bool HasClippedRight() const { return _rightAccumulator->GetAvg() > _high; }

public:

	std::string GetName() const
	{
		return *_name;
	}
	int GetParameterCount() const
	{
		return _settings->GetParameterCount();
	}
	std::string GetParameterName(int index) const
	{
		return _settings->GetParameterName(index);
	}
	float GetParameterValue(int index) const
	{
		return _settings->GetParameterValue(index);
	}
	float GetParameterMin(int index) const
	{
		return _settings->GetParameterMin(index);
	}
	float GetParameterMax(int index) const
	{
		return _settings->GetParameterMax(index);
	}

	// Function should be called to allow override to process (this->Update(..))
	virtual void UpdateParameter(int index, float value)
	{
		_settings->SetParameter(index, value);
	}

protected:

	void AddParameter(const std::string& name, float min, float max, float initialValue)
	{
		_settings->AddParameter(SignalParameter(name, initialValue, min, max));
	}
	
public:

	unsigned int GetSamplingRate() const { return _samplingRate; }
	unsigned int GetNumberOfChannels() const { return _numberOfChannels; }

private:

	std::string* _name;
	float _low;
	float _high;
	unsigned int _numberOfChannels;
	unsigned int _samplingRate;	
	SignalSettings* _settings;
	Accumulator<float>* _leftAccumulator;
	Accumulator<float>* _rightAccumulator;
};

#endif