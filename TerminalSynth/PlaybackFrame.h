#pragma once
#include <cmath>

#ifndef PLAYBACK_FRAME_H
#define PLAYBACK_FRAME_H

class PlaybackFrame
{	
public:

	PlaybackFrame() 
	{
		_left = 0;
		_right = 0;
	};
	PlaybackFrame(float left, float right)
	{
		_left = left;
		_right = right;
	}
	PlaybackFrame(const PlaybackFrame& copy)
	{
		_left = copy.GetLeft();
		_right = copy.GetRight();
	}
	~PlaybackFrame() {};

	/// <summary>
	/// Sets frame according to channel inputs. Any excess channels are set to channel1
	/// </summary>
	void SetFrame(float left, float right)
	{
		_left = left;
		_right = right;
	}

	void SetFrame(const PlaybackFrame* copy)
	{
		_left = copy->GetLeft();
		_right = copy->GetRight();
	}

	void AddFrame(const PlaybackFrame* copy)
	{
		_left += copy->GetLeft();
		_right += copy->GetRight();
	}

	void Clear()
	{
		_left = 0;
		_right = 0;
	}

	float GetLeft() const { return _left; }
	float GetRight() const { return _right; }

	bool IsClipping() const
	{
		return (fabsf(_left) > 1) || (fabsf(_right) > 1);
	}

private:

	float _left;
	float _right;
};

#endif