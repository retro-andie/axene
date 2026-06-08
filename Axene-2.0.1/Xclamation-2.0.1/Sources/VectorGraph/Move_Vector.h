/*
** Move_Vector.h for Xclamation and XAllWrite in VectorGraph/
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
** Started on  Sun Jun  8 17:37:04 1997 Emmanuel Paris
** Last update Sun Jun  8 21:17:22 1997 Emmanuel Paris
*/

#ifndef _Move_Vector_h
#define _Move_Vector_h

#include "VectorGraph.h"
#include "Cadre.h"
#include "ObjectStd.h"
#include <X11/keysym.h>

extern void init_Move_Vector();
extern status_t eventlook_Move_Vector();
extern void abort_Move_Vector();
extern void redraw_Move_Vector();

sf_object_function fc_Move_Vector = 
{
  init_Move_Vector,
  eventlook_Move_Vector,
  abort_Move_Vector,
  redraw_Move_Vector
};

typedef struct 
{
  boolean       source;
  boolean	prems;
  Pixmap	clip;
  Polygone	*polygone;
  vector_mode_t	mode;
  coord_t	sxo,syo;
  int           x1,y1,x2,y2;
  Time		time;
} 
d_Move_Vector;

#endif
