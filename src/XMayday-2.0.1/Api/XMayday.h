/*
** XMayday.h for XMayday in Api/
** XMayday.h
**
** Copyright (C) 1996-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Sometime during     1996 One of the authors
** Last update Sun Jul 13 15:59:22 1997 One of the authors
*/


#ifndef _XMayday_h_
#define _XMayday_h_

#include <sys/types.h>
#include <stddef.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

/*--- Prototypes ---*/
#ifdef __cplusplus
# define ___PROTO(x) x
#else
# ifdef __STDC__
#  define ___PROTO(x) x
# else
#  define ___PROTO(x) ()
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef QUIET_MODE

#include <varargs.h>

#ifdef STATIC_DEF
FILE *Xc_log_file = stderr;
void Xc_LogPrintf(format, va_alist)
char *format;
va_dcl
{
 va_list ap;
  
 va_start(ap);
 vfprintf(Xc_log_file, format, ap);
 va_end(ap);
}

#if !defined(___freebsd) && !defined(___NetBSD)
extern char *sys_errlist[];
extern int sys_nerr;
#endif

char *Xc_strerror(errorcode)
int errorcode;
{
 if((errorcode > 0) && (errorcode < sys_nerr))
  return (char *)sys_errlist[errorcode];
 return("Unknow error");
}

#else
extern FILE *Xc_log_file;
extern void Xc_LogPrintf();
extern char *Xc_strerror();
#endif

#define Xc_exit(code) \
do { \
      Xc_LogPrintf("[%s %d] - Exiting with code %d\n", __FILE__, (int)__LINE__, (code)); \
      exit(code); \
} while(0)

#define Xc_TRACE(args) \
do { \
      Xc_LogPrintf("TRACE [%s %d] - ", __FILE__, (int)__LINE__); \
      Xc_LogPrintf args; \
      putc('\n', Xc_log_file); \
} while(0)

#define Xc_WARNING(args) \
do { \
      Xc_LogPrintf("WARNING [%s %d] - ", __FILE__, (int)__LINE__); \
      Xc_LogPrintf args; \
      putc('\n', Xc_log_file); \
} while(0)

#define Xc_SYSERROR(args) \
do { \
      Xc_LogPrintf("SYSERROR [%s %d] - ", __FILE__, (int)__LINE__); \
      Xc_LogPrintf args; \
      Xc_LogPrintf(": %s\n", Xc_strerror(errno)); \
} while(0)

#define Xc_FATAL(args) \
do { \
      Xc_LogPrintf("FATAL ERROR [%s %d] - ", __FILE__, (int)__LINE__); \
      Xc_LogPrintf args; \
      putc('\n', Xc_log_file); \
      Xc_exit(-1); \
} while(0)

#else

#define Xc_exit(code) exit(code)

#define Xc_TRACE(args) \
printf args; \
putchar('\c')

#define Xc_WARNING(args)

#define Xc_SYSERROR(args) Xc_SyserrorAlert args;

#define Xc_FATAL(args) \
do { \
      Xc_LogPrintf("FATAL ERROR - "); \
      Xc_LogPrintf args; \
      putc('\n', Xc_log_file); \
      Xc_exit(-1); \
} while(0)

#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

typedef unsigned char boolean;

#endif /* _XMayday_h_ */











