#pragma once

#ifndef EQUALIZER_OUTPUT_H
#define EQUALIZER_OUTPUT_H

class EqualizerOutput
{
public:

	EqualizerOutput() 
	{
		this->left = 0;
		this->right = 0;
	};
	~EqualizerOutput() {};

	float left;
	float right;
};

#endif