# MS13-005

```
Due to a problem with isolating window broadcast messages in the Windows kernel,
an attacker can broadcast commands from a lower Integrity Level process to a higher Integrity Level process, 
thereby effecting a privilege escalation. 
This issue affects Windows Vista, 7, 8, Server 2008, Server 2008 R2, Server 2012, and RT. 
Note that spawning a command prompt with the shortcut key combination Win+Shift+# does not work in Vista, 
so the attacker will have to check if the user is already running a command prompt and set SPAWN_PROMPT false. 
Three exploit techniques are available with this module. 
The WEB technique will execute a powershell encoded payload from a Web location. 
The FILE technique will drop an executable to the file system, 
set it to medium integrity and execute it. 
The TYPE technique will attempt to execute a powershell encoded payload directly from the command line, 
but may take some time to complete.
```
- The exp was from [@0vercl0k](https://github.com/0vercl0k/stuffz/blob/master/ms13-005-funz-poc.cpp)

Vulnerability reference:
 * [MS13-005](https://technet.microsoft.com/library/security/ms13-005)
 * [CVE-2013-0008](http://cve.mitre.org/cgi-bin/cvename.cgi?name=cve-2013-0008)


##  load the module within the msf
- [msf](https://www.rapid7.com/db/modules/exploit/windows/local/ms13_005_hwnd_broadcast)
```
msf > use exploit/windows/local/ms13_005_hwnd_broadcast
msf exploit(ms13_005_hwnd_broadcast) > show targets
    ...targets...
msf exploit(ms13_005_hwnd_broadcast) > set TARGET <target-id>
msf exploit(ms13_005_hwnd_broadcast) > show options
    ...show and set options...
msf exploit(ms13_005_hwnd_broadcast) > exploit
```

## Links

- [HWND_BROADCAST](http://blog.cmpxchg8b.com/2013/02/a-few-years-ago-while-working-on.html)