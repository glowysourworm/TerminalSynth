#pragma once

#ifndef SIGNALBASE_H
#define SIGNALBASE_H

#include "Accumulator.h"
#include "Constant.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalParameter.h"
#include "SignalParameterAutomater.h"
#include "SignalSettings.h"
#include <string>
#include <vector>

class SignalBase
{
public:

	SignalBase()
	{
		_low = SIGNAL_LOW;
		_high = SIGNAL_HIGH;
		_settings = new SignalSettings();
		_leftAccumulator = new Accumulator<float>(true);
		_rightAccumulator = new Accumulator<float>(true);
		_parameterAutomaters = new std::vector<SignalParameterAutomater*>();
		_outputSettings = nullptr;
	}
	SignalBase(const SignalSettings& settings)
	{
		_low = SIGNAL_LOW;
		_high = SIGNAL_HIGH;
		_settings = new SignalSettings(settings);
		_leftAccumulator = new Accumulator<float>(true);
		_rightAccumulator = new Accumulator<float>(true);
		_parameterAutomaters = new std::vector<SignalParameterAutomater*>();
		_outputSettings = nullptr;

		for (int index = 0; index < _settings->GetParameterCount(); index++)
		{
			// MEMORY! ~SignalBase
			auto automater = new SignalParameterAutomater();

			// Requires Initialization
			_parameterAutomaters->push_back(automater);
		}
	};
	virtual ~SignalBase() 
	{
		delete _settings;
		delete _leftAccumulator;
		delete _rightAccumulator;

		for (int index = 0; index < _parameterAutomaters->size(); index++)
		{
			delete _parameterAutomaters->at(index);
		}

		delete _parameterAutomaters;
	}

	virtual void Initialize(const PlaybackInfo* outputSettings)
	{
		_outputSettings = outputSettings;

		for (int index = 0; index < _settings->GetParameterCount(); index++)
		{
			_parameterAutomaters->at(index)->Initialize(outputSettings);
		}

		// Track clipping
		_leftAccumulator->ResetFor(true, outputSettings->GetStreamInfo()->streamSampleRate);
		_rightAccumulator->ResetFor(true, outputSettings->GetStreamInfo()->streamSampleRate);
	}

	/// <summary>
	/// Updates SignalBase* by calling UpdateParameter(..)
	/// </summary>
	/// <param name="settings">New SignalSettings object - used for parameter values only!</param>
	void Update(const SignalSettings* settings)
	{
		_settings->Update(settings, false);		// Catches name change and parameter count change

		// Parameter Automation
		for (int index = 0; index < settings->GetParameterCount(); index++)
		{
			// Enabled
			if (settings->GetParameter(index)->GetAutomationEnabled())
			{
				_parameterAutomaters->at(index)->Update(settings->GetParameter(index));
			}
			else
			{
				this->UpdateParameter(index, settings->GetParameterValue(index));
			}
		}
	}

	/// <summary>
	/// (SignalBase) Sets accumulators for signal. In derived class, it should be used to produce the 
	/// output.
	/// </summary>
	virtual void SetFrame(PlaybackFrame* frame)
	{
		// Parameter Automation
		for (int index = 0; index < _settings->GetParameterCount(); index++)
		{
			if (_settings->GetParameter(index)->GetAutomationEnabled())
			{
				float value = _parameterAutomaters->at(index)->GetValue(frame);

				// Call function to set the current parameter value before sample
				// is calculated
				this->UpdateParameter(index, value);
			}		
		}

		_leftAccumulator->Add(frame->GetLeft());
		_rightAccumulator->Add(frame->GetRight());
	}
	virtual bool HasOutput() const = 0;

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
		return _settings->GetName();
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

	SignalSettings GetSettings() const
	{
		return *_settings;
	}

protected:

	void AddParameter(const std::string& name, float min, float max, float initialValue)
	{
		// MEMORY! ~SignalBase
		auto automater = new SignalParameterAutomater();
		automater->Initialize(_outputSettings);
		
		_settings->AddParameter(SignalParameter(name, initialValue, min, max));
		_parameterAutomaters->push_back(automater);
	}

	/// <summary>
	/// Function to update the parameter value for automating the paramter
	/// </summary>
	virtual void UpdateParameter(int index, float value) = 0;
	
private:

	float _low;
	float _high;
	SignalSettings* _settings;
	Accumulator<float>* _leftAccumulator;
	Accumulator<float>* _rightAccumulator;

	// We should try to remove this initialization dependency
	const PlaybackInfo* _outputSettings;
	
	std::vector<SignalParameterAutomater*>* _parameterAutomaters;
};

#endif