/*
** HighGraphStd.h for XQuad in Global/
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
** Started on  Fri Jun 10 15:59:52 1994 Emmanuel Paris
** Last update Tue Oct  3 11:34:33 1995 Antoine Buat
*/

#ifndef __HIGHGRAPH_STD_H__
#define __HIGHGRAPH_STD_H__

#include "xcalibur.h"
#include "Xinfo.h"
#include "PostScript.h"
#include "DocFile.h"
#include "BaseStd.h"
#include "Matrix.h"
#include "vectors.h"


/*--- HighGraph types ---*/
typedef enum {
  HG_NONE = 0,
  HG_BARS,
  HG_HISTOGRAMME,
  HG_SECTOR,
  HG_SURFACE,
  HG_CURVE,
  HG_RADAR,
  HG_LEGEND,
  HG_TITLE,
  HG_AXE
} HG_class_t;


#include "BasicGraphStd.h"
#include "High_Graph.h"

#define F_HIGHGRAPH_STD \
void (*regenerate)(); \
void (*change_xinfo)(); \
error (*change_base)(); \
void (*display_redraw)(); \
void (*add_to_vectorgraph)(); \
void (*sub_to_vectorgraph)(); \
void (*set)(); \
void (*ps_print)(); \
boolean (*read)(); \
boolean (*write)()

#if 0
void (*regenerate) ___PROTO((void *This));
void (*change_xinfo) ___PROTO((void *This, x_info *xinfo)); \
error (*change_base) ___PROTO((void *This, BaseStd_t *new_base)); \
void (*display_redraw) ___PROTO((void *This));
void (*add_to_vectorgraph) ___PROTO((void *This, c_VectorGraph *vectorg)); \
void (*sub_to_vectorgraph) ___PROTO((void *This, c_VectorGraph *vectorg)); \
void (*set) ___PROTO((void *This, ...)); \
void (*ps_print) ___PROTO((void *This, c_PostScript *post)); \
boolean (*read) ___PROTO((c_DocFile *doc, char *keyword, long param, \
			  BaseStd_t *base)); \
boolean (*write) ___PROTO((void *This, c_DocFile *doc))
#endif

#define _vectorg (This->highg->vectorg)
#define _BaseStd (This->highg->BaseStd)

struct high_graph_std_sf {
  F_STD;
  F_HIGHGRAPH_STD;
};

struct high_graph_std_s {
  struct high_graph_std_sf *f;
  c_HighGraph	*highg;
  HG_class_t	 class;
  int sub_class;
  
};

#define HG_NEW(obj) \
((struct high_graph_std_s *)(obj))->f->cons
#define HG_DELETE(obj) \
((struct high_graph_std_s *)(obj))->f->dest
#define HG_COPY(obj) \
((struct high_graph_std_s *)(obj))->f->copy
#define HG_REGENERATE(obj) \
((struct high_graph_std_s *)(obj))->f->regenerate
#define HG_CHANGE_XINFO(obj) \
((struct high_graph_std_s *)(obj))->f->change_xinfo
#define HG_CHANGE_BASE(obj) \
((struct high_graph_std_s *)(obj))->f->change_base
#define HG_DISPLAY_REDRAW(obj) \
((struct high_graph_std_s *)(obj))->f->display_redraw
#define HG_ADD_TO_VECTORGRAPH(obj) \
((struct high_graph_std_s *)(obj))->f->add_to_vectorgraph
#define HG_SUB_TO_VECTORGRAPH(obj) \
((struct high_graph_std_s *)(obj))->f->sub_to_vectorgraph
#define HG_SET(obj) \
((struct high_graph_std_s *)(obj))->f->set
#define HG_PS_PRINT(obj) \
((struct high_graph_std_s *)(obj))->f->ps_print
#define HG_READ(obj) \
  ((struct high_graph_std_s *)(obj))->f->read
#define HG_WRITE(obj) \
  ((struct high_graph_std_s *)(obj))->f->write

#endif /* !__HIGHGRAPH_STD_H__ */
