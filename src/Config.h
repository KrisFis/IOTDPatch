// Copyright Alternity Arts. All Rights Reserved

#pragma once

#include "ASTD/ASTDMinimal.h"

#include <string>

namespace NConfig
{
	constexpr const tchar* CONFIG_FILENAME = TEXT("IOTDPatch.ini");

	bool HasConfigFile(std::string* outFullFilename = nullptr);
	bool TryGetString(const tchar* sectionName, const tchar* variableName, SString& outValue);
	bool TryGetInt32(const tchar* sectionName, const tchar* variableName, int32& outValue);
	bool TryGetDouble(const tchar* sectionName, const tchar* variableName, double& outValue);
	bool TryGetBoolean(const tchar* sectionName, const tchar* variableName, bool& outValue);
}
