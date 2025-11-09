// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ASTD/ASTD.h"

#include <iostream>

std::string ToUTF8(const wchar_t* wstr);
std::wstring ToWChar(const char* str);

void WaitForDebugger();

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
	void WaitForDebugger();
	
	bool IsShutdownRequested();
	void RequestShutdown(const SString& reason = SString::GetEmpty());

	int32 Main();
}