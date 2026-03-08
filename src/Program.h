// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ASTD/ASTDMinimal.h"
#include "ASTD/String.h"

#include <iostream>

std::string ToUTF8(const wchar_t* wstr);
std::wstring ToWChar(const char* str);

FORCEINLINE std::string SStringToStd(const SString& str)
{
#if ASTD_USE_UNICODE
	return ToUTF8(*str);
#else
	return std::string(*str);
#endif
}

namespace NProgram
{
	HMODULE GetEXEHandle();
	HMODULE GetDLLHandle();

	std::string GetInjectedDirectory();

	void WaitForDebugger();

	bool IsShutdownRequested();
	void RequestShutdown(const SString& reason = SString::GetEmpty());
}

namespace _NProgram
{
	void Init(HMODULE DLLHandle);
	int32 Main();
}