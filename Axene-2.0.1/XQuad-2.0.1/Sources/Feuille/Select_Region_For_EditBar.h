/*
** Select_Region_For_EditBar.h for XQuad in Feuille/
** Select Region For EditBar
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
** Started on  Thu Apr  6 14:43:15 1995 Emmanuel Paris
** Last update Fri Apr  7 12:57:49 1995 Emmanuel Paris
*/

#ifndef _Select_Region_For_EditBar_h
#define _Select_Region_For_EditBar_h

#include "Feuille.h"
#include <X11/keysym.h>

#define SRF_SELECT_CELL		0
#define SRF_SELECT_ROW		1
#define SRF_SELECT_COLUMN	2
#define SRF_SELECT_ORIGIN	3

/*extern void init_Select_Region_For_EditBar();*/
extern status_t eventlook_Select_Region_For_EditBar();
/*extern void abort_Select_Region_For_EditBar();*/
/*extern void redraw_Select_Region_For_EditBar();*/

extern sf_feuille_function fc_Select_Region_For_EditBar;

typedef struct 
{
  boolean       source;
  boolean	prems;
  boolean	cntrl;
  boolean	outwindow;
  int		function;
  int		xo,yo;
  int		xc,yc;
  int		column,row;
  int		old_column,old_row;
  Time		time;
  region_t	*region;
  region_t	*region_column;
  region_t	*region_row;
  c_AutoScroll	*AutoScroll;
} 
d_Select_Region_For_EditBar;

#endif

