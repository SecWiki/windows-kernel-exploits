# MS10-059
```
一些Windows应用中使用了 Tracing 功能记录调试信息。使用了这个 Tracing 功能的 Windows 进程会持续监控相关子键的变化，一旦注册表值发生了变化就会立即读取该值。
其中的一个注册表值为 FileDirectory，包含有 Windows目录名。在Local System 账号运行的服务连接到管道时拥有扮演权限的本地用户可以通过扮演为 Local System 账号（或Administrator等特权账号）提升权限。
注册表项对 Users 组开放了 Set Value 权限，因此任何通过认证的用户都可以设置任意值。
```

Vulnerability reference:
 * [MS10-059](https://technet.microsoft.com/library/security/ms10-059)
 * [CVE-2010-2554](http://cve.mitre.org/cgi-bin/cvename.cgi?name=cve-2010-2554)
 
## Usage
```
c:\> Churraskito.exe "C:\windows\system32\cmd.exe" "net user 123 123 /add"
```

## References
[Windows Privilege Escalation Fundamentals](http://www.fuzzysecurity.com/tutorials/16.html)  
