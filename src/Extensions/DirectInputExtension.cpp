// Copyright Alternity Arts. All Rights Reserved.

#include "DirectInputExtension.h"

#include "MinHook.h"
#include "Program.h"

constexpr const tchar* CAMERA_CONTROL_ACTION = TEXT("DIA_GAME_CAMERACONTROL");
constexpr const tchar* CAMERA_X = TEXT("DIA_GAME_AXIS_X");
constexpr const tchar* CAMERA_Y = TEXT("DIA_GAME_AXIS_Y");

BOOL CALLBACK HandleEnumDeviceObjects(LPCDIDEVICEOBJECTINSTANCE pdidInstance, VOID* pvRef)
{
	if (!pdidInstance) return DIENUM_CONTINUE;

	CInputDevicePatched* owner = (CInputDevicePatched*)pvRef;
	owner->_objects.Add(*pdidInstance);

	return DIENUM_CONTINUE;
}

BOOL CALLBACK HandleEnumDevices(LPCDIDEVICEINSTANCE pdidInstance, void* pvRef)
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

SString ToString(EInputKey type, EInputDeviceType deviceType)
{
	switch (deviceType)
	{
		case EInputDeviceType::Mouse:
			switch (type)
			{
				case EInputKey::MouseX: return TEXT("MouseX");
				case EInputKey::MouseY: return TEXT("MouseY");
				case EInputKey::MouseZ: return TEXT("MouseZ");
				case EInputKey::MouseButtonZero: return TEXT("MouseButtonZero");
				case EInputKey::MouseButtonOne: return TEXT("MouseButtonOne");
				case EInputKey::MouseButtonTwo: return TEXT("MouseButtonTwo");
				case EInputKey::MouseButtonThree: return TEXT("MouseButtonThree");
				case EInputKey::MouseButtonFour: return TEXT("MouseButtonFour");
				case EInputKey::MouseButtonFive: return TEXT("MouseButtonFive");
				case EInputKey::MouseButtonSix: return TEXT("MouseButtonSix");
				case EInputKey::MouseButtonSeven: return TEXT("MouseButtonSeven");
				default: break;
			}
			break;
		case EInputDeviceType::Keyboard:
			switch (type)
			{
				case EInputKey::Q: return TEXT("Q");
				case EInputKey::W: return TEXT("W");
				case EInputKey::E: return TEXT("E");
				case EInputKey::R: return TEXT("R");
				case EInputKey::T: return TEXT("T");
				case EInputKey::Y: return TEXT("Y");
				case EInputKey::U: return TEXT("U");
				case EInputKey::I: return TEXT("I");
				case EInputKey::O: return TEXT("O");
				case EInputKey::P: return TEXT("P");
				case EInputKey::A: return TEXT("A");
				case EInputKey::S: return TEXT("S");
				case EInputKey::D: return TEXT("D");
				case EInputKey::F: return TEXT("F");
				case EInputKey::G: return TEXT("G");
				case EInputKey::H: return TEXT("H");
				case EInputKey::J: return TEXT("J");
				case EInputKey::K: return TEXT("K");
				case EInputKey::L: return TEXT("L");
				case EInputKey::Z: return TEXT("Z");
				case EInputKey::X: return TEXT("X");
				case EInputKey::C: return TEXT("C");
				case EInputKey::V: return TEXT("V");
				case EInputKey::B: return TEXT("B");
				case EInputKey::N: return TEXT("N");
				case EInputKey::M: return TEXT("M");
				case EInputKey::F1: return TEXT("F1");
				case EInputKey::F2: return TEXT("F2");
				case EInputKey::F3: return TEXT("F3");
				case EInputKey::F4: return TEXT("F4");
				case EInputKey::F5: return TEXT("F5");
				case EInputKey::F6: return TEXT("F6");
				case EInputKey::F7: return TEXT("F7");
				case EInputKey::F8: return TEXT("F8");
				case EInputKey::F9: return TEXT("F9");
				case EInputKey::F10: return TEXT("F10");
				case EInputKey::F11: return TEXT("F11");
				case EInputKey::F12: return TEXT("F12");
				case EInputKey::Escape: return TEXT("Escape");
				case EInputKey::One: return TEXT("One");
				case EInputKey::Two: return TEXT("Two");
				case EInputKey::Three: return TEXT("Three");
				case EInputKey::Four: return TEXT("Four");
				case EInputKey::Five: return TEXT("Five");
				case EInputKey::Six: return TEXT("Six");
				case EInputKey::Seven: return TEXT("Seven");
				case EInputKey::Eight: return TEXT("Eight");
				case EInputKey::Nine: return TEXT("Nine");
				case EInputKey::Zero: return TEXT("Zero");
				case EInputKey::Minus: return TEXT("Minus");
				case EInputKey::Equals: return TEXT("Equals");
				case EInputKey::Back: return TEXT("Back");
				case EInputKey::Tab: return TEXT("Tab");
				case EInputKey::Semicolon: return TEXT("Semicolon");
				case EInputKey::Apostrophe: return TEXT("Apostrophe");
				case EInputKey::Grave: return TEXT("Grave");
				case EInputKey::LeftShift: return TEXT("LeftShift");
				case EInputKey::Backslash: return TEXT("Backslash");
				case EInputKey::Comma: return TEXT("Comma");
				case EInputKey::Period: return TEXT("Period");
				case EInputKey::Slash: return TEXT("Slash");
				case EInputKey::RightShift: return TEXT("RightShift");
				case EInputKey::Multiply: return TEXT("Multiply");
				case EInputKey::LeftMenu: return TEXT("LeftMenu");
				case EInputKey::Space: return TEXT("Space");
				case EInputKey::Capital: return TEXT("Capital");
				case EInputKey::LeftBracket: return TEXT("LeftBracket");
				case EInputKey::RightBracket: return TEXT("RightBracket");
				case EInputKey::Return: return TEXT("Return");
				case EInputKey::LeftControl: return TEXT("LeftControl");
				case EInputKey::NumpadSeven: return TEXT("NumpadSeven");
				case EInputKey::NumpadEight: return TEXT("NumpadEight");
				case EInputKey::NumpadNine: return TEXT("NumpadNine");
				case EInputKey::Subtract: return TEXT("Subtract");
				case EInputKey::NumpadFour: return TEXT("NumpadFour");
				case EInputKey::NumpadFive: return TEXT("NumpadFive");
				case EInputKey::NumpadSix: return TEXT("NumpadSix");
				case EInputKey::NumpadAdd: return TEXT("NumpadAdd");
				case EInputKey::NumpadOne: return TEXT("NumpadOne");
				case EInputKey::NumpadTwo: return TEXT("NumpadTwo");
				case EInputKey::NumpadThree: return TEXT("NumpadThree");
				case EInputKey::NumpadZero: return TEXT("NumpadZero");
				case EInputKey::Decimal: return TEXT("Decimal");
				case EInputKey::NumpadEquals: return TEXT("NumpadEquals");
				case EInputKey::NumpadEnter: return TEXT("NumpadEnter");
				case EInputKey::RightControl: return TEXT("RightControl");
				case EInputKey::NumpadComma: return TEXT("NumpadComma");
				case EInputKey::NumpadDivide: return TEXT("NumpadDivide");
				case EInputKey::RightAlt: return TEXT("RightAlt");
				case EInputKey::Pause: return TEXT("Pause");
				case EInputKey::Home: return TEXT("Home");
				case EInputKey::Up: return TEXT("Up");
				case EInputKey::PageUp: return TEXT("PageUp");
				case EInputKey::Left: return TEXT("Left");
				case EInputKey::Right: return TEXT("Right");
				case EInputKey::End: return TEXT("End");
				case EInputKey::Down: return TEXT("Down");
				case EInputKey::Next: return TEXT("Next");
				case EInputKey::Insert: return TEXT("Insert");
				case EInputKey::Delete: return TEXT("Delete");
				default: break;
			}
			break;
		default: break;
	}

	return TEXT("Unknown");
}

SString ToString(const GUID& guid)
{
	return SString::Printf(TEXT("{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
	);
}

void SObjectsMap::Add(const DIDEVICEOBJECTINSTANCE& object)
{
	if (!CHECK(!_dwOfsLookup.contains(object.dwOfs)) ||
		!CHECK(!_idLookup.contains(object.dwType)))
	{
		return;
	}

	const int32 newIdx = _objects.Add(object);

	_dwOfsLookup[object.dwOfs] = newIdx;
	_idLookup[object.dwType] = newIdx;
}

void SInputDeviceMap::Reset(const DIACTIONFORMAT& data)
{
	if (IsValid())
	{
		if (_data.rgoAction)
		{
			SMemory::Free(_data.rgoAction);
		}
	}

	_data = data;
	if (data.rgoAction)
	{
		const uint64 bytes = data.dwNumActions * data.dwActionSize;
		_data.rgoAction = (DIACTION*)SMemory::Malloc(bytes);
		SMemory::Copy(_data.rgoAction, data.rgoAction, bytes);
	}
}

CInputDevicePatched::CInputDevicePatched(IDirectInputDevice8A* impl, const DIDEVICEINSTANCE& data)
	: Super(impl)
	, _type((EInputDeviceType)LOBYTE(data.dwDevType))
	, _idAsStr(ToString(data.guidInstance))
	, _typeAsStr(ToString(_type))
	, _data(data)
{
	const HRESULT result = impl->EnumObjects(HandleEnumDeviceObjects, this, DIDFT_ALL);
	if (!CHECK(SUCCEEDED(result)))
	{
		LogWarning(TEXT("CInputDevicePatched: Could not evaluate objects !"));
	}
}

HRESULT CInputDevicePatched::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	const HRESULT result = CDirectInputDevice8Proxy::GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	if (FAILED(result))
	{
		return result;
	}

	for (DWORD i = 0; i < *pdwInOut; ++i)
	{
		DIDEVICEOBJECTDATA& ev = rgdod[i];

		const DIDEVICEOBJECTINSTANCE* object = nullptr;
		switch (_type)
		{
			case EInputDeviceType::Mouse:
				object = _objects.FindViaOfs(ev.dwOfs);
				break;
			case EInputDeviceType::Keyboard:
				// TODO: IMPLEMENT
				break;
			default: break;
		}

		if (!object)
		{
			LogWarning(TEXT("CInputDevicePatched%s: Key '%d' not found"), *_typeAsStr, ev.dwOfs);
		}
	}

	return result;
}

HRESULT CInputDevicePatched::BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
{
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

	return result;
}

HRESULT CInputDevicePatched::SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
{
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

	return result;
}

void CInputDevicePatched::PrintActiveActionMap() const
{
	if (_activeActionMapId.empty())
	{
		LogInfo(TEXT("CInputDevicePatched%s: No active action map"),
			*_typeAsStr,
			_activeActionMapId.c_str()
		);

		return;
	}

	const auto* foundMap = _actionMaps.Find(_activeActionMapId);
	if (!foundMap)
	{
		LogWarning(TEXT("CInputDevicePatched%s: Action Map '%s' not found:"),
			*_typeAsStr,
			_activeActionMapId.c_str()
		);

		return;
	}

	LogInfo(TEXT("--------------- %s (%s) ---------------"),
		_activeActionMapId.c_str(),
		*_typeAsStr
	);

	SString actionsAsStr = SString::GetEmpty();

	const DIACTIONFORMAT& mf = foundMap->GetData();
	for (DWORD i = 0; i < mf.dwNumActions; ++i)
	{
		const auto& action = mf.rgoAction[i];
		if (!InlineIsEqualGUID(GetId(), action.guidInstance)) continue;

		SString keyName = TEXT("<None>");

		if (const DIDEVICEOBJECTINSTANCE* object = _objects.FindViaId(action.dwObjID))
		{
			keyName = object->tszName;
		}

		LogInfo(TEXT("Name: %s, Key: %s"), action.lptszActionName, *keyName);
	}
}

CInputPatched::CInputPatched(IDirectInput8A* impl)
	: Super(impl)
{
	const HRESULT result = impl->EnumDevices(DI8DEVCLASS_ALL, HandleEnumDevices, this, DIEDFL_ATTACHEDONLY);
	if (!CHECK(SUCCEEDED(result)))
	{
		LogWarning(TEXT("CInputPatched: Could not evaluate devices !"));
	}
}

HRESULT CInputPatched::CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
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
	for (const TComPtr<CInputDevicePatched>& device : _devices.GetValues())
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
		const auto& device = _devices.GetByIndex(i);
		const uint16 remaining = (_devices.GetNum() - 1) - i;

		const BOOL result = lpCallback(&device->GetData(), device.Get(), DIEDBS_RECENTDEVICE, remaining, pvRef);
		if (result == DIENUM_STOP) break;
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
