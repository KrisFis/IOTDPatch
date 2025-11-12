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

SString ToString(EInputDeviceType type);
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
	FORCEINLINE SInputDeviceMap(const DIACTIONFORMAT& other) { InitializeAs(other); }

	FORCEINLINE bool IsValid() const { return _data.dwSize > 0 && !_data.rgoAction; }

	FORCEINLINE SInputDeviceMap(const SInputDeviceMap& other) { InitializeAs(other._data); }
	FORCEINLINE SInputDeviceMap& operator=(const SInputDeviceMap& other) { InitializeAs(other._data); return *this; }

	FORCEINLINE ~SInputDeviceMap() { Reset(); }

	FORCEINLINE const DIACTIONFORMAT& GetData() const { return _data; }
	FORCEINLINE operator const DIACTIONFORMAT&() const { return _data; }

	void Reset();

private:
	void InitializeAs(const DIACTIONFORMAT& data);

	DIACTIONFORMAT _data = DIACTIONFORMAT();
};

class CInputDevicePatched final : public CDirectInputDevice8Proxy
{
public:
	typedef CDirectInputDevice8Proxy Super;

	CInputDevicePatched(IDirectInputDevice8* impl, const DIDEVICEINSTANCE& data);

	FORCEINLINE const GUID& GetId() const { return _data.guidInstance; }
	FORCEINLINE const SString& GetIdString() const { return _idAsStr; }
	FORCEINLINE const DIDEVICEINSTANCE& GetData() const { return _data; }

	STDOVERRIDEMETHODIMP GetDeviceState(DWORD cbData, LPVOID lpvData);
	STDOVERRIDEMETHODIMP GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
	STDOVERRIDEMETHODIMP BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);
	STDOVERRIDEMETHODIMP SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);

private:
	EInputDeviceType _type = EInputDeviceType::None;

	SString _idAsStr;
	SString _typeAsStr;

	DIDEVICEINSTANCE _data;
	std::string _activeMapId;

	TFastMap<std::string, SInputDeviceMap> _maps;
};

class CInputPatched final : public CDirectInput8Proxy
{
public:
	typedef CDirectInput8Proxy Super;

	CInputPatched(IDirectInput8* impl);

	STDOVERRIDEMETHODIMP CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
	STDOVERRIDEMETHODIMP EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDOVERRIDEMETHODIMP EnumDevicesBySemantics(LPCSTR pszUserName, LPDIACTIONFORMAT lpActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags);

private:
	friend BOOL CALLBACK HandleEnumDevices(const DIDEVICEINSTANCE* pdidInstance, VOID* pvRef);

	TFastMap<GUID, TComPtr<CInputDevicePatched>> _devices;
};

class CDirectInputExtension final : public IProgramExtension 
{
public:
	typedef IProgramExtension Super;

	FORCEINLINE const TComPtr<CInputPatched>& GetCOM() const { return _input; }

	// ~BEGIN IProgramExtension interface
	virtual void Initialize() override;
	virtual void Tick(double deltaTime) override {}
	virtual void Shutdown() override;
	// ~END IProgramExtension interface

private:
	HMODULE _inputModuleHandle = nullptr;
	LPVOID _createInputHook = nullptr;
	TComPtr<CInputPatched> _input;
};

DECLARE_EXTENSION(CDirectInputExtension)