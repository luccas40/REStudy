#pragma once

typedef int(__stdcall* WinMainC) (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
WinMainC OriginalWinMain;

typedef BOOL(__cdecl* ProtocolHandler) (int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEncrypted);
ProtocolHandler OriginalProtocolHandler;

//wait until this byte is changed so we hook into WinMain
#define THEMIDA_BS_WAIT 0x01310873

//Hook into WinMain so we have access to the entire code
#define WIN_MAIN 0x01310866

//replace the jump address to the rest of the loading game function address which happens to be an offset of 10 01
//E9 10 01 00 00 
#define SKIP_LAUNCHER 0x013109F0

//nop 5bytes to remove the xor 3 encryption of the muerror.log
#define XOR_3_MU_ERROR 0x0130A5F8

//GameGuard checks if its instance is running, 0x755 is their okay code
//so the easiest way i found to bypass it is to invert the jump condition
//from 0x74(JE) to 0x75(JNE)
#define GAMEGUARD_755_CMP 0x00AB49BF

//this one is checking periodically if gg is up and if not show a MessageBoxA and freezes the game
//so we just nop the JNE(2 bytes) cause next instruction is a jump to the return of the function
#define GAMEGUARD_755_CMP2 0x00ab4a08

#define IP_ADDRESS 0x01CB2E88

#define PROTOCOL_HANDLER 0x0137ff65


//found the Lang.mpr password here 0x0106f37e
//can find it easly by finding the string GuardianTooltip Load FAILS
//then trace back the nearest function

//also the key with the xor bytes can be found if we trace the fopen mode _Mode = "rb";


