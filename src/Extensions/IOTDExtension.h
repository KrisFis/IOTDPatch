// Copyright Alternity Arts. All Rights Reserved

#pragma once

#include "ProgramExtension.h"

class CIOTDExtension : public IProgramExtension
{
public:
	// ~BEGIN IProgramExtension interface
	virtual void Tick(double deltaTime) override;
	// ~END IProgramExtension interface
};

DECLARE_EXTENSION(CIOTDExtension)
