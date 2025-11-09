// Copyright Alternity Arts. All Rights Reserved

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define GENERATE_UNKNOWN_PROXY_BODY(Impl)\
	__forceinline STDOVERRIDEMETHODIMP QueryInterface(REFIID riid, LPVOID* ppvObj) { return Impl->QueryInterface(riid, ppvObj); } \
	__forceinline STDOVERRIDEMETHODIMP_(ULONG) AddRef() { return Impl->AddRef(); } \
	__forceinline STDOVERRIDEMETHODIMP_(ULONG) Release() { return Impl->Release(); }

// Provides methods to interact with a specific input device (keyboard, mouse, joystick, etc.),
// manage device state, acquire/release input, and handle force feedback.
class CDirectInputDevice8Proxy : public IDirectInputDevice8
{
public:
	__forceinline CDirectInputDevice8Proxy(IDirectInputDevice8* impl) : Impl(impl) {}
	virtual ~CDirectInputDevice8Proxy() = default;

	GENERATE_UNKNOWN_PROXY_BODY(Impl)

	// Gets pointer to real implementation of IDirectInputDevice8
	__forceinline IDirectInputDevice8* GetImpl() const { return Impl; }

	// @brief Retrieves device capabilities.
	// 
	// @param lpDIDevCaps Receives a DIDEVCAPS structure describing device capabilities.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
	{
		return Impl->GetCapabilities(lpDIDevCaps);
	}

	// @brief Enumerates device objects (axes, buttons, POVs).
	// 
	// @param lpCallback Callback function invoked for each object.
	// @param pvRef User-defined pointer passed to the callback.
	// @param dwFlags Enumeration flags (e.g., DIDFT_ALL).
	// @return S_OK on success, or DIERR_INVALIDPARAM on error.
	__forceinline STDOVERRIDEMETHODIMP EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return Impl->EnumObjects(lpCallback, pvRef, dwFlags);
	}

	// @brief Retrieves a property from the device.
	// 
	// @param rguidProp GUID identifying the property to retrieve.
	// @param pdiph Pointer to a DIPROPHEADER or related structure to receive property value.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
	{
		return Impl->GetProperty(rguidProp, pdiph);
	}

	// @brief Sets a property on the device.
	// 
	// @param rguidProp GUID identifying the property to set.
	// @param pdiph Pointer to a DIPROPHEADER or related structure describing property value.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
	{
		return Impl->SetProperty(rguidProp, pdiph);
	}

	// @brief Acquires the device for input.
	// 
	// @return S_OK on success, DIERR_OTHERAPPHASPRIO if another application has priority, 
	// DIERR_INPUTLOST if input was lost.
	__forceinline STDOVERRIDEMETHODIMP Acquire()
	{
		return Impl->Acquire();
	}

	// @brief Unacquires the device.
	// 
	// @return S_OK on success, or DIERR_NOTACQUIRED if device was not acquired.
	__forceinline STDOVERRIDEMETHODIMP Unacquire()
	{
		return Impl->Unacquire();
	}

	// @brief Retrieves the current state of the device.
	// 
	// @param cbData Size of the data buffer (depends on device type).
	// @param lpvData Pointer to buffer that receives current device state.
	// @return S_OK on success, DIERR_INPUTLOST if device lost, DIERR_NOTACQUIRED if not acquired.
	__forceinline STDOVERRIDEMETHODIMP GetDeviceState(DWORD cbData, LPVOID lpvData)
	{
		return Impl->GetDeviceState(cbData, lpvData);
	}

	// @brief Retrieves buffered device data.
	// 
	// @param cbObjectData Size of a single object data element.
	// @param rgdod Pointer to an array of DIDEVICEOBJECTDATA structures that receives data.
	// @param pdwInOut On input, number of elements in rgdod; on output, number filled.
	// @param dwFlags Flags controlling retrieval (e.g., DIDFT_ALL).
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
	{
		return Impl->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	}

	// @brief Sets the data format for the device.
	// 
	// @param lpdf Pointer to a DIDATAFORMAT structure describing the format.
	// @return S_OK on success, or DIERR_INVALIDPARAM if unsupported.
	__forceinline STDOVERRIDEMETHODIMP SetDataFormat(LPCDIDATAFORMAT lpdf)
	{
		return Impl->SetDataFormat(lpdf);
	}

	// @brief Sets an event notification handle for the device.
	// 
	// @param hEvent Handle to a Windows event object that will be signaled on input.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP SetEventNotification(HANDLE hEvent)
	{
		return Impl->SetEventNotification(hEvent);
	}

	// @brief Sets the cooperative level for the device.
	// 
	// @param hwnd Handle to the application window that owns the device.
	// @param dwFlags Flags controlling cooperative level (foreground/background, exclusive/nonexclusive).
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
	{
		return Impl->SetCooperativeLevel(hwnd, dwFlags);
	}

	// @brief Retrieves information about a specific device object (axis, button, etc.).
	// 
	// @param pdidoi Receives object instance information.
	// @param dwObj Index or identifier of the object.
	// @param dwHow How to interpret dwObj (by offset, by ID, etc.).
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow)
	{
		return Impl->GetObjectInfo(pdidoi, dwObj, dwHow);
	}

	// @brief Retrieves information about the device itself.
	// 
	// @param pdidi Receives device instance information.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP GetDeviceInfo(LPDIDEVICEINSTANCEA pdidi)
	{
		return Impl->GetDeviceInfo(pdidi);
	}

	// @brief Opens the Windows Control Panel for device configuration.
	// 
	// @param hwndOwner Handle to the owner window.
	// @param dwFlags Reserved, must be 0.
	// @return S_OK on success, or DIERR_GENERIC on failure.
	__forceinline STDOVERRIDEMETHODIMP RunControlPanel(HWND hwndOwner, DWORD dwFlags)
	{
		return Impl->RunControlPanel(hwndOwner, dwFlags);
	}

	// @brief Initializes the device.
	// 
	// @param hinst Application instance handle.
	// @param dwVersion DirectInput version (e.g., DIRECTINPUT_VERSION).
	// @param rguid GUID of the device to initialize.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
	{
		return Impl->Initialize(hinst, dwVersion, rguid);
	}

	// @brief Creates a force feedback effect on the device.
	// 
	// @param rguid GUID of the effect type.
	// @param lpeff Pointer to a DIEFFECT structure describing the effect.
	// @param ppdeff Receives the created IDirectInputEffect interface.
	// @param pUnkOuter Optional COM aggregation pointer (usually nullptr).
	// @return S_OK on success, DIERR_INVALIDPARAM if unsupported.
	__forceinline STDOVERRIDEMETHODIMP CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN pUnkOuter)
	{
		return Impl->CreateEffect(rguid, lpeff, ppdeff, pUnkOuter);
	}

	// @brief Enumerates force feedback effects supported by the device.
	// 
	// @param lpCallback Callback invoked for each effect.
	// @param pvRef User-defined pointer passed to the callback.
	// @param dwFlags Enumeration flags.
	// @return S_OK on success, DIERR_UNSUPPORTED if device does not support force feedback.
	__forceinline STDOVERRIDEMETHODIMP EnumEffects(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return Impl->EnumEffects(lpCallback, pvRef, dwFlags);
	}

	// @brief Retrieves information about a specific force feedback effect.
	// 
	// @param peffinfo Receives effect information.
	// @param rguid GUID of the effect to query.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP GetEffectInfo(LPDIEFFECTINFOA peffinfo, REFGUID rguid)
	{
		return Impl->GetEffectInfo(peffinfo, rguid);
	}

	// @brief Retrieves the current force feedback state.
	// 
	// @param pdwOut Receives current force feedback state flags.
	// @return S_OK on success, or DIERR_UNSUPPORTED if device has no force feedback.
	__forceinline STDOVERRIDEMETHODIMP GetForceFeedbackState(LPDWORD pdwOut)
	{
		return Impl->GetForceFeedbackState(pdwOut);
	}

	// @brief Sends a force feedback command to the device.
	// 
	// @param dwFlags Command flags (e.g., DISFFC_RESET, DISFFC_STOPALL).
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP SendForceFeedbackCommand(DWORD dwFlags)
	{
		return Impl->SendForceFeedbackCommand(dwFlags);
	}

	// @brief Enumerates created effect objects.
	// 
	// @param lpCallback Callback invoked for each effect object.
	// @param pvRef User-defined pointer passed to the callback.
	// @param dwFlags Flags controlling enumeration.
	// @return S_OK on success, or DIERR_UNSUPPORTED if no effects exist.
	__forceinline STDOVERRIDEMETHODIMP EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return Impl->EnumCreatedEffectObjects(lpCallback, pvRef, dwFlags);
	}

	// @brief Sends a device-specific escape command.
	// 
	// @param peffesc Pointer to a DIEFFESCAPE structure describing the command.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP Escape(LPDIEFFESCAPE peffesc)
	{
		return Impl->Escape(peffesc);
	}

	// @brief Polls the device for state updates.
	// 
	// @return S_OK on success, or DIERR_NOTACQUIRED / DIERR_INPUTLOST if polling fails.
	__forceinline STDOVERRIDEMETHODIMP Poll()
	{
		return Impl->Poll();
	}

	// @brief Sends buffered device data.
	// 
	// @param cbObjectData Size of each object data element.
	// @param rgdod Pointer to array of DIDEVICEOBJECTDATA to send.
	// @param pdwInOut On input, number of elements; on output, number sent.
	// @param dwFlags Flags controlling sending behavior.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
	{
		return Impl->SendDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
	}

	// @brief Enumerates effects stored in a file.
	// 
	// @param pszFileName Path to effect file.
	// @param lpCallback Callback invoked for each effect in the file.
	// @param pvRef User-defined pointer passed to the callback.
	// @param dwFlags Flags controlling enumeration.
	// @return S_OK on success, or DIERR_UNSUPPORTED if device has no force feedback.
	__forceinline STDOVERRIDEMETHODIMP EnumEffectsInFile(LPCSTR pszFileName, LPDIENUMEFFECTSINFILECALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return Impl->EnumEffectsInFile(pszFileName, lpCallback, pvRef, dwFlags);
	}

	// @brief Writes an effect to a file.
	// 
	// @param pszFileName Path to file to write effect.
	// @param dwEntries Number of entries in effect file.
	// @param pDef Pointer to DIFILEEFFECT structure describing effect.
	// @param dwFlags Flags controlling writing.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP WriteEffectToFile(LPCSTR pszFileName, DWORD dwEntries, LPDIFILEEFFECT pDef, DWORD dwFlags)
	{
		return Impl->WriteEffectToFile(pszFileName, dwEntries, pDef, dwFlags);
	}

	// @brief Builds an action map from the device.
	// 
	// @param lpActionFormat Pointer to action format structure.
	// @param lpszUserName Name of the user for mapping.
	// @param dwFlags Flags controlling mapping.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP BuildActionMap(LPDIACTIONFORMATA lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
	{
		return Impl->BuildActionMap(lpActionFormat, lpszUserName, dwFlags);
	}

	// @brief Sets an action map on the device.
	// 
	// @param lpActionFormat Pointer to action format structure.
	// @param lpszUserName Name of the user for mapping.
	// @param dwFlags Flags controlling mapping.
	// @return S_OK on success, or DIERR_INVALIDPARAM on failure.
	__forceinline STDOVERRIDEMETHODIMP SetActionMap(LPDIACTIONFORMATA lpActionFormat, LPCSTR lpszUserName, DWORD dwFlags)
	{
		return Impl->SetActionMap(lpActionFormat, lpszUserName, dwFlags);
	}

	// @brief Retrieves an image of the device (for visualization purposes).
	// 
	// @param pdiiHeader Pointer to structure that receives device image information.
	// @return S_OK on success, or DIERR_UNSUPPORTED if device has no image info.
	__forceinline STDOVERRIDEMETHODIMP GetImageInfo(LPDIDEVICEIMAGEINFOHEADERA pdiiHeader)
	{
		return Impl->GetImageInfo(pdiiHeader);
	}

private:
	IDirectInputDevice8* Impl = nullptr;
};

// Provides methods for creating and enumerating input devices, 
// managing device configurations, and querying device status.
class CDirectInput8Proxy : public IDirectInput8
{
public:
	__forceinline CDirectInput8Proxy(IDirectInput8* impl) : Impl(impl) {}
	virtual ~CDirectInput8Proxy() = default;

	GENERATE_UNKNOWN_PROXY_BODY(Impl)

	// Gets pointer to real implementation of IDirectInputDevice8
	__forceinline IDirectInput8* GetImpl() const { return Impl; }

	// @brief Creates a DirectInput device instance.
	// 
	// @param rguid GUID of the device (e.g., GUID_SysKeyboard, GUID_SysMouse).
	// @param lplpDirectInputDevice Receives the created device interface.
	// @param pUnkOuter Optional pointer for COM aggregation (usually nullptr).
	// @return S_OK on success, or a DirectInput error code.
	__forceinline STDOVERRIDEMETHODIMP CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
	{
		return Impl->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
	}

	// @brief Enumerates available DirectInput devices.
	// 
	// @param dwDevType Device type filter (e.g., DI8DEVCLASS_ALL).
	// @param lpCallback Callback function for each enumerated device.
	// @param pvRef User data passed to the callback.
	// @param dwFlags Enumeration flags.
	// @return S_OK on success, or a DirectInput error code.
	__forceinline STDOVERRIDEMETHODIMP EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return Impl->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
	}

	// @brief Checks whether a device is currently attached and available.
	// 
	// @param rguidInstance GUID of the device instance.
	// @return S_OK if device is connected, DIERR_INPUTLOST if not.
	__forceinline STDOVERRIDEMETHODIMP GetDeviceStatus(REFGUID rguidInstance)
	{
		return Impl->GetDeviceStatus(rguidInstance);
	}

	// @brief Opens the Windows Control Panel for DirectInput configuration.
	// 
	// @param hwndOwner Handle to the owner window.
	// @param dwFlags Reserved. Must be 0.
	// @return S_OK on success, or DIERR_GENERIC on failure.
	__forceinline STDOVERRIDEMETHODIMP RunControlPanel(HWND hwndOwner, DWORD dwFlags)
	{
		return Impl->RunControlPanel(hwndOwner, dwFlags);
	}

	// @brief Initializes the DirectInput object.
	// 
	// @param hinst Application instance handle.
	// @param dwVersion DirectInput version (e.g., DIRECTINPUT_VERSION).
	// @return S_OK on success, or DIERR_INVALIDPARAM on invalid parameters.
	__forceinline STDOVERRIDEMETHODIMP Initialize(HINSTANCE hinst, DWORD dwVersion)
	{
		return Impl->Initialize(hinst, dwVersion);
	}

	// @brief Finds a device by class GUID and name.
	// 
	// @param rguidClass Class GUID of the device type.
	// @param pszName Device name (partial matches allowed).
	// @param pguidInstance Receives the found device instance GUID.
	// @return S_OK if found, DIERR_NOTFOUND if not.
	__forceinline STDOVERRIDEMETHODIMP FindDevice(REFGUID rguidClass, LPCSTR pszName, LPGUID pguidInstance)
	{
		return Impl->FindDevice(rguidClass, pszName, pguidInstance);
	}

	// @brief Enumerates devices associated with specific input actions.
	// 
	// @param pszUserName Username string (may be nullptr).
	// @param lpActionFormat Pointer to an action format structure.
	// @param lpCallback Callback invoked for each matching device.
	// @param pvRef User data passed to the callback.
	// @param dwFlags Enumeration flags.
	// @return S_OK on success, or DIERR_INVALIDPARAM on error.
	__forceinline STDOVERRIDEMETHODIMP EnumDevicesBySemantics(LPCSTR pszUserName, LPDIACTIONFORMAT lpActionFormat, LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		return Impl->EnumDevicesBySemantics(pszUserName, lpActionFormat, lpCallback, pvRef, dwFlags);
	}

	// @brief Displays a device configuration user interface.
	// 
	// @param lpdiCallback Callback for configuration dialog events.
	// @param lpdiCDParams Pointer to configuration parameters.
	// @param dwFlags Flags controlling behavior.
	// @param pvRefData Optional user-defined data pointer.
	// @return S_OK on success, or DIERR_GENERIC on failure.
	__forceinline STDOVERRIDEMETHODIMP ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
	{
		return Impl->ConfigureDevices(lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
	}

private:
	IDirectInput8* Impl = nullptr;
};