/*
** Keyboard_Box.h for Xclamation and XAllWrite in Keyboard/
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
** Started on  Sat Aug  5 18:26:36 1995 Emmanuel Paris
** Last update Mon Sep 14 17:47:02 1998 Emmanuel Paris
*/

#ifndef _Keyboard_Box_h_
#define	_Keyboard_Box_h_

#include "TextStyle.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "Grid.h"
#include "Label.h"
#include "Frame.h"
#include "ScrolledWindow.h"
#include "DrawingArea.h"

extern void create_Box_Insert ___PROTO((c_Keyboard *This, Widget w_Parent,
					c_TextStyle *style, void (*callback)(),
					void *cb_data));
extern char get_one_char_Box_Insert ___PROTO((c_Keyboard *This, 
					      Widget w_Parent,
					      c_TextStyle *style));

#define BOX_KEYB_DARK "gray30"
#define BOX_KEYB_LIGHT "gray80"
#define BOX_KEYB_NORM "gray65"
#define BOX_KEYB_SELECT "gray60"

typedef struct
{
  Widget        w_This;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  c_Label	*lBKeybTitle;
  c_Label	*lBKeybFontName;
  c_Frame	*fBKeyb;
  c_ScrolledWindow *swBKeyb;
  c_DrawingArea	*drawing;
  
  Display	*display;
  Window	window;
  GC		gc, gc_font, gc_mask;
  
  void		(*callback)();
  void		*cb_data;
  c_TextStyle	*tstyle;
  c_CharMetrics	*metrics;
  c_RasterFont	*rfont;
  int		width, cwidth;
  int		height, cheight;
  int		xs, ys;
  boolean	pressed;
  
  unsigned long		light;		/* drawing color relief */
  unsigned long		dark;
  unsigned long		normal;
  unsigned long		select;
  
  char		got_char;
} keyb_box_t;

#endif





