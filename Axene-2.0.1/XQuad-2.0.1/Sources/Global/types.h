/*
** types.h for Xclamation, XQuad, XAllWrite and XMayday in Global/
** Portable definition for some type declarations
**
** Copyright (C) 1994-2000 Axene.
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
** Started on  Sat Jan 31 16:54:10 1998 Emmanuel Paris
** Last update Sat Jan 15 14:50:32 2000 Emmanuel Paris
*/

#ifndef _types_h_
#define _types_h_

#include <sys/types.h>

#ifdef ___alpha

#ifndef LONG_IS_64BIT
# define LONG_IS_64BIT
#endif

#ifndef _SYS_TYPES_H
typedef long int64_t;
typedef int int32_t;
typedef short int16_t;
#endif
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
#define ____INT64_T

#else /* ___alpha */

#ifndef LONG_IS_32BIT
# define LONG_IS_32BIT
#endif

#if defined(___i386_linux)

# if !defined(_STDINT_H) && !defined(__BIT_TYPES_DEFINED__)
typedef long int32_t;
typedef short int16_t;
# endif

# if !defined(_STDINT_H)
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
# endif

#elif defined(___rs6000) || defined(___rs6000ppc)

# if !defined(_H_INTTYPES)
typedef long int32_t;
typedef short int16_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
# endif

#elif defined(___NetBSD) || defined(___freebsd)

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;

#elif !defined(___hpux10) && !defined(___sun5)
 /* default case */
# if !defined(__BIT_TYPES_DEFINED__) && !defined(___INT32)
# define ___INT32
typedef long int32_t;
# endif

# ifndef ___INT16
typedef short int16_t;
# define ___INT16
# endif

typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
#endif

#if defined(__GNUC__) && defined(HAVE_LONGLONG)
typedef long long int64_t;
#  define ___INT64_T
# endif

#endif	/* !___alpha */

#endif /* _types_h_ */


