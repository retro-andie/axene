/*
** Insert_Move_Delete_Points.h for Xclamation in Page/
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
** Started on  Mon Jul 17 20:21:15 1995 Emmanuel Paris
** Last update Mon Jul 17 20:21:15 1995 Emmanuel Paris
*/


#ifndef _Insert_Move_Delete_Points_h
#define _Insert_Move_Delete_Points_h

#include "Page.h"

#define IDP_INSERT_POINT	0
#define IDP_CONNECT_POINT	1

extern void init_Insert_Point();
extern status_t eventlook_Insert_Point();
extern void abort_Insert_Point();
extern void redraw_Insert_Point();

sf_page_function fc_Insert_Point={
  init_Insert_Point,
  eventlook_Insert_Point,
  abort_Insert_Point,
  redraw_Insert_Point,
}; 

typedef struct
{
  boolean	source;
  boolean	prems;
  boolean	connected;
  int		function;
  int		xo;
  int		yo;
  coord_t	sxo;
  coord_t	syo;
  int		xc;
  int		yc;
  coord_t	sxc;
  coord_t	syc;
  l_Cadre	*lcadre;
  l_Point	*lpoint;
  l_Point	*lp_connect;
  l_Point	*lp_first;
  l_Point	*lp_loop;
  l_Point	**lp_insert;
}
d_Insert_Point;

extern void move_point_mode();
extern void resize_cadre_mode();

extern void init_Delete_Point();
extern status_t eventlook_Delete_Point();
extern void abort_Delete_Point();
extern void redraw_Delete_Point();

sf_page_function fc_Delete_Point={
  init_Delete_Point,
  eventlook_Delete_Point,
  abort_Delete_Point,
  redraw_Delete_Point,
}; 

#endif /* _Insert_Move_Delete_Points_ */
