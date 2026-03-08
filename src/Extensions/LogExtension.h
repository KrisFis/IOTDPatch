// Copyright Alternity Arts. All Rights Reserved.

#pragma once

#include "ProgramExtension.h"

#include <fstream>

class CLogExtension : public IProgramExtension
{
public:
	static constexpr const tchar* LOG_FILENAME = TEXT("IOTDPatched.log"); 

	typedef IProgramExtension Super;

	// ~BEGIN IProgramExtension interface
	virtual void Initialize() override;
	virtual void Tick(double deltaTime) override {}
	virtual void Shutdown() override;
	// ~END IProgramExtension interface

private:
	class StreamDualBuf : public std::streambuf
	{
	public:
		StreamDualBuf() = default;
		StreamDualBuf(std::streambuf* a, std::streambuf* b) : sb1(a), sb2(b) {}

	protected:
		int32 overflow(int32 c) override {
			if (c == EOF) return 0;
			if (sb1) sb1->sputc(c);
			if (sb2) sb2->sputc(c);
			return c;
		}

		int32 sync() override {
			int32 r1 = sb1 ? sb1->pubsync() : 0;
			int32 r2 = sb2 ? sb2->pubsync() : 0;
			return (r1 == 0 && r2 == 0) ? 0 : -1;
		}

	private:
		std::streambuf* sb1 = nullptr;
		std::streambuf* sb2 = nullptr;
	};

	FILE* _conOut = nullptr;
	std::streambuf* _conBuf = nullptr;

	StreamDualBuf _dbuf;
	std::streambuf* _oldBuf = nullptr;

	std::ofstream _logFile;
};

DECLARE_EXTENSION(CLogExtension)