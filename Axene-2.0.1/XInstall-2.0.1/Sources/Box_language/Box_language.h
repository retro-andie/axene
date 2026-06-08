/*
** Box_language.h for XInstall in Box_language/
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Jun 30 00:00:16 1997 Antoine Buat
** Last update Sat Jan 10 17:25:32 1998 Emmanuel Paris
*/

#ifndef _Box_language_h
#define _Box_language_h

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

/*--- define struct ball_pos ---*/
typedef struct _ball_pos
{
 int	pos_in_courbe;
 int	courbe_type;
 int	x;
 int	y;
 int	z;
 int	width;
 int	height;
 int	last_x;
 int	last_y;
 Pixmap	mask;
 Pixmap	image;
 Pixmap	save;
} ball_pos;

/*--- define struct clik_zone ---*/
typedef struct _click_zone
{
 int	x;
 int	y;
 int	width;
 int	height;
 int	ret_code;
 char   ret_string[20];
} click_zone;

/*--- define the method for Class Box_language ---*/
typedef struct
{
  F_STD;
  int (*waitForUnmap)();
} sf_Box_language;

/*--- define the Box_language Class ---*/
typedef struct
{
  sf_Box_language	*f;
  Widget		w_This;

  c_CustomDialog	*Dialog;
  c_Grid		*Grid;
  c_Frame		*FrameIcon;
  c_Icon		*IconTitle;
  c_Image		*Image;
  c_Image		*Image2;
  MemStream		*ImageStream;
  MemStream		*ImageStream2;
  Widget		Drawing;
  XtIntervalId		id;
  XtAppContext		app;
  int			x_start;
  int			y_start;
  ball_pos		ball[5];
  float			angle;
  boolean		lock;
  Display		*display;
  GC			gc_mask;
  GC			gc_image;
  GC			gc_save;
  int			depth;
  Window		wdraw;
  click_zone		*all_zone;
  click_zone		*sel_zone;
  int			sel_num;
} c_Box_language;

extern sf_Box_language fc_Box_language;

#endif
