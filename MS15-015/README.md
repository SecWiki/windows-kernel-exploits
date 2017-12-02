# MS15-015

```
An elevation of privilege vulnerability exists in Microsoft Windows when it fails to properly validate and enforce impersonation levels. 
An attacker who successfully exploited this vulnerability could bypass impersonation-level security checks and gain elevated privileges on a targeted system. 
This vulnerability can be exploited only in the specific scenario where the process uses SeAssignPrimaryTokenPrivilege, which is not available for normal processes.
```


Vulnerability reference:
 * [MS15-015](https://technet.microsoft.com/zh-cn/library/security/ms15-015.aspx)
 * [CVE-2015-0062](http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2015-0062)

## Usage
```
c:\> MS15-015.exe "whoami"
```

## Thanks
感谢 **@浮萍** 帮助补充完善