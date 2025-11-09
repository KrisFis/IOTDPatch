// Copyright Alternity Arts. All Rights Reserved.

#pragma once
#include "ProgramExtension.h"

class CConsoleExtension : public IProgramExtension
{
public:
	typedef IProgramExtension Super;

	// ~BEGIN IProgramExtension interface
	virtual void Initialize() override;
	virtual void Tick(double deltaTime) override {}
	virtual void Shutdown() override;
	// ~END IProgramExtension interface
	
	FILE* File = nullptr;
};

DECLARE_EXTENSION(CConsoleExtension)