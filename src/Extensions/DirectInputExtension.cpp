// Copyright Alternity Arts. All Rights Reserved.

#include "DirectInputExtension.h"

#include "MinHook.h"
#include "Program.h"

BOOL CALLBACK HandleEnumDevices(const DIDEVICEINSTANCE* pdidInstance, void* pvRef)
{
	if (!pdidInstance) return DIENUM_CONTINUE;

	const EInputDeviceType type = (EInputDeviceType)LOBYTE(pdidInstance->dwDevType);
	const SString typeAsStr = *ToString(type);

	const GUID& id = pdidInstance->guidInstance;
	const SString idAsStr = ToString(pdidInstance->guidInstance);

	switch (type)
	{
		case EInputDeviceType::Keyboard:
		case EInputDeviceType::Mouse:
		case EInputDeviceType::Gamepad:
			break;
		default:
			LogDebug(TEXT("CInputPatched: Skipped creation of '%s' device '%s'"), *typeAsStr, *idAsStr);
			return DIENUM_CONTINUE;
	}

	CInputPatched* owner = (CInputPatched*)pvRef;

	LPDIRECTINPUTDEVICE8 newDevice;
	if (FAILED(owner->GetImpl()->CreateDevice(pdidInstance->guidInstance, &newDevice, nullptr)))
	{
		if (newDevice)
		{
			newDevice->Release();
		}

		LogWarning(TEXT("CInputPatched: Failed to create '%s' device '%s'"), *typeAsStr, *idAsStr);
		return DIENUM_CONTINUE;
	}

	const bool alreadyExists = owner->_devices.ContainsByFunc(
		[&id](const TComPtr<CInputDevicePatched>& ptr) -> bool
		{
			return IsEqualGUID(ptr->GetId(), id);
		}
	);

	if (!CHECK(!alreadyExists))
	{
		return DIENUM_CONTINUE;
	}

	LogInfo(TEXT("CInputPatched: Created new '%s' device '%s'"), *typeAsStr, *idAsStr);
	owner->_devices.Emplace(new CInputDevicePatched(newDevice, *pdidInstance), true);

	return DIENUM_CONTINUE;
}

HRESULT WINAPI HandleDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	CDirectInputExtension* ext = NProgram::Extensions::Get<CDirectInputExtension>();
	if (!ext->GetCOM().IsValid())
	{
		LogWarning(TEXT("CInputPatched: Can't find input"));
		return S_FALSE;
	}

	*ppvOut = ext->GetCOM().Get();
	ext->GetCOM()->AddRef(); // since we are exposing, we need to add ref

	return S_OK;
}

SString ToString(EInputDeviceType type)
{
	switch (type)
	{
		case EInputDeviceType::Device: return TEXT("Device");
		case EInputDeviceType::Mouse: return TEXT("Mouse");
		case EInputDeviceType::Keyboard: return TEXT("Keyboard");
		case EInputDeviceType::Joystick: return TEXT("Joystick");
		case EInputDeviceType::Gamepad: return TEXT("Gamepad");
		case EInputDeviceType::Driving: return TEXT("Driving");
		case EInputDeviceType::Flight: return TEXT("Flight");
		case EInputDeviceType::FirstPerson: return TEXT("FirstPerson");
		case EInputDeviceType::DeviceCtrl: return TEXT("DeviceCtrl");
		case EInputDeviceType::ScreenPointer: return TEXT("ScreenPointer");
		case EInputDeviceType::Remote: return TEXT("Remote");
		case EInputDeviceType::Supplemental: return TEXT("Supplemental");
		default: return TEXT("Unknown");
	}
}

SString ToString(const GUID& guid)
{
	return SString::Printf(TEXT("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
	);
}

CInputDevicePatched::CInputDevicePatched(IDirectInputDevice8A* impl, const DIDEVICEINSTANCE& data)
	: Super(impl)
	, _type((EInputDeviceType)LOBYTE(data.dwDevType))
	, _idAsStr(ToString(data.guidInstance))
	, _data(data)
{}

ULONG CInputDevicePatched::AddRef()
{
	const ULONG refs = Super::AddRef();
	LogDebug(TEXT("CInputDevicePatched %s: AddRef '%d'"), *_idAsStr, refs);
	return refs;
}

ULONG CInputDevicePatched::Release()
{
	const ULONG refs = Super::Release();
	LogDebug(TEXT("CInputDevicePatched %s: Release '%d'"), *_idAsStr, refs);
	return refs;
}

HRESULT CInputDevicePatched::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	return CDirectInputDevice8Proxy::GetDeviceState(cbData, lpvData);
}

HRESULT CInputDevicePatched::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	return CDirectInputDevice8Proxy::GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT CInputDevicePatched::BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
{
	if (_map.Format.IsSet())
	{
		*lpActionFormat = _map.Format.GetRef();
		return S_OK;
	}

	const HRESULT result = CDirectInputDevice8Proxy::BuildActionMap(lpActionFormat, lpszUserName, dwFlags);
	if (SUCCEEDED(result) && lpActionFormat)
	{
		_map.Format = *lpActionFormat;
		LogInfo(TEXT("CInputDevicePatched %s: Map built for '%s'"), *_idAsStr, lpszUserName);
	}

	return result;
}

HRESULT CInputDevicePatched::SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
{
	if (!(dwFlags & DIDSAM_FORCESAVE) && _map.Saved)
	{
		return S_OK;
	}

	const HRESULT result = CDirectInputDevice8Proxy::SetActionMap(lpActionFormat, lpszUserName, dwFlags);
	if (SUCCEEDED(result))
	{
		_map.Saved = true;
		LogInfo(TEXT("CInputDevicePatched %s: Map applied for '%s'"), *_idAsStr, lpszUserName);
	}

	return result;
}

CInputPatched::CInputPatched(IDirectInput8A* impl)
	: Super(impl)
{
	impl->EnumDevices(DI8DEVCLASS_ALL, HandleEnumDevices, this, DIEDFL_ATTACHEDONLY);
}

ULONG CInputPatched::AddRef()
{
	const ULONG refs = Super::AddRef();
	LogDebug(TEXT("CInputPatched: AddRef '%d'"), refs);
	return refs;
}

ULONG CInputPatched::Release()
{
	const ULONG refs = Super::Release();
	LogDebug(TEXT("CInputPatched: Release '%d'"), refs);
	return refs;
}

HRESULT CInputPatched::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	const int32 foundIdx = _devices.FindIndexByFunc(
		[&rguid](const TComPtr<CInputDevicePatched>& ptr) -> bool
		{
			return IsEqualGUID(ptr->GetId(), rguid);
		}
	);

	if (foundIdx == INDEX_NONE)
	{
		LogWarning(TEXT("CInputPatched: Can't find '%s' device"), *ToString(rguid));
		return S_FALSE;
	}

	*lplpDirectInputDevice = _devices[foundIdx].Get();
	_devices[foundIdx]->AddRef(); // since we are exposing, we need to add ref
	return S_OK;
}

HRESULT CInputPatched::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	for (const TComPtr<CInputDevicePatched>& device : _devices)
	{
		const BOOL result = lpCallback(&device->GetData(), pvRef);
		if (result == DIENUM_STOP) break;
	}

	return S_OK;
}

HRESULT CInputPatched::EnumDevicesBySemantics(LPCSTR pszUserName, LPDIACTIONFORMAT lpActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	for (uint16 i = 0; i < _devices.GetNum(); ++i)
	{
		const auto& device = _devices[i];
		const uint16 remaining = (_devices.GetNum() - 1) - i;

		const BOOL result = lpCallback(&device->GetData(), device.Get(), DIEDBS_RECENTDEVICE, remaining, pvRef);
		if (result == DIENUM_STOP) break;
	}

	return S_OK;
}

void CDirectInputExtension::Initialize()
{
	Super::Initialize();

	_inputModuleHandle = GetModuleHandle("dinput8.dll");
	if (!CHECK(_inputModuleHandle)) return;

	LPDIRECTINPUT8 pDI = nullptr;
	const HRESULT result = DirectInput8Create(NProgram::GetEXEHandle(), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDI, nullptr);
	if (!CHECK(SUCCEEDED(result)))
	{
		if (pDI) pDI->Release();
		return;
	}

	_input.Reset(new CInputPatched(pDI), true);

	const LPVOID targetHook = GetProcAddress(_inputModuleHandle, "DirectInput8Create");
	if (CHECK(MH_CreateHook(targetHook, &HandleDirectInput8Create, nullptr) == MH_OK) &&
		CHECK(MH_EnableHook(targetHook) == MH_OK))
	{
		_createInputHook = targetHook;
	}
}

void CDirectInputExtension::Shutdown()
{
	CHECK(MH_RemoveHook(_createInputHook) == MH_OK);
	Super::Shutdown();
}