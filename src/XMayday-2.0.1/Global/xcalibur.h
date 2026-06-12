/*
** xcalibur.h for XMayday in Global/
** General includes for XCalibur
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
** Started on  Sun Jun 12 01:47:24 1994 St�phane Boisson
** Last update Mon Mar 16 20:10:40 1998 Emmanuel Paris
*/

#ifndef _xcalibur_h_
#define _xcalibur_h_

#if !defined(NO_XSHM) && !defined(___rs6000) && !defined(___rs6000ppc) && \
!defined(___mips) && !defined(___WIN32)
#define HAVE_XSHM
#endif

#if !defined(NO_XSHAPE) && !defined(___rs6000)
#define HAVE_XSHAPE
#endif

#include "PortCompat.h"

#include <sys/types.h>
#include <stddef.h>

#ifdef SIGNALH
#include <setjmp.h>
#endif

#ifdef ___HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef EFINITIVE_VERSION
# ifndef NTRACE
#  define NTRACE
# endif
# ifndef NHISTORY
#define NHISTORY
# endif
# ifndef NDEBUG
#define NDEBUG
# endif
# ifndef NMEMDEBUG
#define NMEMDEBUG
# endif
# ifndef NXERROR
#define NXERROR
# endif
#endif

/*--- Prototypes ---*/
#ifdef __cplusplus
# define ___PROTO(x) x
#else
# ifdef ___m88k
#  define ___PROTO(x) ()
# else
#  ifdef __STDC__
#   define ___PROTO(x) x
#  else
#   define ___PROTO(x) ()
#  endif
# endif
#endif

/*--- For wrong warning prototype ---*/
#define ___NPROTO(x) ()

/*--- Inline functions ---*/
#ifndef ___INLINE
# define ___INLINE
#endif

/*--- Compose macros ---*/
#ifdef ___GNUC_COMPOSE_MACRO
#undef ___GNUC_COMPOSE_MACRO
#endif
#if defined(__GNUC__) || defined(___iris4d) || defined(___sco386) ||\
defined(___sco386sV) || defined(___ncr_svr4)
#define ___PSEUDO_COMPOSE_MACRO
#else
#ifdef ___i386_unixware
#define ___PSEUDO_COMPOSE_MACRO
#endif
#endif

#define NDEADLINE

#include "ErrorManager.h"
#include "MemoryManager.h"
#include "ObjStd.h"
#include "global_defs.h"
#include "error_defs.h"
#include "misc.h"
#include "units.h"

#endif





