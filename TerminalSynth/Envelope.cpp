#include "Algorithm.h"
#include "Constant.h"
#include "Envelope.h"
#include <algorithm>
#include <cmath>
#include <exception>

Envelope::Envelope() : Envelope(EnvelopeShape::Linear, 0.1f, 0.35f, 0.85f, 0.65f)
{}

Envelope::Envelope(EnvelopeShape shape, float attackSeconds, float releaseSeconds, float attackPeak, float sustainPeak)
{
	_attackSeconds = attackSeconds;
	_releaseSeconds = releaseSeconds;

	_attack = new float[ATTACK_LENGTH];
	_release = new float[RELEASE_LENGTH];

	for (int index = 0; index < ATTACK_LENGTH + RELEASE_LENGTH; index++)
	{
		switch (shape)
		{
		case EnvelopeShape::Linear:
		{
			// Attack
			if (index < ATTACK_LENGTH / 2)
				_attack[index] = (2 * attackPeak) * (index / (float)ATTACK_LENGTH);

			// Sustain
			else if (index < ATTACK_LENGTH)
				_attack[index] = (((2 * (sustainPeak - attackPeak)) / ATTACK_LENGTH) * index) + (2 * attackPeak) - sustainPeak;

			// Release
			else
				_release[index - ATTACK_LENGTH] = -1 * (sustainPeak / RELEASE_LENGTH) * (index - RELEASE_LENGTH - ATTACK_LENGTH);
		}
		break;
		case EnvelopeShape::Poisson:
		{
			if (index < ATTACK_LENGTH)
				_attack[index] = Algorithm::Poisson(index, POISSON_LAMBDA_DEFAULT);
			else
				_release[index] = Algorithm::Poisson(index, POISSON_LAMBDA_DEFAULT);
		}
		break;
		case EnvelopeShape::Gaussian:
		{
			if (index < ATTACK_LENGTH)
				_attack[index] = Algorithm::Gaussian3Sigma(index,  0, ATTACK_LENGTH - 1);
			else
				_release[index] = Algorithm::Gaussian3Sigma(index, ATTACK_LENGTH, RELEASE_LENGTH - 1);
		}
		break;
		default:
			throw new std::exception("Unhandled envelope shape:  Envelope.cpp");
		}
	}

	_engaged = false;
	_hasEngaged = false;

	_disEngagedLevel = 0;
	_engagedTime = 0;
	_disEngagedTime = 0;
}

Envelope::Envelope(const Envelope& copy)
{
	_attackSeconds = copy.GetAttackTime();
	_releaseSeconds = copy.GetReleaseTime();

	_attack = new float[ATTACK_LENGTH];
	_release = new float[RELEASE_LENGTH];

	for (int index = 0; index < ATTACK_LENGTH; index++)
	{
		_attack[index] = copy.GetAttackValue(index);
	}

	for (int index = 0; index < RELEASE_LENGTH; index++)
	{
		_release[index] = copy.GetReleaseValue(index);
	}

	// Defaults
	_engaged = false;
	_hasEngaged = false;

	_disEngagedLevel = 0;
	_engagedTime = 0;
	_disEngagedTime = 0;
}
Envelope::~Envelope()
{
	delete[] _attack;
	delete[] _release;
}

/// <summary>
/// Returns true if there were changes to the envelope
/// </summary>
bool Envelope::Update(const Envelope* envelope)
{
	bool isDirty = IsEqual(envelope);

	_attackSeconds = envelope->GetAttackTime();
	_releaseSeconds = envelope->GetReleaseTime();

	for (int index = 0; index < ATTACK_LENGTH; index++)
	{
		_attack[index] = envelope->GetAttackValue(index);
	}

	for (int index = 0; index < RELEASE_LENGTH; index++)
	{
		_release[index] = envelope->GetReleaseValue(index);
	}

	return isDirty;
}

float Envelope::GetAttackTime() const
{
	return _attackSeconds;
}
float Envelope::GetAttackValue(int index) const
{
	return _attack[index];
}

float Envelope::GetReleaseTime() const
{
	return _releaseSeconds;
}
float Envelope::GetReleaseValue(int index) const
{
	return _release[index];
}
int Envelope::GetAttackLength() const
{
	return ATTACK_LENGTH;
}
int Envelope::GetReleaseLength() const
{
	return RELEASE_LENGTH;
}
void Envelope::SetAttackValue(int index, float value)
{
	_attack[index] = value;
}
void Envelope::SetAttackTime(float value)
{
	_attackSeconds = value;
}
void Envelope::SetReleaseValue(int index, float value)
{
	_release[index] = value;
}
void Envelope::SetReleaseTime(float value)
{
	_releaseSeconds = value;
}
void Envelope::Engage(float absoluteTime)
{
	if (_engaged)
		return;

	_engaged = true;
	_hasEngaged = true;
	_engagedTime = absoluteTime;
}

void Envelope::DisEngage(float absoluteTime)
{
	if (!_engaged)
		return;

	// Store envelope level to calculate release value
	//
	// CALL BEFORE DIS-ENGAGING
	//
	_disEngagedLevel = GetEnvelopeLevel(absoluteTime);

	_engaged = false;
	_disEngagedTime = absoluteTime;
}

bool Envelope::HasOutput(float absoluteTime)
{
	if (!_hasEngaged)
		return false;

	// Note is engaged
	if (_engaged)
		return true;

	// Check to see whether envelope is completed
	else
	{
		// Check release time
		bool hasOutput = GetEnvelopeLevel(absoluteTime) > 0;

		// Reset
		if (!hasOutput)
		{
			_hasEngaged = false;
			_engaged = false;
			_disEngagedLevel = 0;
			_engagedTime = 0;
			_disEngagedTime = 0;
		}

		return hasOutput;
	}
}

bool Envelope::IsEngaged()
{
	return _engaged;
}

bool Envelope::operator!=(const Envelope& envelope)
{
	return !IsEqual(&envelope);
}

bool Envelope::operator==(const Envelope& envelope)
{
	return IsEqual(&envelope);
}

float Envelope::GetEngageTime()
{
	return _engagedTime;
}

float Envelope::GetDisEngageTime()
{
	return _disEngagedTime;
}

float Envelope::GetEnvelopeLevel(float absoluteTime)
{
	if (!_hasEngaged)
		return 0;

	// Check piece-wise function to get envelope level
	//

	// Attack
	if (_engaged)
	{
		// Calculate time along the envelope
		float envelopeTime = absoluteTime - _engagedTime;

		float attackDiv = (_attackSeconds / ATTACK_LENGTH);		
		float attackBucket = envelopeTime / attackDiv;
		float attackBucketIndex = 0;
		float lerpValue = std::modf(attackBucket, &attackBucketIndex);
		int attackIndex = std::min<int>(attackBucketIndex, ATTACK_LENGTH - 1);
		attackIndex = std::max<int>(attackIndex, 0);

		// Attack Interpolation Finished
		if (attackBucketIndex >= ATTACK_LENGTH)
		{
			return _attack[ATTACK_LENGTH - 1];
		}

		// Attack Interpolate
		else
		{
			int attackIndexPrevious = attackIndex == 0 ? 0 : attackIndex - 1;

			return std::lerp(_attack[attackIndexPrevious], _attack[attackIndex], lerpValue);
		}
	}

	// Release
	else
	{
		// Calculate time along the RELEASE
		float envelopeTime = absoluteTime - _disEngagedTime;

		if (envelopeTime < _releaseSeconds)
		{
			float releaseDiv = (_releaseSeconds / RELEASE_LENGTH);
			float releaseBucket = envelopeTime / releaseDiv;
			float releaseBucketIndex = 0;
			float lerpValue = std::modf(releaseBucket, &releaseBucketIndex);
			int releaseIndex = std::min<int>(releaseBucketIndex, RELEASE_LENGTH - 1);
			releaseIndex = std::max<int>(releaseIndex, 0);

			
			// Interpolate from Attack (last)
			if (releaseIndex == 0)
			{
				return std::lerp(_disEngagedLevel, _disEngagedLevel * _release[releaseIndex], lerpValue);
			}
			else
			{
				return std::lerp(_disEngagedLevel * _release[releaseIndex - 1], _disEngagedLevel * _release[releaseIndex], lerpValue);
			}
			
		}
		else
			return 0;
	}
}