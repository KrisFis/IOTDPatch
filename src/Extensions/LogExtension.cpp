// Copyright Alternity Arts. All Rights Reserved.

#include "LogExtension.h"

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

void CLogExtension::Initialize()
{
	Super::Initialize();

#if BUILD_DEBUG
	if (AllocConsole() && freopen_s(&_conOut, "CONOUT$", "w", stdout) == 0)
	{
		//std::ios::sync_with_stdio(true);
		_conBuf = std::cout.rdbuf();

		SetConsoleCtrlHandler(CtrlHandler, TRUE);
	}
#endif

	_logFile = std::ofstream(LOG_FILENAME, std::ios::out | std::ios::trunc);

	_dbuf = StreamDualBuf(_conBuf, _logFile.rdbuf());
	_oldBuf = std::cout.rdbuf(&_dbuf);
}

void CLogExtension::Shutdown()
{
	if (_oldBuf)
	{
		std::cout.rdbuf(_oldBuf);
		_oldBuf = nullptr;
	}

	if (_logFile.is_open())
	{
		_logFile.flush();
		_logFile.close();
	}

#if BUILD_DEBUG
	if (_conOut)
	{
		fclose(_conOut);
		_conOut = nullptr;
	}

	if (_conBuf)
	{
		SetConsoleCtrlHandler(CtrlHandler, FALSE);
		FreeConsole();

		_conBuf = nullptr;
	}
#endif

	Super::Shutdown();
}
