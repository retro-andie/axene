/*
** Move_Image.h for Xclamation and XAllWrite in Images/
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
** Started on  Mon Jul 17 18:14:27 1995 Emmanuel Paris
** Last update Wed Jun 24 18:08:08 1998 Emmanuel Paris
*/

#ifndef _Move_Image_h
#define _Move_Image_h

#include "Cadre.h"
#include "Image.h"
#include "ObjectStd.h"
#include <X11/keysym.h>

extern void init_Move_Image();
extern status_t eventlook_Move_Image();
extern void abort_Move_Image();
extern void redraw_Move_Image();

sf_object_function fc_Move_Image =
{
 init_Move_Image,
 eventlook_Move_Image,
 abort_Move_Image,
 redraw_Move_Image
};

typedef struct 
{
 boolean	source;
 boolean	prems;
 boolean	shift;
 GC		gc;
 Pixmap		clip;
 Polygone	*polygone;
 int		xo, yo;
 coord_t	sxo, syo;
 int		x1, y1, x2, y2;
 coord_t	sx1, sy1, sx2, sy2;
 Time		time;
} 
d_Move_Image;

#endif
