#pragma once

#ifndef ARPEGGIATOR_PARAMETERS_H
#define ARPEGGIATOR_PARAMETERS_H

#include "Constant.h"
#include <istream>
#include <ostream>

struct SynthNoteParameters
{
public:

	SynthNoteMode mode;
	ArpeggiatorChord chord;
	unsigned int arpeggioBPM;
	double portamentoSeconds;

	bool IsEqual(const SynthNoteParameters* other)
	{
		return this->mode == other->mode &&
			this->chord == other->chord &&
			this->arpeggioBPM == other->arpeggioBPM &&
			this->portamentoSeconds == other->portamentoSeconds;
	}

	void Save(std::ostream& stream)
	{
		stream << (int)this->mode;
		stream << (int)this->chord;
		stream << this->arpeggioBPM;
		stream << this->portamentoSeconds;
	}
	void Read(std::istream& stream)
	{
		int mode_, chord_;

		stream >> mode_;
		stream >> chord_;
		stream >> this->arpeggioBPM;
		stream >> this->portamentoSeconds;

		this->mode = (SynthNoteMode)mode_;
		this->chord = (ArpeggiatorChord)chord_;
	}
};

#endif

