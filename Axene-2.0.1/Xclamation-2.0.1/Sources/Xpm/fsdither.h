/*
** fsdither.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Xpm/
** definition for floyd-steinberg ditherizing
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
** Started on  Mon Jul 17 14:08:22 1995 Emmanuel Paris
** Last update Tue Jan 18 17:13:11 2000 Emmanuel Paris
*/

#ifndef _fsdither_h_
#define _fsdither_h_

#include <X11/Xlib.h>

#define PIC8	0
#define PIC24	1
#define PIC16	2

extern void FloydDitherize1();
extern unsigned char *FSDither();

#endif






