#ifndef _SBMRELAY_H_
#define _SMBRELAY_H_

#define _CRT_SECURE_NO_DEPRECATE 
#include <stdio.h>
#ifdef WIN32
#include <windows.h> 
#define socklen_t int
#endif
#include "misc.h"
#include "smb.h"
#include "ntlm.h"


#define GetNTLMPacketFromSmbPacket(a) ((char*)a+0x2b+4)
#define GetNTLMPacket3FromSmbPacket(a) ((char*)a+ sizeof(smheader) -sizeof(((smheader*)a)->buffer) +sizeof(SessionSetupAndX))

#define SmbPacketLen(a) (SREV(a->SmbMessageLength)+4)
//#define PrintErrorMessage(a) printf("%s",a); return(0);
//#define PrintErrorMessage(a) printf(a); return(0);

#define ATTACK_NONE		0x00
#define REPLAY_HTTP		0x01
#define REPLAY_SMB		0x02
#define REPLAY_POP3		0x03
#define REPLAY_IMAP		0x04
#define REPLAY_SMTP		0x05
#define REPLAY_DNS		0x06
#define REPLAY_TELNET	0x07
#define REPLAY_MSSQL	0x08

#define PSEXEC			0x10

#define debug (verbose==2)



//Functions
//int InitSmbHandshake(RELAY *relay,char *Buffer, int BufferSize);
int HandleIncommingSmbRequest(RELAY *relay, char *destinationhostname, int destinationport);

int ReplayAttackAgainst(int Protocol,char *hostname, int port);
int StablishNTLMSession(RELAY relay,char *host, char *lpUserName, char *lpPassword);
int ExecuteCode( RELAY relay);




#endif


