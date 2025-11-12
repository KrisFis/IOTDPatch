// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ASTD/ASTD.h"

#include <iostream>

template<typename Fmt, typename... ArgsT>
FORCEINLINE void LogDebug(Fmt&& fmt, ArgsT&&... args)
{
#if BUILD_DEBUG
	std::cout << "DEBUG: " << *SString::Printf(fmt, Forward<ArgsT>(args)...) << std::endl;
#endif
}

template<typename Fmt, typename... ArgsT>
FORCEINLINE void LogInfo(Fmt&& fmt, ArgsT&&... args)
{
	std::cout << *SString::Printf(fmt, Forward<ArgsT>(args)...) << std::endl;
}

template<typename Fmt, typename... ArgsT>
FORCEINLINE void LogWarning(Fmt&& fmt, ArgsT&&... args)
{
	std::cout << "WARNING: " << *SString::Printf(fmt, Forward<ArgsT>(args)...) << std::endl;
}

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
	HMODULE GetEXEHandle();
	HMODULE GetDLLHandle();

	void WaitForDebugger();

	bool IsShutdownRequested();
	void RequestShutdown(const SString& reason = SString::GetEmpty());

}

namespace _NProgram
{
	void Init(HMODULE DLLHandle);
	int32 Main();
}