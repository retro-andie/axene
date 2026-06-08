/*
** osname.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
** define Xc_SYSTEM_NAME & Xc_PACKAGE_NAME
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
** Started on  Wed Feb  4 00:20:11 1998 Emmanuel Paris
** Last update Tue Aug  8 09:44:05 2000 Emmanuel Paris
*/

#ifndef _osname_h_
#define _osname_h_

#include "builtdate.h"

#ifdef ___mips
#define Xc_def_SYSTEM_NAME "RISC OS 4.52"
#define Xc_def_PACKAGE_NAME "mips-riscos-4.5"
#endif
#ifdef ___m88k
#define Xc_def_SYSTEM_NAME "DGUX 5.4.1"
#define Xc_def_PACKAGE_NAME "m88k-dgux-5.4"
#endif
#ifdef ___rs6000
#define Xc_def_SYSTEM_NAME "AIX 3.2.5"
#define Xc_def_PACKAGE_NAME "rs6000-aix-3.2.5"
#endif
#ifdef ___rs6000ppc
#define Xc_def_SYSTEM_NAME "AIX 4.1.3"
#define Xc_def_PACKAGE_NAME "rs6000ppc-aix-4.1.3"
#endif
#ifdef ___sun4
#define Xc_def_SYSTEM_NAME "SunOS 4.1.4"
#define Xc_def_PACKAGE_NAME "sparc-sunos-4.1.4"
#endif
#ifdef ___sun5
#define Xc_def_SYSTEM_NAME "Solaris 2.5"
#define Xc_def_PACKAGE_NAME "sparc-solaris-2.5"
#endif
#ifdef ___sun5_x86
#define Xc_def_SYSTEM_NAME "Solaris 2.4 x86"
#define Xc_def_PACKAGE_NAME "i486-solaris-2.4"
#endif
#ifdef ___alpha
#define Xc_def_SYSTEM_NAME "OSF/1 1.3"
#define Xc_def_PACKAGE_NAME "alpha-digital-osf1-3.2"
#endif
#ifdef ___WIN32
#define Xc_def_SYSTEM_NAME "WinNT 4.0"
#define Xc_def_PACKAGE_NAME "i486-winnt-4.0"
#endif
#ifdef ___NetBSD
#define Xc_def_SYSTEM_NAME "NetBSD 1.0"
#define Xc_def_PACKAGE_NAME "i386-netbsd-1.0"
#endif
#ifdef ___sco386
#define Xc_def_SYSTEM_NAME "SCO 3.2v4.2"
#define Xc_def_PACKAGE_NAME "i386-sco-3.2v4.2"
#endif
#ifdef ___sco386sV
#define Xc_def_SYSTEM_NAME "SCO 3.2v5"
#define Xc_def_PACKAGE_NAME "i586-sco-3.2v5"
#endif
#ifdef ___ncr_svr4
#define Xc_def_SYSTEM_NAME "NCR UNIX SVR4"
#define Xc_def_PACKAGE_NAME "i386-ncr-svr4"
#endif
#ifdef ___i386_linux
#define Xc_def_SYSTEM_NAME "Linux 1.2"
#define Xc_def_PACKAGE_NAME "i486-linux-1.2"
#endif
#ifdef ___i386_unixware
#define Xc_def_SYSTEM_NAME "Unixware 2.1.2"
#define Xc_def_PACKAGE_NAME "a_definir"
#endif
#ifdef ___linux_libc
#undef Xc_def_SYSTEM_NAME
#undef Xc_def_PACKAGE_NAME
#define Xc_def_SYSTEM_NAME "Linux 2.0 libc"
#define Xc_def_PACKAGE_NAME "i486-linux-2.0-libc"
#endif
#ifdef ___linux_2_0p
#undef Xc_def_SYSTEM_NAME
#undef Xc_def_PACKAGE_NAME
#define Xc_def_SYSTEM_NAME "Linux 2.0 ELF [p]"
#define Xc_def_PACKAGE_NAME "i586-linux-2.0-elf-xm2d"
#endif
#ifdef ___linux_glibc
#undef Xc_def_SYSTEM_NAME
#undef Xc_def_PACKAGE_NAME
#define Xc_def_SYSTEM_NAME "Linux 2.0 glibc"
#define Xc_def_PACKAGE_NAME "i486-linux-glibc"
#endif
#ifdef ___freebsd
#define Xc_def_SYSTEM_NAME "FreeBSD 2.2.2"
#define Xc_def_PACKAGE_NAME "i386-freebsd-2.2"
#endif
#ifdef ___iris4d
#define Xc_def_SYSTEM_NAME "IRIX 5.3"
#define Xc_def_PACKAGE_NAME "mips-sgi-irix-5.3"
#endif
#ifdef ___hpux10
#define Xc_def_SYSTEM_NAME "HP-UX 10.20"
#define Xc_def_PACKAGE_NAME "hppa-hpux-10.20"
#endif
#ifdef ___hpux9
#define Xc_def_SYSTEM_NAME "HP-UX 9.03"
#define Xc_def_PACKAGE_NAME "hppa-hpux-9.03"
#endif
#ifdef ___sparc_linux
#define Xc_def_SYSTEM_NAME "Linux Sparc 2.0 glibc"
#define Xc_def_PACKAGE_NAME "sparc-linux-2.0-glibc"
#endif
#ifdef ___alpha_linux
#undef Xc_def_SYSTEM_NAME
#undef Xc_def_PACKAGE_NAME
#define Xc_def_SYSTEM_NAME "Linux Alpha 2.0 glibc"
#define Xc_def_PACKAGE_NAME "alpha-linux-2.0-glibc"
#endif

#ifdef MAKE_SYSTEM
# define Xc_SYSTEM_NAME MAKE_SYSTEM
#else
# ifdef  Xc_def_SYSTEM_NAME
#  define Xc_SYSTEM_NAME Xc_def_SYSTEM_NAME
# else
#  define Xc_SYSTEM_NAME "unkown"
# endif
#endif

#ifdef MAKE_PACKAGE
# define Xc_PACKAGE_NAME MAKE_PACKAGE
#else
# ifdef  Xc_def_PACKAGE_NAME
#  define Xc_PACKAGE_NAME Xc_def_PACKAGE_NAME
# else
#  define Xc_PACKAGE_NAME "unkown"
# endif
#endif

#endif /* _osname_h_ */
