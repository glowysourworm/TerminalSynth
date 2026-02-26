#include "PlaybackFrame.h"
#include "SoundFileReader.h"
#include <exception>
#include <limits>
#include <sndfile.h>
#include <string>

SoundFileReader::SoundFileReader(const std::string& fileName)
{
	_fileName = new std::string(fileName);
	_sfinfo = new SF_INFO();
	_sndFile = nullptr;
}

SoundFileReader::~SoundFileReader()
{
	delete _fileName;
	delete _sfinfo;
}

bool SoundFileReader::Open()
{
	if (_sndFile != nullptr)
		throw new std::exception("Trying to open sound file with existing file already open!");

	// libsndfile does not instantiate the SF_INFO, so either put it on stack or
	// create heap allocation first (see SoundFileReader(..))
	//
	if ((_sndFile = sf_open(_fileName->c_str(), SFM_READ, _sfinfo)) == NULL)
		return false;

	return true;
}

int SoundFileReader::GetNumberFrames() const
{
	return _sfinfo->frames;
}

int SoundFileReader::GetNumberChannels() const
{
	return _sfinfo->channels;
}

int SoundFileReader::GetSampleRate() const
{
	return _sfinfo->samplerate;
}

PlaybackFrame* SoundFileReader::Read()
{
	if (_sndFile == nullptr)
		throw new std::exception("Must call open before reading sound file!");

	PlaybackFrame* result = new PlaybackFrame[_sfinfo->frames];

	for (int index = 0; index < _sfinfo->frames; index++)
	{
		// Format:  File Type | Data Type
		//
		if ((_sfinfo->format == (SF_FORMAT_WAV | SF_FORMAT_DOUBLE) ||
			 _sfinfo->format == (SF_FORMAT_AIFF | SF_FORMAT_DOUBLE)))
		{
			// MONO
			if (_sfinfo->channels == 1)
			{
				double buffer[1];
				sf_read_double(_sndFile, buffer, 1);
				result[index].SetFrame(buffer[0], buffer[0]);
			}

			// STEREO
			else if (_sfinfo->channels == 2)
			{
				double buffer[2];
				sf_read_double(_sndFile, buffer, 2);
				result[index].SetFrame(buffer[0], buffer[1]);
			}
			else
				throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
		}
		else if ((_sfinfo->format == (SF_FORMAT_WAV | SF_FORMAT_FLOAT) ||
				  _sfinfo->format == (SF_FORMAT_AIFF | SF_FORMAT_FLOAT)))
		{
			// MONO
			if (_sfinfo->channels == 1)
			{
				float buffer[1];
				sf_read_float(_sndFile, buffer, 1);
				result[index].SetFrame(buffer[0], buffer[0]);
			}

			// STEREO
			else if (_sfinfo->channels == 2)
			{
				float buffer[2];
				sf_read_float(_sndFile, buffer, 2);
				result[index].SetFrame(buffer[0], buffer[1]);
			}
			else
				throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
		}
		else if ((_sfinfo->format == (SF_FORMAT_WAV | SF_FORMAT_PCM_16) ||
				  _sfinfo->format == (SF_FORMAT_AIFF | SF_FORMAT_PCM_16)))
		{
			// MONO
			if (_sfinfo->channels == 1)
			{
				short buffer[1];
				sf_readf_short(_sndFile, buffer, 1);
				result[index].SetFrame(buffer[0] / (float)std::numeric_limits<short>::max(), buffer[0] / (float)std::numeric_limits<short>::max());
			}

			// STEREO
			else if (_sfinfo->channels == 2)
			{
				short buffer[2];
				sf_readf_short(_sndFile, buffer, 2);
				result[index].SetFrame(buffer[0] / (float)std::numeric_limits<short>::max(), buffer[1] / (float)std::numeric_limits<short>::max());
			}
			else
				throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
		}
		else
			throw new std::exception("Unhandled libsndfile format type:  WaveTableCache.cpp");
	}

	return result;
}

bool SoundFileReader::Close()
{
	return sf_close(_sndFile) == 0;
}


