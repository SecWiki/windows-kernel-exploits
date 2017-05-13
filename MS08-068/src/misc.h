/*
 Misc data manipulation functions for Smbrelay
 Andres Tarasco
*/
#ifndef _MISC_FUNCTIONS_H_
#define _MISC_FUNCTIONS_H_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>


#pragma pack(1)
#ifdef WIN32
#include <io.h>
#include <windows.h>
#else
 #pragma align 1
 #include <unistd.h>
 #include <fcntl.h>
 #include <sys/socket.h>
 #include <sys/ioctl.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <pthread.h>  //pthread
 #include <ctype.h> //toupper 
 #include <time.h>
 #include <sys/timeb.h>
#define SOCKET int
#define  _strnicmp strncasecmp 
#define closesocket close
#define Sleep(a) sleep(a/1000)
#define INVALID_SOCKET -1
#endif
//#include "smbrelay.h"
#include "ntlm.h"

typedef struct
{
	SOCKET source;
	struct sockaddr_in sourceaddr;

	SOCKET destination;
	struct sockaddr_in destinationaddr;

	int dstProtocol;
	char hostname[256];
} RELAY;

#define CONNECT_TIMEOUT  10
#define SMBWAITTIMEOUT 5
#define DBG_DUMP_ROWS	16


void DumpMem(void* string, int length) ;
char *ReadFileToSend(int *BackdoorFileSize,char*lpBackdoorFile);

int ConnectToRemoteHost(RELAY *relay,char *hostname, int port);
//int SendBytesAndWaitForResponse(RELAY *relay,char *source, int nBytes, char *destination, int MaxReadSize,int timeout);
int SendBytesAndWaitForResponse(SOCKET destination,char *source, int nBytes, char *destinationBuffer, int MaxReadSize,int timeout);
void WriteDataToReportFile(char *lpLogFileFilename, tSmbNtlmAuthResponse* NtlmAuthResponse, char *SourceIpAddress,unsigned char *challenge);
void CleanLine(int verbose);
void usage(void);
void Banner(void);

#endif

