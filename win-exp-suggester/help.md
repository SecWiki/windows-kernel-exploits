## 使用方法

1.首先需要更新漏洞数据库
```
$ ./windows-exploit-suggester.py --update
[*] initiating...
[*] successfully requested base url
[*] scraped ms download url
[+] writing to file 2017-06-14-mssb.xls
[*] done
```

2.然后安装程序依赖：[python-xlrd](https://pypi.python.org/pypi/xlrd)  
```
ubuntu@ubuntu:~/xlrd-0.9.4/xlrd-0.9.4$ sudo python setup.py install
[sudo] password for ubuntu: 
running install
running build
running build_py
running build_scripts
running install_lib
creating /usr/local/lib/python2.7/dist-packages/xlrd
copying build/lib.linux-x86_64-2.7/xlrd/compdoc.py -> /usr/local/lib/python2.7/dist-packages/xlrd
......
```  

3.接着在目标机器中执行systeminfo，并将其输出到文件 win7sp1-systeminfo.txt 中，然而带入参数 –systeminfo 中，并指定数据库位置(就是那个excel文件)
```
$ ./windows-exploit-suggester.py --database 2017-06-14-mssb.xls --systeminfo win7sp1-systeminfo.txt 
[*] initiating...
[*] database file detected as xls or xlsx based on extension
[*] reading from the systeminfo input file
[*] querying database file for potential vulnerabilities
[*] comparing the 15 hotfix(es) against the 173 potential bulletins(s)
[*] there are now 168 remaining vulns
[+] windows version identified as 'Windows 7 SP1 32-bit'
[*] 
[M] MS14-012: Cumulative Security Update for Internet Explorer (2925418) - Critical
[E] MS13-101: Vulnerabilities in Windows Kernel-Mode Drivers Could Allow Elevation of Privilege (2880430) - Important
[M] MS13-090: Cumulative Security Update of ActiveX Kill Bits (2900986) - Critical
[M] MS13-080: Cumulative Security Update for Internet Explorer (2879017) - Critical
[M] MS13-069: Cumulative Security Update for Internet Explorer (2870699) - Critical
[M] MS13-059: Cumulative Security Update for Internet Explorer (2862772) - Critical
[M] MS13-055: Cumulative Security Update for Internet Explorer (2846071) - Critical
[M] MS13-053: Vulnerabilities in Windows Kernel-Mode Drivers Could Allow Remote Code Execution (2850851) - Critical
[M] MS13-009: Cumulative Security Update for Internet Explorer (2792100) - Critical
[M] MS13-005: Vulnerability in Windows Kernel-Mode Driver Could Allow Elevation of Privilege (2778930) - Important
[*] done
```  
下面这条指令可以列出指定操作系统版本存在的所有可能EXP(假设目标系统没有打任何补丁)
```
$ ./windows-exploit-suggester.py --database 2017-06-14-mssb.xls --ostext 'windows server 2008 r2' 
[*] initiating...
[*] database file detected as xls or xlsx based on extension
[*] getting OS information from command line text
[*] querying database file for potential vulnerabilities
[*] comparing the 0 hotfix(es) against the 196 potential bulletins(s)
[*] there are now 196 remaining vulns
[+] windows version identified as 'Windows 2008 R2 64-bit'
[*] 
[M] MS13-009: Cumulative Security Update for Internet Explorer (2792100) - Critical
[M] MS13-005: Vulnerability in Windows Kernel-Mode Driver Could Allow Elevation of Privilege (2778930) - Important
[E] MS11-011: Vulnerabilities in Windows Kernel Could Allow Elevation of Privilege (2393802) - Important
[M] MS10-073: Vulnerabilities in Windows Kernel-Mode Drivers Could Allow Elevation of Privilege (981957) - Important
[M] MS10-061: Vulnerability in Print Spooler Service Could Allow Remote Code Execution (2347290) - Critical
[E] MS10-059: Vulnerabilities in the Tracing Feature for Services Could Allow Elevation of Privilege (982799) - Im
```