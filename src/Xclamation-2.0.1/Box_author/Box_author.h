/*
** Box_author.h for Xclamation in Box_author/
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Tue Feb  1 23:34:28 2000 Antoine Buat
** Last update Wed Apr 26 23:22:38 2000 Antoine Buat
*/

#ifndef _Box_author_h
#define _Box_author_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Resource.h"
#include "Grid.h"
#include "Frame.h"
#include "Form.h"
#include "CustomDialog.h"
#include "Label.h"
#include "ManageWidget.h"
#include "version.h"
#include <X11/Intrinsic.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>

/* define the method for Class Box_author */
typedef struct
{
  F_STD;
  int (*waitForUnmap)();
} sf_Box_author;

typedef struct
{
 XImage*	infoImg;
 Pixmap		quickImg;
} s_quickImg;

typedef struct
{
 Widget		wiID;
 char*		pszAuthor;
 s_quickImg	icAuthor;
} s_widAuth;

/* define the Box_author Class */
typedef struct
{
  sf_Box_author	*f;
  Widget	w_This;

  int		currentAnim;
  int		currentAuthor;
  int		lastColor;
  int		lastAuthor;
  s_widAuth 	funcAuthor[4];
  void		*paramSave;
  GC		gc_copy;

  Display *	display;
  XtIntervalId	id;
  XtAppContext	app;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  c_Frame	*Frame2;
  c_Grid	*Grid2;
  c_Form	*FormIcon;
  Widget	Drawing;	/* drawing area for animation */
  Window	WDraw;		/* window for the former widget */
  c_Label       *Label1;
  c_Label       *Label2;
  c_Label       *Label3;
  c_Label       *Label4;
  c_Label       *MainTitleLabel;
} c_Box_author;

extern sf_Box_author fc_Box_author;
extern c_Resource *GlobResources;

#endif










