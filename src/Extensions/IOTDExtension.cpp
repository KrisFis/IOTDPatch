// Copyright Alternity Arts. All Rights Reserved

#include "IOTDExtension.h"

#include "Program.h"
#include "Extensions/DirectInputExtension.h"

static void PrintActiveMapping(const int32 deviceIdx = INDEX_NONE)
{
	NProgram::Extensions::Get<CDirectInputExtension>()->PrintActiveActionMaps(deviceIdx);
}

void CIOTDExtension::Tick(double deltaTime)
{
	IProgramExtension::Tick(deltaTime);

#if BUILD_DEBUG
	if (GetAsyncKeyState(VK_NUMPAD0)) PrintActiveMapping();
	else if (GetAsyncKeyState(VK_NUMPAD1)) PrintActiveMapping(0);
	else if (GetAsyncKeyState(VK_NUMPAD2)) PrintActiveMapping(1);
	else if (GetAsyncKeyState(VK_NUMPAD3)) PrintActiveMapping(2);
	else if (GetAsyncKeyState(VK_NUMPAD4)) PrintActiveMapping(3);
	else if (GetAsyncKeyState(VK_NUMPAD5)) PrintActiveMapping(4);
	else if (GetAsyncKeyState(VK_NUMPAD6)) PrintActiveMapping(5);
	else if (GetAsyncKeyState(VK_NUMPAD7)) PrintActiveMapping(6);
	else if (GetAsyncKeyState(VK_NUMPAD8)) PrintActiveMapping(7);
	else if (GetAsyncKeyState(VK_NUMPAD9)) PrintActiveMapping(8);
#endif
}
