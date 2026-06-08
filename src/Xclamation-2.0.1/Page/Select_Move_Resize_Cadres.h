/*
** Select_Move_Resize_Cadres.h for Xclamation in Page/
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
** Started on  Mon Jul 17 19:52:46 1995 Emmanuel Paris
** Last update Thu Apr 23 16:38:54 1998 Emmanuel Paris
*/

#ifndef _Select_Move_Resize_Cadres_h
#define _Select_Move_Resize_Cadres_h

#include "Page.h"
#include <X11/keysym.h>
#include "Drag_Cadre.h"

#define SMS_SELECT_CADRE_IN_ZONE	0
#define SMS_SELECT_ONE_CADRE		1
#define	SMS_SELECT_CADRE		2
#define SMS_DESELECT_CADRE		3
#define	SMS_DESELECT_ALL_CADRE		4
#define SMS_MOVE_CADRE			5
#define SMS_COPY_CADRE			6
#define SMS_RESIZE_CADRE		7
#define SMS_MOVE_POINT			8
#define SMS_MOVE_LINE			9
#define SMS_MOVE_ALIGN_MARK		10

extern void init_Select_Move_Resize_Cadre();
extern status_t eventlook_Select_Move_Resize_Cadre();
extern void abort_Select_Move_Resize_Cadre();
extern void redraw_Select_Move_Resize_Cadre();

extern sf_page_function fc_Select_Move_Resize_Cadre;

typedef struct _l_Lpoint
{
 l_Point		*lpoint;
 struct _l_Lpoint	*NextLpoint;
} l_Lpoint;

typedef struct 
{
 boolean       source;
 boolean	prems;
 boolean	shift;
 boolean	outwindow;
 boolean	pressed;
 boolean	change_xfactor;
 boolean	change_yfactor;
 int		function;
 int		xo,yo;
 coord_t	sxo,syo;
 int		xc,yc;
 coord_t	sxc,syc;
 coord_t	xcent;
 coord_t	ycent;
 coord_t	xorig;
 coord_t	yorig;
 real		xfactor;
 real		yfactor;
 c_Cadre	*cadre;
 l_Cadre	*lcadre;
 l_Point	**retpnt;
 l_Point	*point_d;
 l_Point	*point_e;
 l_Point	*svglistp;
 l_Lpoint	*svgllistp;
 Time		time;
 Time		sel_time;
 d_Drag_Cadre	*drag;
 void		*vhline;
 int		orient;
} 
d_Select_Move_Resize_Cadre;

#endif



