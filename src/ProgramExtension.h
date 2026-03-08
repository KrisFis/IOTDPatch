// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include <unordered_map>

#include "ASTD/ASTD.h"

class IProgramExtension
{
public:
	virtual ~IProgramExtension() = default;

	FORCEINLINE bool IsInitialized() const { return _isInitialized; }

	virtual void Initialize() { _isInitialized = true; }
	virtual void Tick(double deltaTime) {}
	virtual void Shutdown() { _isInitialized = false; }

private:
	uint8 _isInitialized : 1 = false;
};

namespace _NProgram::Extensions
{
	void Register(IProgramExtension* ext);

	template<typename T>
	struct TGet { static T* Get() = delete; };

	template<typename T>
	T* MakeAndRegister()
	{
		T* result = new T();
		Register(result);
		return result;
	}
}

namespace NProgram::Extensions
{
	template<typename T>
	FORCEINLINE T* Get() { return _NProgram::Extensions::TGet<T>::Get(); }

	TArray<IProgramExtension*> GetAll();
}

#define DECLARE_EXTENSION(ClassType)																	\
namespace _NProgram::Extensions																			\
{																										\
	template<> struct TGet<ClassType>																	\
	{																									\
		TGet() { Get(); }																				\
		static ClassType* Get()																			\
		{																								\
			static ClassType* staticObj = MakeAndRegister<ClassType>();									\
			return staticObj;																			\
		}																								\
	};																									\
	static TGet<ClassType> ClassType##_Default = {};													\
}