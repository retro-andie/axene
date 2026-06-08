/*
** Box_nombres.old.h for XQuad in Box_nombres/
** define class Box_nombres
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
** Started on  Mon May 22 13:32:26 1995 Robin Castanier
** Last update Wed Sep 20 12:38:54 1995 Antoine Buat
*/

#ifndef _Box_nombres_h
#define _Box_nombres_h

#include "xcalibur.h"
#include "WInterface.h"
#include "ASLBox.h"
#include "Grid.h"
#include "Label.h"
#include "ToggleButton.h"
#include "ItemMenu.h"
#include "WTextField.h"
#include "Frame.h"
#include "Resource.h"
#include "BaseStd.h"
#include <X11/Intrinsic.h>
#include "ModifList.h"

/*--- define the method for Class Box_nombres ---*/
typedef struct
{
  F_STD;
} sf_Box_nombres;

typedef struct {
  unsigned int	max_prec;
  unsigned int	min_prec;
  unsigned int	space_each;
  char		*unit;
  char		coma_sep;
  c_Color	*pos;
  c_Color	*neg;
  int		unitafter;
  char		char_sep;
  int		afftrunc;
  c_FormatNbr	*format;
} format_modif_t;


/*--- define the Box_nombres Class ---*/
typedef struct
{
  sf_Box_nombres *f;
  Widget	w_This;
  c_ModifList	*list_save;
  c_FormatNbr	**format_base;
  c_FormatNbr	*v_format_base;
  c_Color	*color_base;

  c_ASLBox	*PBox;
  c_Grid	*Grid;
  c_Frame	*Frame1;
  c_Frame	*Frame2;
  c_Frame	*Frame3;
  c_Frame	*Frame4;
  c_Grid	*Grid1;
  c_Grid	*Grid2;
  c_Grid	*Grid3;
  c_Grid	*Grid4;
  c_Label	*LaColorPos;
  c_Label	*LaColorNeg;
  c_ItemMenu	*ItColorPos;
  c_ItemMenu	*ItColorNeg;
  c_Label	*LaUnitString;
  c_WTextField	*WTunitString;
  c_ToggleButton  *TBunitPos;
  c_ToggleButton  *TBtruncate;
  c_Label	*LaPrecMin;
  c_WTextField	*WTprecMin;
  c_Label	*LaPrecMax;
  c_WTextField	*WTprecMax;
  c_Label	*LaSepString;
  c_WTextField	*WTsepString;
  c_Label	*LaSepMString;
  c_WTextField	*WTsepMString;
  c_Label	*LaSepPos;
  c_WTextField	*WTsepPos;
  format_modif_t modif;
  c_FormatNbr	**retptr;
  char		unit_string[21];
} c_Box_nombres;

extern sf_Box_nombres fc_Box_nombres;
extern c_Resource *GlobResources;

#endif

