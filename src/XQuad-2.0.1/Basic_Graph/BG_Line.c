/*
** BG_Line.c for XQuad in Basic_Graph/
**
** Copyright (C) 1995-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Sat Jun 24 18:07:33 1995 Emmanuel Paris
** Last update Wed May 29 15:39:49 1996 One of the authors
*/

#define NTRACE
#define NHISTORY

#include "BG_Line.h"
#include <stdarg.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_Line *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_Line *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_Line *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_Line *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_Line *This, 
					 c_VectorGraph *vectorg));
static void sub_to_vectorgraph ___PROTO((c_BG_Line *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_Line *This));
static boolean is_in ___PROTO((c_BG_Line *This, coord_t x, coord_t y));
static void setBG ___PROTO((c_BG_Line *This, ...));
static void vset();
static void calcul_Line ___PROTO((c_BG_Line *This));
static boolean ps_print ___PROTO((c_BG_Line *This, c_PostScript *post));
static boolean readBG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_Line *This, c_DocFile *doc));

sf_BG_Line fc_BG_Line =
{
  cons, dest, copy,
  change_xinfo, change_base, display_redraw, display_handles,
  add_to_vectorgraph, sub_to_vectorgraph, get_bounding_box, is_in, setBG,
  ps_print, readBG, writeBG
};

static void *cons(base)
BaseStd_t	*base;
{
  c_BG_Line	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("BG_Line", sizeof(c_BG_Line)) ) == NULL)
    return NULL;
  This->f = &fc_BG_Line;
  
  This->type = BG_LINE;
  This->BaseStd = base;
  
  This->point1.dx = This->point1.dy = 
    This->point2.dx = This->point2.dy = (coord_t)0;
  This->fg_color = Get_BG_Color(base, BGC_BLACK);
  This->bg_color = Get_BG_Color(base, BGC_WHITE);
  This->line_thickness = SCALE_FROM_POINTS(0.5);
  calcul_Line(This);
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_BG_Line	*This;
{
  Xc_HISTORY(("destructor")); 
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_BG_Line	*This;
BaseStd_t	*base;
{
  c_BG_Line	*bg_copy;
  
  if ( (bg_copy = Xc_malloc("BG_Line", sizeof(c_BG_Line)) ) == NULL)
    return NULL;
  memcpy(bg_copy, This, sizeof(c_BG_Line));
  
  if (bg_copy->BaseStd != base)
    BG_CHANGE_BASE(bg_copy)(bg_copy, base);
  return bg_copy;
}

static void change_xinfo(This, xinfo)
c_BG_Line	*This;
x_info		*xinfo;
{
  memcpy(&This->X_info, xinfo, sizeof(x_info));
}

static error change_base(This, new_base)
c_BG_Line	*This;
BaseStd_t	*new_base;
{
  This->BaseStd = new_base;
  return XC_NO_ERROR;
}

static void display_redraw(This, matrix, scale, dx, dy)
c_BG_Line	*This;
c_Matrix	*matrix;
coord_t		scale;
int		dx;
int		dy;
{
  Display	*display;
  Window	window;
  GC		gc;
  XGCValues	val_gc;
  matrix_t	*m;
  int		x1, x2, y1, y2;
  int		linet;
  
  display = _display;
  window = _window;
  gc = _gc_inside;
  m = F(matrix).get(matrix, 0);
  
  val_gc.function = GXcopy;
  val_gc.foreground = This->fg_color->cell.pixel;
  val_gc.background = This->bg_color->cell.pixel;
  linet = This->line_thickness / scale;
  val_gc.line_width = (linet == 1 ? 0 : linet);
  XChangeGC(display, gc, GCForeground | GCBackground | GCLineWidth
	    | GCFunction /* | GCFillStyle */ ,&val_gc);
  
  x1 = (int)XcM_X(m, This->point1.dx, This->point1.dy) + dx;
  y1 = (int)XcM_Y(m, This->point1.dx, This->point1.dy) + dy;
  x2 = (int)XcM_X(m, This->point2.dx, This->point2.dy) + dx;
  y2 = (int)XcM_Y(m, This->point2.dx, This->point2.dy) + dy;
  XDrawLine(display, window, gc, x1, y1, x2, y2);
}

static void display_handles(This, matrix, scale, dx, dy)
c_BG_Line	*This;
c_Matrix	*matrix;
coord_t		scale;
int		dx;
int		dy;
{
  Display	*display;
  Window	window;
  GC		gc;
  
  display = _display;
  window = _window;
  gc = _gc_inside;
}

static void add_to_vectorgraph(This, vectorg)
c_BG_Line  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).addBasicGraph(vectorg, This);
}

static void sub_to_vectorgraph(This, vectorg)
c_BG_Line  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).subBasicGraph(vectorg, This);
}

static bbox_t *get_bounding_box(This)
c_BG_Line *This;
{
  return &(This->bbox);
}

static boolean is_in(This, x, y)
c_BG_Line	*This;
coord_t		x;
coord_t		y;
{
  if (x >= This->bbox.llx && x <= This->bbox.urx &&
      y >= This->bbox.lly && y <= This->bbox.ury)
    return TRUE;
  return FALSE;
}

static void setBG(c_BG_Line *This, ...)
{
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, This);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_BG_Line *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, BG_Line_set_code_t))
    {
    case XcBG_Line_POINT1:
      Xc_TRACE(("XcBG_Line_POINT1"));
      This->point1 = va_arg(ap, vector_t);
      calcul = TRUE;
      break;
    case XcBG_Line_POINT2:
      Xc_TRACE(("XcBG_Line_POINT2"));
      This->point2 = va_arg(ap, vector_t);
      calcul = TRUE;
      break;
    case XcBG_Line_FG_COLOR:
      Xc_TRACE(("XcBG_Line_FG_COLOR"));
      This->fg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Line_BG_COLOR:
      Xc_TRACE(("XcBG_Line_BG_COLOR"));
      This->bg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Line_DASHES:
      Xc_TRACE(("XcBG_Line_DASHES"));
      /* This->dashes = va_arg(ap, c_Dashes *); */
      break;
    case XcBG_Line_LINE_THICKNESS:
      Xc_TRACE(("XcBG_Line_LINE_THICKNESS"));
      This->line_thickness = va_arg(ap, coord_t);
      break;
    case XcBG_Line_END:
      Xc_TRACE(("XcBG_Line_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow BG_Line_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_Line(This);
}

static void calcul_Line(This)
c_BG_Line	*This;
{
  if (This->point1.dx < This->point2.dx)
  {
    This->bbox.llx = This->point1.dx;
    This->bbox.urx = This->point2.dx;
  }
  else
  {
    This->bbox.llx = This->point2.dx;
    This->bbox.urx = This->point1.dx;    
  }
  if (This->point1.dy < This->point2.dy)
  {
    This->bbox.lly = This->point2.dy;
    This->bbox.ury = This->point1.dy;
  }
  else
  {
    This->bbox.lly = This->point1.dy;
    This->bbox.ury = This->point2.dy;    
  }
}

/* -------------------------------------------------------------------- **
** ps print, read & write BG_Line					**
** -------------------------------------------------------------------- */ 
static boolean ps_print(this, post)
c_BG_Line *this;
c_PostScript *post;
{
  Xc_HISTORY(("ps_print"));
  
  if(this->fg_color->transparency != 0) return TRUE;
  if(!F(this->fg_color).ps_print(this->fg_color, post, TRUE)) return FALSE;

  if(this->line_thickness != post->state.line_width)
    {
      if(!F(post).putLine(post, "$c w", this->line_thickness))
	return FALSE;
      post->state.line_width = this->line_thickness;
    }
  
  return F(post).putLine(post, "$p m $p l S",
			 this->point1.dx, this->point1.dy,
			 this->point2.dx, this->point2.dy);
}

static boolean readBG(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  return FALSE;
}

static boolean writeBG(This, doc)
c_BG_Line	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



