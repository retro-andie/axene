/* 
** port.h for XCalibur in RasterFont/t1imager/
** Header for porting
** 
** Copyright (C) 1994, XCalibur-PAO. All Rights reserved.
** Login <xcalibur@axene.org>
** 
** Started on  Thu Nov  3 14:57:14 1994 Nucleus
** Last update Fri Dec 31 15:57:05 1999 Emmanuel Paris
** 
** This file can be redistributed under the terms of the GNU General
** Public Licence.
*/

#ifndef __XC_T1IMAGER_PORT_H__
#define __XC_T1IMAGER_PORT_H__

#include <stdio.h>
#include "types.h"

#if defined(___m88k) || defined(___sun4) || defined(___sun5) ||\
 defined(___hpux10) || defined(___hpux9) || defined(___sparc_linux)
typedef float real_t;
#else
typedef double real_t;
#endif

#ifndef __XC_ERROR_H__

extern void Xc_LogPrintf();
#if defined(___WIN32) || defined(___linux_glibc)
#define Xc_log_file stderr
#define Xc_history_file stderr
#else
extern FILE *Xc_log_file;
#endif

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

#endif

#endif /* !__XC_T1IMAGER_PORT_H__ */
