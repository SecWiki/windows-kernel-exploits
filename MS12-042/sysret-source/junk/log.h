#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define __DEBUG__
#pragma once

typedef enum { L_DEBUG, L_INFO, L_WARN, L_ERROR } LEVEL, *PLEVEL;

typedef struct _ERRORINFO {
	DWORD	dwErrorNum;
	CHAR	ErrorMsg[256];
	CHAR	*CompletErrorMsg;
} ERRORINFO, *PERRORINFO; 

VOID 
DEBUG_PRINTF(
	IN LEVEL Level,
	IN PCHAR Format, 
	IN ...
	);

VOID 
REPORT_ERROR_EX(
	IN PCHAR Function,
	IN DWORD dwErrorNumber,
	OUT PERRORINFO ErrorInfo
	);

VOID 
REPORT_ERROR( 
	IN PCHAR Function,
	OUT PERRORINFO ErrorInfo
	);