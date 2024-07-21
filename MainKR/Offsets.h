#pragma once

typedef int(__stdcall* WinMainC) (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow);
WinMainC OriginalWinMain;

typedef BOOL(__cdecl* ProtocolHandler) (int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEncrypted);
ProtocolHandler OriginalProtocolHandler;

typedef void(__thiscall* SendPacket) (BYTE*, BYTE*, size_t, int, int);
SendPacket OriginalSendPacket;

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


//can be found if we search for the string "> Login Request.\r\n"
//then the closest function is the SendRequestLogInNew function
//there are 2 calls to encrypt user and pass
//I'm wiping them both here 26 bytes
#define LOGIN_USERPASS_BUXCONVERTER 0x00c6ad51


//after you find the SendRequestLogInNew analize the last function of the else part 
//one of the calls inside that function is to create the structure of the packet
//and you will know cause there will be a memcpy with the exacly size of the packet, for now 0x3c (60)
//and it will be followed by two tests and inside those tests, two calls with also the packet size as argument
//there you will find the xor32 keys and the loop encrypting the packet
//here I'm gonna detour instead of Trampoline because in every packet struct creation they call it
//is either patching all of the packet structures or detour the xor32 function
//another option is to replace the first instruction with a return, im gonna use this
#define XOR32_DATASEND_ENCRYPTION 0x00db5be6

//found this by tracing backwards from ws2_32.Send
//there is a cmp checking for the 3th arg I'm assuming is a bool representing the need to encrypt the data
//I'm hooking this one to log the packets also to force one of the arguments to send direct without going through simple modulus encryption
#define SEND_PACKET 0x00db5a16

//this function is called before calling the SEND_PACKET 
//decided to nop this because it was changing the packet bytes
//at least for the login request packet it changed the 0x0 null ending string
#define POSSIblE_PACKET_TWISTER_SEND 0x00db58ee

//i seriously dont have a clue what this call does
//all i know is that its messing with the packet received bytes and then the client doesnt know what to do
#define POSSIblE_PACKET_TWISTER_RECEIV 0x0137ff8d

//----------------------------------------------------------------------------------------//


//found the Lang.mpr password here 0x0106f37e (3q*#P<8ALZy*soC2&eHwrA^@=)
//can find it easly by finding the string "GuardianTooltip Load FAILS"
//then trace back the nearest function

//also the key with the xor bytes can be found if we trace the fopen mode _Mode = "rb";

//also it looks like they are changing the packet headcode on every update if not every season
//to find the login screen find the string "Connectting error. "
//btw the ProtocolHandler is what calls this function


