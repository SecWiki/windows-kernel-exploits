#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#pragma comment(lib,"psapi.lib")
#pragma once

#define STATUS_INFO_LENGTH_MISMATCH	((NTSTATUS)0xC0000004L)
#define STATUS_SUCCESS              ((NTSTATUS)0x00000000L)
#define NT_SUCCESS(Status)			(((NTSTATUS)(Status)) >= 0)

typedef enum _SYSTEM_INFORMATION_CLASS
{ 
  SystemModuleInformation = 11,
  SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

typedef
NTSTATUS
(NTAPI *NtAllocateVirtualMemory_)(
	__in     HANDLE ProcessHandle,
	__inout  PVOID *BaseAddress,
	__in     ULONG_PTR ZeroBits,
	__inout  PSIZE_T RegionSize,
	__in     ULONG AllocationType,
	__in     ULONG Protect
);

BOOL
AlocNullPageAndFixCondtions(
	VOID
	);

BOOL
SetupKernelShellcode(
	IN ULONG_PTR UsermodeReturnAddress,
	IN ULONG_PTR ProcessId
	);

BOOL
GetDriverImageBase( 
	OUT PULONG_PTR DriverBase, 
	IN PCHAR DriverName
	);

ULONG_PTR
KernelGetProcAddress(
	IN ULONG_PTR UserKernelBase, 
	IN ULONG_PTR RealKernelBase, 
	IN LPCSTR SymName
	);

BOOL
GetKernelBaseInfo(
	OUT PULONG_PTR kernelBase, 
    IN OUT PCHAR kernelImage, 
	IN UINT Size
	);

ULONG_PTR
GetCiEnabledAddress(
	IN HMODULE hModule
	);

ULONG_PTR 
SpawnProcess(
	IN PCHAR szProcess
	);

ULONG_PTR
GetUmsSchedulerAddress(
	VOID
	);

BOOL
HookUmsScheduler(
	VOID
	);

extern "C"
VOID
SetNonCanonicalAddress(
	VOID
	);