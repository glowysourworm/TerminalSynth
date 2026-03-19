#pragma once

#ifndef SIGNALBASE_H
#define SIGNALBASE_H

#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include <string>

class SignalBase
{
public:

	SignalBase() { _name = new std::string(""); }
	SignalBase(const std::string& name)
	{
		_outputSettings = nullptr;
		_name = new std::string(name);
	}
	~SignalBase() 
	{
		delete _name;
	}

	virtual void Initialize(const PlaybackInfo* outputSettings)
	{
		_outputSettings = outputSettings;
	}

	/// <summary>
	/// Function to call to set the frame with the next sample output, overwriting the frame's data.
	/// </summary>
	virtual void SetFrame(PlaybackFrame* frame)
	{
		SetFrameImpl(frame);
	}

	/// <summary>
	/// Function to call to add, to the frame, the next sample output.
	/// </summary>
	virtual void AddFrame(PlaybackFrame* frame)
	{
		PlaybackFrame localFrame(*frame);
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
	virtual void Engage(double absoluteTime) {};

	/// <summary>
	/// Function called when the note is dis-engaged, causing the SignalBase* to become dis-engaged. Any
	/// ringing will be handled with the HasOutput, and SetFrame functions.
	/// </summary>
	virtual void DisEngage(double absoluteTime) {};

	/// <summary>
	/// Function to clear the signal base of all of its internal buffers, and signal history. Any parameters
	/// or signal settings should NOT be cleared or reset to default. This is for any of the signal buffers.
	/// </summary>
	virtual void Clear() {};

public:

	std::string GetName() const { return *_name; }

protected:

	/// <summary>
	/// Function to set the frame with the next sample
	/// </summary>
	virtual void SetFrameImpl(PlaybackFrame* frame) = 0;

	/// <summary>
	/// Returns a const pointer to the output settings
	/// </summary>
	const PlaybackInfo* GetPlaybackInfo() const { return _outputSettings; }
	
private:

	// We should try to remove this initialization dependency
	const PlaybackInfo* _outputSettings;

	std::string* _name;
};

#endif