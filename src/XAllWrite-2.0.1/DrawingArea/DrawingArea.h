/*
** DrawingArea.h for Xclamation, XQuad and XAllWrite in DrawingArea/
** Object and method for DrawingArea
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
** Started on  Mon Jul 17 15:05:00 1994 Antoine Buat
** Last update Sat Jan  8 16:02:27 2000 Emmanuel Paris
*/

#ifndef _DrawingArea_h
#define _DrawingArea_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/DrawingA.h>

/* define the method for Class DrawingArea */
typedef struct
{
  F_STD;
} sf_DrawingArea;

/* define the DrawingArea Class */
typedef struct
{
 sf_DrawingArea		*f;
 Widget			w_This;
} c_DrawingArea;

extern sf_DrawingArea fc_DrawingArea;

#endif




