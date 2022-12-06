#pragma once
#include "GameFlags.h"
#include "GameMenuStatus.h"
#include "GameplayFlags.h"
#include <Windows.h>

namespace shared
{
	inline DWORD base = (DWORD)GetModuleHandleA(NULL);
	inline GameplayFlags g_GameplayFlags = *(GameplayFlags*)(base + 0x17EA094);
	inline GameFlags g_GameFlags = *(GameFlags*)(base + 0x17EA090);
	inline GameMenuStatus g_GameMenuStatus = *(GameMenuStatus*)(base + 0x17E9F9C);
	inline bool IsForegroundWindow = *(bool*)(base + 0x19D509C);

	inline void Core_PlaySound(const char* se, int unused) noexcept
	{
		DWORD address = base + 0xA5E050;
		((void(__cdecl*)(const char*, int))address)(se, unused);
	}
}