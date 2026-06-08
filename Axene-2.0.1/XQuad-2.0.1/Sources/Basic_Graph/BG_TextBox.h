/*
** BG_TextBox.h for XQuad in Basic_Graph/
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
** Last update Wed May 29 15:39:50 1996 One of the authors
*/

#ifndef _BG_TextBox_h_
#define _BG_TextBox_h_

typedef struct sc_BG_TextBox c_BG_TextBox;

#include "BasicGraphStd.h"

/*--- BG set code ---*/
typedef enum {
  XcBG_TextBox_ORIGIN, 
  XcBG_TextBox_WIDTH, XcBG_TextBox_HEIGHT, 
  XcBG_TextBox_TEXTBOX, XcBG_TextBox_STYLE, XcBG_TextBox_RULER,
  XcBG_TextBox_TOP_MARGIN, XcBG_TextBox_BOTTOM_MARGIN,
  XcBG_TextBox_LEFT_MARGIN, XcBG_TextBox_RIGHT_MARGIN,
  XcBG_TextBox_BACK, XcBG_TextBox_FRAME,
  XcBG_TextBox_INS_FG_COLOR, XcBG_TextBox_INS_BG_COLOR,
  XcBG_TextBox_OUT_FG_COLOR, XcBG_TextBox_OUT_BG_COLOR, 
  XcBG_TextBox_DASHES, XcBG_TextBox_PATTERN,
  XcBG_TextBox_LINE_THICKNESS, 
   XcBG_TextBox_END
} BG_TextBox_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_BASICGRAPH_STD;
} sf_BG_TextBox;


/*--- Class ---*/
struct sc_BG_TextBox
{
  sf_BG_TextBox *f;
  
  int		type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
  bbox_t	bbox;
  
  vector_t	origin;
  coord_t	top_margin;
  coord_t	bottom_margin;
  coord_t	
  
  c_Color	*ins_fg_color;
  c_Color	*ins_bg_color;
  c_Color	*out_fg_color;
  c_Color	*out_bg_color;
  
/*  c_Dashes	*dashes; */ /* must create these objects */
/*  c_Pattern	*pattern; */
  coord_t	line_thickness;
};

extern sf_BG_TextBox fc_BG_TextBox;

#endif /* _BG_TextBox_h_ */
