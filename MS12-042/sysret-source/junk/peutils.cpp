#include "peutil.h"

/* WTF _WIN64 is not defined!!!! */
PVOID 
PeGetCodeSectionAddress(
	IN PVOID BaseAddress
	)
{
	PIMAGE_OPTIONAL_HEADER64  pOptionalHeader;

	pOptionalHeader = (PIMAGE_OPTIONAL_HEADER64) OPTHDROFFSET(BaseAddress);
	return (PVOID)(pOptionalHeader->BaseOfCode + (ULONG_PTR)BaseAddress);
}

DWORD 
PeGetCodeSectionSize( 
	IN PVOID BaseAddress
	)
{
	PIMAGE_OPTIONAL_HEADER64	pOptionalHeader;

	pOptionalHeader = (PIMAGE_OPTIONAL_HEADER64) OPTHDROFFSET(BaseAddress);
	return pOptionalHeader->SizeOfCode;
}