/*
** Box_styles.h for Xclamation, XQuad and XAllWrite in Box_styles/
** define class Box_styles
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
** Started on  Mon Dec 26 16:34:26 1994 Robin Castanier
** Last update Thu Jun  3 20:11:05 1999 Emmanuel Paris
*/

#ifndef _Box_styles_h
#define _Box_styles_h

#include "xcalibur.h"
#include "WInterface.h"
#include "TextStyle.h"
#include "ASLBox.h"
#include "Grid.h"
#include "Label.h"
#include "Frame.h"
#include "SpinBox.h"
#include "WTextField.h"
#include "Resource.h"
#include "ItemMenu.h"
#include <X11/Intrinsic.h>
#include "ModifList.h"
#include "ColorList.h"
#include "ToggleButton.h"
#include "OptionMenu.h"
#include <Xm/DrawingA.h>
#include <Xm/Text.h>
#include "FontManager.h"
#include "HelpLine.h"

typedef struct
{
 c_Color	*color;
 c_VectorFont	*font;
 coord_t	size;
 unsigned int	attributes;
 scale_t	width_scale;
 scale_t	u_dy;
 scale_t	u_thick;
 c_Color	*u_color;
 enum textstyle_underline_e	u_type;
 boolean	u_over;
 scale_t	s_dy;
 scale_t	s_thick;
 c_Color	*s_color;
 enum textstyle_underline_e	s_type;
 boolean	s_over;
 scale_t	sh_dx;
 scale_t	sh_dy;
 c_Color	*sh_color;
 scale_t	sub_dy;
 scale_t	sub_scale;
 scale_t	sup_dy;
 scale_t	sup_scale;
 c_TextStyle	*style;
} style_modif_t;

/*--- define the method for Class Box_styles ---*/
typedef struct
{
 F_STD;
} sf_Box_styles;

/*--- define the Box_styles Class ---*/
typedef struct
{
 sf_Box_styles *f;
 Widget	w_This;
 c_ModifList	*list_save;
#ifdef Xc_XALLWRITE
 int		close_reason;
#endif
 c_TextStyle	**Style_base;
 c_TextStyle	*v_Style_base;
 c_Color	*colorbase;
 c_ASLBox	*PBox;

 c_Grid	*Gmain;

 c_Frame       *Fonglet;
 c_Grid	*Gonglet;
 c_ToggleButton *TBglobal;
 c_ToggleButton *TBconfig;

 c_Frame       *Fmain;

 /* grid global */
 c_Grid	*Gglobal;

 c_Label	*Lfamily;
 c_ItemMenu    *IMfont;

 c_ColorList	*CLfont;
 c_Label	*Lfontsize;
 c_SpinBox	*SBfontsize;

 c_Icon	*Ibold;
 c_Icon	*Iitalic;
 c_Icon	*Ishadow;
 c_Icon	*Iunderline;
 c_Icon	*Istrikeout;

 c_ToggleButton	*Tsupscript;
 c_ToggleButton	*Tsubscript;
 c_ToggleButton	*Tsmallcaps;
 c_ToggleButton	*Tbigcaps;
 c_ToggleButton	*Texpand;

 c_Frame	*Foverview;
 /*c_Label	*Loverview;*/
 Widget		w_TextOverview;

 /* grid config */
 c_Grid	*Gconfig;

 /* underline */
 c_Frame	*Funderline;
 c_Grid		*RCunderline;
 c_Label	*Lun_dy;
 c_WTextField	*TFun_dy;
 c_Label	*Lun_thickness;
 c_WTextField	*TFun_thickness;
 c_ColorList	*CLunderline;
 c_OptionMenu	*OMun_type;

 /* strikeout */
 c_Frame	*Fstrikeout;
 c_Grid		*RCstrikeout;
 c_Label	*Lst_dy;
 c_WTextField	*TFst_dy;
 c_Label	*Lst_thickness;
 c_WTextField	*TFst_thickness;
 c_ColorList	*CLstrikeout;
 c_OptionMenu	*OMst_type;

 /* width */
 c_Frame	*Fwidth;
 c_Grid		*RCwidth;
 c_Label	*Lwidth;
 c_WTextField	*TFwidth;

 /* shadow */
 c_Frame	*Fshadow;
 c_Grid		*RCshadow;
 c_Label	*LshadowX;
 c_Label	*LshadowY;
 c_WTextField	*TFsh_dx;
 c_WTextField	*TFsh_dy;
 c_ColorList	*CLshadow;

 /* subscript */
 c_Frame	*Fsubscript;
 c_Label	*Lsuby;
 c_Label	*Lsubscale;
 c_Grid		*RCsubscript;
 c_WTextField	*TFsub_dy;
 c_WTextField	*TFsub_scale;

 /* supscript */
 c_Frame	*Fsupscript;
 c_Label	*Lsupy;
 c_Label	*Lsupscale;
 c_Grid		*RCsupscript;
 c_WTextField	*TFsup_dy;
 c_WTextField	*TFsup_scale;

 /* preview */
 c_Frame	*Fpreview;
 Widget		WDApreview;

 style_modif_t	modif;
#if defined(Xc_XQUAD) || defined(Xc_XALLWRITE)
 c_TextStyle	**retptr;
#endif
 int		current_part;
 boolean	part2_created;

} c_Box_styles;

extern sf_Box_styles fc_Box_styles;
extern c_Resource *GlobResources;
extern c_HelpLine *GlobHelpLine;

#endif
