// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include "mem.h"
#include <iostream>
#include "Offsets.h"
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

EXTERN_DLL_EXPORT int GetMainVersion() 
{
    return 1;
}


BOOL __cdecl HookedProtocolHandler(int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEncrypted) 
{
    std::cout << "Received Packet: ";
    for (int i = 0; i < Size; i++) {
        std::cout << std::hex << std::uppercase << (int)*(ReceiveBuffer + i) << " ";
    }
    std::cout << std::endl;
    return OriginalProtocolHandler(HeadCode, ReceiveBuffer, Size, bEncrypted);
}






void WaitEqual(BYTE* ptr, BYTE value) 
{
    while (*ptr == value) {
        Sleep(1);
    }
}








int __cdecl MyWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow) 
{
    mem::ReplaceAddressIn((BYTE*)SKIP_LAUNCHER, (uintptr_t)0x110); //110 = 10 01 00 = 01 10 = 110

    mem::NOP((BYTE*)XOR_3_MU_ERROR, 5);

    mem::SetByte((BYTE*)GAMEGUARD_755_CMP, 0x75);
    mem::NOP((BYTE*)GAMEGUARD_755_CMP2, 2);

    //memset((void*)IP_ADDRESS, 0, 24);
    //memcpy((void*)IP_ADDRESS, "192.168.1.250", 13);

    OriginalProtocolHandler = (ProtocolHandler)mem::TrampHook((BYTE*)PROTOCOL_HANDLER, (BYTE*)HookedProtocolHandler, 5);


    //return to the rest of the original function
    return OriginalWinMain(hInstance, hPrevInstance, szCmdLine, nCmdShow);
}



DWORD WINAPI MainThread(LPVOID param)
{
    //AllocConsole();
    //freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdout, "output.txt", "w", stdout);
    WaitEqual((BYTE*)THEMIDA_BS_WAIT, 0);
    OriginalWinMain = (WinMainC)mem::TrampHook((BYTE*)WIN_MAIN, (BYTE*)MyWinMain, 8);
    std::cout << "Done Hooking" << std::endl;
    return 0;
}







BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, MainThread, hModule, 0, 0));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

