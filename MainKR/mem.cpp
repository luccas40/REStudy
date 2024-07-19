#pragma once
#include "mem.h"
#include <exception>
#include <iostream>


bool mem::Write(BYTE* src, BYTE* dst, const uintptr_t len)
{
	DWORD currProtection;
	VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &currProtection);
	memcpy_s(dst, len, src, len);
	VirtualProtect(dst, len, currProtection, &currProtection);
	return true;
}

bool mem::SetByte(BYTE* addr, BYTE value)
{
	DWORD currProtection;
	VirtualProtect(addr, 1, PAGE_EXECUTE_READWRITE, &currProtection);
	*addr = value;
	VirtualProtect(addr, 1, currProtection, &currProtection);
	return true;
}
bool mem::SetDword(BYTE* addr, DWORD value)
{
	DWORD currProtection;
	VirtualProtect(addr, 4, PAGE_EXECUTE_READWRITE, &currProtection);
	*(uintptr_t*)(addr) = value;
	VirtualProtect(addr, 4, currProtection, &currProtection);
	return true;
}

bool mem::NOP(BYTE* addr, const uintptr_t len) {
	DWORD currProtection;
	VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &currProtection);
	memset(addr, 0x90, len);
	VirtualProtect(addr, len, currProtection, &currProtection);
	return true;
}

bool mem::ReplaceAddressIn(BYTE* addr, uintptr_t newAddr) {
	DWORD currProtection;
	VirtualProtect(addr, 5, PAGE_EXECUTE_READWRITE, &currProtection);
	*(uintptr_t*)(addr + 1) = newAddr;
	VirtualProtect(addr, 5, currProtection, &currProtection);
	return true;
}

bool mem::Detour(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return false;

	DWORD currProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &currProtection);
	memset(src, 0x90, len);
	uintptr_t relativeAddress = dst - src - 5;
	*src = 0xE9; //jmp
	*(uintptr_t*)(src + 1) = relativeAddress;
	VirtualProtect(src, len, currProtection, &currProtection);
	return true;
}

BYTE* mem::TrampHook(BYTE* src, BYTE* dst, const uintptr_t len)
{
	try {
		if (len < 5) return 0;

		BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		//copy the original bytes so we dont lose it
		memcpy_s(gateway, len, src, len);

		//get the amount of bytes we need to jump
		uintptr_t gatewayRelativeAddress = src - gateway - 5;

		//set the jmp to the end of the gateway back to the original code
		*(gateway + len) = 0xE9;
		*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddress;

		Detour(src, dst, len);
		return gateway;
	}
	catch (const std::exception& ex) {
		std::cout << "error in Tramp Hook";
		system("pause");
		return 0;
	}

}