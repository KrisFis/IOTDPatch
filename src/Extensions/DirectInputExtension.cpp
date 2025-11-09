// Copyright Alternity Arts. All Rights Reserved.

#include "DirectInputExtension.h"

#include "MinHook.h"
#include "Program.h"

HRESULT(WINAPI *oDirectInput8Create)(HINSTANCE		, DWORD			 , REFIID		 , LPVOID*		 , LPUNKNOWN		  ) = nullptr;
HRESULT WINAPI dDirectInput8Create	(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	HRESULT result = oDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	if (FAILED(result) || !ppvOut || !*ppvOut) return result;

	*ppvOut = new CInputPatched((IDirectInput8*)*ppvOut);
	return result;
}

HRESULT CInputPatched::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	HRESULT result = Super::CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
	if (FAILED(result) || !lplpDirectInputDevice || !*lplpDirectInputDevice) return result;

	*lplpDirectInputDevice = new CInputDevicePatched(*lplpDirectInputDevice);
	return result;
}

HRESULT CInputPatched::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return Super::EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT CInputPatched::GetDeviceStatus(REFGUID rguidInstance)
{
	return Super::GetDeviceStatus(rguidInstance);
}

HRESULT CInputPatched::FindDevice(REFGUID rguidClass, LPCSTR pszName, LPGUID pguidInstance)
{
	return Super::FindDevice(rguidClass, pszName, pguidInstance);
}

void CDirectInputExtension::Initialize()
{
	Super::Initialize();

	DInput8Handle = GetModuleHandle("dinput8.dll");
	if (!CHECK(DInput8Handle)) return;

	const LPVOID targetHook = GetProcAddress(DInput8Handle, "DirectInput8Create");
	if (CHECK(MH_CreateHook(targetHook, &dDirectInput8Create, (LPVOID*)&oDirectInput8Create) == MH_OK) &&
		CHECK(MH_EnableHook(targetHook) == MH_OK))
	{
		Hooks.Add(targetHook);
	}
}

void CDirectInputExtension::Shutdown()
{
	for (const LPVOID hook : Hooks)
	{
		CHECK(MH_RemoveHook(hook) == MH_OK);
	}

	Hooks.Empty();
	Super::Shutdown();
}
