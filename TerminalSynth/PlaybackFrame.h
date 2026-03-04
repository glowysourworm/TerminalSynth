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
		_envelopeLevel = 1;
	};
	PlaybackFrame(float left, float right, float envelopeLevel)
	{
		_left = left;
		_right = right;
		_previouslySet = false;
		_envelopeLevel = envelopeLevel;
	}
	PlaybackFrame(const PlaybackFrame& copy)
	{
		_left = copy.GetLeft();
		_right = copy.GetRight();
		_previouslySet = copy.GetPreviouslySet();
		_envelopeLevel = copy.GetEnvelopeLevel();
	}
	~PlaybackFrame() {};

	/// <summary>
	/// Sets frame according to channel inputs. Any excess channels are set to channel1
	/// </summary>
	void SetFrame(float left, float right, float envelopeLevel)
	{
		_left = left;
		_right = right;
		_previouslySet = true;
		_envelopeLevel = envelopeLevel;
	}

	void SetFrame(const PlaybackFrame* copy)
	{
		_left = copy->GetLeft();
		_right = copy->GetRight();
		_envelopeLevel = copy->GetEnvelopeLevel();
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
			this->SetFrame(left, right, _envelopeLevel);
	}

	void Clear()
	{
		_left = 0;
		_right = 0;
		_previouslySet = false;
		_envelopeLevel = 1;
	}

	float GetLeft() const { return _left; }
	float GetRight() const { return _right; }
	float GetEnvelopeLevel() const { return _envelopeLevel; }

	bool IsClipping() const
	{
		return (fabsf(_left) > 1) || (fabsf(_right) > 1);
	}

protected:

	bool GetPreviouslySet() const { return _previouslySet; }

private:

	float _left;
	float _right;
	float _envelopeLevel;

	bool _previouslySet;
};

#endif