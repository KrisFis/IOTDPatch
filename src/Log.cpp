// Copyright Alternity Arts. All Rights Reserved

#include "Log.h"

#include "Config.h"
#include "ASTD/String.h"

#include <iostream>

ELogVerbosity GetMinLogVerbosity()
{
	static ELogVerbosity verbosity = ELogVerbosity::Error;
	static bool evaluated = false;
	if (!evaluated)
	{
		evaluated = true;

		SString minLoggingVerbosityStr;
		if (NConfig::TryGetString(TEXT("Debugging"), TEXT("MinimalLogVerbosity"), minLoggingVerbosityStr))
		{
			minLoggingVerbosityStr.ToLowerInline();
			if (minLoggingVerbosityStr == TEXT("error")) verbosity = ELogVerbosity::Error;
			else if (minLoggingVerbosityStr == TEXT("warning")) verbosity = ELogVerbosity::Warning;
			else if (minLoggingVerbosityStr == TEXT("info")) verbosity = ELogVerbosity::Info;
			else if (minLoggingVerbosityStr == TEXT("verbose")) verbosity = ELogVerbosity::Verbose;
			else if (minLoggingVerbosityStr == TEXT("veryverbose")) verbosity = ELogVerbosity::VeryVerbose;
		}
	}

	return verbosity;
}

void LogImpl(ELogVerbosity verbosity, const SString& message)
{
	std::ostream* stream = nullptr;
	switch (verbosity)
	{
		case ELogVerbosity::Error:
			stream = &std::cerr;
			break;
		default:
			stream = &std::cout;
			break;
	}

	// is automatically routed to logfile and console (see CLogExtension::Initialize)
	*stream << *message << std::endl;
}