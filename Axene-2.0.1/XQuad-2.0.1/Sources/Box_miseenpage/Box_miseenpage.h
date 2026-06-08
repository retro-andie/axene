/*
** Box_miseenpage.h for XQuad in Box_miseenpage/
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
** Started on  Mon Sep 18 16:28:10 1995 Robin Castanier
** Last update Sat Jan  8 16:04:43 2000 Emmanuel Paris
*/

#ifndef _Box_miseenpage_h
#define _Box_miseenpage_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Frame.h"
#include "ManageWidget.h"
#include "ItemMenu.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"
#include "RadioImage.h"
#include "DocumentList.h"
#include "Tableau.h"
#include "PageFormat.h"
#include "RadioBox.h"
#include "ToggleButton.h"
#include "PostScript.h"
#include "config.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_miseenpage */
typedef struct
{
  F_STD;
} sf_Box_miseenpage;

/* define the Box_miseenpage Class */
typedef struct
{
  sf_Box_miseenpage	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*Frame1;
  c_RadioImage		*RIsens;
  c_Frame		*Frame2;
  c_RadioBox		*RBwhatPrint;
  c_Frame		*Frame3;
  c_RadioImage		*RIPortraitPaysage;
  c_Frame		*Frame4;
  c_Grid		*Grid4;
  c_WTextField		*TFMargeH;
  c_Label		*LMargeH;
  c_Label		*LMargeB;
  c_Label		*LMargeG;
  c_Label		*LMargeD;
  c_WTextField		*TFMargeB;
  c_WTextField		*TFMargeG;
  c_WTextField		*TFMargeD;
  c_RadioBox		*RBcentrage;
  c_Frame		*Frame5;
  c_Grid		*Grid5;
  c_ItemMenu		*OFormat;
  c_Label		*Lsize;
  c_WTextField		*TFdocHeight;
  c_Label		*Lcroix;
  c_WTextField		*TFdocWidth;
  c_Frame		*Frame6;
  c_Grid		*Grid6;
  c_ToggleButton	*TBreduire;
  c_ToggleButton	*TBtenirsur;
  c_WTextField		*TFreduire;
  c_WTextField		*TFtenirsur1;
  c_Label		*Lx;
  c_WTextField		*TFtenirsur2;

  /*--- param struct ---*/
  setup_print_t		*setup_param;
  setup_print_t		*setup_save;

  int			fbool_sens;
  int			fbool_PP;

} c_Box_miseenpage;

extern sf_Box_miseenpage fc_Box_miseenpage;

#endif




