/*
** Scale_Cadres.h for Xclamation in Page/
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
** Started on  Mon Jul 17 19:55:12 1995 Emmanuel Paris
** Last update Mon Jul 17 19:55:12 1995 Emmanuel Paris
*/

#ifndef _Scale_Cadres_h
#define _Scale_Cadres_h

#include "Page.h"

extern void init_Scale_Cadre();
extern status_t eventlook_Scale_Cadre();
extern void abort_Scale_Cadre();
extern void redraw_Scale_Cadre();

sf_page_function fc_Scale_Cadre={
  init_Scale_Cadre,
  eventlook_Scale_Cadre,
  abort_Scale_Cadre,
  redraw_Scale_Cadre
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
  int		xm,ym;
  coord_t	sxm,sym;
  int		xc,yc;
  coord_t	xcent,ycent;
  l_Lpoint	*svglistp;
  real		factor;
  Time		time;
} 
d_Scale_Cadre;

#endif
