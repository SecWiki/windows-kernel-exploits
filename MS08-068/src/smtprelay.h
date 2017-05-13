#ifndef _SMTP_RELAY_
#define _SMTP_RELAY_

#include "smbrelay.h"

int HandleIncommingSMTPRequest(RELAY *relay, char *destinationhostname,int destinationport);

#endif

