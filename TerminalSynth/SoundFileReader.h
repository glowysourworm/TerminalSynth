#pragma once

#ifndef SOUNDFILEREADER_H
#define SOUNDFILEREADER_H

#include "PlaybackFrame.h"
#include <sndfile.h>
#include <string>

// libsndfile:  Small tutorial https://digitalsoundandmusic.com/5-3-3-reading-and-writing-formatted-audio-files-in-c/
//
class SoundFileReader
{
public:

	SoundFileReader(const std::string& fileName);
	~SoundFileReader();

	/// <summary>
	/// Opens file and returns true if the file is valid and readable
	/// </summary>
	bool Open();

	/// <summary>
	/// Gets the sound file info data from the file. Must call Open() before using this function.
	/// </summary>
	int GetNumberFrames() const;

	/// <summary>
	/// Gets the sound file info data from the file. Must call Open() before using this function.
	/// </summary>
	int GetNumberChannels() const;

	/// <summary>
	/// Gets the sound file info data from the file. Must call Open() before using this function.
	/// </summary>
	int GetSampleRate() const;

	/// <summary>
	/// (MEMORY!) Creates a playback frame array and returns it on the heap. This must be deleted by user code.
	/// </summary>
	PlaybackFrame* Read();

	/// <summary>
	/// Closes and completes the stream
	/// </summary>
	bool Close();

private:

	std::string* _fileName;
	SNDFILE* _sndFile;
	SF_INFO* _sfinfo;
};

#endif