/*
** BG_SectorText.h for XQuad in Basic_Graph/
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

#ifndef _BG_SectorText_h_
#define _BG_SectorText_h_

typedef struct sc_BG_SectorText c_BG_SectorText;

#include "BasicGraphStd.h"
#include "BG_Sector.h"
#include "BG_Text.h"

/*--- BG set code ---*/
typedef enum {
  XcBG_SectorText_ANGLE1, XcBG_SectorText_ANGLE2, XcBG_SectorText_BBOX_CLIP,
  XcBG_SectorText_RAYON, XcBG_SectorText_CENTER, XcBG_SectorText_COLOR, 
  XcBG_SectorText_STRING, XcBG_SectorText_PERCENT, XcBG_SectorText_STYLE, 
  XcBG_SectorText_END
} BG_SectorText_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_BASICGRAPH_STD;
} sf_BG_SectorText;


/*--- Class ---*/
struct sc_BG_SectorText
{
  sf_BG_SectorText *f;
  
  BG_class_t	type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
  bbox_t	bbox;
  
  bbox_t	bbox_clip;
  scale_t	percent_value;
  char		percent_str[10];
  c_BG_Sector	*sector;
  c_BG_Text	*percent_string;
  c_BG_Text	*string;
};

extern sf_BG_SectorText fc_BG_SectorText;

#endif /* _BG_SectorText_h_ */
