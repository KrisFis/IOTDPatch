// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ProgramExtension.h"
#include "DirectInput.h"

class CInputDevicePatched final : public CDirectInputDevice8Proxy
{
public:
	typedef CDirectInputDevice8Proxy Super;
	using Super::Super;

	STDOVERRIDEMETHODIMP GetDeviceState(DWORD cbData, LPVOID lpvData);
	STDOVERRIDEMETHODIMP GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
	STDOVERRIDEMETHODIMP BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);
	STDOVERRIDEMETHODIMP SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);
};

class CInputPatched final : public CDirectInput8Proxy
{
public:
	typedef CDirectInput8Proxy Super;
	using Super::Super;

	STDOVERRIDEMETHODIMP CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
	STDOVERRIDEMETHODIMP EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDOVERRIDEMETHODIMP EnumDevicesBySemantics(LPCSTR pszUserName, LPDIACTIONFORMAT lpActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags);
};

class CDirectInputExtension final : public IProgramExtension 
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