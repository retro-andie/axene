/*
** Box_attrib.h for Xclamation, XQuad and XAllWrite in Box_attrib/
** define class Box_attrib
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
** Started on  Mon Dec  5 16:34:26 1994 Robin Castanier
** Last update Wed Mar 12 16:54:18 1997 Robin Castanier
*/

#ifndef _Box_attrib_h
#define _Box_attrib_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Label.h"
#include "ASLBox.h"
#include "Frame.h"
#include "Grid.h"
#include "ColorList.h"
#include "WTextField.h"
#include "Resource.h"
#include <X11/Intrinsic.h>
#ifdef Xc_XCLAMATION
#include "Page.h"
#else
#include "FrameManager.h"
#endif
#include "ModifList.h"

/*--- define the method for Class Box_attrib ---*/
typedef struct
{
  F_STD;
} sf_Box_attrib;

/*--- define the Box_attrib Class ---*/
typedef struct
{
  sf_Box_attrib 	*f;
  Widget		w_This;
  c_ModifList		*list_save;
  c_ASLBox		*PBox;
  c_Grid		*Grid;
  c_Grid		*G_param;
  c_Label		*Lback_color;
  c_ColorList		*menubackcolor;
  c_Label		*Lborder_color;
  c_ColorList		*menubordercolor;
  c_Label		*Lborder_width;
  c_WTextField		*Wborder_width;
  c_Frame		*F_color;
  Widget		drawing;
  l_Cadre		*selectedcadre;
  l_Cadre		*lcadre;
  GC			gc;
} c_Box_attrib;

extern sf_Box_attrib fc_Box_attrib;
extern c_Resource *GlobResources;

#endif

















