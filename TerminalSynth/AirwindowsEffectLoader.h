#pragma once


#ifndef AIRWIN_EFFECT_LOADER_H
#define AIRWIN_EFFECT_LOADER_H

#include "AirwinRegistryEntry.h"
#include "SignalParameter.h"
#include "SignalSettings.h"
#include <airwin_consolidated_base.h>
#include <cctype>

class AirwindowsEffectLoader
{
public:

	static void LoadSettings(AirwinRegistryEntry* registryEntry, AudioEffectX* plugin, SignalSettings& destination);
};

void AirwindowsEffectLoader::LoadSettings(AirwinRegistryEntry* registryEntry, AudioEffectX* plugin, SignalSettings& destination)
{
	// Initialize
	destination.SetName(registryEntry->GetName());
	destination.SetCategory(registryEntry->GetCategory());
	destination.SetInfoText(registryEntry->GetWhatText());
	destination.SetIsEnabled(true);

	// Set Parameters
	//
	for (int index = 0; index < registryEntry->GetNumberOfParams() - 1; index++)
	{
		char paramName[100];
		plugin->getParameterName(index, paramName);

		// VALIDATE STRING! THESE HAD PROBLEMS WITH NULL TERMINATORS!
		for (int index = 0; index < 100; index++)
		{
			if (!std::isalnum(paramName[index]))
			{
				paramName[index] = '\0';
				break;
			}
		}

		SignalParameter parameter(paramName, plugin->getParameter(index), 0.0f, 1.0f);
		destination.AddParameter(parameter);
	}
}

#endif