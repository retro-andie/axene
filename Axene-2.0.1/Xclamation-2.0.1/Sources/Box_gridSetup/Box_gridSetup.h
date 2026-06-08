/*
** Box_gridSetup.h for Xclamation in Box_gridSetup/
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
** Started on  Fri Oct  2 19:13:38 1998 Robin Castanier
** Last update Mon Oct  5 14:08:11 1998 Robin Castanier
*/

#ifndef _Box_gridSetup_h
#define _Box_gridSetup_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Grid.h"
#include "Frame.h"
#include "WTextField.h"
#include "CustomDialog.h"
#include "Label.h"
#include "ManageWidget.h"
#include <X11/Intrinsic.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>

/* defint the method for Class Box_gridSetup */
typedef struct
{
  F_STD;
} sf_Box_gridSetup;

/* define the Box_gridSetup Class */
typedef struct
{
  sf_Box_gridSetup	*f;
  Widget	w_This;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  c_Frame	*FrameH;
  c_Frame	*FrameV;
  c_Grid	*GridH;
  c_Grid	*GridV;
  c_Label       *lTitle;
  c_Label       *lOrigX;
  c_WTextField	*tfOrigX;
  c_Label       *lOrigY;
  c_WTextField	*tfOrigY;
  c_Label       *lSpacingX;
  c_WTextField	*tfSpacingX;
  c_Label       *lSpacingY;
  c_WTextField	*tfSpacingY;

} c_Box_gridSetup;

extern sf_Box_gridSetup fc_Box_gridSetup;

#endif










