#pragma once

#ifndef METRIC_UI_INFO_H
#define METRIC_UI_INFO_H

/// <summary>
/// Real time metrics for UI handling
/// </summary>
struct MetricUIInfo
{
public:

	MetricUIInfo()
	{
		this->avgUIMilli = 0;
		this->avgUIDataFetchMicro = 0;
		this->avgUILockAcqcuireNano = 0;
		this->avgUIRenderingMilli = 0;
		this->avgUISleepMilli = 0;
	}
	MetricUIInfo(const MetricUIInfo& copy)
	{
		this->avgUIMilli = copy.avgUIMilli;
		this->avgUIDataFetchMicro = copy.avgUIDataFetchMicro;
		this->avgUILockAcqcuireNano = copy.avgUILockAcqcuireNano;
		this->avgUIRenderingMilli = copy.avgUIRenderingMilli;
		this->avgUISleepMilli = copy.avgUISleepMilli;
	}

	float avgUIMilli;
	float avgUIDataFetchMicro;
	float avgUILockAcqcuireNano;
	float avgUIRenderingMilli;
	float avgUISleepMilli;
};

#endif