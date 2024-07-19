#pragma once
#include "Windows.h"
namespace mem {

	bool Write(BYTE* src, BYTE* dst, const uintptr_t len);
	bool SetByte(BYTE* addr, BYTE value);
	bool SetDword(BYTE* addr, DWORD value);
	bool NOP(BYTE* addr, const uintptr_t len);
	bool ReplaceAddressIn(BYTE* addr, uintptr_t newAddr);
	bool Detour(BYTE* src, BYTE* dst, const uintptr_t len);
	BYTE* TrampHook(BYTE* src, BYTE* dst, const uintptr_t len);
}
