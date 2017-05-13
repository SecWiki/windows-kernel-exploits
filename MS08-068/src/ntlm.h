#ifndef _NTLMER_H_
#define _NTLMER_H_
/* $Id$
   Single file NTLM system to create and parse authentication messages.

   http://www.reversing.org
   ilo-- ilo@reversing.org  

   I did copy&paste&modify several files to leave independent NTLM code 
   that compile in cygwin/linux environment. Most of the code was ripped 
   from Samba implementation so I left the Copying statement. Samba core
   code was left unmodified from 1.9 version.

   Also libntlm was ripped but rewrote, due to fixed and useless interface.
   Copyright and licensing information is in ntlm.c file.

   NTLM Interface, just two functions: 

   void BuildAuthRequest(tSmbNtlmAuthRequest *request, long flags, char *host, char *domain);
   if flags is 0 minimun security level is selected, otherwise new value superseeds.
   host and domain are optional, they may be NULLed.

   void buildAuthResponse(tSmbNtlmAuthChallenge *challenge, tSmbNtlmAuthResponse *response, long flags, char *user, char *password, char *domain, char *host);
   Given a challenge, generates a response for that user/passwd/host/domain.
   flags, host, and domain superseeds given by server. Leave 0 and NULL for server authentication


   This is an usage sample: 


		...
		//beware of fixed sized buffer, asserts may fail, don't use long strings :)
		//Yes, I Know, year 2k6 and still with this shit..
		unsigned char buf[4096];
		unsigned char buf2[4096];

		//send auth request: let the server send it's own hostname and domainname
		buildAuthRequest((tSmbNtlmAuthRequest*)buf2,0,NULL,NULL);
		to64frombits(buf, buf2, SmbLength((tSmbNtlmAuthResponse*)buf2));
		send_to_server(buf);

		//receive challenge
		receive_from_server(buf);

		//build response with hostname and domainname from server
		buildAuthResponse((tSmbNtlmAuthChallenge*)buf,(tSmbNtlmAuthResponse*)buf2,0,"username","password",NULL,NULL);
		to64frombits(buf, buf2, SmbLength((tSmbNtlmAuthResponse*)buf2));
		send_to_server(buf);

		//get reply and Check if ok
		... 


   included bonus!!:
   Base64 code
   int  from64tobits(char *out, const char *in);
   void to64frombits(unsigned char *out, const unsigned char *in, int inlen);




   You don't need to read the rest of the file. 
*/


/* 
 * These structures are byte-order dependant, and should not
 * be manipulated except by the use of the routines provided
 */

#pragma pack(1)
#ifdef WIN32
#else
#pragma align 1
#endif


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>



#ifdef WIN32
#else
 #include <unistd.h>
#define _strdup strdup
#endif


//
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long int uint64;
typedef unsigned char  uint8;



#define SERVERNAMEITEM	0x01
#define DOMAINNAMEITEM	0x02
#define DNSITEM			0x03
#define FQDNITEM		0x04

typedef struct
{
uint16  ItemType;
uint16  ItemLength;
uint8	buffer[256];
}tSmbStrItem; //Challenge (NTLM type 2)

typedef struct
{
    uint8  HMAC[16];
    uint32 Header;
    uint32 Reserved;
    uint8  Filetime[8];
    uint8  ClientChallenge[8];
    uint32 Unknown;
    tSmbStrItem DomainName;
} NtlmV2Packet;

typedef struct
{
uint16  len;
uint16  maxlen;
uint32  offset;
}tSmbStrHeader;

typedef struct
{
char          ident[8];
uint32        msgType;
uint32        flags;
tSmbStrHeader    host;
tSmbStrHeader    domain;
uint8         buffer[1024];
uint32        bufIndex;
}tSmbNtlmAuthRequest;

typedef struct
{
char          ident[8];
uint32        msgType;
tSmbStrHeader    uDomain;
uint32        flags;
uint8         challengeData[8];
uint8         reserved[8];
tSmbStrHeader    emptyString;
uint8         buffer[16384];
//uint32        bufIndex;
}tSmbNtlmAuthChallenge;



typedef struct
{
char          ident[8];
uint32        msgType;
tSmbStrHeader    lmResponse;
tSmbStrHeader    ntResponse;
tSmbStrHeader    uDomain;
tSmbStrHeader    uUser;
tSmbStrHeader    uWks;
tSmbStrHeader    sessionKey;
uint32        flags;
uint8         buffer[1024];
uint32        bufIndex;
}tSmbNtlmAuthResponse;


/* reversing interface */
/* ntlm functions */
void BuildAuthRequest(tSmbNtlmAuthRequest *request, long flags, char *host, char *domain);
// if flags is 0 minimun security level is selected, otherwise new value superseeds.
// host and domain are optional, they may be NULLed.

void BuildAuthChallenge(tSmbNtlmAuthChallenge *pChallenge, char *DomainName, char *ServerName,  char *DNS, char *FQDN, uint8 flags, uint8 *SpoofedChallenge);
//All parameters are optional. IF DomainName is not set, the default name will be "SMBRELAY3"
//if flags is 0 minimun security level is selected, otherwise new value superseeds.
//if SpoofedChallenge is not set, the default Challenge value is 0x1122334455667788
//That Challenge value allows bruteforce attacks with winrtgen built LM/ntlm tables

void buildAuthResponse(tSmbNtlmAuthChallenge *challenge, tSmbNtlmAuthResponse *response, long flags, char *user, char *password, char *domainname, char *host, tSmbNtlmAuthResponse* OptionalNtlmPacket3);
//Given a challenge, generates a response for that user/passwd/host/domain.
//flags, host, and domain superseeds given by server. Leave 0 and NULL for server authentication
//Additional parameter OptionalNtlmPacket3 have been added to support relay attacks by changing challenge data




//int NtlmChallengeSize(tSmbNtlmAuthChallenge *Challenge);
//#define NtlmChallengeSize( (tSmbNtlmAuthChallenge *)Challenge) ( sizeof(tSmbNtlmAuthChallenge) - sizeof(Challenge->buffer) +Challenge->uDomain.maxlen + Challenge->emptyString.maxlen)
#define NtlmChallengeSize(Challenge) ( sizeof(tSmbNtlmAuthChallenge) - sizeof(Challenge->buffer) +Challenge->uDomain.maxlen + Challenge->emptyString.maxlen)
//#define NtlmAuth

/*int NtlmChallengeSize(tSmbNtlmAuthChallenge *Challenge){
	return( sizeof(tSmbNtlmAuthChallenge) - sizeof(Challenge->buffer) +Challenge->uDomain.maxlen + Challenge->emptyString.maxlen);
}
*/

__inline void SetChallengeKey(tSmbNtlmAuthChallenge *challenge, uint8 *SpoofedChallenge);

/* Base64 code*/
int from64tobits(char *out, const char *in);
void to64frombits(unsigned char *out, const unsigned char *in, int inlen);

// info functions
void dumpAuthRequest(FILE *fp, tSmbNtlmAuthRequest *request);
void dumpAuthChallenge(FILE *fp, tSmbNtlmAuthChallenge *challenge);
void dumpAuthResponse(FILE *fp, tSmbNtlmAuthResponse *response);

#define SmbLength(ptr) (((ptr)->buffer - (uint8*)(ptr)) + (ptr)->bufIndex)

void GetNTLMPacketInfo(tSmbNtlmAuthResponse* NtlmAuthResponse, char* UserName, char *DomainName, char *WorkstationName, int verbose);

void chartoWide(char *destination, char *source, int len);
void Widetochar(char *destination, char *source, int len);

#endif

