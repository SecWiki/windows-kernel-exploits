#ifndef _HTTPRELAY_H_
#define _HTTPRELAY_H_

#include "smbrelay.h"

//HTTP Functions
int HandleIncommingHTTPRequest(RELAY *relay, char *destinationhostname, int destinationport);
int ReadRequest(RELAY *relay, char *request, int requestsize);
char **ParseHeaders(char *lpBuffer, unsigned int *nheaders);
char *GetHeaderValue(char **header, int nheaders, char *Header) ;

#endif


