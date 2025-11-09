// Copyright Alternity Arts. All Rights Reserved.

#include "ProgramExtension.h"

#include <mutex>

struct SExtensions
{
	TArray<IProgramExtension*> Array;
	std::mutex Mutex;
};

static SExtensions& GetGlobalExtensions() // has to be function to make sure that there's always just ONE instance 
{
	static SExtensions extensions;
	return extensions;
}

void _NProgram::Extensions::Register(IProgramExtension* ext)
{
	auto& exts = GetGlobalExtensions();
	std::lock_guard lock(exts.Mutex);

	exts.Array.Add(ext);
}

TArray<IProgramExtension*> NProgram::Extensions::GetAll()
{
	auto& exts = GetGlobalExtensions();
	std::lock_guard lock(exts.Mutex);

	return exts.Array;
}