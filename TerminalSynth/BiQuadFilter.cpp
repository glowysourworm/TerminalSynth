#include "BiQuadFilter.h"
#include "OutputSettings.h"
#include "PlaybackFrame.h"
#include "SignalBase.h"
#include "SignalSettings.h"
#include <cmath>
#include <exception>
#include <numbers>

BiQuadFilter::BiQuadFilter(FilterType filterType, unsigned int samplingRate) 
	: BiQuadFilter(filterType, samplingRate, 1.0f, 0.15f, 0.5f)
{}

BiQuadFilter::BiQuadFilter(FilterType filterType, unsigned int samplingRate, float corner, float resonance) 
	: BiQuadFilter(filterType, samplingRate, 1.0f, corner, resonance)
{}

BiQuadFilter::BiQuadFilter(FilterType filterType, unsigned int samplingRate, float dbGain, float corner, float resonance) : SignalBase("BiQuadFilter")
{
	_type = filterType;

	this->AddParameter("GainDb", 0.01f, 3.0f, dbGain);					// [0,1] -> [0.01dB, 3dB]
	this->AddParameter("Corner", 0.0f, samplingRate / 4.0f, corner);	// Set based on the sampling rate [0, F_s / 4]
	this->AddParameter("Q", 0.01f, 1.0f, resonance);					// (see Bi-Quad equation sheet)

	_a0 = 0;
	_a1 = 0;
	_a2 = 0;

	_b0 = 0;
	_b1 = 0;
	_b2 = 0;

	_input1 = new PlaybackFrame();
	_input2 = new PlaybackFrame();
	_output1 = new PlaybackFrame();
	_output2 = new PlaybackFrame();
}

BiQuadFilter::~BiQuadFilter()
{
	delete _input1;
	delete _input2;
	delete _output1;
	delete _output2;
}

void BiQuadFilter::Initialize(const SignalSettings* settings, const OutputSettings* parameters)
{
	SignalBase::Initialize(settings, parameters);

	switch (_type)
	{
	case BiQuadFilter::FilterType::LPF:			Set_LPF(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::HPF:			Set_HPF(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::BPF_Gain:	Set_BPF_Gain(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::BPF_Flat:	Set_BPF_Flat(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::Notch:		Set_Notch(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::APF:			Set_APF(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::PeakingEQ:	Set_PeakingEQ(parameters->GetSamplingRate()); break; 
	case BiQuadFilter::FilterType::LowShelf:	Set_LowShelf(parameters->GetSamplingRate()); break;
	case BiQuadFilter::FilterType::HighShelf:	Set_HighShelf(parameters->GetSamplingRate()); break;
	default:
		throw new std::exception("Unhandled BiQuad Filter type:  BiQuadFilter::Initialize");
	}
}

void BiQuadFilter::SetFrame(PlaybackFrame* frame, float absoluteTime)
{
	float inputLeft = frame->GetLeft();
	float inputRight = frame->GetRight();

	float outputLeft = ((_b0 / _a0) * inputLeft) +
					   ((_b1 / _a0) * _input1->GetLeft()) +
					   ((_b2 / _a0) * _input2->GetLeft()) -
					   ((_a1 / _a0) * _output1->GetLeft()) -
					   ((_a2 / _a0) * _output2->GetLeft());

	float outputRight = ((_b0 / _a0) * inputRight) +
						((_b1 / _a0) * _input1->GetRight()) +
						((_b2 / _a0) * _input2->GetRight()) -
						((_a1 / _a0) * _output1->GetRight()) -
						((_a2 / _a0) * _output2->GetRight());

	// Track Input
	_input2->SetFrame(_input1);
	_input1->SetFrame(frame);

	frame->SetFrame(outputLeft, outputRight);

	// Track Output
	_output2->SetFrame(_output1);
	_output1->SetFrame(frame);
}

bool BiQuadFilter::HasOutput(float absoluteTime) const
{
	return true;
}

void BiQuadFilter::Set_LPF(unsigned int samplingRate)
{
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	_a0 = 1.0f + alpha;
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - alpha;

	_b0 = 0.5f * (1.0f - cosf(w0));
	_b1 = 1.0f - cosf(w0);
	_b2 = 0.5f * (1.0f - cosf(w0));
}
void BiQuadFilter::Set_HPF(unsigned int samplingRate)
{
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	_a0 = 1.0f + alpha;
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - alpha;

	_b0 = 0.5f * (1.0f + cosf(w0));
	_b1 = -1.0f * (1.0f + cosf(w0));
	_b2 = 0.5f * (1.0f + cosf(w0));
}
void BiQuadFilter::Set_BPF_Gain(unsigned int samplingRate)
{
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	_a0 = 1.0f + alpha;
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - alpha;

	_b0 = q * alpha;
	_b1 = 0;
	_b2 = -1.0f * q * alpha;
}
void BiQuadFilter::Set_BPF_Flat(unsigned int samplingRate)
{
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	_a0 = 1.0f + alpha;
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - alpha;

	_b0 = alpha;
	_b1 = 0;
	_b2 = -1.0f * alpha;
}
void BiQuadFilter::Set_Notch(unsigned int samplingRate)
{
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	_a0 = 1.0f + alpha;
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - alpha;

	_b0 = 1.0f;
	_b1 = -2.0f * cosf(w0);
	_b2 = 1.0f;
}
void BiQuadFilter::Set_APF(unsigned int samplingRate)
{
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	_a0 = 1.0f + alpha;
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - alpha;

	_b0 = 1.0f - alpha;
	_b1 = -2.0f * cosf(w0);
	_b2 = 1.0f + alpha;
}
void BiQuadFilter::Set_PeakingEQ(unsigned int samplingRate)
{
	float dbGain = this->GetGainDb();
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	// A (Gain)
	float AGain = powf(10, dbGain / 40.0f);

	_a0 = 1.0f + (alpha / AGain);
	_a1 = -2.0f * cosf(w0);
	_a2 = 1.0f - (alpha / AGain);

	_b0 = 1.0f + (alpha * AGain);
	_b1 = -2.0f * cosf(w0);
	_b2 = 1.0f - (alpha * AGain);
}
void BiQuadFilter::Set_LowShelf(unsigned int samplingRate)
{
	float dbGain = this->GetGainDb();
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	// A (Gain)
	float AGain = powf(10, dbGain / 40.0f);

	float AMinusOne = AGain - 1.0f;
	float APlusOne = AGain + 1.0f;

	_a0 = APlusOne + (AMinusOne * cosf(w0)) + (2.0f * sqrtf(AGain) * alpha);
	_a1 = -2.0f * (AMinusOne + (APlusOne * cosf(w0)));
	_a2 = APlusOne + (AMinusOne * cosf(w0)) - (2.0f * sqrtf(AGain) * alpha);

	_b0 = AGain * (APlusOne - (AMinusOne * cosf(w0)) + (2.0f * sqrtf(AGain) * alpha));
	_b1 = 2.0f * AGain * (AMinusOne - (APlusOne * cosf(w0)));
	_b2 = AGain * (APlusOne - (AMinusOne * cosf(w0)) - (2.0f * sqrtf(AGain) * alpha));
}
void BiQuadFilter::Set_HighShelf(unsigned int samplingRate)
{
	float dbGain = this->GetGainDb();
	float corner = this->GetCorner();
	float q = this->GetQ();

	// Omega-naught
	float w0 = 2 * std::numbers::pi * corner * samplingRate;

	// Alpha
	float alpha = sinf(w0) / (2.0f * q);

	// A (Gain)
	float AGain = powf(10, dbGain / 40.0f);

	float AMinusOne = AGain - 1.0f;
	float APlusOne = AGain + 1.0f;

	_a0 = APlusOne - (AMinusOne * cosf(w0)) + (2.0f * sqrtf(AGain) * alpha);
	_a1 = 2.0f * (AMinusOne - (APlusOne * cosf(w0)));
	_a2 = APlusOne - (AMinusOne * cosf(w0)) - (2.0f * sqrtf(AGain) * alpha);

	_b0 = AGain * (APlusOne + (AMinusOne * cosf(w0)) + (2.0f * sqrtf(AGain) * alpha));
	_b1 = -2.0f * AGain * (AMinusOne + (APlusOne * cosf(w0)));
	_b2 = AGain * (APlusOne + (AMinusOne * cosf(w0)) - (2.0f * sqrtf(AGain) * alpha));
}