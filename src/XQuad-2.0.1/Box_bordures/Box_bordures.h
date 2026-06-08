/*
** Box_bordures.h for XQuad in Box_bordures/
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
** Started on  Mon Sep 18 17:09:02 1995 Robin Castanier
** Last update Wed Mar 12 15:06:21 1997 Emmanuel Paris
*/

#ifndef _Box_bordures_h
#define _Box_bordures_h

typedef struct sc_Box_bordures c_Box_bordures; 

#include "xcalibur.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Frame.h"
#include "ColorList.h"
#include "Label.h"
#include "Grid.h"
#include "OptionMenu.h"
#include "BaseStd.h"
#include <Xm/DrawingA.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#define BOX_BORD_DARK "gray30"
#define BOX_BORD_LIGHT "gray80"
#define BOX_BORD_NORM "gray60"

#define BB_NB_PATTERN	11
#define BB_NB_DASHES	5

typedef struct {
  c_Box_bordures	*box;
  int			item;
} BB_item_t;

/* define the method for Class Box_bordures */
typedef struct
{
  F_STD;
} sf_Box_bordures;

/* define the Box_bordures Class */
struct sc_Box_bordures
{
  sf_Box_bordures	*f;
  Widget		w_This;
  c_CustomDialog	*Dialog;
  c_Label		*Ltitre;
  c_Grid		*Grid;
  c_Frame		*Frame1;
  c_Grid		*Grid1;
  c_Label		*Lcontour;
  c_Label		*Lhaut;
  c_Label		*Lbas;
  c_Label		*Lgauche;
  c_Label		*Ldroite;
  c_Frame		*SFrame1;
  c_Frame		*SFrame2;
  c_Frame		*SFrame3;
  c_Frame		*SFrame4;
  c_Frame		*SFrame5;
  Widget		drawing1;
  Widget		drawing2;
  Widget		drawing3;
  Widget		drawing4;
  Widget		drawing5;
  c_Frame		*Frame2;
  c_Grid		*Grid2; 
  c_Label		*Lpattern;
  c_Label		*Ldash;
  c_Label		*LFcolor;
  c_Label		*LBcolor;
  c_OptionMenu		*OMenu1;
  c_OptionMenu		*OMenu2;
  c_ColorList		*clForeg;
  c_ColorList		*clBackg;

  BaseStd_t		*BaseStd;
  cell_border_t		*borders;
  c_Color		*foreground;
  c_Color		*background;
  int			pattern;
  int			dashes;
  
  BB_item_t		pattern_item[BB_NB_PATTERN];
  BB_item_t		dashes_item[BB_NB_DASHES];
  
  int			d_width;
  int			d_height;
  int			source;
  
  unsigned long		light;		/* drawing color relief */
  unsigned long		dark;
  unsigned long		normal;
};

extern sf_Box_bordures fc_Box_bordures;

#endif




