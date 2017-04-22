# MS15-076

- We can Copies a file to any privileged location on disk

- The POC was from [@monoxgas](https://github.com/monoxgas/Trebuchet)


Vulnerability reference:
 * [MS15-076](https://technet.microsoft.com/en-us/library/security/ms15-076.aspx)
 * [CVE-2015-2370](http://cve.mitre.org/cgi-bin/cvename.cgi?name=cve-2015-2370)
 

### notes
- Exploit can only be one once every 2-3 minutes. This is because RPC can be held up by LocalSystem
- The destination file can't already exist
- Tested on x64/x86 Windows 7/8.1
- Microsoft.VisualStudio.OLE.Inerop.dll must be in the same directory

### Usage

c:\> trebuchet.exe C:\Users\Bob\Evil.txt C:\Windows\System32\Evil.dll


### Links

* [lightly modified Proof of Concept by James Forshaw with Google]( https://code.google.com/p/google-security-research/issues/detail?id=325)


