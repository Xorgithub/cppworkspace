//
// Common routines for resolving addresses and hostnames
//
// Files:
//      resolve.h       - Header file for common routines
//
// Description:
//      This file contains common name resolution and name printing
//      routines and is used by many of the samples on this CD.
//
// Compile:
//      See iocpserver.cpp
//
// Usage:
//      See iocpserver.cpp
//
//
#ifndef _RESOLVE_H_
#define _RESOLVE_H_

#ifdef _cplusplus
extern "C" {
#endif

int              PrintAddress(SOCKADDR *sa, int salen);
int              FormatAddress(SOCKADDR *sa, int salen, char *addrbuf, int addrbuflen);
struct addrinfo *ResolveAddress(char *addr, char *port, int af, int type, int proto);

#ifdef _cplusplus
}
#endif

#endif
