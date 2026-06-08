/*
** Xpm.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and AxeneOffice 
** 	in Xpm/
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
** Started on  Tue Jul 18 13:07:20 1995 Emmanuel Paris
** Last update Wed Apr  9 16:21:13 1997 Emmanuel Paris
*/

#ifndef _Xpm_h_
#define _Xpm_h_

extern void XPMload();
extern void XPMGetAttrib();
extern void XPMGetColor();
extern void XPMColorAlloc();
extern void XPMCreateImage();
extern char **ZPMtoXPM();
#endif
