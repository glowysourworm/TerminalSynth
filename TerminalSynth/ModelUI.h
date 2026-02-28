#pragma once

#ifndef MODELUI_H
#define MODELUI_H

#include <string>

class ModelUI
{
public:

	ModelUI() {}
	~ModelUI() {}

	virtual std::string GetName() const = 0;
};

#endif