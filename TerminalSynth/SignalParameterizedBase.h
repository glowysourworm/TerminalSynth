#pragma once

#ifndef SIGNAL_PARAMETERIZED_BASE_H
#define SIGNAL_PARAMETERIZED_BASE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalBase.h"
#include "SignalParameter.h"
#include "SignalParameterAutomater.h"
#include "SignalSettings.h"
#include <string>
#include <vector>

class SignalParameterizedBase : public SignalBase
{
public:

	SignalParameterizedBase(const std::string& name) : SignalBase(name)
	{
		_settings = new SignalSettings();
		_parameterAutomaters = new std::vector<SignalParameterAutomater*>();
		_outputSettings = nullptr;
	}
	SignalParameterizedBase(const SignalSettings& settings) : SignalBase(settings.GetName())
	{
		_settings = new SignalSettings(settings);
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
	~SignalParameterizedBase()
	{
		delete _settings;

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
	/// Function to call to set the frame with the next sample output, overwriting the frame's data.
	/// </summary>
	void SetFrame(PlaybackFrame* frame) override
	{
		UpdateParameterAutomaters(frame);
		SetFrameImpl(frame);
	}

	/// <summary>
	/// Function to call to add, to the frame, the next sample output.
	/// </summary>
	void AddFrame(PlaybackFrame* frame) override
	{
		PlaybackFrame localFrame(*frame);

		UpdateParameterAutomaters(&localFrame);
		SetFrameImpl(&localFrame);

		frame->AddFrame(localFrame.GetLeft(), localFrame.GetRight());
	}

	/// <summary>
	/// Function used to alert the caller that the SignalBase* component still has output.
	/// </summary>
	virtual bool HasOutput(double absoluteTime) const = 0;

	/// <summary>
	/// Function called when the note is engaged, causing the SignalBase* to become engaged.
	/// </summary>
	virtual void Engage(double absoluteTime)
	{
		EngageParameterAutomaters(absoluteTime, true);
	}

	/// <summary>
	/// Function called when the note is dis-engaged, causing the SignalBase* to become dis-engaged. Any
	/// ringing will be handled with the HasOutput, and SetFrame functions.
	/// </summary>
	virtual void DisEngage(double absoluteTime)
	{
		EngageParameterAutomaters(absoluteTime, false);
	}

	/// <summary>
	/// Function to clear the signal base of all of its internal buffers, and signal history. Any parameters
	/// or signal settings should NOT be cleared or reset to default. This is for any of the signal buffers.
	/// </summary>
	virtual void Clear()
	{

	}

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

	/// <summary>
	/// Function to add a parameter to the SignalSettings* for this SignalBase*
	/// </summary>
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

	/// <summary>
	/// Function to set the frame with the next sample
	/// </summary>
	virtual void SetFrameImpl(PlaybackFrame* frame) = 0;

	/// <summary>
	/// Function to update parameter automaters before playback
	/// </summary>
	void UpdateParameterAutomaters(PlaybackFrame* frame)
	{
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
	}

	void EngageParameterAutomaters(double absoluteTime, bool engaged)
	{
		for (int index = 0; index < _settings->GetParameterCount(); index++)
		{
			if (_settings->GetParameter(index)->GetAutomationEnabled())
			{
				if (engaged)
					_parameterAutomaters->at(index)->Engage(absoluteTime);
				else
					_parameterAutomaters->at(index)->DisEngage(absoluteTime);
			}
		}
	}

private:

	SignalSettings* _settings;

	// We should try to remove this initialization dependency
	const PlaybackInfo* _outputSettings;

	std::vector<SignalParameterAutomater*>* _parameterAutomaters;
};

#endif