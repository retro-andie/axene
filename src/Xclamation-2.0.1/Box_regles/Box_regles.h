/*
** Box_regles.h for Xclamation in Box_regles/
** define class Box_regles
**
** Copyright (C) 1994-2000 Axene.
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
** Started on  Mon Dec 26 16:34:26 1994 Robin Castanier
** Last update Mon Jun  5 16:52:51 1995 Emmanuel Paris
*/

#ifndef _Box_regles_h
#define _Box_regles_h

#include "xcalibur.h"
#include "WInterface.h"
#include "TextRuler.h"
#include "ASLBox.h"
#include "Grid.h"
#include "Label.h"
#include "Frame.h"
#include "WTextField.h"
#include "Resource.h"
#include "Icon.h"
#include "ToggleButton.h"
#include "RowColumn.h"
#include <X11/Intrinsic.h>
#include "ModifList.h"

typedef struct {
  coord_t l_margin;
  coord_t r_margin;
  coord_t p_indent;
  int il_mode;
  coord_t il_line;
  coord_t il_para;
  textruler_just_t justif;
  scale_t word_distri;
  scale_t min_ratio;
  scale_t max_ratio;
  c_TextRuler	*ruler;
} ruler_modif_t;

/*--- define the method for Class Box_regles ---*/
typedef struct
{
  F_STD;
} sf_Box_regles;

/*--- define the Box_regles Class ---*/
typedef struct
{
  sf_Box_regles *f;
  Widget	w_This;
  c_ModifList	*list_save;
  c_TextRuler	**ruler_base;
  c_TextRuler	*v_ruler_base;
  c_ASLBox	*PBox;
  c_Frame       *Frame1;
  c_Frame       *Frame2;
  c_Frame       *Frame3;
  c_Frame       *Frame4;
  c_Grid	*Grid;
  c_Grid	*Grid1;
  c_Grid	*Grid2;
  c_RowColumn	*Row1;
  c_Grid	*Grid4;
  c_Label	*Lalinea;
  c_Label	*LmargeG;
  c_Label	*LmargeD;
  c_WTextField	*TFalinea;
  c_WTextField	*TFmargeG;
  c_WTextField	*TFmargeD;
  c_Label	*Ldistri;
  c_Label	*LratioMin;
  c_Label	*LratioMax;
  c_WTextField	*TFdistri;
  c_WTextField	*TFratioMin;
  c_WTextField	*TFratioMax;
  c_Icon        *Icon1;
  c_Icon        *Icon2;
  c_Icon        *Icon3;
  c_Icon        *Icon4;
  c_ToggleButton *Tinterligne;
  c_Label	*Linterlig;
  c_Label	*Linterpar;
  c_WTextField	*TFinterlig;
  c_WTextField	*TFinterpar;
  ruler_modif_t	modif;
} c_Box_regles;

extern sf_Box_regles fc_Box_regles;
extern c_Resource *GlobResources;

#endif
