/*
** BasicGraphStd.h for XQuad in Global/
** Interface between Cadre Class and Objects for display
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
** Started on  Fri Jun 10 15:59:52 1994 LDE X-Calibur-PAO
** Last update Tue Oct  3 11:34:49 1995 Antoine Buat
*/

#ifndef __BASICGRAPH_STD_H__
#define __BASICGRAPH_STD_H__

#include "xcalibur.h"
#include "Xinfo.h"
#include "VectorGraph.h"
#include "PostScript.h"
#include "DocFile.h"
#include "BaseStd.h"
#include "Matrix.h"
#include "vectors.h"
#include "BG_ManageBase.h"

#define BG_AREA_WIDTH		((coord_t)(SCALE_FROM_MILLIMETERS(100)))
#define BG_AREA_HEIGHT		BG_AREA_WIDTH

typedef enum {
  BG_SECTOR = 0,
  BG_BOX,
  BG_LINE,
  BG_POLYGONE,
  BG_TEXT,
  BG_SECTORTEXT,
  BG_POLYLINE
} BG_class_t;

/*--- BG_Point struct ---*/
typedef struct _BG_Point
{
  vector_t		point;
  struct _BG_Point	*NextPoint;
} BG_Point;

#define F_BASICGRAPH_STD \
void (*change_xinfo)(); \
error (*change_base)(); \
void (*display_redraw)(); \
void (*display_handles)(); \
void (*add_to_vectorgraph)(); \
void (*sub_to_vectorgraph)(); \
bbox_t *(*get_bounding_box)(); \
boolean (*is_in)(); \
void (*set)(); \
boolean (*ps_print)(); \
boolean (*read)(); \
boolean (*write)()
     
#if 0
void (*change_xinfo) ___PROTO((void *This, x_info *xinfo)); \
error (*change_base) ___PROTO((void *This, BaseStd_t *new_base)); \
void (*display_redraw) ___PROTO((void *This,c_Matrix *matrix, coord_t scale, \
				 int dx, int dy)); \
void (*display_handles) ___PROTO((void *This,c_Matrix *matrix, coord_t scale, \
				  int dx, int dy)); \
void (*add_to_vectorgraph) ___PROTO((void *This, c_VectorGraph *vectorg)); \
void (*sub_to_vectorgraph) ___PROTO((void *This, c_VectorGraph *vectorg)); \
bbox_t *(*get_bounding_box) ___PROTO((void *This)); \
boolean (*is_in) ___PROTO((void *This, coord_t x, coord_t y)); \
void (*set) ___PROTO((void *This, ...)); \
boolean (*ps_print) ___PROTO((void *This, c_PostScript *post)); \
boolean (*read) ___PROTO((c_DocFile *doc, char *keyword, long param, \
			  BaseStd_t *base)); \
boolean (*write) ___PROTO((void *This, c_DocFile *doc))
#endif     
     
struct basic_graph_std_sf {
  F_STD;
  F_BASICGRAPH_STD;
};

struct basic_graph_std_s {
  struct basic_graph_std_sf *f;
  BG_class_t	type;
  x_info	X_info;
  BaseStd_t	*BaseStd;
};

#define BG_NEW(obj) \
((struct basic_graph_std_s *)(obj))->f->cons
#define BG_DELETE(obj) \
((struct basic_graph_std_s *)(obj))->f->dest
#define BG_COPY(obj) \
((struct basic_graph_std_s *)(obj))->f->copy
#define BG_CHANGE_XINFO(obj) \
((struct basic_graph_std_s *)(obj))->f->change_xinfo
#define BG_CHANGE_BASE(obj) \
((struct basic_graph_std_s *)(obj))->f->change_base
#define BG_DISPLAY_REDRAW(obj) \
((struct basic_graph_std_s *)(obj))->f->display_redraw
#define BG_DISPLAY_HANDLES(obj) \
((struct basic_graph_std_s *)(obj))->f->display_handles
#define BG_ADD_TO_VECTORGRAPH(obj) \
((struct basic_graph_std_s *)(obj))->f->add_to_vectorgraph
#define BG_SUB_TO_VECTORGRAPH(obj) \
((struct basic_graph_std_s *)(obj))->f->sub_to_vectorgraph
#define BG_GET_BOUNDING_BOX(obj) \
((struct basic_graph_std_s *)(obj))->f->get_bounding_box
#define BG_IS_IN(obj) \
((struct basic_graph_std_s *)(obj))->f->is_in
#define BG_SET(obj) \
((struct basic_graph_std_s *)(obj))->f->set
#define BG_PS_PRINT(obj) \
((struct basic_graph_std_s *)(obj))->f->ps_print
#define BG_READ(obj) \
  ((struct basic_graph_std_s *)(obj))->f->read
#define BG_WRITE(obj) \
  ((struct basic_graph_std_s *)(obj))->f->write

#endif /* !__BASICGRAPH_STD_H__ */
