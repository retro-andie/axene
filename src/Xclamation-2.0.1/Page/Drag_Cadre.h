/*
** Drag_Cadre.h for Xclamation in Page/
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
** Started on  Mon Jul 17 20:23:37 1995 Emmanuel Paris
** Last update Mon Jul 17 20:23:37 1995 Emmanuel Paris
*/


#ifndef _Drag_Cadre_h
#define _Drag_Cadre_h

#include "Page.h"

#define DROP_FOR_MOVE	0
#define DROP_FOR_COPY	1

typedef struct {
  Display	*display;
  Window	root_window;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  c_Cursor	*cursor;
  boolean	accepted;
  boolean	function;
  coord_t	sScale;
  int		x_orig;
  int		y_orig;
  int		x_dest;
  int		y_dest;
} d_Drag_Cadre;

typedef struct {
  void		(*eventlook_objet)();
  void		*objet;
  int		sFactor;
  int		pid;
} d_Drag_data;

extern d_Drag_Cadre	*drag_cadre_init();
extern void 		drag_cadre_abort();
extern void 		drag_cadre_done();
extern void 		drag_cadre_motion();
extern void 		drag_cadre_trace();
extern void 		drag_cadre_move_done();
extern void 		drag_cadre_copy_done();

#endif







