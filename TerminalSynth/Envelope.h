#pragma once
#include "Constant.h"
#include <istream>
#include <ostream>

#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope
{
public:
	Envelope();
	Envelope(EnvelopeShape shape, double attack, double decay, double release, double attackPeak, double sustainPeak);
	Envelope(const Envelope& copy);
	~Envelope();

	bool Update(const Envelope* envelope);

	void Engage(double absoluteTime);
	void DisEngage(double absoluteTime);
	bool HasOutput(double absoluteTime);
	bool IsEngaged();
	double GetEnvelopeLevel(double absoluteTime);
	double GetEngageTime();
	double GetDisEngageTime();

	bool operator!=(const Envelope& envelope);
	bool operator==(const Envelope& envelope);

public:

	void Save(std::ostream& stream)
	{
		stream << (int)_shape;
		stream << _attack;
		stream << _decay;
		stream << _release;
		stream << _attackPeak;
		stream << _sustainPeak;
	}
	void Read(std::istream& stream) 
	{
		int envelopeShape;

		stream >> envelopeShape;
		stream >> _attack;
		stream >> _decay;
		stream >> _release;
		stream >> _attackPeak;
		stream >> _sustainPeak;

		_shape = (EnvelopeShape)envelopeShape;
	}

	bool IsEqual(const Envelope* other)
	{
		bool result = true;

		result &= _shape == other->GetShape();
		result &= _attack == other->GetAttack();
		result &= _decay == other->GetDecay();
		result &= _release == other->GetRelease();
		result &= _attackPeak == other->GetAttackPeak();
		result &= _sustainPeak == other->GetSustainPeak();

		return result;
	}

public:

	EnvelopeShape GetShape() const;
	double GetAttack() const;
	double GetDecay() const;
	double GetRelease() const;
	double GetAttackPeak() const;
	double GetSustainPeak() const;

	void SetShape(EnvelopeShape value);
	void SetAttack(double value);
	void SetDecay(double value);
	void SetRelease(double value);
	void SetAttackPeak(double value);
	void SetSustainPeak(double value);

private:

	double GetEnvelopeLevelImpl(double envelopeTime);

private:

	EnvelopeShape _shape;

	double _attack;
	double _decay;
	double _release;

	double _attackPeak;
	double _sustainPeak;

	// Values stored when SynthNote is pressed / released
	//

	bool _engaged;	  // Set to TRUE when synth note is pressed
	bool _hasEngaged; // Set to TRUE after first engagement

	// Absolute time values when voice is engaged / dis-engaged
	//
	double _engagedTime;
	double _disEngagedTime;
	double _disEngagedLevel;
};
#endif