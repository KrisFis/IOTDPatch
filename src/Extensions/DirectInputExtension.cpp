// Copyright Alternity Arts. All Rights Reserved.

#include "DirectInputExtension.h"

#include "MinHook.h"
#include "Program.h"

HRESULT(__stdcall *oGetDeviceState)(IDirectInputDevice8*		, DWORD  	  , LPVOID		  ) = nullptr;
HRESULT __stdcall dGetDeviceState  (IDirectInputDevice8* thisPtr, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = oGetDeviceState(thisPtr, cbData, lpvData);
	if (FAILED(result)) return result;

	if (cbData == sizeof(DIMOUSESTATE)) // mouse
	{
		
	}
	else if (cbData == sizeof(DIMOUSESTATE2)) // mouse
	{
		
	}

	return result;
}

HRESULT(__stdcall *oGetDeviceData)(IDirectInputDevice8*		   , DWORD			   , LPDIDEVICEOBJECTDATA	   , LPDWORD		 , DWORD		) = nullptr;
HRESULT __stdcall dGetDeviceData  (IDirectInputDevice8* thisPtr, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	HRESULT result = oGetDeviceData(thisPtr, cbObjectData, rgdod, pdwInOut, dwFlags);
	if (FAILED(result)) return result;

	*pdwInOut = 0; // TEST: block input
	return result;
}

HRESULT(__stdcall *oCreateDevice)(IDirectInput8*		, REFGUID	   , LPDIRECTINPUTDEVICE8A*			, LPUNKNOWN			 ) = nullptr;
HRESULT __stdcall dCreateDevice	 (IDirectInput8* thisPtr, REFGUID rguid, LPDIRECTINPUTDEVICE8A* ppDevice, LPUNKNOWN pUnkOuter)
{
	HRESULT result = oCreateDevice(thisPtr, rguid, ppDevice, pUnkOuter);
	if (FAILED(result) || !ppDevice || !*ppDevice) return result;

	CDirectInputExtension* ext = NProgram::Extensions::Get<CDirectInputExtension>();
	if (!CHECK(ext->IsInitialized())) return result;

	TDirectInputObject dIDevice(*ppDevice);
	if (dIDevice.HookVTableFunction(9, &dGetDeviceState, (LPVOID*)&oGetDeviceState) &&
		dIDevice.HookVTableFunction(10, &dGetDeviceData, (LPVOID*)&oGetDeviceData))
	{
		ext->DInputDevices.Add(Move(dIDevice));
	}
	else
	{
		std::cout << TEXT("Hooking IDirectInputDevice8::GetDeviceState failed") << std::endl;
	}

	return result;
}

HRESULT(WINAPI *oDirectInput8Create)(HINSTANCE		, DWORD			 , REFIID		 , LPVOID*		 , LPUNKNOWN		  ) = nullptr;
HRESULT WINAPI dDirectInput8Create	(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	HRESULT result = oDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	if (FAILED(result) || !ppvOut || !*ppvOut) return result;

	CDirectInputExtension* ext = NProgram::Extensions::Get<CDirectInputExtension>();
	if (!ext->IsInitialized()) return result;
	
	auto& dInput = ext->DInput;

	dInput.Reset((IDirectInput8*)*ppvOut);
	if (!dInput.HookVTableFunction(3, &dCreateDevice, (LPVOID*)&oCreateDevice))
	{
		dInput.Reset();
		std::cout << TEXT("Hooking IDirectInput8::CreateDevice failed") << std::endl;
	}
	
	return result;
}

namespace NDirectInput
{
	bool Hook(LPVOID targetFunc, LPVOID detourFunc, LPVOID* originalFunc)
	{
		if (const MH_STATUS status = MH_CreateHook(targetFunc, detourFunc, originalFunc);
			!CHECK(status == MH_OK))
		{
			std::cout << "CreateHook failed. Status: " << MH_StatusToString(status) << std::endl;
			return false;
		}

		if (const MH_STATUS status = MH_EnableHook(targetFunc);
			!CHECK(status == MH_OK))
		{
			std::cout << "EnableHook failed. Status: " << MH_StatusToString(status) << std::endl;
			return false;
		}

		return true;
	}

	bool Unhook(const TArray<LPVOID>& hooks)
	{
		bool result = true;
		for (const LPVOID hook : hooks)
		{
			result &= MH_RemoveHook(hook) == MH_OK;
		}
		return result;
	}
}


void CDirectInputExtension::Initialize()
{
	Super::Initialize();
	
	DInput8Handle = GetModuleHandle("dinput8.dll");
	if (!CHECK(DInput8Handle)) return;

	if (LPVOID targetHook = GetProcAddress(DInput8Handle, "DirectInput8Create");
		NDirectInput::Hook(targetHook, &dDirectInput8Create, (LPVOID*)&oDirectInput8Create))
	{
		ModuleHooks.Add(targetHook);
	}

	DInputDevices.Reserve(4);
}

void CDirectInputExtension::Shutdown()
{
	DInputDevices.Empty();
	DInput.Reset();
	
	NDirectInput::Unhook(ModuleHooks);
	Super::Shutdown();
}
