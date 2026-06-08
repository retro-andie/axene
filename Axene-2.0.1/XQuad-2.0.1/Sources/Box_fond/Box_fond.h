/*
** Box_fond.h for XQuad in Box_fond/
** define class Box_fond
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
** Started on  Mon Dec  5 16:34:26 1994 Emmanuel Paris
** Last update Wed Mar 12 20:55:43 1997 Emmanuel Paris
*/

#ifndef _Box_fond_h
#define _Box_fond_h

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Label.h"
#include "Frame.h"
#include "Grid.h"
#include "ColorList.h"
#include "Resource.h"
#include "BaseStd.h"
#include <Xm/DrawingA.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#define BOX_FOND_DARK "gray30"
#define BOX_FOND_LIGHT "gray80"
#define BOX_FOND_NORM "gray60"

/*--- define the method for Class Box_fond ---*/
typedef struct
{
  F_STD;
} sf_Box_fond;

/*--- define the Box_fond Class ---*/
typedef struct
{
  sf_Box_fond 	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Label		*Ltitre;
  c_Frame		*Frame1;
  c_Grid		*Grid1;
  c_Label		*LaColorFor;
  c_ColorList		*ClColorFor;
  c_Label		*LaColorBac;
  c_ColorList		*ClColorBac;
  c_Frame		*Frame2;
  Widget		drawing;
  BaseStd_t		*BaseStd;
  cell_inside_t		*inside;
  unsigned long		light;		/* drawing color relief */
  unsigned long		dark;
  unsigned long		normal;
  boolean		source;
} c_Box_fond;

extern sf_Box_fond fc_Box_fond;

#endif
