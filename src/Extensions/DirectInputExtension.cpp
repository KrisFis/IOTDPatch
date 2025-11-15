// Copyright Alternity Arts. All Rights Reserved.

#include "DirectInputExtension.h"

#include <set>

#include "MinHook.h"
#include "Program.h"

constexpr const char* CAMERA_CONTROL_ACTION_NAME = "DIA_GAME_CAMERACONTROL";
constexpr const char* CAMERA_X_NAME = "DIA_GAME_AXIS_X";
constexpr const char* CAMERA_Y_NAME = "DIA_GAME_AXIS_Y";

static struct
{
	std::set<DWORD> ActionSemantic_CameraControl;
	std::set<DWORD> ActionSemantic_CameraX;
	std::set<DWORD> ActionSemantic_CameraY;

	float MouseSensitivity = 0.2f;
	float CarryMouseX = 0.f, CarryMouseY = 0.f;
	bool ControlActionPressed = false;
} GRuntime;

// Checks if a given device object matches a semantic
// Returns TRUE if it matches, FALSE otherwise
bool DoesObjectMatchSemantic(
	DWORD semantic,                  // dwSemantic from DIACTION
	const DIDEVICEINSTANCE* instance,
	const DIDEVICEOBJECTINSTANCE* object,
	bool exact = true                // optional strictness
)
{
	// Extract low 8 bits for value/index
	DWORD value = semantic & 0xFF;

	// Extract axis info (bits 15-18)
	DWORD axis = (semantic >> 15) & 0xF;

	// Determine the expected type (DIDFT_*) from semantic
	DWORD expectedType = 0;
	switch (semantic & 0x700)      // semantic type mask
	{
		case 0x200: expectedType = DIDFT_ABSAXIS; break;
		case 0x300: expectedType = DIDFT_RELAXIS; break;
		case 0x400: expectedType = DIDFT_BUTTON; break;
		case 0x600: expectedType = DIDFT_POV; break;
		default: return false;       // unknown/unsupported type
	}

	// Check if the object's type matches the expected type
	if (!(DIDFT_GETTYPE(object->dwType) & expectedType))
		return false;

	// High 8 bits of semantic encode device class / special handling
	DWORD deviceCategory = semantic & 0xFF000000;

	switch (deviceCategory)
	{
		case 0x81000000: // Keyboard-specific semantic
			return ((instance->dwDevType & 0xF) == DI8DEVTYPE_KEYBOARD) &&
				   (object->dwOfs == value);

		case 0x82000000: // Mouse-specific semantic
			return ((instance->dwDevType & 0xF) == DI8DEVTYPE_MOUSE) &&
				   (object->dwOfs == value);

		case 0x83000000: // Not supported / ignored semantic
			return false;

		default:
			// Filter out keyboard/mouse if already handled
			if ((instance->dwDevType & 0xF) == DI8DEVTYPE_KEYBOARD) return false;
			if ((instance->dwDevType & 0xF) == DI8DEVTYPE_MOUSE)    return false;

			// fallthrough for general semantic
		case 0xFF000000:
			// Axis index must match if specified
			if (axis && (axis - 1) != DIDFT_GETINSTANCE(object->dwType))
				return false;

			// Optional: strict value check
			return !exact || !value || (value == DIDFT_GETINSTANCE(object->dwType) + 1);
	}
}

struct SFindDeviceObjectBySematicsParam
{
	const DIACTION* Action = nullptr;
	const DIDEVICEINSTANCE* Device = nullptr;
	LPCDIDEVICEOBJECTINSTANCE OutObject = nullptr;
};

BOOL CALLBACK FindDeviceObjectBySemantics(LPCDIDEVICEOBJECTINSTANCE obj, VOID* pvRef)
{
	SFindDeviceObjectBySematicsParam& param = *(SFindDeviceObjectBySematicsParam*)pvRef;

	// deviceMask = HIWORD(dwSemantic)
	// actionIndex = HIBYTE(dwSemantic)
	// actionCode = LOBYTE(dwSemantic)

	if (DoesObjectMatchSemantic(param.Action->dwSemantic, param.Device, obj, false))
	{
		param.OutObject = obj;
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

BOOL CALLBACK HandleEnumDevices(LPCDIDEVICEINSTANCE pdidInstance, VOID* pvRef)
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

	TComPtr<CInputDevicePatched>& device = owner->_devices.FindOrAdd(id);
	if (!CHECK(!device.IsValid()))
	{
		return DIENUM_CONTINUE;
	}

	LogInfo(TEXT("CInputPatched: Created new '%s' device '%s'"), *typeAsStr, *idAsStr);
	device.Reset(new CInputDevicePatched(newDevice, *pdidInstance), true);

	return DIENUM_CONTINUE;
}

HRESULT WINAPI HandleDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	const CDirectInputExtension* ext = NProgram::Extensions::Get<CDirectInputExtension>();
	if (!ext->GetInput().IsValid())
	{
		LogWarning(TEXT("CInputPatched: Can't find input"));
		return S_FALSE;
	}

	*ppvOut = ext->GetInput().Get();
	ext->GetInput()->AddRef(); // since we are exposing, we need to add ref

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

void SInputDeviceMap::Reset(const DIACTIONFORMAT& data)
{
	if (IsValid())
	{
		if (_format.rgoAction)
		{
			SMemory::Free(_format.rgoAction);
		}
	}

	_format = data;
	if (data.rgoAction)
	{
		const uint64 bytes = data.dwNumActions * data.dwActionSize;
		_format.rgoAction = (DIACTION*)SMemory::Malloc(bytes);
		SMemory::Copy(_format.rgoAction, data.rgoAction, bytes);
	}
}

CInputDevicePatched::CInputDevicePatched(IDirectInputDevice8A* impl, const DIDEVICEINSTANCE& data)
	: Super(impl)
	, _type((EInputDeviceType)LOBYTE(data.dwDevType))
	, _idAsStr(ToString(data.guidInstance))
	, _typeAsStr(ToString(_type))
	, _info(data)
{
}

HRESULT CInputDevicePatched::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	const HRESULT result = CDirectInputDevice8Proxy::GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	if (FAILED(result))
	{
		return result;
	}

	const SInputDeviceMap* activeMap = _actionMaps.Find(_activeActionMapId);
	if (!activeMap)
	{
		LogWarning(TEXT("CInputDevicePatched%s %s: Active map '%s' not found"),
			*_typeAsStr,
			BUILD_DEBUG ? *_idAsStr : "",
			_activeActionMapId.c_str()
		);
		return result;
	}

	for (DWORD i = 0; i < *pdwInOut; ++i)
	{
		// we need to offset by size of the object
		DIDEVICEOBJECTDATA& ev = *(DIDEVICEOBJECTDATA*)((char*)rgdod + cbObjectData * i);

		// It's offset in array of [ DWORD ActionState[NumActions] ]
		const DWORD actionIdx = ev.dwOfs / sizeof(DWORD);

		if (!LIKELY(_actionStates.IsValidIndex(actionIdx)))
		{
			LogWarning(TEXT("CInputDevicePatched%s %s: Invalid action index '%lu' encountered"),
				*_typeAsStr,
				BUILD_DEBUG ? *_idAsStr : "",
				ev.dwOfs
			);
			continue;
		}

		_actionStates[actionIdx] = ev.dwData;

		// FINALLY the patch for sensitivity
		const DIACTION& action = activeMap->GetActions()[actionIdx];
		if (GRuntime.ActionSemantic_CameraControl.contains(action.dwSemantic))
		{
			GRuntime.ControlActionPressed = LOBYTE(ev.dwData) > 0;
			LogDebug(TEXT("Control %s"), GRuntime.ControlActionPressed ? TEXT("pressed") : TEXT("released"));
		}
		else if (GRuntime.ControlActionPressed)
		{
			float* carryDelta = nullptr;
			if (GRuntime.ActionSemantic_CameraX.contains(action.dwSemantic)) carryDelta = &GRuntime.CarryMouseX;
			else if (GRuntime.ActionSemantic_CameraY.contains(action.dwSemantic)) carryDelta = &GRuntime.CarryMouseY;

			if (carryDelta)
			{
				const float scaledDelta = (float)((LONG)ev.dwData) * GRuntime.MouseSensitivity + *carryDelta;
				const LONG outDelta = std::lroundf(scaledDelta);

				*carryDelta = scaledDelta - outDelta; // carry over what was rounded
				ev.dwData = (DWORD)outDelta;
			}
		}
	}

	return result;
}

HRESULT CInputDevicePatched::BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
{
	LogDebug(TEXT("CInputDevicePatched::BuildActionMap"));

	SInputDeviceMap& mapFormat = _actionMaps.FindOrAdd(lpActionFormat->tszActionMap);
	if (mapFormat.IsValid())
	{
		*lpActionFormat = mapFormat;
		return S_OK;
	}

	const HRESULT result = CDirectInputDevice8Proxy::BuildActionMap(lpActionFormat, lpszUserName, dwFlags);
	if (FAILED(result))
	{
		LogWarning(TEXT("CInputDevicePatched%s %s: Map '%s' build failed for '%s'"),
			*_typeAsStr,
			BUILD_DEBUG ? *_idAsStr : "",
			lpActionFormat->tszActionMap,
			lpszUserName
		);
		return result;
	}

	mapFormat = *lpActionFormat;
	LogInfo(TEXT("CInputDevicePatched%s %s: Map '%s' built for '%s'"),
		*_typeAsStr,
		BUILD_DEBUG ? *_idAsStr : "",
		lpActionFormat->tszActionMap,
		lpszUserName
	);

	// Patch for Runtime
	{
		for (uint32 i = 0; i < lpActionFormat->dwNumActions; ++i)
		{
			const DIACTION& action = *(DIACTION*)((char*)lpActionFormat->rgoAction + (lpActionFormat->dwActionSize * i));
			if (SCString::Compare(action.lptszActionName, CAMERA_CONTROL_ACTION_NAME) == 0)
			{
				GRuntime.ActionSemantic_CameraControl.emplace(action.dwSemantic);
			}
			else if (SCString::Compare(action.lptszActionName, CAMERA_X_NAME) == 0)
			{
				GRuntime.ActionSemantic_CameraX.emplace(action.dwSemantic);
			}
			else if (SCString::Compare(action.lptszActionName, CAMERA_Y_NAME) == 0)
			{
				GRuntime.ActionSemantic_CameraY.emplace(action.dwSemantic);
			}
		}
	}

	return result;
}

HRESULT CInputDevicePatched::SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
{
	LogDebug(TEXT("CInputDevicePatched::SetActionMap"));

	if (!(dwFlags & DIDSAM_FORCESAVE) && _activeActionMapId.compare(lpActionFormat->tszActionMap) == 0)
	{
		return S_OK;
	}

	const HRESULT result = CDirectInputDevice8Proxy::SetActionMap(lpActionFormat, lpszUserName, dwFlags);
	if (FAILED(result))
	{
		LogWarning(TEXT("CInputDevicePatched%s %s: Map '%s' application failed for '%s'"), 
			*_typeAsStr,
			BUILD_DEBUG ? *_idAsStr : "",
			lpActionFormat->tszActionMap,
			lpszUserName
		);
		return result;
	}

	_activeActionMapId = lpActionFormat->tszActionMap;
	LogInfo(TEXT("CInputDevicePatched%s %s: Map '%s' applied for '%s'"), 
		*_typeAsStr,
		BUILD_DEBUG ? *_idAsStr : "",
		lpActionFormat->tszActionMap,
		lpszUserName
	);

	// Refresh Action States
	{
		_actionStates.Empty(lpActionFormat->dwNumActions);
		_actionStates.AddDefaulted(lpActionFormat->dwNumActions);
	}

	return result;
}

void CInputDevicePatched::PrintActiveActionMap() const
{
	const auto* foundMap = _actionMaps.Find(_activeActionMapId);
	if (!foundMap) return;

	LogInfo(TEXT("--------------- %s (%s) ---------------"),
		_activeActionMapId.c_str(),
		*_typeAsStr
	);

	DIDEVICEOBJECTINSTANCE object = DIDEVICEOBJECTINSTANCE(); // zero memory
	object.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

	const DIACTIONFORMAT& mf = foundMap->GetFormat();
	for (DWORD i = 0; i < mf.dwNumActions; ++i)
	{
		const DIACTION& action = *(DIACTION*)((char*)mf.rgoAction + (mf.dwActionSize * i));
		if (!InlineIsEqualGUID(GetId(), action.guidInstance)) continue;

		SString keyName = TEXT("<None>");
		if (SUCCEEDED(GetImpl()->GetObjectInfo(&object, action.dwObjID, DIPH_BYID)))
		{
			keyName = object.tszName;
		}

		LogInfo(TEXT("Name: %s, Key: %s"), action.lptszActionName, *keyName);
	}
}

CInputPatched::CInputPatched(IDirectInput8A* impl)
	: Super(impl)
{
	if (!CHECK(SUCCEEDED(impl->EnumDevices(DI8DEVCLASS_ALL, HandleEnumDevices, this, DIEDFL_ATTACHEDONLY))))
	{
		LogWarning(TEXT("CInputPatched: Could not fetch devices !"));
	}
}

HRESULT CInputPatched::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	LogDebug(TEXT("CInputDevicePatched::CreateDevice"));

	TComPtr<CInputDevicePatched> devices = _devices.FindCopy(rguid);
	if (!devices.IsValid())
	{
		LogWarning(TEXT("CInputPatched: Can't find '%s' device"), *ToString(rguid));
		return S_FALSE;
	}

	*lplpDirectInputDevice = devices.Get();
	devices->AddRef(); // since we are exposing, we need to add ref
	return S_OK;
}

HRESULT CInputPatched::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	LogDebug(TEXT("CInputDevicePatched::EnumDevices"));

	for (const TComPtr<CInputDevicePatched>& device : _devices.GetValues())
	{
		const BOOL result = lpCallback(&device->GetInfo(), pvRef);
		if (result == DIENUM_STOP) break;
	}

	return S_OK;
}

HRESULT CInputPatched::EnumDevicesBySemantics(LPCSTR pszUserName, LPDIACTIONFORMAT lpActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	LogDebug(TEXT("CInputDevicePatched::EnumDevicesBySemantics"));

	if (!lpActionFormat || lpActionFormat->dwNumActions == 0) return DIERR_INVALIDPARAM;

	for (int32 dI = 0; dI < _devices.GetNum(); ++dI)
	{
		const TComPtr<CInputDevicePatched>& device = _devices.GetByIndex(dI);
		const uint16 remaining = (_devices.GetNum() - 1) - dI;

		bool success = false;
		for (int32 aI = 0; aI < lpActionFormat->dwNumActions; ++aI)
		{
			SFindDeviceObjectBySematicsParam fParams =
			{
				.Action = (DIACTION*)((char*)lpActionFormat->rgoAction + (lpActionFormat->dwActionSize * aI)),
				.Device = &device->GetInfo()
			};

			if (FAILED(device->EnumObjects(FindDeviceObjectBySemantics, &fParams, dwFlags))) break; // NO NEED TO CHECK DEVICE MORE
			else if (!fParams.OutObject) continue;

			success = true;
			break;
		}

		if (success && lpCallback(&device->GetInfo(), device.Get(), DIEDBS_RECENTDEVICE, remaining, pvRef) == DIENUM_STOP)
		{
			break;
		}
	}

	return S_OK;
}

void CDirectInputExtension::PrintActiveActionMaps(const int32 deviceIdx) const
{
	if (!_input.IsValid()) return;

	const auto& devicesMap = _input->GetDevices();

	LogInfo(TEXT("--------------- BEGIN Print Active Action Map ---------------"));

	if (deviceIdx >= 0)
	{
		if (auto* device = devicesMap.FindByIndex(deviceIdx))
		{
			(*device)->PrintActiveActionMap();
		}
	}
	else
	{
		for (const auto& device : devicesMap.GetValues())
		{
			device->PrintActiveActionMap();
		}
	}

	LogInfo(TEXT("--------------- END Print Active Action Map ---------------"));
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

void CDirectInputExtension::Tick(double deltaTime)
{
	IProgramExtension::Tick(deltaTime);

#if BUILD_DEBUG
	if (GetAsyncKeyState(VK_NUMPAD0)) PrintActiveActionMaps();
	else if (GetAsyncKeyState(VK_NUMPAD1)) PrintActiveActionMaps(0);
	else if (GetAsyncKeyState(VK_NUMPAD2)) PrintActiveActionMaps(1);
	else if (GetAsyncKeyState(VK_NUMPAD3)) PrintActiveActionMaps(2);
	else if (GetAsyncKeyState(VK_NUMPAD4)) PrintActiveActionMaps(3);
	else if (GetAsyncKeyState(VK_NUMPAD5)) PrintActiveActionMaps(4);
	else if (GetAsyncKeyState(VK_NUMPAD6)) PrintActiveActionMaps(5);
	else if (GetAsyncKeyState(VK_NUMPAD7)) PrintActiveActionMaps(6);
	else if (GetAsyncKeyState(VK_NUMPAD8)) PrintActiveActionMaps(7);
	else if (GetAsyncKeyState(VK_NUMPAD9)) PrintActiveActionMaps(8);
#endif
}

void CDirectInputExtension::Shutdown()
{
	CHECK(MH_RemoveHook(_createInputHook) == MH_OK);
	Super::Shutdown();
}
