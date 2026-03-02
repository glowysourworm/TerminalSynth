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
		_previouslySet = false;
	};
	PlaybackFrame(float left, float right)
	{
		_left = left;
		_right = right;
		_previouslySet = false;
	}
	PlaybackFrame(const PlaybackFrame& copy)
	{
		_left = copy.GetLeft();
		_right = copy.GetRight();
		_previouslySet = copy.GetPreviouslySet();
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

	void AddFrame(const PlaybackFrame* copy)
	{
		if (_previouslySet)
		{
			_left += copy->GetLeft() / 2.0f;
			_right += copy->GetRight() / 2.0f;
		}
		else
			this->SetFrame(copy);
	}

	void Clear()
	{
		_left = 0;
		_right = 0;
		_previouslySet = false;
	}

	float GetLeft() const { return _left; }
	float GetRight() const { return _right; }

	bool IsClipping() const
	{
		return (fabsf(_left) > 1) || (fabsf(_right) > 1);
	}

protected:

	bool GetPreviouslySet() const { return _previouslySet; }

private:

	float _left;
	float _right;

	bool _previouslySet;
};

#endif