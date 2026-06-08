/*
** Box_install.h for XInstall in Box_install/
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
** Last update Tue Jan 20 12:23:49 1998 Emmanuel Paris
*/

#ifndef _Box_install_h
#define _Box_install_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Frame.h"
#include "Icon.h"
#include "ManageWidget.h"
#include "Label.h"
#include "XLabel.h"
#include "Grid.h"
#include "Form.h"
#include "ProgressBar.h"
#include "Box_select.h"

#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_install */
typedef struct
{
  F_STD;
} sf_Box_install;

/* define the Box_install Class */
typedef struct
{
  sf_Box_install	*f;
  Widget		w_This;

  /*-- Xt generic --*/
  XtAppContext		app;

  /*-- field --*/
  char			*DstDir;
  char			*SrcDir;
  unsigned long int	total_size;

  /*-- interface --*/
  c_CustomDialog	*Dialog;

  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*FrameInfo;
  c_Label		*Lcurfile;

  c_Frame		*FrameProgress;

  c_Grid		*GridProgress;
  c_Frame		*FrameFileGlob;

  c_Grid		*GridFile;
  c_Frame		*FramePackageGlob;

  c_Grid		*GridPackage;
  c_Frame               *FrameIcon;
  c_Form		*FormIcon;
  c_Icon                *Icon;

  c_Frame		*FrameTotalGlob;
  c_Grid		*GridTotal;
  c_Label		*Lfile;
  c_XLabel		*LfileName;
  c_Frame		*FrameFile;
  c_ProgressBar		*PBfile;
  c_XLabel		*Lfilep;
  c_Label		*Lpackage;
  c_XLabel		*LpackageName;
  c_Frame		*FramePackage;
  c_ProgressBar		*PBpackage;
  c_XLabel		*Lpackagep;
  c_Label		*Ltotal;
  c_Frame		*FrameTotal;
  c_ProgressBar		*PBtotal;
  c_XLabel		*Ltotalp;

  c_Frame		*FrameSpace;
  c_Form		*FormSpace;
  c_Label		*Lspace;
  c_ProgressBar		*PBspace;
  c_XLabel		*Lspacep;
  int			last_spacep;
  Widget		wSspace;
  c_Label		*Lspaceal;
  c_XLabel		*Lspacea;
  int			disk_block;
} c_Box_install;

extern sf_Box_install fc_Box_install;

#endif

