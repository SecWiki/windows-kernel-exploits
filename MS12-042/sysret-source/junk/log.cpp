#include "log.h"

VOID 
REPORT_ERROR( 
	IN PCHAR Function,
	OUT PERRORINFO ErrorInfo
	)
{
	ErrorInfo->dwErrorNum = GetLastError();
    REPORT_ERROR_EX( Function,
		             GetLastError(),
				     ErrorInfo);
}

VOID 
REPORT_ERROR_EX(
	IN PCHAR Function,
	IN DWORD dwErrorNumber,
	OUT PERRORINFO ErrorInfo
	)
{
	BOOL bErrorHandle;
	HMODULE hErrorDllHandle;

	ErrorInfo->dwErrorNum = dwErrorNumber;
	bErrorHandle = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL,
									ErrorInfo->dwErrorNum,
									MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
									ErrorInfo->ErrorMsg,
									256,
									NULL);
	if ( bErrorHandle == FALSE )
	{
		// load library and check the error again for network related errors
		hErrorDllHandle = LoadLibraryEx( "netmsg.dll",
										 NULL,
										 DONT_RESOLVE_DLL_REFERENCES);
		if ( hErrorDllHandle != NULL )
		{
			bErrorHandle = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										  NULL,
										  ErrorInfo->dwErrorNum,
										  MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
										  ErrorInfo->ErrorMsg,
										  256,
										  NULL);
		}
	}
	if ( bErrorHandle == FALSE )
	{
		strncpy(ErrorInfo->ErrorMsg,"Unknown Error", 256);
	}

	// allocate memory for completed error message 
	ErrorInfo->CompletErrorMsg = (CHAR *) GlobalAlloc( GMEM_FIXED, 512 );
	sprintf( ErrorInfo->CompletErrorMsg ,"%s failed with error %d (%s)\n", Function, ErrorInfo->dwErrorNum, ErrorInfo->ErrorMsg );
	//snprintf instead ?
	DEBUG_PRINTF(L_ERROR,"%s",ErrorInfo->CompletErrorMsg);
}


VOID 
DEBUG_PRINTF(
	IN LEVEL Level,
	IN PCHAR Format, 
	IN ...
	)
{
    CHAR Buffer[1024] = {0};
    va_list Args;

	va_start(Args, Format);
		vsnprintf_s(Buffer, sizeof Buffer, _TRUNCATE, Format, Args);
	va_end(Args);

#ifdef __DEBUG__
	switch (Level) {
        case L_DEBUG: fprintf(stdout, "[?] %s\n", Buffer); break;
        case L_INFO:  fprintf(stdout, "[+] %s\n", Buffer); break;
        case L_WARN:  fprintf(stderr, "[*] %s\n", Buffer); break;
        case L_ERROR: fprintf(stderr, "[!] %s\n\a", Buffer); break;
    }
	fflush(stdout);
	fflush(stderr);
	return;
#endif

}