#!/bin/bash
#
# winshock_test.sh
#
# This script tries to determine whether the target system has the
# winshock (MS14-066) patches applied or not.
# This is done by checking if the SSL ciphers introduced by MS14-066 are
# available on the system.
#
#
# Authors:
#  Stephan Peijnik <speijnik@anexia-it.com>
#
# The MIT License (MIT)
#
# Copyright (c) 2014 ANEXIA Internetdienstleistungs GmbH
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

VERSION=0.2.1
HOST=$1
PORT=${2:-443}

if [ -z "$HOST" -o -z "$PORT" ]
then
  echo "Usage: $0 host [port]"
  echo "port defaults to 443."
  exit 1
fi

echo "Checking if script is up-to-date..."
REMOTE_VERSION=$(curl -k https://raw.githubusercontent.com/anexia-it/winshock-test/master/winshock_test.sh 2>/dev/null | grep '^VERSION=' | sed -e 's/^VERSION=//g')

if [[ "$REMOTE_VERSION" != "$VERSION" ]]
then
  echo -e "\033[91mYou are running an outdated version of this script."
  echo "The most recent version is $REMOTE_VERSION."
  echo -e "It is highly recommended to update your script first.\033[0m"
  read -p "Do you want to continue? (y/N) " -n 1 -r
  if [[ ! "$REPLY" =~ ^[Yy]$ ]]
  then
    exit 2
  fi
else
  echo "Script is up-to-date."
fi

echo -e "\n\033[91m"
cat <<IMP
*** IMPORTANT ***
This script is intended to give you a hint on whether the MS14-66 patches
have been installed or not.

Please do NOT rely on the results this script is giving, as the correctness
of the results can be impacted by manual modifications of cipher suites
with tools like IIS Crypto or load balancers or SSL-offloaders between
you and the target host.

Also, this script is unreliable if the target system is running
Windows Server 2012 R2, as the ciphers this script is testing for were present 
on Windows Server 2012 R2 without the MS14-066 updates as well.
If the checks are executed against IIS the result for Windows Server 2012 R2 
will be presented as "UNKNOWN".
IMP

echo -e "\033[93m"

cat <<WARN
*** WARNING ***
A negative result presented by this script does NOT mean that you do not
have to install the MS14-66 patches on the target system. 
Make sure to update all your Windows installations, regardless of the
results this script gives you.

WARN

echo -e "\033[0m"
read -p "I have read and understood the messages above. (y/N) " -n 1 -r
if [[ ! "$REPLY" =~ ^[yY]$ ]]
then
  echo -e "\n\033[91mAborting. Please re-run and confirm that you have read and understood the messages above.\033[0m"
  exit 4
fi
echo ""

# According to https://technet.microsoft.com/library/security/ms14-066 the
# following ciphers were added with the patch:
# * TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
# * TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
# * TLS_RSA_WITH_AES_256_GCM_SHA384
# * TLS_RSA_WITH_AES_128_GCM_SHA256
#
# The OpenSSL cipher names for these ciphers are:
MS14_066_CIPHERS="DHE-RSA-AES256-GCM-SHA384 DHE-RSA-AES128-GCM-SHA256 AES256-GCM-SHA384 AES128-GCM-SHA256"
# Ciphers supported by Windows Server 2012R2
WINDOWS_SERVER_2012R2_CIPHERS="ECDHE-RSA-AES256-SHA384 ECDHE-RSA-AES256-SHA"

# Test if OpenSSL does support the ciphers we're checking for...
echo -n "Testing if OpenSSL supports the ciphers we are checking for: "
openssl_ciphers=$(openssl ciphers)

for c in $MS14_066_CIPHERS
do
  if ! echo $openssl_ciphers | grep -q $c 2>&1 >/dev/null
  then
    echo -e "\033[91mNO (OpenSSL does not support $c cipher.)\033[0m"
    echo -e "\033[91mAborting."
    exit 5
  fi
done

echo -e "\033[92mYES\033[0m"

SERVER=$HOST:$PORT

echo -e "\n\033[94mTesting ${SERVER} for availability of SSL ciphers added in MS14-066...\033[0m"

patched="no"
for cipher in ${MS14_066_CIPHERS}
do
  echo -en "Testing cipher ${cipher}: "
  result=$(echo -n | openssl s_client -cipher "$cipher" -connect $SERVER 2>&1)
  if [[ "$result" =~ "connect:errno=" ]]
  then
    err=$(echo $result | grep ^connect: \
      | sed -e 's/connect:errno=.*//g' -e 's/connect: //g')
    echo -e "\033[93mConnection error: $err"
    echo -e "Aborting checks.\033[0m"
    exit 1
  elif [[ "$result" =~ "SSL23_GET_SERVER_HELLO:unknown protocol" ]]
  then
    echo -e "\033[93mNo SSL/TLS support on target port."
    echo -e "Aborting checks.\033[0m"
    exit 1
  elif [[ "$result" =~ "SSL_CTX_set_cipher_list:no cipher match" ]]
  then
    echo -e "\033[93mYour version of OpenSSL is not supported."
    echo -e "Aborting checks.\033[39m"
    exit 1
  elif [[ "$result" =~ "Cipher is ${cipher}" || "$result" =~ "Cipher    : ${cipher}" ]]
  then
    echo -e "\033[92mSUPPORTED\033[0m"
    if [[ "$patched" == "no" ]]
    then
      patched="yes"
    fi
  else
    echo -e "\033[91mUNSUPPORTED\033[0m"
  fi
done

windows_server_2012_or_later="no"
windows_server_2012_r2="no"
iis_detected="no"
# added by @stoep: check whether a 443 port runs IIS
if [[ "$PORT" == "443" ]]
then
  iis=$(curl -k -I https://$SERVER 2> /dev/null | grep "Server" )
  echo -n "Testing if IIS is running on port 443: "
  if [[ $iis == *Microsoft-IIS* ]]
  then 
    iis_version=$(echo $iis | sed -e 's|Server: Microsoft-IIS/||g')
    iis_detected="yes"
    echo -e "\033[92mYES - Version ${iis_version}\033[0m"
    if [[ $iis_version == *8.5* ]]
    then
      echo -e "\033[91mWindows Server 2012 R2 detected. Results of this script will be inconclusive.\033[0m"
      windows_server_2012_or_later="yes"
      windows_server_2012_r2="yes"
    elif [[ $iis_version == *8.0* ]]
    then
      windows_server_2012_or_later="yes"
      windows_server_2012_r2="no"
    fi
  else
    echo -e "\033[91mNO\033[0m"
  fi
fi

# Check if Windows Server 2012 or later is running on the remote system...
if [[ "$windows_server_2012_or_later" == "no"  && "$iis_detected" == "no" ]]
then
  echo -e "\033[94mChecking if target system is running Windows Server 2012 or later...\033[0m"
  for cipher in ${WINDOWS_SERVER_2012R2_CIPHERS}
  do
    echo -en "Testing cipher ${cipher}: "
    result=$(echo -n | openssl s_client -cipher "$cipher" -connect $SERVER 2>&1)
    if [[ "$result" =~ "connect:errno=" ]]
    then
      err=$(echo $result | grep ^connect: \
        | sed -e 's/connect:errno=.*//g' -e 's/connect: //g')
      echo -e "\033[93mConnection error: $err"
      echo -e "Aborting checks.\033[0m"
      exit 1
    elif [[ "$result" =~ "SSL23_GET_SERVER_HELLO:unknown protocol" ]]
    then
      echo -e "\033[93mNo SSL/TLS support on target port."
      echo -e "Aborting checks.\033[0m"
      exit 1
    elif [[ "$result" =~ "Cipher is ${cipher}" || "$result" =~ "Cipher    : ${cipher}" ]]
    then
      echo -e "\033[92mSUPPORTED\033[0m"
      if [[ "$windows_server_2012_or_later" == "no" ]]
      then
        windows_server_2012_or_later="yes"
        break
      fi
    else
      echo -e "\033[91mUNSUPPORTED\033[0m"
    fi
  done
fi

if [[ "$patched" == "yes" && "$windows_server_2012_or_later" == "no" ]]
then
  patched="\033[92mYES\033[0m"
elif [[ "$patched" == "yes" ]]
then
  patched="\033[93mUNKNOWN"
  if [[ "$windows_server_2012_r2" == "yes" ]]
  then
    patched="$patched: Windows Server 2012 R2 detected."
  else
    patched="$patched: Windows Server 2012 or later detected."
  fi
else
  patched="\033[91mNO\033[0m"
fi

echo -e "\033[94m$SERVER is patched: $patched\033[0m"
echo -e "\n\033[93m"
cat <<EOF 
*** IMPORTANT ***

Please keep in mind that the patch-status reported above is only a hint and
may generate both false-positive and false-negative results in some cases.

If Windows Server 2012 R2 is reported above results WIIL BE incorrect.
If Windows Server 2012 or later is reported above results MAY BE incorrect,
please test again against IIS running on port 443.

The information above may be incorrect if:

* the available SSL ciphers have been modified manually
* you are not directly connecting to the target system
* the target system is running Windows Server 2012 R2

Please do apply the MS14-066 patches to all your systems regardless
of the results presented above!

EOF
echo -en "\033[0m"
exit 0
