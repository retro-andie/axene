/*
** BG_Matrice.h for XQuad in Basic_Graph/
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

#ifndef _BG_Matrice_h_
#define _BG_Matrice_h_

typedef struct sc_BG_Matrice c_BG_Matrice;

#include "BasicGraphStd.h"

typedef enum {
  XcBG_Matrice_END
} BG_Matrice_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_BASICGRAPH_STD;
} sf_BG_Matrice;


/*--- Class ---*/
struct sc_BG_Matrice
{
  sf_BG_Matrice *f;
  
  int		type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
  bbox_t	bbox;
};

extern sf_BG_Matrice fc_BG_Matrice;

#endif /* _BG_Matrice_h_ */



