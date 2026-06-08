/*
** get_realname.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Utils/
** Generate final name for this software.
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
** Started on  Mon Jan 12 15:47:45 1998 Emmanuel Paris
** Last update Wed Feb  4 00:46:26 1998 Emmanuel Paris
*/

#include "stdio.h"
#include "version.h"

#define ___PROTO(x) ()
#define ___NPROTO(x) ()

#include "global_defs.h"
#include "osname.h"

int main(argc, argv)
int argc;
char **argv;
{
 if (argc == 2)
  printf("%s-v%1d%1d%1d%s.%s\n", Xc_SOFTWARE_NAME, MAJOR, MINOR, PL, argv[1],
	 Xc_PACKAGE_NAME);
 else
  printf("%s-v%1d%1d%1d.%s\n", Xc_SOFTWARE_NAME, MAJOR, MINOR, PL, 
	 Xc_PACKAGE_NAME);
 return 0;
}
