/*
** Select_Move_Resize_Cells.h for XQuad in Feuille/
** definition for Select_Move_Resize' functions
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
** Started on  Mon Sep 18 16:28:10 1995 Emmanuel Paris
** Last update Sat Jan  8 16:06:11 2000 Emmanuel Paris
*/

#ifndef _Select_Move_Resize_Cells_h
#define _Select_Move_Resize_Cells_h

#include "Feuille.h"
#include <X11/keysym.h>
#include "Drag_Cadre.h"

#define SMS_SELECT_CELL		0
#define SMS_SELECT_ROW		1
#define SMS_SELECT_COLUMN	2
#define SMS_SELECT_ORIGIN	3
#define SMS_RESIZE_COLUMN	4
#define SMS_RESIZE_ROW		5
#define SMS_SELECT_ONE_CADRE	6
#define	SMS_SELECT_CADRE	7
#define SMS_DESELECT_CADRE	8
#define	SMS_DESELECT_ALL_CADRE	9
#define SMS_MOVE_CADRE		10
#define SMS_COPY_CADRE		11
#define SMS_RESIZE_CADRE	12
#define SMS_SPREAD_H_REGION	13
#define SMS_SPREAD_V_REGION	14
#define SMS_COPY_REGION		15
#define SMS_INSERT_H_REGION	16
#define SMS_INSERT_V_REGION	17

/*extern void init_Select_Move_Resize_Cell();*/
extern status_t eventlook_Select_Move_Resize_Cell();
/*extern void abort_Select_Move_Resize_Cell();*/
/*extern void redraw_Select_Move_Resize_Cell();*/

extern sf_feuille_function fc_Select_Move_Resize_Cell;

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
  boolean	cntrl;
  boolean	outwindow;
  boolean	pressed;
  int		function;
  int		xo,yo;
  int		xc,yc;
  int		column,row;
  int		old_column,old_row;
  Time		time;
  region_t	region;
  region_t	region_column;
  region_t	region_row;
  c_AutoScroll	*AutoScroll;
  l_Cadre	*lcadre;
  l_Lpoint	*svgllistp;
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
d_Select_Move_Resize_Cell;

#endif



