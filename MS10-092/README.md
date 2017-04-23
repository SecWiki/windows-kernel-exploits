# MS10-092 
- Windows Task Scheduler Privilege Escalation
```
This module exploits the Task Scheduler 2.0 XML 0day exploited by Stuxnet.
When processing task files, the Windows Task Scheduler only uses a CRC32 checksum to validate that the file has not been tampered with. 
Also, In a default configuration, normal users can read and write the task files that they have created. 
By modifying the task file and creating a CRC32 collision, an attacker can execute arbitrary commands with SYSTEM privileges.
NOTE: Thanks to webDEViL for the information about disable/enable.
```
- The exp was from [@webDEViL](https://www.exploit-db.com/author/?a=587)

Vulnerability reference:
 * [MS10-092](https://technet.microsoft.com/library/security/ms10-092)
 * [CVE-2010-3338](https://www.exploit-db.com/exploits/15589/)
 * [Task Scheduler '.XML' Privilege Escalation](https://www.exploit-db.com/exploits/19930/)

## Usage
[MS10-092-YouTube](https://www.youtube.com/watch?v=gd-F1dlWBAw)



## load the module within the Metasploit
[msf](https://www.rapid7.com/db/modules/exploit/windows/local/ms10_092_schelevator)
```
msf > use exploit/windows/local/ms10_092_schelevator
msf exploit(ms10_092_schelevator) > show targets
    ...targets...
msf exploit(ms10_092_schelevator) > set TARGET <target-id>
msf exploit(ms10_092_schelevator) > show options
    ...show and set options...
msf exploit(ms10_092_schelevator) > exploit
```