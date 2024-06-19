#include "./HWKiller/Hwk.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        if (InitMinHook() && InitNTQSIHook()) {
            MessageBoxA(
                NULL,
                "HWKiller Is Ready!",
                "HWKiller",
                MB_ICONEXCLAMATION | MB_OK
            );
        }
       
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

