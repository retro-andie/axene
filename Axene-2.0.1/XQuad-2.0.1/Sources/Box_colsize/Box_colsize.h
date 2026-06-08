/*
 * Box_colsize.h	     - Object and method definition for WarningDialog
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#ifndef _Box_colsize_h
#define _Box_colsize_h

#include "xcalibur.h"
#include "ColRow_Size.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Frame.h"
#include "ManageWidget.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"
#include "ToggleButton.h"
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>

/* define the method for Class Box_colsize */
typedef struct
{
  F_STD;
} sf_Box_colsize;

/* define the Box_colsize Class */
typedef struct
{
  sf_Box_colsize	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Label		*Ltitre;
  c_Frame		*Frame1;
  c_Grid		*Grid1;
  c_Label		*Lcolsize;
  c_WTextField		*WTcolsize;
  c_ToggleButton	*TBcolsize;
  c_Frame		*Frame2;
  c_Grid		*Grid2;
  c_Label		*Lrowsize;
  c_WTextField		*WTrowsize;
  c_ToggleButton	*TBrowsize;
  colrow_size_t		*colrow;
} c_Box_colsize;

extern sf_Box_colsize fc_Box_colsize;

#endif




