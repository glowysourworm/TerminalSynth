#include "Algorithm.h"
#include "Constant.h"
#include "Envelope.h"
#include <algorithm>
#include <exception>

Envelope::Envelope() : Envelope(EnvelopeShape::Linear, 0.1, 0.15, 0.35, 0.85, 0.65)
{}

Envelope::Envelope(EnvelopeShape shape, double attack, double decay, double release, double attackPeak, double sustainPeak)
{
	_attack = attack;
	_release = release;
	_decay = decay;
	_attackPeak = attackPeak;
	_sustainPeak = sustainPeak;

	_shape = shape;

	_engaged = false;
	_hasEngaged = false;

	_disEngagedLevel = 0;
	_engagedTime = 0;
	_disEngagedTime = 0;
}

Envelope::Envelope(const Envelope& copy)
{
	_attack = copy.GetAttack();
	_decay = copy.GetDecay();
	_release = copy.GetRelease();
	_attackPeak = copy.GetAttackPeak();
	_sustainPeak = copy.GetSustainPeak();
	_shape = copy.GetShape();

	// Defaults
	_engaged = false;
	_hasEngaged = false;

	_disEngagedLevel = 0;
	_engagedTime = 0;
	_disEngagedTime = 0;
}
Envelope::~Envelope()
{}

/// <summary>
/// Returns true if there were changes to the envelope
/// </summary>
bool Envelope::Update(const Envelope* envelope)
{
	bool isDirty = IsEqual(envelope);

	_attack = envelope->GetAttack();
	_decay = envelope->GetDecay();
	_release = envelope->GetRelease();
	_attackPeak = envelope->GetAttackPeak();
	_sustainPeak = envelope->GetSustainPeak();
	_shape = envelope->GetShape();

	return isDirty;
}

double Envelope::GetAttack() const
{
	return _attack;
}
double Envelope::GetAttackPeak() const
{
	return _attackPeak;
}
double Envelope::GetDecay() const
{
	return _decay;
}
double Envelope::GetRelease() const
{
	return _release;
}
EnvelopeShape Envelope::GetShape() const
{
	return _shape;
}
double Envelope::GetSustainPeak() const
{
	return _sustainPeak;
}
void Envelope::SetAttack(double value)
{
	_attack = value;
}
void Envelope::SetAttackPeak(double value)
{
	_attackPeak = value;
}
void Envelope::SetDecay(double value)
{
	_decay = value;
}
void Envelope::SetRelease(double value)
{
	_release = value;
}
void Envelope::SetShape(EnvelopeShape value)
{
	_shape = value;
}
void Envelope::SetSustainPeak(double value)
{
	_sustainPeak = value;
}
void Envelope::Engage(double absoluteTime)
{
	if (_engaged)
		return;

	_engaged = true;
	_hasEngaged = true;
	_engagedTime = absoluteTime;
}

void Envelope::DisEngage(double absoluteTime)
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

bool Envelope::HasOutput(double absoluteTime)
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

double Envelope::GetEngageTime()
{
	return _engagedTime;
}

double Envelope::GetDisEngageTime()
{
	return _disEngagedTime;
}

double Envelope::GetEnvelopeLevel(double absoluteTime)
{
	if (!_hasEngaged)
		return 0;

	// Check piece-wise function to get envelope level
	//

	if (_engaged)
	{
		return GetEnvelopeLevelImpl(absoluteTime);
	}
	else
	{
		if (absoluteTime - _disEngagedTime < _release)
			return GetEnvelopeLevelImpl(absoluteTime);

		else
			return 0;
	}
}

double Envelope::GetEnvelopeLevelImpl(double absoluteTime)
{
	double envelopeTime = absoluteTime - _engagedTime;
	double envelopeTimeDisEngage = _disEngagedTime - _engagedTime;
	double result = 0;

	switch (_shape)
	{
	case EnvelopeShape::Linear:
	{
		if (_engaged)
		{
			// Attack
			if (envelopeTime < _attack)
				result = (_attackPeak / _attack) * envelopeTime;

			// Decay
			else if (envelopeTime < _attack + _decay)
			{
				double decaySlope = _attack / _decay;
				double slope = (_sustainPeak - _attackPeak) / _decay;

				result = (slope * (envelopeTime - _attack)) + _attackPeak;
			}

			// Sustain
			else
				result = _sustainPeak;
		}

		// Release
		else
			result = ((-1 * (_sustainPeak / _release)) * envelopeTime) + (_sustainPeak * (envelopeTimeDisEngage / _release)) + _disEngagedLevel;
	}
	break;
	case EnvelopeShape::Gamma:
	{
		if (_engaged)
		{
			// Gamma Distributrion PDF
			// 
			// Peak "Mode" = (alpha - 1) * theta
			// 
			// For spread, try [alpha * theta = attack + decay + release] and [(alpha - 1) * theta = attack]
			//
			double gammaValue = Algorithm::GammaDistribution(envelopeTime, (_attack / (_release + _decay)) + 1, _release + _decay);

			// Attack
			if (envelopeTime < _attack)
				result = gammaValue;

			// Decay
			else if (envelopeTime < _attack + _decay)
				result = gammaValue;

			else
				result = std::max(gammaValue, _sustainPeak);
		}

		// Release (Linear)
		else
		{
			result = ((-1 * (_sustainPeak / _release)) * envelopeTime) + (_sustainPeak * (envelopeTimeDisEngage / _release)) + _disEngagedLevel;
		}
	}
	break;
	case EnvelopeShape::Gaussian:
	{
		if (_engaged)
		{
			// Gaussian Distributrion PDF
			// 
			// Mean:  Attack
			// Sigma: Attack + Decay + Release
			// 
			double gaussianValue = Algorithm::Gaussian(envelopeTime, _attack, _attack + _decay + _release);

			// Attack
			if (envelopeTime < _attack)
				result = gaussianValue;

			// Decay
			else if (envelopeTime < _attack + _decay)
				result = gaussianValue;

			else
				result = std::max(gaussianValue, _sustainPeak);
		}

		// Release (Linear)
		else
		{
			result = ((-1 * (_sustainPeak / _release)) * envelopeTime) + (_sustainPeak * (envelopeTimeDisEngage / _release)) + _disEngagedLevel;
		}
	}
	break;
	default:
		throw new std::exception("Unhandled envelope shape:  Envelope.cpp");
	}

	return std::min<double>(std::max<double>(result, 0), 1);
}