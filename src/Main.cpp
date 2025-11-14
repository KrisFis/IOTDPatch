// Copyright Alternity Arts. All Rights Reserved.

#include "Program.h"

#include "MinHook.h"

HMODULE GModule = nullptr;
DWORD WINAPI DllThreadUnload(LPVOID lpParameter)
{
	Sleep(100);
	FreeLibraryAndExitThread(GModule, 0);
}

DWORD WINAPI DllThreadMain(LPVOID)
{
	_NProgram::Init(GModule);
	_NProgram::Main();

	CreateThread(nullptr, 0, DllThreadUnload, nullptr, 0 , nullptr);
	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	if (reasonForCall != DLL_PROCESS_ATTACH) return TRUE;

	GModule = hModule;
	CreateThread(nullptr, 0, DllThreadMain, nullptr, 0, nullptr);

	return TRUE;
}