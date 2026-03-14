#pragma once
#include <cmath>
#include <limits>

#ifndef PLAYBACK_FRAME_H
#define PLAYBACK_FRAME_H

class PlaybackFrame
{	
public:

	PlaybackFrame() 
	{
		_left = 0;
		_right = 0;
		_previouslySet = false;
		_timeCursor = 0;
		_streamTime = 0;
	};
	PlaybackFrame(float left, float right)
	{
		_left = left;
		_right = right;
		_previouslySet = false;
		_timeCursor = 0;
		_streamTime = 0;
	}
	PlaybackFrame(const PlaybackFrame& copy)
	{
		_left = copy.GetLeft();
		_right = copy.GetRight();
		_previouslySet = copy.GetPreviouslySet();
		_timeCursor = copy.GetTimeCursor();
		_streamTime = copy.GetStreamTime();
	}
	~PlaybackFrame() {};

	/// <summary>
	/// Sets frame according to channel inputs. Any excess channels are set to channel1
	/// </summary>
	void SetFrame(float left, float right)
	{
		_left = left;
		_right = right;
		_previouslySet = true;
	}

	void SetFrame(const PlaybackFrame* copy)
	{
		_left = copy->GetLeft();
		_right = copy->GetRight();
		_previouslySet = true;
	}

	void AddFrame(float left, float right)
	{
		if (_previouslySet)
		{
			_left += left /* / 2.0f */;
			_right += right /* / 2.0f */;
		}
		else
			this->SetFrame(left, right);
	}

	/// <summary>
	/// Clears sample values for the frame. Does not clear the time cursor
	/// </summary>
	void ClearSample()
	{
		_left = 0;
		_right = 0;
		_previouslySet = false;
	}

	/// <summary>
	/// Sets stream time to the system value. THIS MAY DEVELOP SLIPPAGE BETWEEN THE TIME
	/// CURSOR AND THE SYSTEM STREAM TIME!
	/// </summary>
	void ResetStreamTime(double streamTime)
	{
		_streamTime = streamTime;
	}

	void IncrementTimeCursor(double deltaStreamTime) 
	{ 
		_timeCursor++; 

		// This is used for some components that require a time in seconds. The buffers
		// must be set according to the current sample index!!! Otherwise, there will be
		// garbage output trying to keep up with the time.
		//
		_streamTime += deltaStreamTime;

		// This should be reset during idle periods!!!
		if (_timeCursor >= std::numeric_limits<size_t>::max())
			_timeCursor = 0;
	}

	float GetLeft() const { return _left; }
	float GetRight() const { return _right; }

	bool IsClipping() const
	{
		return (fabsf(_left) > 1) || (fabsf(_right) > 1);
	}

	size_t GetTimeCursor() const { return _timeCursor; }
	double GetStreamTime() const { return _streamTime; }

protected:

	bool GetPreviouslySet() const { return _previouslySet; }

private:

	// This is the primary time cursor for this sample. This should be propagated
	// with the signal chain in order to utilize the playback buffers correctly.
	size_t _timeCursor;
	double _streamTime;

	float _left;
	float _right;

	bool _previouslySet;
};

#endif