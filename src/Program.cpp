// Copyright Alternity Arts. All Rights Reserved

#include "Program.h"

#include <chrono>
#include <thread>

#include "MinHook.h"
#include "ProgramExtension.h"

std::string ToUTF8(const wchar_t* wstr)
{
	if (!wstr || !*wstr)
		return {};

	int len = WideCharToMultiByte(
		CP_UTF8,
		0, 
		wstr,
		-1, 
		nullptr,
		0,
		nullptr,
		nullptr
	);

	std::string out(len - 1, '\0'); // -1 to strip null terminator

	WideCharToMultiByte(
		CP_UTF8,
		0,
		wstr,
		-1,
		out.data(),
		len,
		nullptr,
		nullptr
	);

	return out;
}

std::wstring ToWChar(const char* str)
{
	if (!str || !*str)
		return {};

	const int32 len = MultiByteToWideChar(
		CP_UTF8,
		0,
		str,
		-1,
		nullptr,
		0
	);

	std::wstring out(len - 1, L'\0');

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str,
		-1,
		out.data(),
		len
	);

	return out;
}

namespace _NProgram
{
	std::atomic GShutdownRequested = false;

	bool Init()
	{
		if (!CHECK(MH_Initialize() == MH_OK))
		{
			return false;
		}

		for (IProgramExtension* extension : NProgram::Extensions::GetAll())
		{
			extension->Initialize();
		}

		return true;
	}

	void Shutdown()
	{
		for (IProgramExtension* extension : NProgram::Extensions::GetAll())
		{
			extension->Shutdown();
		}

		MH_Uninitialize();
	}

	void Tick(double deltaTime)
	{
		if (GetAsyncKeyState(VK_NUMPAD1))
		{
			std::cout << "Hello World!" << std::endl;
		}

		for (IProgramExtension* extension : NProgram::Extensions::GetAll())
		{
			extension->Tick(deltaTime);
		}
	}
}

namespace NProgram
{
	using namespace _NProgram;

	void WaitForDebugger()
	{
		while (!IsDebuggerPresent())
		{
			Sleep(100);
		}
	}
	
	bool IsShutdownRequested()
	{
		return GShutdownRequested;
	}

	void RequestShutdown(const SString& reason)
	{
		GShutdownRequested.store(true);
	}

	int32 Main()
	{
		if (const bool initResult = Init(); !initResult)
		{
			Shutdown();
			return EXIT_FAILURE;
		}

		constexpr double TICKS_PER_SECOND = 10.0;
		constexpr std::chrono::duration<double> TICKS_DURATION(1.0 / TICKS_PER_SECOND);

		auto previous = std::chrono::steady_clock::now();
		while (!GShutdownRequested.load())
		{
			auto loopStart = std::chrono::steady_clock::now();

			std::chrono::duration<double> delta = loopStart - previous;
			previous = loopStart;

			Tick(delta.count());

			auto loopEnd = std::chrono::steady_clock::now();
			auto elapsed = loopEnd - loopStart;

			if (elapsed < TICKS_DURATION)
				std::this_thread::sleep_for(TICKS_DURATION - elapsed);
		}

		Shutdown();
		return EXIT_SUCCESS;
	}
}
