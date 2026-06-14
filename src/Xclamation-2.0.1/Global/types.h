/*
** types.h for Xclamation, XQuad, XAllWrite and XMayday in Global/
** Portable definition for some type declarations
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
** Started on  Sat Jan 31 16:54:10 1998 Emmanuel Paris
** Last update Sat Jan 15 14:50:32 2000 Emmanuel Paris
*/

#ifndef _types_h_
#define _types_h_

/*
** Portable integer type definitions for ANSI C89 / POSIX.1-1997.
**
** Platform aliases (defined by PortCompat.h before this header):
**   ___sun5_8   -> ___sun5   (Solaris 8 / SunOS 5.8)
**   ___aix43    -> ___rs6000ppc  (AIX 4.3)
**   ___freebsd4 -> ___freebsd    (FreeBSD 4.x)
**   ___netbsd15 -> ___NetBSD     (NetBSD 1.5)
*/

#include <sys/types.h>

#ifdef ___alpha

/*
** DEC Alpha (OSF/1 / Digital UNIX / Tru64): LP64 model.
** long is 64 bits, int is 32 bits.
*/
# ifndef LONG_IS_64BIT
#  define LONG_IS_64BIT
# endif

# ifndef _SYS_TYPES_H
typedef long           int64_t;
typedef int            int32_t;
typedef short          int16_t;
typedef signed char    int8_t;
# endif
# ifndef _BITS_STDINT_UINTN_H
typedef unsigned int   uint32_t;
# endif
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
# define ____INT64_T

#else /* !___alpha */

# if defined(__x86_64__) || defined(__aarch64__) || defined(__LP64__)
#  ifndef LONG_IS_64BIT
#   define LONG_IS_64BIT
#  endif
# else
#  ifndef LONG_IS_32BIT
#   define LONG_IS_32BIT
#  endif
# endif

# if defined(___i386_linux) || defined(___linux_glibc) || \
     defined(___linux_libc) || defined(___linux_2_0p) || \
     defined(___sparc_linux) || defined(___alpha_linux)

/* Linux: use glibc's __BIT_TYPES_DEFINED__ guard to avoid conflicts */
#  if !defined(_STDINT_H) && !defined(__BIT_TYPES_DEFINED__)
typedef long           int32_t;
typedef short          int16_t;
typedef signed char    int8_t;
#  endif
#  if !defined(_STDINT_H) && !defined(_BITS_STDINT_UINTN_H)
#   ifdef LONG_IS_64BIT
typedef unsigned int   uint32_t;
#   else
typedef unsigned long  uint32_t;
#   endif
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
#  endif

# elif defined(___rs6000) || defined(___rs6000ppc)

/* AIX: _H_INTTYPES guard prevents redefinition */
#  if !defined(_H_INTTYPES)
typedef long           int32_t;
typedef short          int16_t;
typedef signed char    int8_t;
typedef unsigned long  uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
#  endif

# elif defined(___NetBSD) || defined(___freebsd) || defined(___openbsd)

/* BSD family (FreeBSD 2.x-4.x, NetBSD 1.x-1.5, OpenBSD 2.x):
   <sys/types.h> already defines u_int32_t etc; we provide the
   POSIX names only if the system has not done so.
   NetBSD 1.6 sets __BIT_TYPES_DEFINED__ via machine/int_types.h and
   defines uint32_t as a macro (#define uint32_t __uint32_t); guard
   against both to avoid typedef conflicts. */
#  if !defined(_SYS_INT_TYPES_H_) && !defined(__int8_t_defined) && \
      !defined(__BIT_TYPES_DEFINED__)
typedef signed char    int8_t;
#  endif
#  if !defined(__BIT_TYPES_DEFINED__) && !defined(_SYS_INT_TYPES_H_)
typedef short          int16_t;
typedef long           int32_t;
#  endif
#  if !defined(_BITS_STDINT_UINTN_H) && !defined(_SYS_INT_TYPES_H_) && \
     !defined(_UINT32_T) && !defined(uint32_t)
#   ifdef LONG_IS_64BIT
typedef unsigned int   uint32_t;
#   else
typedef unsigned long  uint32_t;
#   endif
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
#  endif

# elif !defined(___hpux10) && !defined(___sun5)

/* Default case: covers SunOS 4.1 (___sun4), IRIX (___iris4d), SCO,
   NCR SVR4, UnixWare, DG/UX, RISC OS, and any unlisted platform. */
#  if !defined(__BIT_TYPES_DEFINED__) && !defined(___INT32)
#   define ___INT32
typedef long int32_t;
#  endif

#  ifndef ___INT16
typedef short int16_t;
#   define ___INT16
#  endif

#  ifndef ___INT8
typedef signed char int8_t;
#   define ___INT8
#  endif

#  ifndef _BITS_STDINT_UINTN_H
#   ifdef LONG_IS_64BIT
typedef unsigned int   uint32_t;
#   else
typedef unsigned long  uint32_t;
#   endif
#  endif
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

# endif /* platform chain */

/*
** int64_t: no portable 64-bit type exists in strict ANSI C89 for
** 32-bit platforms.  On 64-bit LP64 platforms 'long' is 64 bits.
** With GCC + HAVE_LONGLONG, 'long long' provides 64-bit storage
** as a compiler extension (not a C89 standard type).
*/
# ifdef LONG_IS_64BIT
#  if !defined(___INT64_T) && !defined(_SYS_TYPES_H)
typedef long           int64_t;
#   define ___INT64_T
#  endif
# elif defined(__GNUC__) && defined(HAVE_LONGLONG)
#  if !defined(___INT64_T)
typedef long long      int64_t;
#   define ___INT64_T
#  endif
# endif

#endif /* !___alpha */

#endif /* _types_h_ */


