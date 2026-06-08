/*
** Box_key.h for XInstall in Box_key/
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
** Last update Sat Jan 15 18:09:23 2000 Emmanuel Paris
*/

#ifndef _Box_key_h
#define _Box_key_h

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
#include "WTextField.h"
#include "Image.h"

#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>

/*--- define ---*/
#define TFK1	1
#define TFK2	2
#define TFFN	4

#define TFALL	(TFK1+TFK2+TFFN)

#define NB_ICON		5   /* number of icons to flip */

/*--- define the method for Class Box_key ---*/
typedef struct
{
  F_STD;
  int (*waitForUnmap)();
} sf_Box_key;

/*--- define the Box_key Class ---*/
typedef struct
{
  sf_Box_key	*f;
  Widget		w_This;

  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Frame		*FrameIcon;
  c_Frame		*FrameInfo;
  c_Label		*LInfo;
  c_Label		*LFName;
  c_WTextField		*TFFName;
  c_Label		*LCompany;
  c_WTextField		*TFCompany;
  c_Label		*LKey;
  c_WTextField		*TFkey1;
  c_Label		*LKminus;
  c_WTextField		*TFkey2;
  c_Image		*Image;
  Widget		Drawing;
  int			Tf_field;
  MemStream		*ImageStream;
  c_Frame		*FrameAsk;
  c_Grid		*GridAskP;
  c_Grid		*GridAsk;
  XtIntervalId		id;
  XtAppContext		app;
  int			iPixmap;
  c_Form		*FormIcon;
  c_Icon	        *IconFlip[NB_ICON];
  unsigned long		lFlipTime;
  boolean		isSensitive;
}
 c_Box_key;

extern sf_Box_key fc_Box_key;

#endif

