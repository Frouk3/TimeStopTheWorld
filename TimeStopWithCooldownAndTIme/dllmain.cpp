// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "TimeStop.h"
#include "game/shared.h"
#include <Windows.h>

DWORD WINAPI thing(LPVOID) noexcept
{
    static bool once = false;
    while (true)
    {
        if (!once)
        {
            TimeStop::LoadConfig();
            once = true;
        }
        TimeStop::Update();
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&thing, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

