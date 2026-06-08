/*
** HG_Axe.h for XQuad in High_Graph/
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

#ifndef _HG_Axe_h_
#define _HG_Axe_h_

typedef struct sc_HG_Axe c_HG_Axe;

#include "HighGraphStd.h"
#include "BG_Text.h"
#include "BG_Line.h"
#include "BG_Polyline.h"

/*--- sub class define ---*/
enum {
  XcHG_Axe_VERTICAL,
  XcHG_Axe_HORIZONTAL,
  XcHG_Axe_POLAIRE
};

/*--- HG set code ---*/
typedef enum {
  XcHG_Axe_END
} HG_Axe_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_HIGHGRAPH_STD;
  coord_t (*convert_value) ___PROTO((c_HG_Axe *This, real value));
} sf_HG_Axe;


/*--- Class ---*/
struct sc_HG_Axe
{
  sf_HG_Axe *f;
  c_HighGraph	*highg;
  HG_class_t	class;
  int		sub_class;
  
  int		nb_axe;
  c_BG_Line	**axe;
  
  int		nb_grad;
  c_BG_Line	**grad_line;
  c_BG_Line	**grad_line_mark;
  c_BG_Polyline	**grad_polyline_mark;
  c_BG_Text	**grad_string;
  char		**grad_number;
  
  real		grad_scale;
  real		grad_value_origin;
  coord_t	grad_origin;
  coord_t	grad_zero;
};

extern sf_HG_Axe fc_HG_Axe;

#endif /* _HG_Axe_h_ */





















