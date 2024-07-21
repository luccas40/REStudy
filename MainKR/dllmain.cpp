// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include "mem.h"
#include <iostream>
#include <sstream>
#include "Offsets.h"
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

EXTERN_DLL_EXPORT int GetMainVersion() 
{
    return 1;
}
std::string GetHexString(BYTE* packet, int start, int end) 
{
    std::stringstream buffer;
    for (int i = start; i < end; i++) {
        buffer << std::hex << std::uppercase << (int)*(packet + i) << " ";
    }
    return buffer.str();
}


//hooking into a __thiscall is messy
//we have to send to a __fastcall which sends the EDX register too
void __fastcall SendPacketHook(BYTE* This, void* _EDX, BYTE* packet, size_t packetSize, int shouldEncrypt, int noidea) {

    //std::cout << "Hooked Func is: " << packetSize << " " << arg2 << " " << arg3 << std::endl;
    std::cout << "Send Packet: " << GetHexString(packet, 0, packetSize) << std::endl;
    return OriginalSendPacket(This, packet, packetSize, 0, noidea);
}

BOOL __cdecl HookedProtocolHandler(int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEncrypted) 
{
    BYTE* buf = new BYTE[Size];
    memcpy(buf, ReceiveBuffer, Size);

    typedef int (*blabla)(BYTE, BYTE*, size_t);
    ((blabla)0x00db5e03)(HeadCode, buf, Size);
    std::cout << "Received Packet: " << GetHexString(buf, 0, Size) << std::endl;
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
    
    memset((void*)IP_ADDRESS, 0, 24);
    memcpy((void*)IP_ADDRESS, "192.168.1.250", 13);

    OriginalProtocolHandler = (ProtocolHandler)mem::TrampHook((BYTE*)PROTOCOL_HANDLER, (BYTE*)HookedProtocolHandler, 5);
    OriginalSendPacket = (SendPacket)mem::TrampHook((BYTE*)SEND_PACKET, (BYTE*)SendPacketHook, 8);

    mem::NOP((BYTE*)LOGIN_USERPASS_BUXCONVERTER, 26);
    mem::Write((BYTE*)new BYTE[3]{ 0xc2, 0x0c, 0 }, (BYTE*)XOR32_DATASEND_ENCRYPTION, 3);
    mem::NOP((BYTE*)POSSIblE_PACKET_TWISTER_RECEIV, 5);    
    mem::NOP((BYTE*)POSSIblE_PACKET_TWISTER_SEND, 5);

    //FIX SHOW LOGIN SCREEN
    //this will make the client use the argument rather than the local variable 
    //because the local variable is the "decrypted" version of the argument
    //but since we are not encrypting anything we dont need the local variable
    //this is only for this packet apparently
    //replace the LEA with the MOV
    //assign the local variable with the argument rather than the decrypted stuff
    //then we need to fix the comparison to throw the error
    mem::Write((BYTE*)(new BYTE[6]{ 0x8b, 0x85, 0x08, 0, 0, 0 }), (BYTE*)0x0134ba2a, 6);
    mem::SetByte((BYTE*)0x0134baf9, 0xEB); //was 0x75 JNE replaced with 0xEB JMP


    //return to the rest of the original function
    return OriginalWinMain(hInstance, hPrevInstance, szCmdLine, nCmdShow);
}



DWORD WINAPI MainThread(LPVOID param)
{
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    //freopen_s((FILE**)stdout, "output.txt", "w", stdout);
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

