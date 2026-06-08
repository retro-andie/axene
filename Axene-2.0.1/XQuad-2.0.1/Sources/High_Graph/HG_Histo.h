/*
** HG_Histo.h for XQuad in High_Graph/
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

#ifndef _HG_Histo_h_
#define _HG_Histo_h_

typedef struct sc_HG_Histo c_HG_Histo;

#include "HighGraphStd.h"
#include "HG_Axe.h"
#include "BG_Line.h"
#include "BG_Box.h"
#include "BG_Text.h"

/*--- BG set code ---*/
typedef enum {
  XcHG_Histo_END
} HG_Histo_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_HIGHGRAPH_STD;
} sf_HG_Histo;


/*--- Class ---*/
struct sc_HG_Histo
{
  sf_HG_Histo *f;
  c_HighGraph	*highg;
  HG_class_t	class;
  int		sub_class;
  
  c_BG_Line		*axe;
  c_BG_Line		*separator;
  
  int			nb_histo;
  c_BG_Box		**box;
  c_BG_Text		**string;
  c_BG_Text		*ordonnee;  
};

extern sf_HG_Histo fc_HG_Histo;

#endif /* _HG_Histo_h_ */
