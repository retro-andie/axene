/*
** Cursor_General.h for XAllWrite in Cursor/
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Fri Oct 24 16:16:38 1997 Emmanuel Paris
*/

#ifndef _Cursor_General_
#define _Cursor_General_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_STANDARD = NB_CURSOR,
  C_NORMAL,
  C_SELECT,
  C_MOVE,
  C_COPY,
  C_WAIT,
  C_SIZING,
  C_EDIT_TEXT,
  C_WIN_MOVE,
  C_WIN_BOTTOM_LEFT,
  C_WIN_BOTTOM_RIGHT,
  C_WIN_BOTTOM,
  C_WIN_LEFT,
  C_WIN_RIGHT,
  C_WIN_ICON,
  C_WIN_CLOSE,
  C_WIN_TITLE,
  C_WIN_MAX,
  C_WIN_UNMAX,
  C_MAG_PLUS,
  C_MAG_MINUS,
  LAST_CURSOR_GENERAL
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_CURSOR_GENERAL

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/standard.zpm"
#include "../Pixmaps/arrow.gray.zpm"
#include "../Pixmaps/move.zpm"
#include "../Pixmaps/copy.zpm"
#include "../Pixmaps/edit_text.zpm"
#include "../Pixmaps/zoom.avant.zpm"
#include "../Pixmaps/zoom.arriere.zpm"
#include "../Pixmaps/arrow_top.zpm"
#include "../Pixmaps/arrow_bottom.zpm"

#endif

#define CURSOR_SET_GENERAL \
{ DEFINED_CURSOR_XPM,(void *)standard_xpm,				      \
    STANDARD_CURSOR, (void *)XC_top_left_arrow},	/* C_STANDARD */      \
{ DEFINED_CURSOR_XPM, (void *)arrow_gray_xpm,				      \
    STANDARD_CURSOR, (void *)XC_left_ptr},		/* C_NORMAL */	      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_cross},	/* C_SELECT */	      \
{ DEFINED_CURSOR_XPM, (void *)move_xpm,					      \
    STANDARD_CURSOR, (void *)XC_fleur},			/* C_MOVE */	      \
{ DEFINED_CURSOR_XPM, (void *)copy_xpm,					      \
    STANDARD_CURSOR, (void *)XC_plus },                 /* C_COPY */	      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_watch},	/* C_WAIT */	      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_sizing},	/* C_SIZING */	      \
{ DEFINED_CURSOR_XPM, (void *)edit_text_xpm,				      \
    STANDARD_CURSOR, (void *)XC_xterm },		/* C_EDIT_TEXT */     \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_fleur},	/* C_WIN_MOVE */      \
{ NO_CURSOR, 0,								      \
    STANDARD_CURSOR, (void *)XC_bottom_left_corner}, /* C_WIN_BOTTOM_LEFT */  \
{ NO_CURSOR, 0,								      \
    STANDARD_CURSOR, (void *)XC_bottom_right_corner}, /* C_WIN_BOTTOM_RIGHT */\
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_bottom_side},	/* C_WIN_BOTTOM */    \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_left_side},	/* C_WIN_LEFT */      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_right_side},	/* C_WIN_RIGHT */     \
{ DEFINED_CURSOR_XPM, (void *)arrow_bottom_xpm,				      \
    STANDARD_CURSOR, (void *)XC_icon},			/* C_WIN_ICON */      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_dot},		/* C_WIN_CLOSE */     \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_left_ptr},	/* C_WIN_TITLE */     \
{ DEFINED_CURSOR_XPM, (void *)arrow_top_xpm,				      \
    STANDARD_CURSOR, (void *)XC_based_arrow_up},	/* C_WIN_MAX */       \
{ DEFINED_CURSOR_XPM, (void *)arrow_bottom_xpm,	       			      \
    STANDARD_CURSOR, (void *)XC_based_arrow_down},	/* C_WIN_UNMAX */     \
{ DEFINED_CURSOR_XPM,  (void *)zoom_avant_xpm,				      \
    STANDARD_CURSOR, (void *)XC_draped_box},		/* C_MAG_PLUS */      \
{ DEFINED_CURSOR_XPM,  (void *)zoom_arriere_xpm,			      \
    STANDARD_CURSOR, (void *)XC_diamond_cross},		/* C_MAG_MINUS */     

#endif



