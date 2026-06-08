/*
** Box_legal.h for Xclamation, XQuad and XAllWrite in Box_legal/
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
** Started on  Tue Nov 14 11:49:23 1995 Robin Castanier
** Last update Mon Oct 19 03:03:50 1998 Robin Castanier
*/

#ifndef _Box_legal_h
#define _Box_legal_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Grid.h"
#include "Frame.h"
#include "Icon.h"
#include "CustomDialog.h"
#include "Label.h"
#include "ManageWidget.h"
#include "ScrolledWindow.h"
#include <X11/Intrinsic.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>

/* defint the method for Class Box_legal */
typedef struct
{
  F_STD;
} sf_Box_legal;

/* define the Box_legal Class */
typedef struct
{
  sf_Box_legal	*f;
  Widget	w_This;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  Widget	w_Frame1;
  c_Icon        *IconSoft;
  c_Icon        *Icon;
  c_Frame	*Frame2;
  c_Grid	*Grid2;
  c_Grid	*Grid2_1;
  c_Label       *Label1;
  c_Label       *lBLSysInfo;
  c_Label       *LabelAdress;
  c_Label       *LabelCopy;
  c_Frame	*Frame3;
  c_Grid	*Grid3;
  c_Label       *lDeadLineDate;
  c_Label       *lDeadLineDay;
  c_ScrolledWindow	*SText;
  c_Label		*LText;
  XtIntervalId	id;
  XtAppContext	app;
  boolean	bColor;
  boolean	bBlink;
} c_Box_legal;

extern sf_Box_legal fc_Box_legal;

#endif










