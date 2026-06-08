/*
** Box_directory.h for XInstall in Box_directory/
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
** Last update Sun Jan 11 01:32:38 1998 Emmanuel Paris
*/

#ifndef _Box_directory_h
#define _Box_directory_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Frame.h"
#include "Icon.h"
#include "ManageWidget.h"
#include "Label.h"
#include "Grid.h"
#include "Form.h"
#include "WTextField.h"

#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_directory */
typedef struct
{
  F_STD;
  int (*waitForUnmap)();
} sf_Box_directory;

/* define the Box_directory Class */
typedef struct
{
  sf_Box_directory	*f;
  Widget		w_This;

  /*-- field --*/
  char			*DstDir;
  char			*SrcDir;

  /*-- interface --*/
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*FrameSrc;
  c_Frame		*FrameSrcInfo;
  c_Grid		*GridSrc;
  c_Icon		*IconSrc;
  c_Label		*LSrc;
/*c_Label		*LSrcD;*/
  c_WTextField		*LSrcD;
  c_Label		*Ltitre2;
  c_Frame		*FrameDst;
  c_Grid		*GridDst;
  c_Icon		*IconDst;
  c_Label		*LDst;
/*c_Label		*LDstD;*/
  c_WTextField		*LDstD;
  c_Frame		*FrameSize;
  c_Label		*LDstSpace;
} c_Box_directory;

extern sf_Box_directory fc_Box_directory;

#endif







