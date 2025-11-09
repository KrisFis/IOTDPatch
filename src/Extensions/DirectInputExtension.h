// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ProgramExtension.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace NDirectInput
{
	bool Hook(LPVOID targetFunc, LPVOID detourFunc, LPVOID* originalFunc);
	bool Unhook(const TArray<LPVOID>& hooks);
}

template<typename T>
struct TDirectInputObject
{
	TDirectInputObject() = default;
	TDirectInputObject(T* obj) : Obj(obj) {}
	~TDirectInputObject() { NDirectInput::Unhook(Hooks); }

	FORCEINLINE bool IsValid() const { return !!Obj; }
	
	FORCEINLINE T& Get() const { return *Obj; }
	FORCEINLINE T* operator->() const { return Obj; }
	FORCEINLINE T& operator*() const { return *Obj; }

	bool HookVTableFunction(uint8 idx, LPVOID detourFunc, LPVOID* originalFunc)
	{
		void** vTable = *(void***)Obj;
		void* funcAddr = vTable[idx];

		const bool hooked = NDirectInput::Hook(funcAddr, detourFunc, originalFunc);
		if (hooked) Hooks.Add(funcAddr);
		return hooked;
	}

	FORCEINLINE void Reset(T* obj = nullptr)
	{
		NDirectInput::Unhook(Hooks);
		Hooks.Empty();
		Obj = obj;
	}
	
	T* Obj = nullptr;
	TArray<LPVOID> Hooks;
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
	TArray<LPVOID> ModuleHooks;

	TDirectInputObject<IDirectInput8> DInput;
	TArray<TDirectInputObject<IDirectInputDevice8>> DInputDevices;
};

DECLARE_EXTENSION(CDirectInputExtension)