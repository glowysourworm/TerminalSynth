#include "CombFilter.h"
#include "PlaybackFrame.h"
#include "PlaybackInfo.h"
#include "SignalParameterizedBase.h"
#include <queue>

CombFilter::CombFilter(float delaySeconds, float gain, bool feedback) : SignalParameterizedBase("CombFilter")
{
	_bufferL = new std::queue<float>();
	_bufferR = new std::queue<float>();

	_delaySeconds = delaySeconds;
	_gain = gain;
	_feedback = feedback;
}

CombFilter::~CombFilter()
{
	delete _bufferL;
	delete _bufferR;
}

void CombFilter::Initialize(const PlaybackInfo* parameters)
{
	//SignalParameterizedBase::Initialize(parameters);

	this->AddParameter("Delay", 0.01f, 1.0f, _delaySeconds);
	this->AddParameter("Gain", 0.0f, 1.0f, _gain);
	this->AddParameter("Feedback", 0.0f, 1.0f, _feedback ? 1.0f : 0.0f);

	float delaySeconds = this->GetParameterValue(0);

	int bufferSize = (int)(delaySeconds * parameters->GetStreamInfo()->streamSampleRate);

	// Initialize the queue
	for (int i = 0; i < bufferSize; i++)
	{
		_bufferL->push(0);
		_bufferR->push(0);
	}
}

void CombFilter::SetFrameImpl(PlaybackFrame* frame)
{
	float delay = this->GetParameterValue(0);
	float gain = this->GetParameterValue(1);
	bool feedback = this->GetParameterValue(2) > 0.5f;

	// Calculate sample from front of the queue (SAME FOR BOTH FORMS)
	float outputL = frame->GetLeft() + (gain * _bufferL->front());
	float outputR = frame->GetRight() + (gain * _bufferR->front());

	// Remove used sample
	_bufferL->pop();

	// Feed-forward -> store input sample
	if (!feedback)
	{
		_bufferL->push(frame->GetLeft());
		_bufferR->push(frame->GetRight());
	}

	// Feed-back -> store result
	else
	{
		_bufferL->push(outputL);
		_bufferR->push(outputR);
	}

	frame->SetFrame(outputL, outputR);
}

bool CombFilter::HasOutput(double absoluteTime) const
{
	return this->GetParameterValue(1) > 0.0f;
}
