#pragma once
#include "Constant.h"
#include <istream>
#include <ostream>

#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope
{
protected:

	const int ATTACK_LENGTH = 4;
	const int RELEASE_LENGTH = 4;

	const int POISSON_LAMBDA_DEFAULT = 3;

public:
	Envelope();
	Envelope(EnvelopeShape shape, float attackSeconds, float releaseSeconds, float attackPeak, float sustainPeak);
	Envelope(const Envelope& copy);
	~Envelope();

	bool Update(const Envelope* envelope);

	void Engage(float absoluteTime);
	void DisEngage(float absoluteTime);
	bool HasOutput(float absoluteTime);
	bool IsEngaged();
	float GetEnvelopeLevel(float absoluteTime);
	float GetEngageTime();
	float GetDisEngageTime();

	bool operator!=(const Envelope& envelope);
	bool operator==(const Envelope& envelope);

public:

	void Save(std::ostream& stream)
	{
		for (int index = 0; index < ATTACK_LENGTH; index++)
		{
			stream << _attack[index];
		}

		for (int index = 0; index < RELEASE_LENGTH; index++)
		{
			stream << _release[index];
		}
	}
	void Read(std::istream& stream) 
	{
		for (int index = 0; index < ATTACK_LENGTH; index++)
		{
			stream >> _attack[index];
		}

		for (int index = 0; index < RELEASE_LENGTH; index++)
		{
			stream >> _release[index];
		}
	}

	bool IsEqual(const Envelope* other)
	{
		bool result = true;

		result &= _attackSeconds == other->GetAttackTime();
		result &= _releaseSeconds == other->GetReleaseTime();

		for (int index = 0; index < ATTACK_LENGTH; index++)
		{
			result &= other->GetAttackValue(index) == _attack[index];
		}

		for (int index = 0; index < RELEASE_LENGTH; index++)
		{
			result &= other->GetReleaseValue(index) == _release[index];
		}

		return result;
	}

public:

	float GetAttackTime() const;
	float GetAttackValue(int index) const;
	void SetAttackValue(int index, float value);
	void SetAttackTime(float value);

	float GetReleaseTime() const;
	float GetReleaseValue(int index) const;
	void SetReleaseValue(int index, float value);
	void SetReleaseTime(float value);

	int GetAttackLength() const;
	int GetReleaseLength() const;

private:

	float _attackSeconds;
	float _releaseSeconds;

	// Attack / Release Values
	float* _attack;
	float* _release;

	// Values stored when SynthNote is pressed / released
	//

	bool _engaged;	  // Set to TRUE when synth note is pressed
	bool _hasEngaged; // Set to TRUE after first engagement

	// Absolute time values when voice is engaged / dis-engaged
	//
	float _engagedTime;
	float _disEngagedTime;
	float _disEngagedLevel;
};
#endif