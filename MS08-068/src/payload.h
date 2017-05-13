#ifndef _PAYLOAD_
#define _PAYLOAD_
#include "smbrelay.h"
#include "smb.h"
#include "ntlm.h"

void CleanLine(int verbose);
smheader *BuildSmbPacket1(void);
smheader *GetSmbPacket2(RELAY *relay,smheader* Packet1);
smheader *GetSmbPacket3(smheader* SmbPacket2,char *lpUserName, char *lpPassword,  char *domainname, char *host, tSmbNtlmAuthResponse* OptionalNtlmPacket3);
smheader *GetSmbPacket3Alt(smheader* SmbPacket2,char *lpUserName, char *lpPassword,  char *domainname, char *host, tSmbNtlmAuthResponse* OptionalNtlmPacket3);

int WriteRemoteFile(RELAY relay, smheader *buffer, char *lpFileName);
char *GenerateFTPTransfer(char *buffer,char *host, int port, char *username, char *password, char *downloadfile,char *optionalparameter);
int AttackWeakServices(RELAY relay, char *buf,char *path, uint16 FID, char *ServicePath);


#endif

