/*
** Box_print.h for XMayday in Box_print/
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sun Sep  1 07:37:25 1996 Robin Castanier
** Last update Sun Mar 16 11:58:11 1997 Robin Castanier
*/


#ifndef _Box_print_h
#define _Box_print_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Frame.h"
#include "ManageWidget.h"
#include "ItemMenu.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"
#include "PageFormat.h"
#include "RadioBox.h"
#include "ToggleButton.h"
#include "printer.h"
#include <X11/Intrinsic.h>

/* define the method for Class Box_print */
typedef struct
{
  F_STD;
} sf_Box_print;

/* define the Box_print Class */
typedef struct
{
  sf_Box_print	*f;
  Widget		w_This;
  
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*Frame1;
  c_Frame		*Frame2;
  c_Frame		*Frame4;
  c_Frame		*Frame5;
  c_Frame		*Frame6;
  
  c_Grid		*GridFormat;
  c_Label		*Lsize;
  c_WTextField		*TFdocWidth;
  c_Label		*Lcroix;
  c_WTextField		*TFdocHeight;
  c_ItemMenu		*IMpageFormat;

  c_Grid		*Grid5;
  c_RadioBox		*RBparam1;
  c_RadioBox		*RBparam2;
    
  c_ItemMenu		*IMexportType;
  c_ItemMenu		*IMfontType;
  
  c_ItemMenu		*IMprinter;
  
  /*--- param struct ---*/
  print_param_t		*param;
  boolean		nopostscript;
  XtWidgetGeometry	geometry;
} c_Box_print;

extern sf_Box_print fc_Box_print;

#endif







