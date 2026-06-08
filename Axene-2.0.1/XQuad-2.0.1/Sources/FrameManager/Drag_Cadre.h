/*
 * Drag_Cadre.h    - definition for Drag_Cadre' functions
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#ifndef _Drag_Cadre_h
#define _Drag_Cadre_h

#include "FrameManager.h"

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







