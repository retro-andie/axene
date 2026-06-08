/*
** BG_Polyline.h for XQuad in Basic_Graph/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Sat Jun 24 18:07:33 1995 Emmanuel Paris
** Last update Wed May 29 15:39:49 1996 One of the authors
*/

#ifndef _BG_Polyline_h_
#define _BG_Polyline_h_

typedef struct sc_BG_Polyline c_BG_Polyline;

#include "BasicGraphStd.h"

/*--- BG set code ---*/
typedef enum {
  XcBG_Polyline_ADD_POINT, XcBG_Polyline_ADD_POINTS,
  XcBG_Polyline_FG_COLOR, XcBG_Polyline_BG_COLOR,
  XcBG_Polyline_DASHES, XcBG_Polyline_LINE_THICKNESS,
  XcBG_Polyline_END
} BG_Polyline_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_BASICGRAPH_STD;
} sf_BG_Polyline;

/*--- Class ---*/
struct sc_BG_Polyline
{
  sf_BG_Polyline *f;
  
  BG_class_t	type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
  bbox_t	bbox;		

  int		nb_point;
  BG_Point	*list_point;
  
  c_Color	*fg_color;
  c_Color	*bg_color;
  
/*  c_Dashes	*dashes; */ /* must create these objects */
/*  c_Pattern	*pattern; */
  coord_t	line_thickness;
};

extern sf_BG_Polyline fc_BG_Polyline;

#endif /* _BG_Polyline_h_ */
