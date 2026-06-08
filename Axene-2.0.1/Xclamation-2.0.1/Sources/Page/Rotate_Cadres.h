/*
** Rotate_Cadres.h for Xclamation in Page/
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
** Started on  Mon Jul 17 19:56:10 1995 Emmanuel Paris
** Last update Mon Jul 17 19:56:10 1995 Emmanuel Paris
*/

#ifndef _Rotate_Cadres_h
#define _Rotate_Cadres_h

#include "Page.h"
#include <X11/keysym.h>

extern void init_Rotate_Cadre();
extern status_t eventlook_Rotate_Cadre();
extern void abort_Rotate_Cadre();
extern void redraw_Rotate_Cadre();

sf_page_function fc_Rotate_Cadre={
  init_Rotate_Cadre,
  eventlook_Rotate_Cadre,
  abort_Rotate_Cadre,
  redraw_Rotate_Cadre
};

typedef struct _l_Lpoint
{
  l_Point		*lpoint;
  struct _l_Lpoint	*NextLpoint;
} l_Lpoint;

typedef struct 
{
  boolean       source;
  boolean	center;
  boolean	set_center;
  boolean	shift;
  int		xm,ym;
  coord_t	sxm,sym;
  int		xc,yc;
  coord_t	xcent,ycent;
  l_Lpoint	*svglistp;
  angle_t	angle;
  Time		time;
} 
d_Rotate_Cadre;

#endif





