/*
** Select_Move_Edit_Text.h for XAllWrite in Page/
** Select, move and edit text.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Jun 16 13:53:08 1997 Emmanuel Paris
** Last update Thu Apr 23 16:38:56 1998 Emmanuel Paris
*/

#ifndef _Select_Move_Edit_h_
#define _Select_Move_Edit_h_

#include "Page.h"
#include <X11/keysym.h>
#include "Drag_Cadre.h"

#define SMS_NO_FUNCTION		0
#define SMS_SELECT_TEXT		1
#define SMS_SELECT_TEXT_LINE	2
#define SMS_MOVE_TEXT		3
#define SMS_COPY_TEXT		4
#define SMS_SELECT_ONE_CADRE	5
#define	SMS_SELECT_CADRE	6
#define SMS_DESELECT_CADRE	7
#define	SMS_DESELECT_ALL_CADRE	8
#define SMS_MOVE_CADRE		9
#define SMS_COPY_CADRE		10
#define SMS_RESIZE_CADRE	11
#define SMS_MOVE_POINT		12
#define SMS_MOVE_LINE		13

extern void init_Select_Move_Edit_Text();
extern status_t eventlook_Select_Move_Edit_Text();
extern void abort_Select_Move_Edit_Text();
extern void redraw_Select_Move_Edit_Text();

extern sf_page_function fc_Select_Move_Edit_Text;

typedef struct _l_Lpoint
{
  l_Point		*lpoint;
  struct _l_Lpoint	*NextLpoint;
} l_Lpoint;

typedef struct 
{
 boolean	source;
 boolean	prems;
 boolean	shift;
 boolean	cntrl;
 boolean	pressed;
 boolean	outwindow;
 int		function;
 Time		time;
 Time		sel_time;
 c_AutoScroll	*AutoScroll;
 l_Cadre	*lcadre;
 l_Point	**retpnt;
 l_Point	*point_d;
 l_Point	*point_e;
 l_Point	*svglistp;
 l_Lpoint	*svgllistp;
 xa_item_t	*select_start;
 int		select_start_pos;
 xa_item_t	*select_end;
 int		select_end_pos;
 boolean	start_end;	/* true if cursor is on start pos */
 text_line_t	*stl;
 coord_t	sx;
 int		nb_click;
 int		xo, yo;
 int		xc, yc;
 coord_t	xorig;
 coord_t	yorig;
 coord_t	xcent;
 coord_t	ycent;
 boolean	change_xfactor;
 boolean	change_yfactor;
 real		xfactor;
 real		yfactor;
 coord_t	sxo,syo;
 coord_t	sxc,syc;
 d_Drag_Cadre	*drag;
} 
d_Select_Move_Edit_Text;

#endif /* _Select_Move_Edit_h_ */
