/*
** ErrorManager.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Definitions for errors and debugging
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Sun Jun 12 01:59:12 1994 St�phane Boisson
** Last update Thu Dec 30 17:52:13 1999 Emmanuel Paris
*/

#ifndef __XC_ERROR_H__
#define __XC_ERROR_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>

extern void Xc_LogPrintf ___PROTO((char *format, ...));
extern void Xc_HistoryPrintf ___PROTO((char *format, ...));
extern void Xc_ReportTime ___PROTO((int report));
extern char *Xc_strerror ___PROTO((int errorcode));
extern void exit ___PROTO((int));

#if defined(___WIN32) || defined(___linux_glibc)
#define Xc_log_file stderr
#define Xc_history_file stderr
#else
extern FILE *Xc_log_file, *Xc_history_file;
#endif
extern void Xc_SyserrorAlert ___PROTO((char *format, ...));

#ifndef NDEBUG

#define Xc_exit(code) \
do { \
    Xc_LogPrintf("[%s %d] - Exiting with code %d\n", __FILE__, (int)__LINE__, (code)); \
    exit(code); \
} while(0)

#define Xc_FATAL(args) \
do { \
    Xc_LogPrintf("FATAL ERROR [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
    Xc_exit(-1); \
} while(0)

#define Xc_WARNING(args) \
do { \
    Xc_LogPrintf("WARNING [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
} while(0)

#define Xc_ERROR(args) \
do { \
    Xc_LogPrintf("ERROR [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
} while(0)

#define Xc_SYSERROR(args) \
do { \
    Xc_LogPrintf("SYSERROR [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    Xc_LogPrintf(": %s\n", Xc_strerror(errno)); \
    Xc_SyserrorAlert args; \
} while(0)

#ifdef __STDC__
#define Xc_ASSERT(_EX) \
do if(!(_EX)) \
{ \
    Xc_LogPrintf("[%s %d] - Assertion \"%s\" failed\n", \
	    __FILE__, (int)__LINE__, #_EX); \
    abort(); \
} while(0)
#else
#define Xc_ASSERT(_EX) \
do if(!(_EX)) \
{ \
    Xc_LogPrintf("[%s %d] - Assertion \"%s\" failed\n", \
	    __FILE__, (int)__LINE__, "_EX"); \
    abort(); \
} while(0)
#endif

#define Xc_BREAK(args) \
do { \
    Xc_LogPrintf("BREAK [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
    abort(); \
} while(0)

#else  /* !NDEBUG */

#define Xc_exit(code) exit(code)

#define Xc_FATAL(args) \
do { \
    Xc_LogPrintf("FATAL ERROR - "); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
    Xc_exit(-1); \
} while(0)

#define Xc_WARNING(args) \
do { \
    Xc_LogPrintf("WARNING - "); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
} while(0)

#define Xc_ERROR(args) \
do { \
    Xc_LogPrintf("ERROR - "); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
} while(0)

#if 0
#define Xc_SYSERROR(args) \
do { \
    Xc_LogPrintf("SYSERROR - "); \
    Xc_LogPrintf args; \
    Xc_LogPrintf("`%s'\n", Xc_strerror(errno)); \
    Xc_SyserrorAlert args; \
} while(0)
#else
#define Xc_SYSERROR(args) Xc_SyserrorAlert args;
#endif

#define Xc_ASSERT(x)
#define Xc_BREAK(x)

#endif  /* NDEBUG */


#ifndef NTRACE
#define Xc_TRACE(args) \
do { \
    Xc_LogPrintf("TRACE [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
} while(0)
#define Xc_TIMING(args) \
do { \
    Xc_LogPrintf("TIMING [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
    Xc_ReportTime(-1); \
} while(0)
#define Xc_TIMING_START(args) \
do { \
    Xc_LogPrintf("TIMING [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_LogPrintf args; \
    putc('\n', Xc_log_file); \
    Xc_ReportTime(0); \
} while(0)
#else /* !NTRACE */
#define Xc_TRACE(x)
#define Xc_TIMING(x)
#define Xc_TIMING_START(x)
#endif /* NTRACE */

#ifndef NHISTORY
#define Xc_HISTORY(args) \
do { \
    Xc_HistoryPrintf("HISTORY [%s %d] - ", __FILE__, (int)__LINE__); \
    Xc_HistoryPrintf args; \
    putc('\n', Xc_history_file); \
} while(0)
#else /* !NHISTORY */
#define Xc_HISTORY(x)
#endif /* NHISTORY */

#endif /* !__XC_ERROR_H__ */
