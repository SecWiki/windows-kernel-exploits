# MS08-068

```
This module will relay SMB authentication requests to another host, gaining access to an authenticated SMB session if successful. If the connecting user is an administrator and network logins are allowed to the target machine, this module will execute an arbitrary payload. 
To exploit this, the target system	must try to	authenticate to this module. The easiest way to force a SMB authentication attempt is by embedding a UNC path (\\SERVER\SHARE) into a web page or email message. 
When the victim views the web page or email, their system will automatically connect to the server specified in the UNC share (the IP address of the system running this module) and attempt to authenticate. 
Unfortunately, this module is not able to clean up after itself. The service and payload file listed in the output will need to be manually removed after access has been gained. 
The service created by this tool uses a randomly chosen name and description, so the services list can become cluttered after repeated exploitation. 
The SMB authentication relay attack was first reported by Sir Dystic on March 31st, 2001 at @lanta.con in Atlanta, Georgia. On November 11th 2008 Microsoft released bulletin MS08-068. 
This bulletin includes a patch which prevents the relaying of challenge keys back to the host which issued them, preventing this exploit from working in the default configuration. 
It is still possible to set the SMBHOST parameter to a third-party host that the victim is authorized to access, but the "reflection" attack has been effectively broken.
```
  
- The exp is from [exp-db](https://www.exploit-db.com/exploits/20/)
Vulnerability reference:
 * [MS08-068](https://technet.microsoft.com/zh-cn/zh/library/security/ms08-068)
 * [CVE-2008-4037](http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2008-4037)


## load the module within the Metasploit console
```
msf > use exploit/windows/smb/smb_relay
msf exploit(smb_relay) > show targets
    ...targets...
msf exploit(smb_relay) > set TARGET <target-id>
msf exploit(smb_relay) > show options
    ...show and set options...
msf exploit(smb_relay) > exploit
    
```




