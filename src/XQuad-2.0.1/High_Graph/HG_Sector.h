/*
** HG_Sector.h for XQuad in High_Graph/
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

#ifndef _HG_Sector_h_
#define _HG_Sector_h_

typedef struct sc_HG_Sector c_HG_Sector;

#include "HighGraphStd.h"
#include "BG_SectorText.h"

/*--- BG set


 code ---*/
typedef enum {
  XcHG_Sector_END
} HG_Sector_set_code_t;

/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_HIGHGRAPH_STD;
} sf_HG_Sector;


/*--- Class ---*/
struct sc_HG_Sector
{
  sf_HG_Sector *f;
  c_HighGraph	*highg;
  HG_class_t	class;
  int		sub_class;

  int			nb_sector;
  c_BG_SectorText	**sector;
  c_BG_Text		*ordonnee;  
};

extern sf_HG_Sector fc_HG_Sector;

#endif /* _HG_Sector_h_ */



