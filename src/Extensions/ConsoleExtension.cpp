// Copyright Alternity Arts. All Rights Reserved.

#include "ConsoleExtension.h"

#include "Program.h"

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			NProgram::RequestShutdown(TEXT("CtrlHandler"));
			return TRUE;
		default:
			return FALSE;
	}
}

void CConsoleExtension::Initialize()
{
	Super::Initialize();

	AllocConsole();
	freopen_s(&File, "CONOUT$", "w", stdout);

	SetConsoleCtrlHandler(CtrlHandler, TRUE);
}

void CConsoleExtension::Shutdown()
{
	if (File)
	{
		SetConsoleCtrlHandler(CtrlHandler, FALSE);

		fclose(File);
		File = nullptr;
		FreeConsole();
	}
	
	Super::Shutdown();
}
