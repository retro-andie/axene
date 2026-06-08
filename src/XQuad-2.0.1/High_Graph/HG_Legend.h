/*
** HG_Legend.h for XQuad in High_Graph/
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
** Last update Wed May 29 15:40:25 1996 One of the authors
*/

#ifndef _HG_Legend_h_
#define _HG_Legend_h_

typedef struct sc_HG_Legend c_HG_Legend;

#include "HighGraphStd.h"
#include "BG_Text.h"
#include "BG_Line.h"
#include "BG_Box.h"

/*--- sub class define ---*/
enum {
  XcHG_Legend_BOX = 0,
  XcHG_Legend_LINE = 1,
  XcHG_Legend_MOTIF = 1,
  XcHG_Legend_ABSC = 0,
  XcHG_Legend_ORDO = 2,
  XcHG_Legend_SENS = 2
};

/*--- HG set code ---*/
typedef enum {
  XcHG_Legend_END
} HG_Legend_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_HIGHGRAPH_STD;
} sf_HG_Legend;


/*--- Class ---*/
struct sc_HG_Legend
{
  sf_HG_Legend *f;
  c_HighGraph	*highg;
  HG_class_t	class;
  int		sub_class;
  
  c_BG_Line		*separator;
  
  int			nb_legend;
  c_BG_Line		**line;		/* line xor box, depend of sub_class */
  c_BG_Box		**box;
  c_BG_Text		**string;  
};

extern sf_HG_Legend fc_HG_Legend;

#endif /* _HG_Legend_h_ */
