#ifndef __GAP_UNBASE_H__
#define __GAP_UNBASE_H__

#if 0

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>

#include <sys/select.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned long long  U_LLONG;

#endif

///*
//#ifndef BOOL
//#define BOOL int
//#endif

#ifdef __cplusplus
#  define DECL_BEGIN extern "C" {
#  define DECL_END   }
#else
#  define DECL_BEGIN
#  define DECL_END
#endif

#define BEGIN do {
#define END   } while (0)
//*/

//static inline void prefetch(const void *x) {;}

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#endif
