/*
** Box_functions.h for XQuad in Box_functions/
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
** Started on  Fri Aug 11 15:49:24 1995 Robin Castanier
** Last update Sun Mar  9 23:02:29 1997 Robin Castanier
*/

#ifndef _Box_functions_h
#define _Box_functions_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "ManageWidget.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Grid.h"
#include "Frame.h"
#include "Label.h"
#include "WTextField.h"
#include "ToggleButton.h"
#include "ItemList.h"
#include "Parser.h"
#include "config.h"
#include <X11/Intrinsic.h>

#define Xq_FT_ALL		-1

/* define the method for Class Box_functions */
typedef struct
{
  F_STD;
} sf_Box_functions;

/* define the Box_functions Class */
typedef struct
{
  sf_Box_functions	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_ItemList		*ILfunctions;
  c_WTextField		*tfFunction;
  c_Grid		*GridType;
  c_Frame		*FrameType;
  c_ToggleButton	*atbFunctionTypes[8];
  char                  szBaratin[20];
  void 			*edited_item;
  c_Parser		*Parser;
  Widget		wSep;
} c_Box_functions;

extern sf_Box_functions fc_Box_functions;

#endif





