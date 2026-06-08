/*
 * Box_imprimer.h	     - Object and method definition for WarningDialog
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
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
  c_ToggleButton	*TBselection;
  c_ToggleButton	*TBtoutes;
  c_Label		*LPrintDe;
  c_WTextField		*WTPrintDe;
  c_Label		*LPrintA;
  c_WTextField		*WTPrintA;

  c_Frame		*Frame3;
  c_Grid		*Grid3;
  c_Label		*Lnbcopies;
  c_WTextField		*NbDeCopies;


  /*--- param struct ---*/
  print_param_t		*print_param;

} c_Box_imprimer;

extern sf_Box_imprimer fc_Box_imprimer;

#endif




