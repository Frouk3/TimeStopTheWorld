#pragma once
#include <Windows.h>

namespace shared
{
	inline DWORD base = (DWORD)GetModuleHandleA(NULL);

	inline void Core_PlaySound(const char* se, int unused) noexcept
	{
		DWORD address = base + 0xA5E050;
		((void(__cdecl*)(const char*, int))address)(se, unused);
	}

	inline bool IsKeyPressed(int key) noexcept
	{
		static bool first_press = true;

		if (GetAsyncKeyState(key) & 0x8000)
		{
			if (first_press)
			{
				first_press = false;
				return true;
			}
		}
		else
		{
			first_press = true;
			return false;
		}

		return false;
	}
}