/*
** EditBar.h for XQuad in EditBar/
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
** Started on  Mon Jul 17 13:54:22 1995 Robin Castanier
** Last update Wed Dec 11 18:11:44 1996 Emmanuel Paris
*/

#ifndef _EditBar_h
#define _EditBar_h

typedef struct _c_EditBar c_EditBar;


#include "xcalibur.h"
#include "ManageWidget.h"
#include "Form.h"
#include "Frame.h"
#include "Label.h"
#include "Icon.h"
#include "Grid.h"
#include "WTextField.h"
#include <X11/Intrinsic.h>
#include "Feuille.h"
#include "Moteur.h"

#define MODE_MOVE 0
#define MODE_REGION 1

/* define the method for Class EditBar */
typedef struct
{
  F_STD;
  void (*query_aff)();
  void (*drag_aff)();
  void (*refresh_cell)();
  void (*set_focus)();
  void (*SelectMove)();
  void (*NewSelect)();
  void (*certify)();
  void (*scroll_set)();
  void (*scroll_aff)();
  void (*scroll_reset)();
  void (*equal_callback)();
  void (*clear_bar)();
} sf_EditBar;

/* define the EditBar Class */
struct _c_EditBar
{
 sf_EditBar	*f;
 Widget		w_This;
 c_Form		*Form;
 c_Label	*Position;
 c_WTextField	*Edit;
 c_Feuille	*sel_feuille;
 cell		*sel_cell;
 int		sel_x;
 int		sel_y;
 int		select_len;
 int	        select_s;
 int		mode_sel;
 char		label_str[20];
 boolean	edit_mode;

 Display	*display;
 Window		window;
 GC		gc_scraff;
 int		Width;
 int		Height;
 XFontStruct	*fnt_struct;
 int		font_height;
 
 char		string[Xq_FORMULA_LENGTH + 1];
 c_Icon		*Icon_Ok;
 c_Icon		*Icon_Esc;
 c_Frame       	*FramePos;
 c_Frame       	*FrameButton;
 c_Grid		*RowButton;

 boolean	edited;		/* in edition */
};

extern sf_EditBar fc_EditBar;

#endif















