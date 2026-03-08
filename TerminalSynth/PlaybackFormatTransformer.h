#pragma once

#ifndef PLAYBACK_FORMAT_TRANSFORMER_H
#define PLAYBACK_FORMAT_TRANSFORMER_H

#include "Constant.h"
#include <bit>
#include <cstdint>
#include <exception>
#include <limits>
#include <string.h>

class PlaybackFormatTransformer
{
public:

	static void Transform(AudioStreamFormat format, float input, char* output, int& outputLength)
	{
		switch (format)
		{
		case AudioStreamFormat::Float32:
			Transform32_Float(input, output, outputLength);
			break;
		case AudioStreamFormat::Int32:
			Transform32_Integer(input, output, outputLength);
			break;
		case AudioStreamFormat::Int16:
			Transform16_Integer(input, output, outputLength);
			break;
		case AudioStreamFormat::Int8:
			Transform8_Integer(input, output, outputLength);
			break;
		default:
			throw new std::exception("Unhandled format:  PlaybackFormatTransformer.h");
		}
	}

private:

	static void Transform8_Integer(float input, char* output, int& outputLength)
	{
		outputLength = sizeof(int8_t);

		int8_t outputSample = input * std::numeric_limits<int8_t>::max();

		std::memcpy(output, &outputSample, outputLength);
	}
	static void Transform16_Integer(float input, char* output, int& outputLength)
	{
		outputLength = sizeof(int16_t);

		int16_t outputSample = input * std::numeric_limits<int16_t>::max();

		std::memcpy(output, &outputSample, outputLength);
	}
	static void Transform32_Integer(float input, char* output, int& outputLength)
	{
		outputLength = sizeof(int32_t);

		int32_t outputSample = input * std::numeric_limits<int32_t>::max();

		std::memcpy(output, &outputSample, outputLength);
	}

	static void Transform8_Float(float input, char* output, int& outputLength)
	{
		throw new std::exception("Unhandled format type:  PlaybackFormatTransfomer.h");
	}
	static void Transform16_Float(float input, char* output, int& outputLength)
	{
		throw new std::exception("Unhandled format type:  PlaybackFormatTransfomer.h");
	}
	static void Transform32_Float(float input, char* output, int& outputLength)
	{
		outputLength = sizeof(float);

		std::memcpy(output, &input, outputLength);
	}
};

#endif