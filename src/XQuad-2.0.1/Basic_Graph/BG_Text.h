/*
** BG_Text.h for XQuad in Basic_Graph/
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

#ifndef _BG_Text_h_
#define _BG_Text_h_

typedef struct sc_BG_Text c_BG_Text;

#include "BasicGraphStd.h"

/*--- BG set code ---*/
typedef enum {
  XcBG_Text_ORIGIN, 
  XcBG_Text_WIDTH, XcBG_Text_HEIGHT, XcBG_Text_SIZE,
  XcBG_Text_STRING, XcBG_Text_STYLE, XcBG_Text_RULER,
  XcBG_Text_END
} BG_Text_set_code_t;

#define XcText_SIZE_NOT_DEFINED	 -1

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_BASICGRAPH_STD;
} sf_BG_Text;


/*--- Class ---*/
struct sc_BG_Text
{
  sf_BG_Text *f;
  
  BG_class_t	type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
  bbox_t	bbox;
  
  vector_t	origin;
  vector_t	size;
  
  char		*string;
  c_TextStyle	*tstyle;
  ruler_t	ruler;
  
  c_RasterFont		*rfont;
  c_CharMetrics		*metrics;
  scale_t		factor;
};

extern sf_BG_Text fc_BG_Text;

#endif /* _BG_Text_h_ */
