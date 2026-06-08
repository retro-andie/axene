/*
** Box_imprimer.h for Xclamation in Box_imprimer/
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
** Started on  Mon Jul 17 14:02:21 1995 Robin Castanier
** Last update Mon Jul 17 14:02:21 1995 Robin Castanier
*/


#ifndef _Box_imprimer_h
#define _Box_imprimer_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Frame.h"
#include "ManageWidget.h"
#include "ItemMenu.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"
#include "DocumentList.h"
#include "Document.h"
#include "PageFormat.h"
#include "RadioBox.h"
#include "ToggleButton.h"
#include "PostScript.h"
#include "config.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_imprimer */
typedef struct
{
  F_STD;
} sf_Box_imprimer;

/* define the Box_imprimer Class */
typedef struct
{
  sf_Box_imprimer	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*Frame1;
  c_ItemMenu		*IMprinter;
  c_Frame		*Frame2;
  c_Grid		*Grid2;
  c_Label		*Lnbcopies;
  c_WTextField		*NbDeCopies;
  c_Frame		*Frame3;
  c_RadioBox		*RBwhatPrint;
  c_Frame		*Frame4;
  c_Grid		*Grid4;
  c_ToggleButton	*TBtoutes;
  c_Label		*LPrintDe;
  c_WTextField		*WTPrintDe;
  c_Label		*LPrintA;
  c_WTextField		*WTPrintA;
  c_Frame		*Frame5;
  c_RadioBox		*RBhowPrint;
  
  /*--- param struct ---*/
  print_param_t		*print_param;

} c_Box_imprimer;

extern sf_Box_imprimer fc_Box_imprimer;

#endif




