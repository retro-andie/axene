/*
** BG_Polygone.h for XQuad in Basic_Graph/
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

#ifndef _BG_Polygone_h_
#define _BG_Polygone_h_

typedef struct sc_BG_Polygone c_BG_Polygone;

#include "BasicGraphStd.h"

/*--- BG set code ---*/
typedef enum {
  XcBG_Polygone_ADD_POINT, XcBG_Polygone_ADD_POINTS,
  XcBG_Polygone_INS_FG_COLOR, XcBG_Polygone_INS_BG_COLOR,
  XcBG_Polygone_OUT_FG_COLOR, XcBG_Polygone_OUT_BG_COLOR, 
  XcBG_Polygone_DASHES, XcBG_Polygone_PATTERN,
  XcBG_Polygone_LINE_THICKNESS, XcBG_Polygone_END
} BG_Polygone_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_BASICGRAPH_STD;
} sf_BG_Polygone;

/*--- Class ---*/
struct sc_BG_Polygone
{
  sf_BG_Polygone *f;
  
  BG_class_t	type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
  bbox_t	bbox;		

  int		nb_point;
  BG_Point	*list_point;
  
  c_Color	*ins_fg_color;
  c_Color	*ins_bg_color;
  c_Color	*out_fg_color;
  c_Color	*out_bg_color;
  
/*  c_Dashes	*dashes; */ /* must create these objects */
/*  c_Pattern	*pattern; */
  coord_t	line_thickness;
};

extern sf_BG_Polygone fc_BG_Polygone;

#endif /* _BG_Polygone_h_ */
