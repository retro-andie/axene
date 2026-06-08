/*
** Box_keygen.h for XInstall in Box_keygen/
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
** Started on  Mon Jul 17 14:00:16 1995 Robin Castanier
** Last update Tue Jan  6 17:44:36 1998 Robin Castanier
*/

#ifndef _Box_keygen_h
#define _Box_keygen_h

/*--- includes ---*/
#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Frame.h"
#include "Icon.h"
#include "ManageWidget.h"
#include "Label.h"
#include "Grid.h"
#include "Image.h"

#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>

/*--- define the method for Class Box_keygen ---*/
typedef struct
{
  F_STD;
  int (*waitForUnmap)();
} sf_Box_keygen;

/*--- define the Box_keygen Class ---*/
typedef struct
{
  sf_Box_keygen	*f;
  Widget		w_This;

  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Grid		*GridIcon;
  c_Frame		*FrameIcon1;
  c_Icon		*Icon1;
  c_Frame		*FrameIcon2;
  c_Icon		*Icon2;
  c_Frame		*FrameIcon3;
  c_Icon		*Icon3;
  c_Frame		*FrameIcon4;
  c_Icon		*Icon4;
  c_Frame		*FrameInfo;
  c_Label		*Label1;
  c_Frame		*FrameLabel1;
  c_Label		*Label2;
  c_Frame		*FrameLabel2;
  c_Label		*Label3;
  c_Frame		*FrameLabel3;
  c_Label		*Label4;
  c_Frame		*FrameLabel4;
  c_Label		*LInfo;

  boolean               verbose;
}
 c_Box_keygen;

extern sf_Box_keygen fc_Box_keygen;

#endif



