#pragma once

#ifndef PLAYBACK_FRAME_H
#define PLAYBACK_FRAME_H

#include <cmath>

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

	void AddFrame(float left, float right)
	{
		if (_previouslySet)
		{
			_left += left /* _squareRoot2 */;
			_right += right /* _squareRoot2 */;
		}
		else
			this->SetFrame(left, right);
	}
	 
	void MultFrame(float constantLeft, float constantRight)
	{
		_left *= constantLeft;
		_right *= constantRight;
	}

	/// <summary>
	/// Clears sample values for the frame
	/// </summary>
	void ClearSample()
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

	const float _squareRoot2 = std::sqrtf(2);

	float _left;
	float _right;

	bool _previouslySet;
};

#endif