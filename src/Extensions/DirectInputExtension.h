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
	FORCEINLINE SInputDeviceMap(const DIACTIONFORMAT& other) { Reset(other); }
	FORCEINLINE ~SInputDeviceMap() { Reset(); }

	FORCEINLINE bool IsValid() const { return _format.dwSize > 0 && _format.dwActionSize > 0; }
	FORCEINLINE int32 GetNumOfActions() const { return _format.dwNumActions; }
	FORCEINLINE const DIACTION* GetActions() const { return _format.rgoAction; }

	FORCEINLINE SInputDeviceMap(const SInputDeviceMap& other) { Reset(other._format); }
	FORCEINLINE SInputDeviceMap& operator=(const SInputDeviceMap& other) { Reset(other._format); return *this; }

	FORCEINLINE const DIACTIONFORMAT* operator->() const { return &_format; }
	FORCEINLINE const DIACTIONFORMAT& operator*() const { return _format; }

	FORCEINLINE const DIACTIONFORMAT& GetFormat() const { return _format; }
	FORCEINLINE operator const DIACTIONFORMAT&() const { return _format; }

	void Reset(const DIACTIONFORMAT& data = {});

private:
	DIACTIONFORMAT _format = DIACTIONFORMAT();
};

class CInputDevicePatched final : public CDirectInputDevice8Proxy
{
public:
	typedef CDirectInputDevice8Proxy Super;

	CInputDevicePatched(IDirectInputDevice8* impl, const DIDEVICEINSTANCE& data);

	FORCEINLINE const std::string& GetActiveActionMap() const { return _activeActionMapId; }
	FORCEINLINE const TFastMap<std::string, SInputDeviceMap>& GetActionMaps() const { return _actionMaps; }

	FORCEINLINE const GUID& GetId() const { return _info.guidInstance; }
	FORCEINLINE const SString& GetIdString() const { return _idAsStr; }
	FORCEINLINE const DIDEVICEINSTANCE& GetInfo() const { return _info; }

	STDOVERRIDEMETHODIMP GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
	STDOVERRIDEMETHODIMP BuildActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);
	STDOVERRIDEMETHODIMP SetActionMap(LPDIACTIONFORMAT lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags);

	void PrintActiveActionMap() const;

private:
	EInputDeviceType _type = EInputDeviceType::None;

	SString _idAsStr;
	SString _typeAsStr;

	DIDEVICEINSTANCE _info;
	std::string _activeActionMapId;

	TFastMap<std::string, SInputDeviceMap> _actionMaps;
	TArray<uint32> _actionStates; // pooled action states
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