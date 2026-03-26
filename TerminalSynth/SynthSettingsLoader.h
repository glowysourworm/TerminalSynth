#pragma once

#ifndef SYNTH_SETTINGS_LOADER_H
#define SYNTH_SETTINGS_LOADER_H

#include "SynthSettings.h"
#include <string>

class SynthSettingsLoader
{
public:

	// Terminal Synth Default Configuration File
	const char* DEFAULT_CONFIG_FILE_NAME = ".terminal-synth-config";

public:

	SynthSettingsLoader()
	{
		_instance = nullptr;
		_instance = CreateDefeaultConfiguration(true);
	}
	~SynthSettingsLoader()
	{
		delete _instance;
	}

	/// <summary>
	/// Creates a basic configuration with features:  Sound Bank (off), STK (off)
	/// </summary>
	SynthSettings* CreateDefeaultConfiguration(bool setAsCurrent);

	/// <summary>
	/// Loads configuration from file with option to set as current (see GetCurrent())
	/// </summary>
	SynthSettings* LoadConfiguration(const std::string& fileName, bool setAsCurrent);

	/// <summary>
	/// Saves (current) configuration to (default) file with option to set as current (see GetCurrent())
	/// </summary>
	void SaveConfiguration();

	/// <summary>
	/// Saves configuration to (default) file with option to set as current (see GetCurrent())
	/// </summary>
	void SaveConfiguration(SynthSettings* configuration, bool setAsCurrent);

	/// <summary>
	/// Saves configuration to file with option to set as current (see GetCurrent())
	/// </summary>
	void SaveConfiguration(SynthSettings* configuration, const std::string& fileName, bool setAsCurrent);

	/// <summary>
	/// Gets current configuration instance
	/// </summary>
	SynthSettings* GetCurrent() const { return _instance; }

private:

	SynthSettings* _instance;
};

#endif