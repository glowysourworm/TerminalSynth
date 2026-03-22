#pragma once

#ifndef PLAYBACK_TIME_H
#define PLAYBACK_TIME_H

struct PlaybackTime
{
	/// <summary>
	/// Absolute stream time! This is not necessarily a reliable sampling tool. It is best to use the
	/// frame cursor; and the stream will keep up!
	/// </summary>
	double streamTime;

	/// <summary>
	/// Cursor for the absolute frame of the stream. Good for approx. 1 billion years of playback! ^_^
	/// </summary>
	size_t frameCursor;

	/// <summary>
	/// Gets the stream time from the frame cursor - which is the most accurate way to calculate the
	/// current stream time. You can use the supplied stream time, also, but it may be stale depending
	/// on when Port Audio updates it (please use this function instead, unless it's just an envelope, or
	/// some other user parameter)
	/// </summary>
	double FromCursor(float samplingRate) const
	{
		return frameCursor / samplingRate;
	}
};

#endif