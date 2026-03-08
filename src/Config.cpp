// Copyright Alternity Arts. All Rights Reserved

#include "Config.h"

#include <filesystem>

#include "Program.h"

namespace NConfig
{
	bool HasConfigFile(std::string* outFullFilename)
	{
		const std::string configFile = NProgram::GetInjectedDirectory() + "/" + CONFIG_FILENAME;
		bool found = std::filesystem::exists(configFile);
		if (outFullFilename) *outFullFilename = std::move(configFile);
		return found;
	}

	bool TryGetString(const tchar* sectionName, const tchar* variableName, SString& outValue)
	{
		std::string configFile;
		if (!HasConfigFile(&configFile)) return false;

		char buffer[SCString::SMALL_BUFFER_SIZE];
		DWORD result = GetPrivateProfileStringA(sectionName, variableName, nullptr, buffer, SCString::SMALL_BUFFER_SIZE, configFile.c_str());
		if (result == 0) return false;

		outValue = buffer;
		return true;
	}

	bool TryGetInt32(const tchar* sectionName, const tchar* variableName, int32& outValue)
	{
		SString rawValue;
		if (!TryGetString(sectionName, variableName, rawValue)) return false;

		outValue = rawValue.ToInt32();
		return true;
	}

	bool TryGetDouble(const tchar* sectionName, const tchar* variableName, double& outValue)
	{
		SString rawValue;
		if (!TryGetString(sectionName, variableName, rawValue)) return false;

		outValue = rawValue.ToDouble();
		return true;
	}

	bool TryGetBoolean(const tchar* sectionName, const tchar* variableName, bool& outValue)
	{
		SString rawValue;
		if (!TryGetString(sectionName, variableName, rawValue)) return false;

		rawValue.ToLowerInline();

		if (rawValue == TEXT("1") || rawValue == TEXT("true") || rawValue == TEXT("yes")) outValue = true;
		else if (rawValue == TEXT("0") || rawValue == TEXT("false") || rawValue == TEXT("no")) outValue = false;
		else return false;

		return true;
	}
}
