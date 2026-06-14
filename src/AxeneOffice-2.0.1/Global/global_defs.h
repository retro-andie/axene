/*
** global_defs.h for AxeneOffice in Global/
** Global definitions
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
** Started on  Sun Jun 12 01:49:44 1994 St�phane Boisson
** Last update Sun Jan 16 14:54:10 2000 Emmanuel Paris
*/

#ifndef _global_defs_h_
#define _global_defs_h_

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef FUZZY
#define FUZZY	(boolean)(-1)
#endif

#ifndef NULL
#define NULL	((void *)0)
#endif

#ifndef SEEK_END
#define SEEK_END L_XTND 
#endif

#ifndef SEEK_SET
#define SEEK_SET L_SET
#endif

#ifndef SEEK_CUR
#define SEEK_CUR L_INCR 
#endif

#define Xc_LITTLE_ENDIAN	1234
#define Xc_BIG_ENDIAN		4321	
#define Xc_PDP_ENDIAN		3412

#if defined(___i386_linux) || defined(___sco386) || defined(___sco386sV) || \
defined(___NetBSD) || defined(___freebsd) || defined(___ncr_svr4) || \
defined(___i386_unixware) || defined(___alpha) || defined(___WIN32) || \
defined(___sun5_x86)
# define Xc_BYTE_ORDER Xc_LITTLE_ENDIAN
#else
# define Xc_BYTE_ORDER Xc_BIG_ENDIAN
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define MAX_SEGMENT	8000   /* for XDrawSegments */
#define MAX_POINT	16000  /* for XDrawPoints */	
#define DBL_CLK_DELAY	250    /* double click delay */

#ifndef ABS
#define ABS(a) (((a)<0)?-(a):(a))
#endif

#ifndef SGN
#define SGN(a) (((a)<0)?-1:1)
#endif

#ifndef SWAP
#define SWAP(type, first, second) \
  { type c_swap; c_swap = (first); first = (second); (second) = c_swap; }
#endif

#include <ctype.h>

#ifndef isdigit
#define isdigit(a) (((a)>='0') && ((a)<='9'))
#endif

#ifndef offsetof
#define offsetof(type, menber) ((long) &(((type *) 0)->menber))
#endif

extern int atoi ___PROTO((const char *string));
extern long atol ___PROTO((const char *string));

typedef unsigned char boolean;
typedef unsigned char status_t;

#if defined(___m88k) || defined(___sun4) || defined(___sun5) || \
defined(___hpux10)
typedef float real;
#else
typedef double real;
#endif

#ifndef ___HAVE_SETENV
#define setenv(a,b,c) Xsetenv(a,b)
#endif

#ifdef ___HAVE_STRCASECMP
extern int strcasecmp();
#else
# define strcasecmp(a,b) Xstrcasecmp(a,b)
#endif

#if !defined(___HAVE_MEMMOVE) || defined(___sun4)
# define memmove(dest, src, len) bcopy((src), (dest), (len))
#endif

#if defined(___ncr_svr4) || defined(___sco386) || defined(___sun5_x86)
#define bcopy(src, dest, size) memmove(dest, src, size)
#define bzero(dest, size) memset(dest, 0, size)
#else
extern void bcopy ___NPROTO((void *src, void *dest, int n));
extern void bzero ___NPROTO((void *s, int n));
#endif

#if defined(___NetBSD) || defined(___openbsd)
# define gethostent _gethtent
#endif

#if defined(___sun5_x86)
extern int gethostname ___PROTO((char *name, int namelen));
#endif

#ifndef __GNUC__
#if defined(___alpha) || defined(___iris4d) || defined(___sco386) || defined(___sco386sV) || defined(___hpux10) || defined(___rs6000)
extern int rand();
#else
extern unsigned long rand();
#endif
#endif

#ifndef ___HAVE_RINT
# define rint(a) Xrint(a)
#endif

#define SHORT int

#if  defined(___i386_linux)
#define EDITRES_COMPATIBLE
#endif

#ifdef NXERROR
#define PREVENT_GADGET_WARNING
#endif
#define WITH_ZPM

/*--- Constants ---*/
#define Xc_ENVIRONNEMENT "AXENEOFFICE_LIB"
#define Xc_ENVIRONNEMENT_GLOBAL "AXENE_LIB"
#define Xc_SOFTWARE_NAME "AxeneOffice"
#define Xc_AXENEOFFICE
#endif




