/*
** PortCompat.h for Axene products in Global/
** ANSI C89 / POSIX.1-1997 portability and platform compatibility
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
*/

/*
** IMPORTANT: This header must be included BEFORE any system headers.
** It defines feature-test macros that control which symbols are
** exposed by system headers under POSIX.1-1997 (Issue 5).
**
** Supported platforms (year-2000 versions):
**
**   ___i386_linux      Linux 2.2/2.4 i386 (glibc 2.1/2.2)
**   ___linux_glibc     Linux with GNU libc 2.x (year-2000)
**   ___linux_libc      Linux with libc5
**   ___linux_2_0p      Linux 2.0+ ELF
**   ___sparc_linux     Linux Sparc with glibc
**   ___alpha_linux     Linux Alpha with glibc
**   ___linux_modern    Linux 3.x-6.x / glibc 2.17+ (modern; any arch)
**   ___sun4            SunOS 4.1.x (Solaris 1.x, BSD-derived)
**   ___sun5            Solaris 2.x / SunOS 5.x SPARC (Solaris 7 = 5.7)
**   ___sun5_8          Solaris 8 / SunOS 5.8 SPARC
**   ___sun5_x86        Solaris x86 (2.4-2.6)
**   ___rs6000          AIX 3.x (POWER/RS6000)
**   ___rs6000ppc       AIX 4.1 (PowerPC)
**   ___aix43           AIX 4.3 (PowerPC)
**   ___freebsd         FreeBSD 2.x-3.x
**   ___freebsd4        FreeBSD 4.x
**   ___freebsd12       FreeBSD 12.x
**   ___freebsd_modern  FreeBSD 13.x-14.x (alias: ___freebsd)
**   ___NetBSD          NetBSD 1.x
**   ___netbsd15        NetBSD 1.5
**   ___netbsd9         NetBSD 9.x
**   ___netbsd_modern   NetBSD 10.x+ (alias: ___NetBSD)
**   ___openbsd         OpenBSD 2.x
**   ___openbsd7        OpenBSD 7.x
**   ___openbsd_modern  OpenBSD 7.x+ (alias: ___openbsd)
**   ___macos           macOS 10.12+ / Darwin (alias: ___freebsd)
**   ___alpha           DEC OSF/1 / Digital UNIX / Tru64 UNIX
**   ___hpux10          HP-UX 10.x
**   ___hpux9           HP-UX 9.x
**   ___iris4d          SGI IRIX 5.x/6.x
**   ___mips            RISC OS 4.x
**   ___m88k            DG/UX 5.4
**   ___sco386          SCO UNIX 3.2v4
**   ___sco386sV        SCO UnixWare 2.x/3.x
**   ___ncr_svr4        NCR Unix SVR4
**   ___i386_unixware   Novell/SCO UnixWare 2.1
**   ___WIN32           Windows NT (placeholder)
**
** To add support for a new UNIX platform, add a new ___xxx macro
** in the Makefile (via -D___xxx) and add corresponding entries
** in the #ifdef chains below.
*/

#ifndef _PortCompat_h_
#define _PortCompat_h_

/*
** =========================================================
** SECTION 1: POSIX / X/Open feature-test macros
**
** These must be defined before any system header is included.
** _POSIX_C_SOURCE 199506L requests POSIX.1-1995 (Issue 4, plus
** POSIX.1b realtime and POSIX.1c threads), which is the basis
** of the 1997 consolidated standard (Issue 5).
** =========================================================
*/

/*
** NetBSD: _NETBSD_SOURCE already enables full POSIX + BSD extensions.
** Setting _POSIX_C_SOURCE would suppress BSD-only symbols (M_PI, random(),
** u_short, u_int, etc.) that system headers like sys/mount.h depend on.
** For all other platforms, _POSIX_C_SOURCE=199506L requests POSIX.1-1995.
*/
#if !defined(___NetBSD) && !defined(___netbsd15) && \
    !defined(___netbsd9) && !defined(___netbsd_modern)
# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 199506L
# endif
#endif

/*
** SunOS 4.1 (BSD-derived): basic POSIX.1-1988 is the limit;
** _POSIX_SOURCE activates the POSIX namespace in old SunOS headers.
*/
#if defined(___sun4) && !defined(_POSIX_SOURCE)
# define _POSIX_SOURCE
#endif

/*
** AIX: _ALL_SOURCE exposes the full POSIX + BSD + XPG namespace.
** Without it, AIX headers hide many standard functions.
*/
#if (defined(___rs6000) || defined(___rs6000ppc) || defined(___aix43)) \
    && !defined(_ALL_SOURCE)
# define _ALL_SOURCE
#endif

/*
** HP-UX 10.x: _HPUX_SOURCE enables POSIX + HP extensions.
*/
#if defined(___hpux10) && !defined(_HPUX_SOURCE)
# define _HPUX_SOURCE
#endif

/*
** IRIX 5.x/6.x: _SGI_SOURCE enables SGI + POSIX extensions.
** _XOPEN_SOURCE enables XPG4 conformance.
*/
#if defined(___iris4d)
# ifndef _SGI_SOURCE
#  define _SGI_SOURCE
# endif
# ifndef _XOPEN_SOURCE
#  define _XOPEN_SOURCE
# endif
#endif

/*
** Modern Linux (glibc 2.17+ / kernel 3.0+): use _DEFAULT_SOURCE.
** This replaces the deprecated _BSD_SOURCE and _SVID_SOURCE pair
** (deprecated since glibc 2.20).  It implies _BSD_SOURCE + _SVID_SOURCE
** + _ATFILE_SOURCE and is available on any glibc >= 2.19.
*/
#if defined(___linux_modern)
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
#endif

/*
** Legacy Linux (glibc 2.x, year-2000 targets): use _BSD_SOURCE and
** _SVID_SOURCE to pull in BSD-compat and SVID extensions.
** Not set when ___linux_modern is defined, as _DEFAULT_SOURCE supersedes both.
*/
#if !defined(___linux_modern) && \
    (defined(___i386_linux) || defined(___linux_glibc) || \
     defined(___linux_libc) || defined(___linux_2_0p) || \
     defined(___sparc_linux) || defined(___alpha_linux))
# ifndef _BSD_SOURCE
#  define _BSD_SOURCE
# endif
# ifndef _SVID_SOURCE
#  define _SVID_SOURCE
# endif
#endif

/*
** Solaris 8 (SunOS 5.8) and 7 (SunOS 5.7): map ___sun5_8 onto ___sun5
** so all existing ___sun5 guards cover Solaris 8 automatically.
*/
#if defined(___sun5_8) && !defined(___sun5)
# define ___sun5
#endif

/*
** AIX 4.3: map ___aix43 onto ___rs6000ppc so existing AIX 4.x
** guards cover AIX 4.3 automatically.
*/
#if defined(___aix43) && !defined(___rs6000ppc)
# define ___rs6000ppc
#endif

/*
** FreeBSD 4.x: map ___freebsd4 onto ___freebsd so existing guards apply.
*/
#if defined(___freebsd4) && !defined(___freebsd)
# define ___freebsd
#endif

/*
** NetBSD 1.5: map ___netbsd15 onto ___NetBSD so existing guards apply.
*/
#if defined(___netbsd15) && !defined(___NetBSD)
# define ___NetBSD
#endif

/*
** OpenBSD 2.x: BSD-derived; expose the existing BSD API.
*/
#if defined(___openbsd) && !defined(_BSD_SOURCE)
# define _BSD_SOURCE
#endif

/*
** Modern Linux: map ___linux_modern onto ___linux_glibc so all existing
** ___linux_glibc guards cover modern Linux builds automatically.
*/
#if defined(___linux_modern) && !defined(___linux_glibc)
# define ___linux_glibc
#endif

/*
** FreeBSD 12+: map ___freebsd12 / ___freebsd_modern onto ___freebsd.
*/
#if (defined(___freebsd12) || defined(___freebsd_modern)) && !defined(___freebsd)
# define ___freebsd
#endif

/*
** NetBSD 9+: map ___netbsd9 / ___netbsd_modern onto ___NetBSD.
*/
#if (defined(___netbsd9) || defined(___netbsd_modern)) && !defined(___NetBSD)
# define ___NetBSD
#endif

/*
** NetBSD 9+ removed statfs(2) in favour of the POSIX statvfs(2).
** Provide a source-level shim so existing code using struct statfs /
** statfs() compiles unchanged.  f_frsize in statvfs maps to f_bsize
** in the old BSD struct statfs (fundamental block size).
** NetBSD 1.x (1.5, 1.6) retains statfs() in <sys/mount.h>; no shim needed.
*/
#if defined(___netbsd9) || defined(___netbsd_modern)
# include <sys/statvfs.h>
# ifndef statfs
#  define statfs  statvfs
# endif
# ifndef f_bsize
#  define f_bsize f_frsize
# endif
#endif

/*
** OpenBSD 7+: map ___openbsd7 / ___openbsd_modern onto ___openbsd.
*/
#if (defined(___openbsd7) || defined(___openbsd_modern)) && !defined(___openbsd)
# define ___openbsd
#endif

/*
** macOS (Darwin 10.12+): BSD-derived libc; use FreeBSD-compatible type
** guards.  _DEFAULT_SOURCE and _BSD_SOURCE are glibc-specific and must
** not be set on Darwin.
*/
#if defined(___macos) && !defined(___freebsd)
# define ___freebsd
#endif

/*
** =========================================================
** SECTION 2: Compiler capability macros
**
** Derived from the target compiler and platform.
** Do NOT use __GNUC__ directly in source code after this
** point; use these macros instead.
** =========================================================
*/

/*
** ___HAVE_UNISTD_H: the platform provides <unistd.h>.
** This covers virtually all POSIX.1 platforms circa 2000.
** SunOS 4.1 has a limited unistd.h; Windows NT does not.
*/
#if defined(___i386_linux) || defined(___linux_glibc) || \
    defined(___linux_libc) || defined(___linux_2_0p) || \
    defined(___sparc_linux) || defined(___alpha_linux) || \
    defined(___sun5) || defined(___sun5_x86) || \
    defined(___rs6000) || defined(___rs6000ppc) || \
    defined(___freebsd) || defined(___NetBSD) || \
    defined(___openbsd) || \
    defined(___alpha) || defined(___hpux10) || defined(___hpux9) || \
    defined(___iris4d) || defined(___mips) || defined(___m88k) || \
    defined(___sco386) || defined(___sco386sV) || \
    defined(___ncr_svr4) || defined(___i386_unixware) || \
    defined(___sun4)
# define ___HAVE_UNISTD_H
#endif

/*
** ___HAVE_STDLIB_H: the platform provides <stdlib.h>.
*/
#if defined(__STDC__) || \
    defined(___i386_linux) || defined(___linux_glibc) || \
    defined(___sun5) || defined(___rs6000) || defined(___rs6000ppc) || \
    defined(___freebsd) || defined(___NetBSD) || defined(___openbsd) || \
    defined(___alpha) || defined(___hpux10) || defined(___iris4d) || \
    defined(___sco386) || defined(___sco386sV) || defined(___ncr_svr4) || \
    defined(___i386_unixware) || defined(___sun4)
# define ___HAVE_STDLIB_H
#endif

/*
** ___HAVE_STRINGS_H: the platform uses <strings.h> (BSD)
** rather than (or in addition to) <string.h>.
*/
#if defined(___sun4) || defined(___freebsd) || defined(___NetBSD) || \
    defined(___openbsd)
# define ___HAVE_STRINGS_H
#endif

/*
** ___HAVE_MEMMOVE: the platform provides memmove().
** SunOS 4.1 provides only bcopy; use the bcopy -> memmove
** alias defined in global_defs.h for those platforms.
*/
#if !defined(___sun4) && !defined(___m88k) && !defined(___mips)
# define ___HAVE_MEMMOVE
#endif

/*
** ___HAVE_STRCASECMP: the platform provides strcasecmp().
** NCR SVR4 and old SCO require Xstrcasecmp() instead.
*/
#if !defined(___ncr_svr4) && !defined(___sco386)
# define ___HAVE_STRCASECMP
#endif

/*
** ___HAVE_SETENV: the platform provides setenv(3).
** Many SVR4 platforms do not; Xsetenv() is the fallback.
*/
#if defined(___i386_linux) || defined(___linux_glibc) || \
    defined(___freebsd) || defined(___NetBSD) || defined(___openbsd) || \
    defined(___alpha) || defined(___rs6000) || defined(___rs6000ppc)
# define ___HAVE_SETENV
#endif

/*
** ___HAVE_RINT: the platform provides rint(3).
** Old SCO and NCR SVR4 need Xrint() instead.
*/
#if !defined(___sco386) && !defined(___ncr_svr4)
# define ___HAVE_RINT
#endif

/*
** =========================================================
** SECTION 3: Inline keyword
**
** Under strict ANSI C89, 'inline' is not a keyword.
** GCC supports it as an extension even in -ansi mode,
** but to avoid all GNU extensions we suppress it here.
** C++ code may still use 'inline' as it is standard in C++98.
** =========================================================
*/

#ifndef ___INLINE
# define ___INLINE
#endif

/*
** =========================================================
** SECTION 4: C++ extern "C" wrapper macro
** =========================================================
*/

#ifdef __cplusplus
# define ___BEGIN_DECLS  extern "C" {
# define ___END_DECLS    }
#else
# define ___BEGIN_DECLS
# define ___END_DECLS
#endif

/*
** =========================================================
** SECTION 5: Placeholder stubs for future UNIX ports
**
** Copy a block below and fill in the details when porting
** to a new platform.  Define the ___xxx macro on the
** compiler command line (e.g. -D___newplatform).
** =========================================================
*/

/*
** --- FUTURE PORT TEMPLATE ---
** #if defined(___newplatform)
**   #define ___HAVE_UNISTD_H
**   #define ___HAVE_STDLIB_H
**   #define ___HAVE_MEMMOVE
**   #define ___HAVE_STRCASECMP
**   #define ___HAVE_SETENV
**   #define ___HAVE_RINT
**   ... add feature-test macros for the new platform's libc ...
** #endif
*/

/*
** --- Tru64 UNIX 5.x (formerly Digital UNIX / OSF/1) ---
** Already covered by ___alpha above.
** For Tru64 5.x specifically, define ___tru64_5 and -D___alpha.
*/

/*
** --- DYNIX/ptx (Sequent) ---
** #if defined(___dynixptx)
**   #define ___dynixptx
**   #define _POSIX_C_SOURCE 199506L
**   #define ___HAVE_UNISTD_H
**   #define ___HAVE_STDLIB_H
** #endif
*/

/*
** --- Interix (POSIX subsystem for Windows NT) ---
** #if defined(___interix)
**   #define _ALL_SOURCE
**   #define ___HAVE_UNISTD_H
**   #define ___HAVE_STDLIB_H
** #endif
*/

/*
** --- QNX 4.x/6.x (POSIX RTOS) ---
** #if defined(___qnx)
**   #define _POSIX_C_SOURCE 199506L
**   #define ___HAVE_UNISTD_H
**   #define ___HAVE_STDLIB_H
**   #define ___HAVE_MEMMOVE
**   #define ___HAVE_STRCASECMP
**   #define ___HAVE_SETENV
**   #define ___HAVE_RINT
** #endif
*/

/*
** --- Reliant UNIX (Siemens NIXOS) ---
** #if defined(___reliant_unix)
**   ... add defines ...
** #endif
*/

#endif /* _PortCompat_h_ */
