/*
** readkey.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
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
** Started on  Mon Jul 17 19:22:50 1995 Antoine Buat
** Last update Thu Dec 30 17:47:08 1999 Emmanuel Paris
*/

#include "PortCompat.h"

#if defined(___freebsd) || defined(___NetBSD) || defined(___openbsd) || \
    defined(___linux_glibc) || defined(___i386_linux) || \
    defined(___linux_libc) || defined(___linux_2_0p) || \
    defined(___sparc_linux) || defined(___alpha_linux) || \
    defined(___alpha) || defined(___rs6000) || defined(___rs6000ppc) || \
    defined(___hpux9) || defined(___hpux10) || defined(___sun4)
#include <sys/termios.h>
#else
#include <sys/termio.h>
#endif

#ifdef ___HAVE_UNISTD_H
#include <unistd.h>
#include <sys/ioctl.h>
#endif


char xc_read_key_with_echo()
{
  unsigned short save_flag;
  unsigned short save_c_cc;
#ifdef TCGETA
  struct termio buf;
#else
  struct termios buf;
#endif
  char Car;
  
#ifdef TCGETA
  ioctl (0,TCGETA,&buf);
#else
  tcgetattr(0, &buf);
#endif
  save_flag = buf.c_lflag;
  save_c_cc = buf.c_cc[VMIN];
  buf.c_lflag &= ~ICANON;
  buf.c_cc[VMIN] = 1;
  
#ifdef TCSETA
  ioctl (0,TCSETA,&buf);
#else
  tcsetattr(0, TCSANOW, &buf);
#endif
  while (!read(0, &Car, 1));
  
  buf.c_lflag = save_flag;
  buf.c_cc[VMIN] = save_c_cc;
  
#ifdef TCSETA
  ioctl (0,TCSETA,&buf);
#else
  tcsetattr(0, TCSANOW, &buf);
#endif
  return Car;
}

char xc_read_key_without_echo()
{
  unsigned short save_flag;
  unsigned short save_c_cc;
#ifdef TCGETA
  struct termio buf;
#else
  struct termios buf;
#endif
  char Car;
  
#ifdef TCGETA
  ioctl (0,TCGETA,&buf);
#else
  tcgetattr(0, &buf);
#endif
  save_flag = buf.c_lflag;
  save_c_cc = buf.c_cc[VMIN];
  buf.c_lflag &= ~(ECHO|ICANON);
  buf.c_cc[VMIN] = 1;
  
#ifdef TCSETA
  ioctl (0,TCSETA,&buf);
#else
  tcsetattr(0, TCSANOW, &buf);
#endif
  while (!read(0, &Car, 1));
  
  buf.c_lflag = save_flag;
  buf.c_cc[VMIN] = save_c_cc;
  
#ifdef TCSETA
  ioctl (0,TCSETA,&buf);
#else
  tcsetattr(0, TCSANOW, &buf);
#endif
  return Car;
}
