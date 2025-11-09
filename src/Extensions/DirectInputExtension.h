// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ProgramExtension.h"
#include "DirectInput.h"

class CInputDevicePatched : public CDirectInputDevice8Proxy
{
public:
	typedef CDirectInputDevice8Proxy Super;
	using Super::Super;
};

class CInputPatched : public CDirectInput8Proxy
{
public:
	typedef CDirectInput8Proxy Super;
	using Super::Super;

	STDOVERRIDEMETHODIMP CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
	STDOVERRIDEMETHODIMP EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDOVERRIDEMETHODIMP GetDeviceStatus(REFGUID rguidInstance);
	STDOVERRIDEMETHODIMP FindDevice(REFGUID rguidClass, LPCSTR pszName, LPGUID pguidInstance);
};

class CDirectInputExtension : public IProgramExtension 
{
public:
	typedef IProgramExtension Super;
	
	// ~BEGIN IProgramExtension interface
	virtual void Initialize() override;
	virtual void Tick(double deltaTime) override {}
	virtual void Shutdown() override;
	// ~END IProgramExtension interface

	HMODULE DInput8Handle = nullptr;
	TArray<LPVOID> Hooks;
};

DECLARE_EXTENSION(CDirectInputExtension)