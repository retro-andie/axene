/*
** Icon.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Icon/
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
** Started on  Mon Jul 17 17:52:32 1995 Emmanuel Paris
** Last update Tue Feb  1 14:33:48 2000 Emmanuel Paris
*/


#ifndef _Icon_h
#define _Icon_h

#define XcPush		0
#define XcToggle	1
#define XcSeparator	2
#define XcLabel         3

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Label.h>

/* define the method for Class Icon */
typedef struct
{
 F_STD;
 void (*Load)();
} sf_Icon;

/* define the Icon Class */
/* remember Ximage include fields: -width -height -data .. */
typedef struct
{
 sf_Icon	*f;
 Widget		w_This;
 XImage		*Image;		/* for up */
 XImage		*Image2;	/* for down */
 Pixmap		PixImg;		/* for up */
 Pixmap		PixImg2;	/* for down */
 Pixmap		DisPixImg;
 int		Type;
} c_Icon;

extern sf_Icon fc_Icon;

#endif


