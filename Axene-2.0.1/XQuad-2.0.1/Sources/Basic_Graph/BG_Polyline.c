/*
** BG_Polyline.c for XQuad in Basic_Graph/
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
** Started on  Sat Jun 24 18:07:33 1995 Emmanuel Paris
** Last update Wed May 29 15:39:49 1996 One of the authors
*/

#define NTRACE
#define NHISTORY

#include "BG_Polyline.h"
#include <varargs.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_Polyline *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_Polyline *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_Polyline *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_Polyline *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_Polyline *This, 
					 c_VectorGraph *vectorg));
static void sub_to_vectorgraph ___PROTO((c_BG_Polyline *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_Polyline *This));
static boolean is_in ___PROTO((c_BG_Polyline *This, coord_t x, coord_t y));
static void setBG (); /* ___PROTO((c_BG_Polyline *This, ...)); */
static void vset();
static void calcul_polyline ___PROTO((c_BG_Polyline *This));
static boolean ps_print ___PROTO((c_BG_Polyline *This, c_PostScript *post));
static boolean readBG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_Polyline *This, c_DocFile *doc));

sf_BG_Polyline fc_BG_Polyline =
{
  cons, dest, copy,
  change_xinfo, change_base, display_redraw, display_handles,
  add_to_vectorgraph, sub_to_vectorgraph, get_bounding_box, is_in, setBG,
  ps_print, readBG, writeBG
};

static void *cons(base)
BaseStd_t	*base;
{
  c_BG_Polyline	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("BG_Polyline", sizeof(c_BG_Polyline)) ) == NULL)
    return NULL;
  This->f = &fc_BG_Polyline;
  
  This->type = BG_POLYLINE;
  This->BaseStd = base;
  
  This->nb_point = 0;
  This->list_point = NULL;
  This->fg_color = Get_BG_Color(base, BGC_BLACK);
  This->bg_color = Get_BG_Color(base, BGC_WHITE);
  This->line_thickness = SCALE_FROM_POINTS(0.5);
  calcul_polyline(This);
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_BG_Polyline	*This;
{
  Xc_HISTORY(("destructor")); 
  if (This->nb_point)
  {
    BG_Point	*point1, *point2;
    
    point1 = This->list_point;
    while(point1)
    {
      point2 = point1->NextPoint;
      Xc_free(point1);
      point1 = point2;
    }
  }
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_BG_Polyline	*This;
BaseStd_t	*base;
{
  c_BG_Polyline	*bg_copy;
  
  if ( (bg_copy = Xc_malloc("BG_Polyline", sizeof(c_BG_Polyline)) ) == NULL)
    return NULL;
  memcpy(bg_copy, This, sizeof(c_BG_Polyline));
  
  if (bg_copy->BaseStd != base)
    BG_CHANGE_BASE(bg_copy)(bg_copy, base);
  return bg_copy;
}

static void change_xinfo(This, xinfo)
c_BG_Polyline	*This;
x_info		*xinfo;
{
  memcpy(&This->X_info, xinfo, sizeof(x_info));
}

static error change_base(This, new_base)
c_BG_Polyline	*This;
BaseStd_t	*new_base;
{
  This->BaseStd = new_base;
  return XC_NO_ERROR;
}

static void display_redraw(This, matrix, scale, dx, dy)
c_BG_Polyline	*This;
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
  int		i, linet;
  BG_Point	*bg_point;
  XPoint	*xpoint;
  
  display = _display;
  window = _window;
  gc = _gc_inside;
  m = F(matrix).get(matrix, 0);
  
  if (!This->nb_point) return;
  
  xpoint = (XPoint *)Xc_malloc("xpoints", sizeof(XPoint)*(This->nb_point));
  bg_point = This->list_point;
  for(i=0; i<This->nb_point; i++)
    {
      xpoint[i].x = 
	(int)XcM_X(m, bg_point->point.dx, bg_point->point.dy) + dx;
      xpoint[i].y = 
	(int)XcM_Y(m, bg_point->point.dx, bg_point->point.dy) + dy;
      bg_point = bg_point->NextPoint;
    }
  
  if (This->line_thickness)
    {
      val_gc.function = GXcopy;
      val_gc.foreground = This->fg_color->cell.pixel;
      val_gc.background = This->bg_color->cell.pixel;
      val_gc.join_style = JoinMiter;
      linet = This->line_thickness / scale;
      val_gc.line_width = (linet == 1 ? 0 : linet);
      XChangeGC(display, gc, GCFunction | GCForeground | GCBackground 
		| GCJoinStyle | GCLineWidth /* | GCFillStyle */ ,&val_gc);
      
      XDrawLines(display, window, gc, 
		 xpoint, This->nb_point, CoordModeOrigin);
    }

  Xc_free(xpoint);
}

static void display_handles(This, matrix, scale, dx, dy)
c_BG_Polyline	*This;
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
c_BG_Polyline  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).addBasicGraph(vectorg, This);
}

static void sub_to_vectorgraph(This, vectorg)
c_BG_Polyline  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).subBasicGraph(vectorg, This);
}

static bbox_t *get_bounding_box(This)
c_BG_Polyline *This;
{
  return &(This->bbox);
}

static boolean is_in(This, x, y)
c_BG_Polyline	*This;
coord_t		x;
coord_t		y;
{
  if (x >= This->bbox.llx && x <= This->bbox.urx &&
      y >= This->bbox.lly && y <= This->bbox.ury)
    return TRUE;
  return FALSE;
}

static void setBG(This, va_alist)
c_BG_Polyline *This;
va_dcl
{
  va_list ap;
  
  Xc_HISTORY(("set"));

  va_start(ap);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_BG_Polyline *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  int		nb_point;
  BG_Point	*bg_point;

  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, BG_Polyline_set_code_t))
    {
    case XcBG_Polyline_ADD_POINT:
      Xc_TRACE(("XcBG_Polyline_POINT2"));
      calcul = TRUE;
      
      bg_point = (BG_Point *)Xc_malloc("bg_point", sizeof(BG_Point));
      bg_point->point = va_arg(ap, vector_t);
      bg_point->NextPoint = This->list_point;
      This->list_point = bg_point;
      This->nb_point = This->nb_point + 1;
      break;
    case XcBG_Polyline_ADD_POINTS:
      Xc_TRACE(("XcBG_Polyline_POINT2"));
      calcul = TRUE;
      nb_point = va_arg(ap, int);
      break;
    case XcBG_Polyline_FG_COLOR:
      Xc_TRACE(("XcBG_Polyline_FG_COLOR"));
      This->fg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Polyline_BG_COLOR:
      Xc_TRACE(("XcBG_Polyline_BG_COLOR"));
      This->bg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Polyline_DASHES:
      Xc_TRACE(("XcBG_Polyline_DASHES"));
      /* This->dashes = va_arg(ap, c_Dashes *); */
      break;
    case XcBG_Polyline_LINE_THICKNESS:
      Xc_TRACE(("XcBG_Polyline_LINE_THICKNESS"));
      This->line_thickness = va_arg(ap, coord_t);
      break;
    case XcBG_Polyline_END:
      Xc_TRACE(("XcBG_Polyline_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow BG_Polyline_set_code"));
      break;
    }
  } while(!out_flag);
/*  if (calcul)
    calcul_polyline(This);*/
}

static void calcul_polyline(This)
c_BG_Polyline	*This;
{
  int		i;
  BG_Point	*bg_point;
  
  bg_point = This->list_point;
  if (bg_point)
    {
      This->bbox.llx = This->bbox.urx = bg_point->point.dx;
      This->bbox.lly = This->bbox.ury = bg_point->point.dy;
      
      for(i=1; i<This->nb_point; i++)
	{
	  if (This->bbox.llx > bg_point->point.dx)
	    This->bbox.llx = bg_point->point.dx;
	  if (This->bbox.urx < bg_point->point.dx)
	    This->bbox.urx = bg_point->point.dx; 
	  if (This->bbox.lly < bg_point->point.dy)
	    This->bbox.lly = bg_point->point.dy;
	  if (This->bbox.ury > bg_point->point.dy)
	    This->bbox.ury = bg_point->point.dy;
	}
    }
  else
    This->bbox.llx = This->bbox.urx = This->bbox.lly = 
      This->bbox.ury = (coord_t)0.0;
}


/* ----------------------------------------------------------------- ** 
** ps_print - Print polyline                                         ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_BG_Polyline *this;
c_PostScript *post;
{
  BG_Point *ptr;

  Xc_HISTORY(("ps_print"));

  if (this->list_point == NULL || this->fg_color->transparency != 0)
    return TRUE;

  if (!F(this->fg_color).ps_print(this->fg_color, post, TRUE))
    return FALSE;

  if(this->line_thickness != post->state.line_width)
  {
    if(!F(post).putLine(post, "$c w", this->line_thickness))
      return FALSE;
    post->state.line_width = this->line_thickness;
  }
  
  ptr = this->list_point;
  if(!F(post).putLine(post, "$p m", ptr->point.dx, ptr->point.dy))
    return FALSE;

  while((ptr = ptr->NextPoint) != NULL)
    if(!F(post).putLine(post, "$p $s", ptr->point.dx, ptr->point.dy,
			post->ai_mode? "L":"l")) return FALSE;
  
  return F(post).putLine(post, "S");
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
c_BG_Polyline	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



