// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ProgramExtension.h"
#include "DirectInput.h"

enum class EInputDeviceType : uint8
{
	None = 0,

	Device = DI8DEVTYPE_DEVICE,
	Mouse = DI8DEVTYPE_MOUSE,
	Keyboard = DI8DEVTYPE_KEYBOARD,
	Joystick = DI8DEVTYPE_JOYSTICK,
	Gamepad = DI8DEVTYPE_GAMEPAD,
	Driving = DI8DEVTYPE_DRIVING,
	Flight = DI8DEVTYPE_FLIGHT,
	FirstPerson = DI8DEVTYPE_1STPERSON,
	DeviceCtrl = DI8DEVTYPE_DEVICECTRL,
	ScreenPointer = DI8DEVTYPE_SCREENPOINTER,
	Remote = DI8DEVTYPE_REMOTE,
	Supplemental = DI8DEVTYPE_SUPPLEMENTAL,
};

enum class EInputKey : uint8
{
	None = 0,

	MouseX = DIMOFS_X,
	MouseY = DIMOFS_Y,
	MouseZ = DIMOFS_Z,

	MouseButtonZero = DIMOFS_BUTTON0,
	MouseButtonOne = DIMOFS_BUTTON1,
	MouseButtonTwo = DIMOFS_BUTTON2,
	MouseButtonThree = DIMOFS_BUTTON3,
	MouseButtonFour = DIMOFS_BUTTON4,
	MouseButtonFive = DIMOFS_BUTTON5,
	MouseButtonSix = DIMOFS_BUTTON6,
	MouseButtonSeven = DIMOFS_BUTTON7,

	Q = DIK_Q,
	W = DIK_W,
	E = DIK_E,
	R = DIK_R,
	T = DIK_T,
	Y = DIK_Y,
	U = DIK_U,
	I = DIK_I,
	O = DIK_O,
	P = DIK_P,
	A = DIK_A,
	S = DIK_S,
	D = DIK_D,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,

	Z = DIK_Z,
	X = DIK_X,
	C = DIK_C,
	V = DIK_V,
	B = DIK_B,
	N = DIK_N,
	M = DIK_M,

	F1 = DIK_F1,
	F2 = DIK_F2,
	F3 = DIK_F3,
	F4 = DIK_F4,
	F5 = DIK_F5,
	F6 = DIK_F6,
	F7 = DIK_F7,
	F8 = DIK_F8,
	F9 = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,

	Escape = DIK_ESCAPE,
	One = DIK_1,
	Two = DIK_2,
	Three = DIK_3,
	Four = DIK_4,
	Five = DIK_5,
	Six = DIK_6,
	Seven = DIK_7,
	Eight = DIK_8,
	Nine = DIK_9,
	Zero = DIK_0,
	Minus = DIK_MINUS,
	Equals = DIK_EQUALS,
	Back = DIK_BACK,
	Tab = DIK_TAB,
	Semicolon = DIK_SEMICOLON,
	Apostrophe = DIK_APOSTROPHE,
	Grave = DIK_GRAVE,
	LeftShift = DIK_LSHIFT,
	Backslash = DIK_BACKSLASH,

	Comma = DIK_COMMA,
	Period = DIK_PERIOD,
	Slash = DIK_SLASH,
	RightShift = DIK_RSHIFT,
	Multiply = DIK_MULTIPLY,
	LeftMenu = DIK_LMENU,
	Space = DIK_SPACE,
	Capital = DIK_CAPITAL,

	LeftBracket = DIK_LBRACKET,
	RightBracket = DIK_RBRACKET,
	Return = DIK_RETURN,
	LeftControl = DIK_LCONTROL,

	NumpadSeven = DIK_NUMPAD7,
	NumpadEight = DIK_NUMPAD8,
	NumpadNine = DIK_NUMPAD9,
	Subtract = DIK_SUBTRACT,
	NumpadFour = DIK_NUMPAD4,
	NumpadFive = DIK_NUMPAD5,
	NumpadSix = DIK_NUMPAD6,
	NumpadAdd = DIK_ADD,
	NumpadOne = DIK_NUMPAD1,
	NumpadTwo = DIK_NUMPAD2,
	NumpadThree = DIK_NUMPAD3,
	NumpadZero = DIK_NUMPAD0,
	Decimal = DIK_DECIMAL,
	NumpadEquals = DIK_NUMPADEQUALS,
	NumpadEnter = DIK_NUMPADENTER,
	RightControl = DIK_RCONTROL,
	NumpadComma = DIK_NUMPADCOMMA,
	NumpadDivide = DIK_DIVIDE,
	RightAlt = DIK_RALT,
	Pause = DIK_PAUSE,
	Home = DIK_HOME,
	Up = DIK_UP,
	PageUp = DIK_PRIOR,
	Left = DIK_LEFT,
	Right = DIK_RIGHT,
	End = DIK_END,
	Down = DIK_DOWN,
	Next = DIK_NEXT,
	Insert = DIK_INSERT,
	Delete = DIK_DELETE
};

SString ToString(EInputDeviceType type);
SString ToString(EInputKey type, EInputDeviceType deviceType);
SString ToString(const GUID& guid);

template<>
struct std::hash<GUID>
{
	_NODISCARD size_t operator()(const GUID& g) const noexcept
	{
		size_t h = g.Data1 ^ g.Data2 ^ g.Data3;
		h ^= (g.Data4[0] | (g.Data4[1]<<8) | (g.Data4[2]<<16) | (g.Data4[3]<<24));
		return h;
	}
};

template<typename KeyT, typename ValueT>
struct TFastMap
{
	FORCEINLINE const TArray<ValueT>& GetValues() const { return _elements; }
	FORCEINLINE int32 GetNum() const { return _elements.GetNum(); }

	FORCEINLINE bool Contains(const KeyT& id) const { return _lookup.contains(id); }
	FORCEINLINE const ValueT* Find(const KeyT& id) const
	{
		const auto& it  = _lookup.find(id);
		return it != _lookup.end() ? &_elements[it->second] : nullptr;
	}

	FORCEINLINE const ValueT* FindByIndex(const int32 idx) const
	{
		return _elements.IsValidIndex(idx) ? &_elements[idx] : nullptr;
	}

	FORCEINLINE ValueT& GetByIndex(const int32 idx) { return _elements[idx]; }
	FORCEINLINE const ValueT& GetByIndex(const int32 idx) const { return _elements[idx]; }

	FORCEINLINE ValueT FindCopy(const KeyT& id) const
	{
		if (const ValueT* result = Find(id))
		{
			return *result;
		}
		return ValueT();
	}

	ValueT& FindOrAdd(const KeyT& id)
	{
		if (const auto& it = _lookup.find(id);
			it != _lookup.end())
		{
			return _elements[it->second];
		}

		_lookup[id] = _elements.GetNum();
		return _elements.AddDefaulted_GetRef();
	}

	FORCEINLINE void Reset()
	{
		_elements.Empty();
		_lookup.clear();
	}

private:
	TArray<ValueT> _elements;
	std::unordered_map<KeyT, uint16> _lookup;
};

struct SInputDeviceMap
{
	SInputDeviceMap() = default;
	FORCEINLINE SInputDeviceMap(const DIACTIONFORMAT& other) { Reset(other); }
	FORCEINLINE ~SInputDeviceMap() { Reset(); }

	FORCEINLINE bool IsValid() const { return _data.dwSize > 0 && _data.dwActionSize > 0; }

	FORCEINLINE SInputDeviceMap(const SInputDeviceMap& other) { Reset(other._data); }
	FORCEINLINE SInputDeviceMap& operator=(const SInputDeviceMap& other) { Reset(other._data); return *this; }

	FORCEINLINE const DIACTIONFORMAT* operator->() const { return &_data; }
	FORCEINLINE const DIACTIONFORMAT& operator*() const { return _data; }

	FORCEINLINE const DIACTIONFORMAT& GetData() const { return _data; }
	FORCEINLINE operator const DIACTIONFORMAT&() const { return _data; }

	void Reset(const DIACTIONFORMAT& data = {});

private:
	DIACTIONFORMAT _data = DIACTIONFORMAT();
};

class CInputDevicePatched final : public CDirectInputDevice8Proxy
{
public:
	typedef CDirectInputDevice8Proxy Super;

	CInputDevicePatched(IDirectInputDevice8* impl, const DIDEVICEINSTANCE& data);

	FORCEINLINE const std::string& GetActiveActionMap() const { return _activeActionMapId; }
	FORCEINLINE const TFastMap<std::string, SInputDeviceMap>& GetActionMaps() const { return _actionMaps; }

	FORCEINLINE const GUID& GetId() const { return _data.guidInstance; }
	FORCEINLINE const SString& GetIdString() const { return _idAsStr; }
	FORCEINLINE const DIDEVICEINSTANCE& GetData() const { return _data; }

	STDOVERRIDEMETHODIMP GetDeviceState(DWORD cbData, LPVOID lpvData);
	STDOVERRIDEMETHODIMP GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
	STDOVERRIDEMETHODIMP BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);
	STDOVERRIDEMETHODIMP SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);

	void PrintActiveActionMap() const;

private:
	friend BOOL CALLBACK HandleEnumDeviceObjects(LPCDIDEVICEOBJECTINSTANCE, VOID*);

	EInputDeviceType _type = EInputDeviceType::None;

	SString _idAsStr;
	SString _typeAsStr;

	DIDEVICEINSTANCE _data;
	std::string _activeActionMapId;

	TFastMap<std::string, SInputDeviceMap> _actionMaps;
	std::unordered_map<DWORD, DIDEVICEOBJECTINSTANCE> _objects;
};

class CInputPatched final : public CDirectInput8Proxy
{
public:
	typedef CDirectInput8Proxy Super;

	CInputPatched(IDirectInput8* impl);

	FORCEINLINE const TFastMap<GUID, TComPtr<CInputDevicePatched>>& GetDevices() const { return _devices; }

	STDOVERRIDEMETHODIMP CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
	STDOVERRIDEMETHODIMP EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDOVERRIDEMETHODIMP EnumDevicesBySemantics(LPCSTR pszUserName, LPDIACTIONFORMAT lpActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags);

private:
	friend BOOL CALLBACK HandleEnumDevices(LPCDIDEVICEINSTANCE, VOID*);

	TFastMap<GUID, TComPtr<CInputDevicePatched>> _devices;
};

class CDirectInputExtension final : public IProgramExtension 
{
public:
	typedef IProgramExtension Super;

	FORCEINLINE const TComPtr<CInputPatched>& GetInput() const { return _input; }

	void PrintActiveActionMaps(const int32 deviceIdx = INDEX_NONE) const;

	// ~BEGIN IProgramExtension interface
	virtual void Initialize() override;
	virtual void Tick(double deltaTime) override;
	virtual void Shutdown() override;
	// ~END IProgramExtension interface

private:
	HMODULE _inputModuleHandle = nullptr;
	LPVOID _createInputHook = nullptr;
	TComPtr<CInputPatched> _input;
};

DECLARE_EXTENSION(CDirectInputExtension)