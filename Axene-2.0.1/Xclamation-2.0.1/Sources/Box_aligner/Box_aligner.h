/*
** Box_aligner.h for Xclamation in Box_aligner/
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
** Started on  Mon Jul 17 13:56:25 1995 Robin Castanier
** Last update Mon Jul 17 13:56:25 1995 Robin Castanier
*/

#ifndef _Box_aligner_h
#define _Box_aligner_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Frame.h"
#include "ManageWidget.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"
#include "ToggleButton.h"
#include "Icon.h"
#include "Align_Cadre.h"
#include "RowColumn.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_aligner */
typedef struct
{
  F_STD;
} sf_Box_aligner;

/* define the Box_aligner Class */
typedef struct
{
  sf_Box_aligner	*f;
  Widget		w_This;
  align_param_t		*param;
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*Frame1;
  c_Grid		*Grid1;
  c_Icon		*Icon1H;
  c_Icon		*Icon2H;
  c_Icon		*Icon3H;
  c_Icon		*Icon4H;
  c_RowColumn		*RowH;
  c_Label		*LespacementH;
  c_WTextField		*WToffsetH;
  c_ToggleButton	*TBdistriH;
  c_Frame		*Frame2;
  c_Grid		*Grid2;
  c_Icon		*Icon1V;
  c_Icon		*Icon2V;
  c_Icon		*Icon3V;
  c_Icon		*Icon4V;
  c_RowColumn		*RowV;
  c_Label		*LespacementV;
  c_WTextField		*WToffsetV;
  c_ToggleButton	*TBdistriV;
  Boolean		TrueBool;
} c_Box_aligner;

extern sf_Box_aligner fc_Box_aligner;

#endif




