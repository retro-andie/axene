/*
** Cursor_Tableur.h for XQuad in Cursor/
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
** Last update Wed Jun  4 18:48:21 1997 Emmanuel Paris
*/

#ifndef _Cursor_Tableur_
#define _Cursor_Tableur_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_LOCK_CADRE = NB_CURSOR,
  C_SELECT_CELLS,
  C_ARROW_MOVE,
  C_RESIZE_COL,
  C_RESIZE_ROW,
  C_SELECT_ALL,
  C_SELECT_ROW,
  C_SELECT_COL,
  C_CREATE_RECTANGLE,
  C_CREATE_SQUARE,
  C_SELECT_CADRE,
  C_DESELECT_CADRE,
  C_SPREAD_LEFT_REGION,
  C_SPREAD_RIGHT_REGION,
  C_SPREAD_UP_REGION,
  C_SPREAD_DOWN_REGION,
  C_SPREAD_H_CENTER_REGION,
  C_SPREAD_V_CENTER_REGION,
  C_COPY_REGION,
  C_MOVE_REGION,
  C_RESIZE_COL_LEFT,
  C_RESIZE_COL_RIGHT,
  C_RESIZE_ROW_UP,
  C_RESIZE_ROW_DOWN,
  C_HELP_ASK,
  LAST_TABLEUR_CURSOR
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_TABLEUR_CURSOR

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/verrou.zpm"
#include "../Pixmaps/arrow.zpm"
#include "../Pixmaps/arrow_move.zpm"
#include "../Pixmaps/resize_col.zpm"
#include "../Pixmaps/resize_col_right.zpm"
#include "../Pixmaps/resize_row.zpm"
#include "../Pixmaps/resize_row_bottom.zpm"
#include "../Pixmaps/select_cells.zpm"
#include "../Pixmaps/select_all.zpm"
#include "../Pixmaps/select_col.zpm"
#include "../Pixmaps/select_row.zpm"
#include "../Pixmaps/cross_rect.zpm"
#include "../Pixmaps/cross_carre.zpm"
#include "../Pixmaps/spread.left.zpm"
#include "../Pixmaps/spread.right.zpm"
#include "../Pixmaps/spread.up.zpm"
#include "../Pixmaps/spread.down.zpm"
#include "../Pixmaps/spread.h.center.zpm"
#include "../Pixmaps/spread.v.center.zpm"
#include "../Pixmaps/select_copy.zpm"
#include "../Pixmaps/select_move.zpm"
#include "../Pixmaps/xmayday.ask.zpm"

#endif

#define CURSOR_SET_TABLEUR \
{ NO_CURSOR,0,DEFINED_CURSOR_XPM,  (void *)verrou_xpm},  /* C_LOCK */	      \
{ DEFINED_CURSOR_XPM, select_cells_xpm,					      \
    STANDARD_CURSOR, (void *)XC_icon},		 /* C_SELECT_CELLS */	      \
{ DEFINED_CURSOR_XPM, arrow_move_xpm,					      \
    STANDARD_CURSOR, (void *)XC_fleur},		  /* C_ARROW_MOVE */	      \
{ DEFINED_CURSOR_XPM, resize_col_xpm,					      \
    STANDARD_CURSOR, (void *)XC_sb_h_double_arrow},  /* C_RESIZE_COL */	      \
{ DEFINED_CURSOR_XPM, resize_row_xpm,					      \
    STANDARD_CURSOR, (void *)XC_sb_v_double_arrow},  /* C_RESIZE_ROW */	      \
{ DEFINED_CURSOR_XPM, select_all_xpm,					      \
    STANDARD_CURSOR, (void *)XC_icon},		   /* C_SELECT_ALL */	      \
{ DEFINED_CURSOR_XPM, select_row_xpm,					      \
    STANDARD_CURSOR, (void *)XC_icon},	 	   /* C_SELECT_ROW */	      \
{ DEFINED_CURSOR_XPM, select_col_xpm,					      \
    STANDARD_CURSOR, (void *)XC_icon},	 	   /* C_SELECT_COL */	      \
{ DEFINED_CURSOR_XPM, cross_rect_xpm,					      \
    STANDARD_CURSOR, (void *)XC_crosshair},	   /* C_CREATE_RECTANGLE */   \
{ DEFINED_CURSOR_XPM, cross_carre_xpm,					      \
    STANDARD_CURSOR, (void *)XC_crosshair},	   /* C_CREATE_SQUARE */      \
{ DEFINED_CURSOR_XPM, arrow_xpm,					      \
    STANDARD_CURSOR, (void *)XC_target},	   /* C_SELECT_CADRE */	      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_dot},	   /* C_DESELECT_CADRE */     \
{ DEFINED_CURSOR_XPM, spread_left_xpm,					      \
    STANDARD_CURSOR, (void *)XC_left_side},	   /*C_SPREAD_LEFT_REGION */  \
{ DEFINED_CURSOR_XPM, spread_right_xpm,					      \
    STANDARD_CURSOR, (void *)XC_right_side},	   /*C_SPREAD_RIGHT_REGION */ \
{ DEFINED_CURSOR_XPM, spread_up_xpm,					      \
    STANDARD_CURSOR, (void *)XC_top_side},	   /*C_SPREAD_UP_REGION */    \
{ DEFINED_CURSOR_XPM, spread_down_xpm,					      \
    STANDARD_CURSOR, (void *)XC_bottom_side},	   /*C_SPREAD_DOWN_REGION */  \
{ DEFINED_CURSOR_XPM, spread_h_center_xpm,				      \
    STANDARD_CURSOR, (void *)XC_sb_h_double_arrow}, /*C_SPREAD_H_CENTER_REG*/ \
{ DEFINED_CURSOR_XPM, spread_v_center_xpm,				      \
    STANDARD_CURSOR, (void *)XC_sb_v_double_arrow}, /*C_SPREAD_V_CENTER_REG*/ \
{ DEFINED_CURSOR_XPM, select_copy_xpm,					      \
    STANDARD_CURSOR, (void *)XC_exchange},	   /*C_COPY_REGION */	      \
{ DEFINED_CURSOR_XPM, select_move_xpm,					      \
    STANDARD_CURSOR, (void *)XC_exchange},	   /*C_MOVE_REGION */	      \
{ DEFINED_CURSOR_XPM, resize_col_xpm,					      \
    STANDARD_CURSOR, (void *)XC_sb_h_double_arrow},  /* C_RESIZE_COL_LEFT */  \
{ DEFINED_CURSOR_XPM, resize_col_right_xpm,		       		      \
    STANDARD_CURSOR, (void *)XC_sb_h_double_arrow},  /* C_RESIZE_COL_RIGHT */ \
{ DEFINED_CURSOR_XPM, resize_row_xpm,					      \
    STANDARD_CURSOR, (void *)XC_sb_v_double_arrow},  /* C_RESIZE_ROW_UP */    \
{ DEFINED_CURSOR_XPM, resize_row_bottom_xpm,   				      \
    STANDARD_CURSOR, (void *)XC_sb_v_double_arrow},  /* C_RESIZE_ROW_DOWN */  \
{ DEFINED_CURSOR_XPM, (void *)xmayday_ask_xpm,				      \
    STANDARD_CURSOR, (void *)XC_question_arrow },	/* C_HELP_ASK */


#endif






