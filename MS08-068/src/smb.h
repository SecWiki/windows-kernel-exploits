
/* $Id$ 
   SMBRELAY - Small SMB Library packet generation.

   Author: Andres Tarasco - (atarasco at gmail.com )
   URL:		http://www.tarasco.org

   This is not a complete library, but its functional for sending special crafted messages needed to perform attacks with smbrelay3
   Lots of improvements need to be done but at this time works for me. 
   Currently the following messages are supported:	

   a) SMB NTLM Authentication  (NegotiateProtocolRequest and SessionSetupAndX)
   b) Connecting To resources (like c$, ipc$, admin$, pipes, ...) (TreeConnectAndX )
   c) Opening remote Files for Writting (by default) and reading (NtCreateAndX)
   d) Writting remote files (WriteAndX)
   e) Opening Remote Service Control Manager (Connecting, creating a service, open a service and starting a service)
   Maybe more.. take a look to the code ;)

   Data structures have been added by reversing Raw dumps with the help of ethereal disectors. 
   NOTE: Services manipulation and RPCBinds are hardcoded.
   */
#ifndef _SMB_H
#define _SMB_H
#define _CRT_SECURE_NO_DEPRECATE
#include "ntlm.h"
#define SREV(x) ((((x)&0xFF)<<8) | (((x)>>8)&0xFF))
//#define SREV htonl
#define SMBPACKETLEN(x) ((x->SmbMessageLength) +4 )
//#define GetDceRpcPacketFromBuffer(buf) (DceRpcBind*) (char*) (buf + SREV(((smheader*)buf)->SmbMessageLength) +4 - ( (OpenScManagerW*)((smheader*)buf)->buffer)->TotalDataCount)
#define GetDceRpcPacketFromBuffer(buf) ((DceRpcBind*)(char*) (buf + SREV(((smheader*)buf)->SmbMessageLength) +4 - ( (SMB_COM_TRANSACTION_STRUCT*)((smheader*)buf)->buffer)->TotalDataCount))


 
#define SMB_COM_CREATE_DIRECTORY    0x00
#define SMB_COM_DELETE_DIRECTORY    0x01
#define SMB_COM_OPEN                0x02
#define SMB_COM_CREATE              0x03
#define SMB_COM_CLOSE               0x04
#define SMB_COM_FLUSH               0x05
#define SMB_COM_DELETE              0x06
#define SMB_COM_RENAME              0x07
#define SMB_COM_QUERY_INFORMATION   0x08
#define SMB_COM_SET_INFORMATION     0x09
#define SMB_COM_READ                0x0A
#define SMB_COM_WRITE               0x0B
#define SMB_COM_LOCK_BYTE_RANGE     0x0C
#define SMB_COM_UNLOCK_BYTE_RANGE   0x0D
#define SMB_COM_CREATE_TEMPORARY    0x0E
#define SMB_COM_CREATE_NEW          0x0F
#define SMB_COM_CHECK_DIRECTORY     0x10
#define SMB_COM_PROCESS_EXIT        0x11
#define SMB_COM_SEEK                0x12
#define SMB_COM_LOCK_AND_READ       0x13
#define SMB_COM_WRITE_AND_UNLOCK    0x14
#define SMB_COM_READ_RAW            0x1A
#define SMB_COM_READ_MPX            0x1B
#define SMB_COM_READ_MPX_SECONDARY  0x1C
#define SMB_COM_WRITE_RAW           0x1D
#define SMB_COM_WRITE_MPX           0x1E
#define SMB_COM_WRITE_COMPLETE      0x20
#define SMB_COM_SET_INFORMATION2    0x22
#define SMB_COM_QUERY_INFORMATION2  0x23
#define SMB_COM_LOCKING_ANDX        0x24
#define SMB_COM_TRANSACTION         0x25
#define SMB_COM_TRANSACTION_SECONDARY 0x26

#define SMB_COM_IOCTL               0x27
#define SMB_COM_IOCTL_SECONDARY     0x28
#define SMB_COM_COPY                0x29
#define SMB_COM_MOVE                0x2A
#define SMB_COM_ECHO                0x2B
#define SMB_COM_WRITE_AND_CLOSE     0x2C
#define SMB_COM_OPEN_ANDX           0x2D
#define SMB_COM_READ_ANDX           0x2E
#define SMB_COM_WRITE_ANDX          0x2F
#define SMB_COM_CLOSE_AND_TREE_DISC 0x31
#define SMB_COM_TRANSACTION2        0x32
#define SMB_COM_TRANSACTION2_SECONDARY 0x33
#define SMB_COM_FIND_CLOSE2         0x34
#define SMB_COM_FIND_NOTIFY_CLOSE   0x35
#define SMB_COM_TREE_CONNECT        0x70
#define SMB_COM_TREE_DISCONNECT     0x71
#define SMB_COM_NEGOTIATE           0x72
#define SMB_COM_SESSION_SETUP_ANDX  0x73
#define SMB_COM_LOGOFF_ANDX         0x74
#define SMB_COM_TREE_CONNECT_ANDX   0x75

#define SMB_COM_QUERY_INFORMATION_DISK 0x80
#define SMB_COM_SEARCH              0x81
#define SMB_COM_FIND                0x82
#define SMB_COM_FIND_UNIQUE         0x83
#define SMB_COM_NT_TRANSACT         0xA0
#define SMB_COM_NT_TRANSACT_SECONDARY 0xA1
#define SMB_COM_NT_CREATE_ANDX      0xA2
#define SMB_COM_NT_CANCEL           0xA4
#define SMB_COM_OPEN_PRINT_FILE     0xC0
#define SMB_COM_WRITE_PRINT_FILE    0xC1
#define SMB_COM_CLOSE_PRINT_FILE    0xC2
#define SMB_COM_GET_PRINT_QUEUE     0xC3

#define SMB_COM_READ_BULK               0xD8
#define SMB_COM_WRITE_BULK              0xD9
#define SMB_COM_WRITE_BULK_DATA         0xDA


//--------------------
#define SMBCLOSE			0x04
//#define SERVICEOPERATION	0x25
#define READANDX			0x2e
#define WRITEANDX			0x2f 
#define FINDFIRST2			0x32
#define NEGOTIATEPROTOCOLREQUEST 0x72
#define SESSIONSETUPANDX	0x73
#define SESSIONLOGOFF       0x74
#define TREECONNETANDX		0x75
#define NTCREATEANDX		0xa2


//SERVICEOPERATION WideChar subcommands
#define STOPSERVICE         1
#define CHANGESERVICECONFIG 11
#define CREATESERVICE		12
#define OPENSCMANAGER		15
#define OPENSERVICEW	    16
#define STARTSERVICE		19
#define OPENSERVICEA		28



//SESSIONSETUPANDX Subcommands
#define CONTINUERESPONSE 1
#define ERRORRESPONSE	 2

//WRITEANDX subcommands
#define RPCBIND				0x0b

#define RPC_RESPONSE (uint8)0x02

#define STATUS_SHARING_VIOLATION 0xc0000043
#define STATUS_BAD_NETWORK_NAME 0xcc


#ifndef WCHAR
#ifdef WIN32
typedef wchar_t WCHAR;
#else
typedef unsigned short WCHAR;
#endif
#endif

/*
typedef struct {
  unsigned long Data1;
  unsigned short Data2;
  unsigned short Data3;
  byte Data4[8];
} guid;
*/
typedef struct {
	uint16 SmbMessageType; //0x00
	uint16 SmbMessageLength; 
	uint8 ProtocolHeader[4]; //"\xffSMB"
	uint8 SmbCommand; 
	uint32 NtStatus; //0x00000000
	uint8 flags; //0x18 - pathnames not case sensitive & pathnames canonicalized
	uint16 flags2;  //0xC001 (Unicode & Nt error types & longfilename support
	uint16 ProcessIDHigh; //0x00
	uint8 signature[8]; //0x00000000000
	uint16 reserved; //0x0000
	uint16 TreeId; 
	uint16 ProccessID; //0xfeff
	uint16 UserID; 
	uint16 multipleID;  //Incremental 64bytes en cada request.
	char buffer[16384]; // Custom SmbCommand data
} smheader;


typedef struct {
	uint8 VersionMayor;
	uint8 VersionMinor;
	uint8 PacketType;
	uint8 PacketFlags;
	uint32 DataRepresentation;
	uint16 FragmentLength;
	uint16 AuthLength;
	uint32 CallID;
	uint32 AllocHint;
	uint16 ContextID;
	uint16 OpNum;
} DceRpcRequest;


typedef struct {
	uint32 ReferentID;
	uint32 MaxCount;
	uint32 Offset;
	uint32 ActualCount;
	//uint16 padding; 
    uint16 lpFileNameW[256];
	uint32 Database;
	uint32 AccessMask;
} MSCStruct; //Used when Calling OpenSCManagerW() againt SMB


typedef struct {	
    uint8 uuid[16];
	uint16 MayorVersion;
	uint16 MinorVersion;
    
} DceInterface;

typedef struct {
	uint16 ContextID;
	uint8 NumberOfTransItems;
	uint8 padding;
	DceInterface AbstractSyntaxis;
	DceInterface TransferSyntaxis;
} CtxItem;

typedef struct {
	uint8 VersionMayor;
	uint8 VersionMinor;
	uint8 PacketType;
	uint8 PacketFlags;
	uint32 DataRepresentation;
	uint16 FragmentLength;
	uint16 AuthLength;
	uint32 CallID;
	uint16 MaxXmitFragment;
	uint16 MaxRecvFragment;
	uint32 AssocGroup;
	uint8 NumberOfCtx;
	uint8 padding[3];
} DceRpcBind;

typedef struct {
	uint8 WordCount;
	uint16 TotalParameterCount;
	uint16 TotalDataCount;
	uint16 MaxParameterCount;
	uint16 MaxDataCount;
	uint8 MaxSetupCount;
	uint8 reserved;
	uint16 flags;
	uint32 timeout; //0x00000000
	uint16 reserved2;
	uint16 ParameterCount;
	uint16 ParameterOffset;
	uint16 DataCount;
	uint16 DataOffset;
	uint8 SetupCount; 
	uint8 reserved3;
	uint16 Function; //0x0026 for openscManager - Find_First2
	uint16 FID;
	uint16 ByteCount; 
	uint8 padding;
	uint8 TransactionName[14];
	uint16 padding2;
	//DCERPC struct
	//Microosft ServiceControl
} SMB_COM_TRANSACTION_STRUCT;//OpenScManagerW;





typedef struct {
    uint8 ContextHandle[20];
    uint32 ServiceType; 
    uint32 StartType; //0x02
    uint32 unknown1; //0xFFFFFFFF
    uint16 unknown2; //0x00
    uint16 unknown3; //0x02
    uint32 MaxLenLow;
    uint32 MaxLenHigh;
    uint32 Lenght;
    uint8 buffer[1024];
    uint8 blank[32];


    /*
//    uint16 unknown6; //0x0310;
    uint32 unknown7; //0x00000000
    uint32 unknown8; //0x00000000
    uint16 unknown9; //0x04
    uint16 unknown10; //0x02

    uint32 unknown11; //0x02
    uint32 unknown12; //0x00000000

    uint32 unknown13; //0x02
    uint32 unknown14; //0x00000000
    //uint8 blank[12];
    uint32 unknown15;
*/
/*
0000031B  00 00 00 00 f2 b7 95 63  63 eb f1 49 8d 4d 70 22 .......c c..I.Mp"
0000032B  67 87 b1 b5 
                      10 00 00 00  02 00 00 00 ff ff ff ff g....... ........
0000033B  00 00 02 00 
                      09 00 00 00  00 00 00 00 09 00 00 00 ........ ........
0000034B  73 00 6d 00 72 00 73 00  2e 00 65 00 78 00 65 00 s.m.r.s. ..e.x.e.
0000035B  00 00 
                10 03 00 00 00 00  00 00 00 00 04 00 02 00 ........ ........

0000036B  02 00 00 00 00 00 00 00  02 00 00 00 00 00 00 00 ........ ........
0000037B  00 00 00 00 00 00 00 00  00 00 00 00             ........ ....
*/
} ChangeServiceConfigWStruct;



typedef struct {
	uint8 WordCount; //0x14
	uint8 AndXCommand; //0xff no further command
	uint8 reserved2; //0x00
	uint16 AndXOffset; 
	uint16 FID;
	uint32 Offset;
	uint32 reserved;
	uint16 WriteMode;
	uint16 Remaining;
	uint16 DataLengthHigh;
	uint16 DataLengthLow;
//	uint16 DataLengthRow;
	uint16 DataOffset;
	uint32 HighOffset;
	uint16 ByteCount;
	uint8 Padding;	
} WriteAndX;

typedef struct {
	uint8 WordCount; //0x14
	uint8 AndXCommand; //0xff no further command
	uint8 reserved2; //0x00
	uint16 AndXOffset; 
	uint16 FID;
	uint32 offset;
	uint16 MaxCountLow;
	uint16 MinCount;
	uint32 reserved; //0xffffffff
	uint16 remaining;
	uint32 HighOffset;
	uint16 ByteCount;
} ReadAndX;


typedef struct {
	uint8 WordCount; //Number of parameters in this struct
	uint8 AndXCommand; //0xff no further command
	uint8 reserved2; //0x00
	uint16 AndXOffset; 

	uint16 MaxBuffer; 
	uint16 MaxMpxCount; 
	uint16 VcNumber; //0x0000
	uint32 SessionKey; //0x00000000
	uint16 SecurityBloblength; 
	uint32 reserved3; //0x00000000
	uint32 capabilities; //0x200000D4
	uint16 ByteCount; 
} SessionSetupAndX; 

typedef struct {
	uint8 WordCount; //Number of parameters in this struct
	uint8 AndXCommand; //0xff no further command
	uint8 reserved2; //0x00
	uint16 AndXOffset; 
	uint16 Action;
	uint16 SecurityBloblength; 
	uint16 ByteCount;
//	uint8 padding;
} SessionSetupAndXResponse; 

/*
typedef struct {
	uint8 BufferFormat;
	char  Name[256];
} DIALECT;

*/
typedef struct {
	uint8 BufferFormat;
	char  *Name;
} DIALECT;


typedef struct {
	uint8 WordCount;
	uint16 ByteCount;
//	DIALECT *Dialects;
    char *Dialects;
} NegotiateProtocolRequest;

typedef struct {
	uint8 WordCount; //Number of parameters in this struct
    uint16 DialecIndex;
    uint8 SecurityMode;
    uint16 MaxMxpCount;
    uint16 MaxVcs;
    uint32 MaxBufferSize;
    uint32 MaxRawBuffer;
    uint32 SessionKey;
    uint32 Capabilities;
    //uint64 ServerTime;
    uint8  ServerTime[8];
    uint16 ServerTimeZone;
    uint8 KeyLength;
    uint16 ByteCount;
    uint8 ServerGuid[16];
} NegotiateProtocolResponse; 


typedef struct {
	uint8 WordCount; 
	uint8 AndXCommand; //0xff no further command
	uint8 reserved2; //0x00
	uint16 AndXOffset; 
	uint16 flags;
	uint16 PasswordLen;	//Set to 0x01
	uint16 ByteCount; 
	uint8 Password; //Set to 0x00
    //resource
    //service ????\0
} TreeConnectAndX; 

typedef struct {
	uint8 WordCount; 
	uint16 ByteCount; 
} TreeConnectAndXResponse;


typedef struct {
	uint8 WordCount;
	uint16 TotalParameterCount;
	uint16 TotalDataCount;
	uint16 MaxParameterCount;
	uint16 MaxDataCount;
	uint8 MaxSetupCount;
	uint8 reserved;
	uint16 flags;
	uint32 timeout; //0x00000000
	uint16 reserved2;
	uint16 ParameterCount;
	uint16 ParameterOffset;
	uint16 DataCount;
	uint16 DataOffset;
	uint8 SetupCount; 
	uint8 reserved3;
	uint16 SubCommand; //0x0001 - Find_First2
	uint16 ByteCount; 

    uint8 padding[3];
    uint16 SearchAttributes;
    uint16 SearchCount;
    uint16 Searchflags;
    uint16 LevelOfInterest;
    uint32 StorageType;
    uint8 *SearchPattern;//[100];


	//uint8 Find_First2[18]; //lo metemos hardcodeado.
} FIND_FIRST2;

/*
	uint8 padding[3];
    uint16 SearchAttributes;
    uint16 SearchCount;
    uint16 Searchflags;
    uint16 LevelOfInterest;
    uint32 StorageType;
    uint8 *SearchPattern;//[100];
*/




typedef struct {
	uint8 WordCount; //0x24
	uint16 FID; //0xff no further command
	uint32 LastWrite;
	uint16 ByteCount;
} CLOSE;

typedef struct {
	uint8 WordCount; //0x24
	uint8 AndXCommand; //0xff no further command
	uint8 reserved1; //0x00
	uint16 AndXOffset; 
	uint8 reserved2;
	uint16 FilenameLen; 
	uint32 CreationFlags; //0x16
	uint32 RootFID;
	uint32 AccessMask;//0x30196
	uint32 AllocationSize; //Size del fichero
	uint32 AllocationSizeHigh;
	uint32 FileAttributes;
	uint32 ShareAccess;
	uint32 Disposition; //0x05000000
	uint32 CreateOptions; 
	uint32 Impersonation;
	uint8 SecurityFlags;
	uint16 ByteCount;
	uint8  padding;
} NTCreateAndX;


typedef struct  {
    uint32 MaxLenLow;
    uint32 MaxLenHigh;
    uint32 Lenght;
    uint8  data[1024];
} DataBlock;

typedef struct {
    uint8 ContextHandle[20];
    DataBlock ServiceName;
    uint32 unknown; // ?? 0002
    DataBlock ServiceDescription;
    uint32 unknown1; //0x000f01ff
    uint32 ServiceType;
    uint32 StartType;
    uint32 unknown2; //0x0000000
    DataBlock path;
    uint8  blank[28];    
} CreateServiceWStruct;

typedef struct {
    uint8 ContextHandle[20];
    /*
    uint32 MaxLenLow;
    uint32 MaxLenHigh;
    uint32 Lenght;

    uint8 buffer[512]; //service Name
    */
    DataBlock ServiceName;
    //uint16 unknown;
    uint32 flags; //22 00 
} OpenServiceAStruct;


#ifndef WIN32
    #define SERVICE_QUERY_CONFIG 1
    #define SERVICE_CHANGE_CONFIG 2
    #define SERVICE_QUERY_STATUS 4
    #define SERVICE_ENUMERATE_DEPENDENTS 8
    #define SERVICE_START 16
    #define SERVICE_STOP 32
    #define SERVICE_PAUSE_CONTINUE 64
    #define SERVICE_INTERROGATE 128
    #define SERVICE_USER_DEFINED_CONTROL 256
    #define SERVICE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SERVICE_QUERY_CONFIG|SERVICE_CHANGE_CONFIG|SERVICE_QUERY_STATUS|SERVICE_ENUMERATE_DEPENDENTS|SERVICE_START|SERVICE_STOP|SERVICE_PAUSE_CONTINUE|SERVICE_INTERROGATE|SERVICE_USER_DEFINED_CONTROL)

    #define SC_MANAGER_ALL_ACCESS	0xf003f
    #define SC_MANAGER_CONNECT	1
    #define SC_MANAGER_CREATE_SERVICE	2
    #define SC_MANAGER_ENUMERATE_SERVICE	4
    #define SC_MANAGER_LOCK	8
    #define SC_MANAGER_QUERY_LOCK_STATUS	16
    #define SC_MANAGER_MODIFY_BOOT_CONFIG	32

// Service TYPE
    #define SERVICE_WIN32_OWN_PROCESS      0x00000010
    #define SERVICE_WIN32_SHARE_PROCESS    0x00000020

//start TYPE
    #define SERVICE_BOOT_START             0x00000000
    #define SERVICE_SYSTEM_START           0x00000001
    #define SERVICE_AUTO_START             0x00000002
    #define SERVICE_DEMAND_START           0x00000003
    #define SERVICE_DISABLED               0x00000004
#endif

//Build SMB Packet
smheader *BuildSmbPacket(smheader *PreviousSmbMessage,uint8 SmbCommand,uint8 SubCommand, void *data, int DataSize);

//Stub Generation for OpenServices Functions..
int CreateServiceWStub(char *destionation,char *ContextHandle,char *ServiceName, char *ServiceDescription,char *path, int StartType, int ServiceType);
int ChangeServiceConfigWStub(char *destionation,char *ContextHandle,char *path, int StartType, int ServiceType);
int OpenServiceWStub(char *destionation,char *ContextHandle,char *servicename,  int flags);
int OpenScManagerWStub(char *destionation, char *data, int flags);
//Tree Connect
int BuildTreeConnectAndXStub(char *destination,char *password, char *resource, char *service);
//SMB Negotiation
char *AddDialect(char *data, char *name, uint8 type, int *PacketSize);

#ifdef WIN32
CtxItem *AddBinddingInformation(CtxItem *CurrentCtx, char *Abstract, char *Transfer, int MayorVersion, int MinorVersion,int *PacketSize);
#endif
void SetEnviroment(uint16 sessionUserID, uint16 sessionTreeID, uint16 sessionFID);
#endif


