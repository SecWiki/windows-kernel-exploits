winshock-test
=============

Bash script that tests if a system is Winshock (MS14-066) vulnerable.

This script was developed by the emergency response team at [ANEXIA](http://www.anexia-wwc.com).

Important
---------

winshock_test.sh does behavioural analysis based upon the available SSL ciphers.
If you either do not have direct access to the target system due to SSL-offloading in any form or manually modified the available SSL ciphers the script will
be giving **false positives**.


Disclaimer
----------

This script does in no way try to exploit the vulnerability described in MS14-066.
It merely checks for hints on whether the target system has been patched or not.
For details, have a look at the script itself or read the short 'How it works'
part of this document below.

Usage
-----

```shell
./winshock_test.sh 10.0.0.1 3389
./winshock_test.sh 10.0.0.2 443
```

Be sure to use a port on which a service is listening for SSL connections,
which you can actually connect to. If the script takes long to complete,
chances are good that either no service is listening on that port or
a firewall is blocking access to that port.

The examples above use the default RDP and HTTPS ports for those checks.

Also, please ensure you are connecting directly to the target system. A
load balancer or any other form of SSL-offloading may impact the results
and may generate false-positives or false-negatives.


How it works
------------

MS14-066 introduced four new SSL ciphers, so a check can be made if
the target system supports those previously unsupported ciphers.
winshock_test.sh uses this fact by simply checking if those ciphers are
supported by the target system or not. If they are supported, the patches
have been applied.








