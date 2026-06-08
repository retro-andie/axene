/*
** ColorList.h for Xclamation, XQuad and XAllWrite in ColorList/
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
** Started on  Fri Feb 21 11:48:10 1997 Emmanuel Paris
** Last update Mon Apr 14 17:03:27 1997 Emmanuel Paris
*/

#ifndef _ColorList_h
#define _ColorList_h

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "WInterface.h"
#include "ManageWidget.h"
#include "Color.h"
#include "Frame.h"
#include "RowColumn.h"
#include "Icon.h"
#include "Label.h"
#include "PushButton.h"
#define CL_EXTERN_DEFINES
#include "ColorListP.h"
#undef CL_EXTERN_DEFINES

#define CL_POPUP_DARK "gray30"
#define CL_POPUP_LIGHT "gray85"
#define CL_POPUP_NORM "gray60"

#define CL_CELL_WIDTH	16	
#define CL_CELL_HEIGHT	16	

#define CL_SKIP_TRANSPARENCY	0
#define CL_KEEP_TRANSPARENCY	1
#define CL_ADD_AUTOMATIC	2

/* define the method for Class ColorList */
typedef struct
{
  F_STD;
  void (*setColorBase)();
  void (*setColor)();
  c_Color *(*getColor)();
  void (*setCallback)();
} sf_ColorList;

/* define the ColorList Class */
typedef struct
{
  sf_ColorList		*f;
  Widget		w_This;
  c_RowColumn		*rowcolumn;
  c_Icon		*icon;
  c_Frame		*lframe;
  c_Label		*label;
  c_PushButton		*pushb;
  Widget		arrow;
  
  Widget		popup;
  Widget		drawing;
  
  void			(*cb_func)();
  void			*cb_data;
  
  int			type;
  c_Color		*color_base;
  boolean		keepTransparent;
  boolean		addAutomatic;
  
  unsigned long		light;		/* drawing color relief */
  unsigned long		dark;
  unsigned long		normal;
  XFontStruct		*font;
  
  GC			gc;
  int			nb_col;
  int			nb_row;
  int			nb_color;
  c_Color		*color_set;
  c_Color		*color_selected;
} c_ColorList;

extern sf_ColorList fc_ColorList;

#endif




