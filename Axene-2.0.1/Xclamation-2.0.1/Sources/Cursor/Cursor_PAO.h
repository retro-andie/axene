/*
** Cursor_PAO.h for Xclamation in Cursor/
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
** Started on  Mon Jul 17 14:20:58 1995 Emmanuel Paris
** Last update Wed Jun  4 17:32:22 1997 Emmanuel Paris
*/

#ifndef _Cursor_PAO_
#define _Cursor_PAO_

#ifndef NB_CURSOR
#define NB_CURSOR 0
#endif

enum {
  C_GET_POINT  =  NB_CURSOR,
  C_GET_LINE,
  C_SELECT_CADRE,
  C_DESELECT_CADRE,
  C_LOCK_CADRE,
  C_ROTATE_CENTER,
  C_SCALE,
  C_CONNECT_POINT,
  C_END_MARK,
  C_CREATE_RECTANGLE,
  C_CREATE_SQUARE,
  C_CREATE_ELIPSE,
  C_CREATE_CIRCLE,
  C_CREATE_POLYGONE,
  C_ARROW_MOVE,
  C_CIRCLE_DEL,
  C_CROSS_DEL,
  C_SQUARE_ADD,
  C_CIRCLE_ADD,
  C_CROSS_ADD,
  C_SELECT_ZONE,
  C_ROTATE_POINTER,
  C_ZOOM_IN_ZONE,
  C_HAND_OPEN,
  C_HAND_CLOSE,
  C_SCALE_POINTER,
  C_LINK_FRAME_SOURCE,
  C_LINK_BEFORE,
  C_LINK_AFTER,
  C_LINK_ATBEGIN,
  C_LINK_ATEND,
  C_HELP_ASK,
  LAST_PAO_CURSOR
};

#undef NB_CURSOR
#define	NB_CURSOR LAST_PAO_CURSOR

#ifdef _PRIVATE_CURSOR_

#include "../Pixmaps/verrou.zpm"
#include "../Pixmaps/scale.zpm"
#include "../Pixmaps/scale.axe.zpm"
#include "../Pixmaps/connect.zpm"
#include "../Pixmaps/connect.end.zpm"
#include "../Pixmaps/connect.end2.zpm"
#include "../Pixmaps/rotation.zpm"
#include "../Pixmaps/cross_rect.zpm"
#include "../Pixmaps/cross_carre.zpm"
#include "../Pixmaps/cross_oval.zpm"
#include "../Pixmaps/cross_circ.zpm"
#include "../Pixmaps/cross_poly.zpm"
#include "../Pixmaps/arrow.zpm"
#include "../Pixmaps/move.segment.zpm"
#include "../Pixmaps/arrow_move.zpm"
#include "../Pixmaps/delete.point.found.zpm"
#include "../Pixmaps/delete.point.zpm"
#include "../Pixmaps/add.point.alone.zpm"
#include "../Pixmaps/add.point.online.zpm"
#include "../Pixmaps/add.point.zpm"
#include "../Pixmaps/arrow.red.zpm"
#include "../Pixmaps/rotation.axe.zpm"
#include "../Pixmaps/hand.open.zpm"
#include "../Pixmaps/hand.close.zpm"
#include "../Pixmaps/link.frame.source.zpm"
#include "../Pixmaps/link.before.zpm"
#include "../Pixmaps/link.after.zpm"
#include "../Pixmaps/link.atbegin.zpm"
#include "../Pixmaps/link.atend.zpm"
#include "../Pixmaps/xmayday.ask.zpm"

#endif

#define CURSOR_SET_PAO \
  { NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_hand1},	 /* C_GET_POINT */    \
{ DEFINED_CURSOR_XPM,(void *)move_segment_xpm,				      \
    STANDARD_CURSOR, (void *)XC_circle},		 /* C_GET_LINE */     \
{ DEFINED_CURSOR_XPM,(void *)arrow_xpm,					      \
    STANDARD_CURSOR, (void *)XC_target},	 /* C_SELECT_CADRE */	      \
{ NO_CURSOR,0,STANDARD_CURSOR, (void *)XC_dot},	 /* C_DESELECT_CADRE */	      \
{ NO_CURSOR,0,DEFINED_CURSOR_XPM,  (void *)verrou_xpm},	/* C_LOCK */	      \
{ DEFINED_CURSOR_XPM,  (void *)rotation_xpm,				      \
    STANDARD_CURSOR, (void *)XC_exchange},		/* C_ROTATE_CENTER*/  \
{ NO_CURSOR,0,DEFINED_CURSOR_XPM, (void *)scale_xpm},   /* C_SCALE */	      \
{ NO_CURSOR,0,DEFINED_CURSOR_XPM, (void *)connect_xpm}, /* C_CONNECT_POINT*/  \
{ DEFINED_CURSOR_XPM,connect_end_xpm,					      \
    DEFINED_CURSOR_XPM, (void *)connect_end2_xpm},	    /* C_END_MARK */  \
{ DEFINED_CURSOR_XPM, (void *)cross_rect_xpm,				      \
      STANDARD_CURSOR, (void *)XC_crosshair},	/* C_CREATE_RECTANGLE */      \
{ DEFINED_CURSOR_XPM, (void *)cross_carre_xpm,				      \
    STANDARD_CURSOR, (void *)XC_crosshair},	       /* C_CREATE_SQUARE */  \
{ DEFINED_CURSOR_XPM, (void *)cross_oval_xpm,				      \
    STANDARD_CURSOR, (void *)XC_crosshair},	       /* C_CREATE_ELIPSE */  \
{ DEFINED_CURSOR_XPM, (void *)cross_circ_xpm,				      \
    STANDARD_CURSOR, (void *)XC_crosshair},	       /* C_CREATE_CIRCLE */  \
{ DEFINED_CURSOR_XPM, (void *)cross_poly_xpm,				      \
    STANDARD_CURSOR, (void *)XC_crosshair},	       /* C_CREATE_POLYGONE */\
{ DEFINED_CURSOR_XPM, (void *)arrow_move_xpm,				      \
    STANDARD_CURSOR, (void *)XC_fleur},	       /* C_ARROW_MOVE */	      \
{ DEFINED_CURSOR_XPM, (void *)delete_point_found_xpm,			      \
    STANDARD_CURSOR, (void *)XC_circle},	       /* C_CIRCLE_DEL */     \
{ DEFINED_CURSOR_XPM, (void *)delete_point_xpm,				      \
    STANDARD_CURSOR, (void *)XC_pirate},	       /* C_CROSS_DEL */      \
{ DEFINED_CURSOR_XPM, (void *)add_point_alone_xpm,			      \
    STANDARD_CURSOR, (void *)XC_dotbox},	       /* C_SQUARE_ADD */     \
{ DEFINED_CURSOR_XPM, (void *)add_point_online_xpm,			      \
    STANDARD_CURSOR, (void *)XC_circle},	       /* C_CIRCLE_ADD */     \
{ DEFINED_CURSOR_XPM, (void *)add_point_xpm,				      \
    STANDARD_CURSOR, (void *)XC_crosshair},	       /* C_CROSS_ADD */      \
{ DEFINED_CURSOR_XPM,(void *)arrow_red_xpm,				      \
    STANDARD_CURSOR, (void *)XC_left_ptr},	       /* C_SELECT_ZONE */    \
{ DEFINED_CURSOR_XPM,(void *)rotation_axe_xpm,				      \
    STANDARD_CURSOR, (void *)XC_exchange},	       /* C_ROTATE_POINTER */ \
{ DEFINED_CURSOR_XPM, (void *)cross_rect_xpm,				      \
    STANDARD_CURSOR, (void *)XC_crosshair},	       /* C_ZOOM_IN_ZONE */   \
{ DEFINED_CURSOR_XPM, (void *)hand_open_xpm,				      \
    STANDARD_CURSOR, (void *)XC_sizing},	       /* C_HAND_OPEN */      \
{ DEFINED_CURSOR_XPM, (void *)hand_close_xpm,				      \
    STANDARD_CURSOR, (void *)XC_sizing},	       /* C_HAND_CLOSE */     \
{ DEFINED_CURSOR_XPM, (void *)scale_axe_xpm,				      \
    STANDARD_CURSOR, (void *)XC_fleur},	       /* C_SCALE_POINTER */	      \
{ DEFINED_CURSOR_XPM, link_frame_source_xpm,				      \
    STANDARD_CURSOR, (void *)XC_double_arrow },   /* C_LINK_FRAME_SOURCE */   \
{ DEFINED_CURSOR_XPM, link_before_xpm,					      \
    STANDARD_CURSOR, (void *)XC_bottom_side },      /* C_LINK_BEFORE */	      \
{ DEFINED_CURSOR_XPM, link_after_xpm,					      \
    STANDARD_CURSOR, (void *)XC_top_side },         /* C_LINK_AFTER */	      \
{ DEFINED_CURSOR_XPM, link_atbegin_xpm,					      \
    STANDARD_CURSOR, (void *)XC_right_side },      /* C_LINK_ATBEGIN */	      \
{ DEFINED_CURSOR_XPM, link_atend_xpm,					      \
    STANDARD_CURSOR, (void *)XC_left_side },         /* C_LINK_ATEND */       \
{ DEFINED_CURSOR_XPM, (void *)xmayday_ask_xpm,				      \
    STANDARD_CURSOR, (void *)XC_question_arrow },	/* C_HELP_ASK */

#endif










