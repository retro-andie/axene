/*
** Box_color.h for Xclamation, XQuad and XAllWrite in Box_color/
** define class Box_color
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
** Last update Thu Jun  3 19:10:51 1999 Emmanuel Paris
*/

#ifndef _Box_color_h
#define _Box_color_h

#include "xcalibur.h"
#include "WInterface.h"
#include "Color.h"
#include "ASLBox.h"
#include "Grid.h"
#include "ToggleButton.h"
#include "Label.h"
#include "WTextField.h"
#include "Scale.h"
#include "Frame.h"
#include "Resource.h"
#include <X11/Intrinsic.h>
#include "ModifList.h"

#define XcG_NONE_PART	-1
#define XcG_CMYK_PART	0
#define XcG_RGB_PART	1
#define XcG_HSL_PART	2
#define XcG_GRAY_PART	3
#ifdef __YCBCR__
#define XcG_YCBCR_PART	4
#define XcG_COUNT_PART	5
#else
#define XcG_COUNT_PART	4
#endif

/*--- define the method for Class Box_color ---*/
typedef struct
{
  F_STD;
} sf_Box_color;

/*--- define the Box_color Class ---*/
typedef struct
{
  sf_Box_color *f;
  Widget	w_This;
  c_ModifList	*list_save;
#ifdef Xc_XALLWRITE
  int		close_reason;
#endif
  c_Color	**color_base;
  c_Color	*v_color_base;
  c_ASLBox	*PBox;
  c_Grid	*Gmain;
  c_Frame       *Fonglet;
  c_Grid	*Gonglet;
  c_ToggleButton *TBonglet[XcG_COUNT_PART];
  c_Frame       *Fmain;
  c_Grid	*G_type[XcG_COUNT_PART];
  c_Grid	*G_void;

  /*--- preview ---*/
  c_Frame	*F_color;
  c_Label	*L_color;

  /*--- CMYK ---*/
  c_Label	*L_C;
  c_Label	*L_M;
  c_Label	*L_Y;
  c_Label	*L_K;
  c_WTextField	*V_C;
  c_WTextField	*V_M;
  c_WTextField	*V_Y;
  c_WTextField	*V_K;
  c_Scale	*S_C;
  c_Scale	*S_M;
  c_Scale	*S_Y;
  c_Scale	*S_K;

  /*--- RGB ---*/
  c_Label	*L_R;
  c_Label	*L_G;
  c_Label	*L_B;
  c_WTextField	*V_R;
  c_WTextField	*V_G;
  c_WTextField	*V_B;
  c_Scale	*S_R;
  c_Scale	*S_G;
  c_Scale	*S_B;

  /*--- HSL ---*/
  c_Label	*L_H;
  c_Label	*L_S;
  c_Label	*L_L;
  c_WTextField	*V_H;
  c_WTextField	*V_S;
  c_WTextField	*V_L;
  c_Scale	*S_H;
  c_Scale	*S_S;
  c_Scale	*S_L;

  /*--- YCbCr ---*/
  c_Label	*L_Y1;
  c_Label	*L_Cb;
  c_Label	*L_Cr;
  c_WTextField	*V_Y1;
  c_WTextField	*V_Cb;
  c_WTextField	*V_Cr;
  c_Scale	*S_Y1;
  c_Scale	*S_Cb;
  c_Scale	*S_Cr;

  /*--- gray ---*/
  c_Label	*L_G1;
  c_WTextField	*V_G1;
  c_Scale	*S_G1;

  rgb_t		rgb_r;

  cmyk_t	cmyk;
  rgb_t		rgb;
  hsl_t		hsl;
  ycbcr_t	ycbcr;
  color_cells_t cell;
  int		current_part;
  boolean	part_created[XcG_COUNT_PART];

} c_Box_color;

extern sf_Box_color fc_Box_color;
extern c_Resource *GlobResources;

#endif
