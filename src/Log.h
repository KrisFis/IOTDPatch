// Copyright Alternity Arts. All Rights Reserved

#pragma once

#include "ASTD/ASTDMinimal.h"

enum class ELogVerbosity : uint8
{
	Error = 0,
	Warning,
	Info,
	Verbose,
	VeryVerbose
};

ELogVerbosity GetMinLogVerbosity();
void LogImpl(ELogVerbosity verbosity, const SString& message);
FORCEINLINE bool LogVerbosityEnabled(ELogVerbosity verbosity) { return (uint8)verbosity <= (uint8)GetMinLogVerbosity(); }

#define LOG(Verbosity, Msg) if (LogVerbosityEnabled(ELogVerbosity::Verbosity)) LogImpl(ELogVerbosity::Verbosity, Msg)
#define LOGF(Verbosity, Fmt, ...) if (LogVerbosityEnabled(ELogVerbosity::Verbosity)) LogImpl(ELogVerbosity::Verbosity, SString::Printf(Fmt, ## __VA_ARGS__))