/*
** BG_Box.c for XQuad in Basic_Graph/
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

#include "BG_Box.h"
#include <varargs.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_Box *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_Box *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_Box *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_Box *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_Box *This, 
					 c_VectorGraph *vectorg));
static void sub_to_vectorgraph ___PROTO((c_BG_Box *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_Box *This));
static boolean is_in ___PROTO((c_BG_Box *This, coord_t x, coord_t y));
static void setBG (); /* ___PROTO((c_BG_Box *This, ...)); */
static void vset();
static void calcul_box ___PROTO((c_BG_Box *This));
static boolean ps_print ___PROTO((c_BG_Box *This, c_PostScript *post));
static boolean readBG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_Box *This, c_DocFile *doc));

sf_BG_Box fc_BG_Box =
{
  cons, dest, copy,
  change_xinfo, change_base, display_redraw, display_handles,
  add_to_vectorgraph, sub_to_vectorgraph, get_bounding_box, is_in, setBG,
  ps_print, readBG, writeBG
};

static void *cons(base)
BaseStd_t	*base;
{
  c_BG_Box	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("BG_Box", sizeof(c_BG_Box)) ) == NULL)
    return NULL;
  This->f = &fc_BG_Box;
  
  This->type = BG_BOX;
  This->BaseStd = base;
  
  This->point1.dx = This->point1.dy = SCALE_FROM_MILLIMETERS(7);
  This->point2.dx = BG_AREA_WIDTH - SCALE_FROM_MILLIMETERS(7);  
  This->point2.dy = BG_AREA_HEIGHT - SCALE_FROM_MILLIMETERS(7);
  This->ins_fg_color = Get_BG_Color(base, BGC_TRANSPARENT);
  This->ins_bg_color = Get_BG_Color(base, BGC_GRAY_DARK);
  This->out_fg_color = Get_BG_Color(base, BGC_BLACK);
  This->out_bg_color = Get_BG_Color(base, BGC_WHITE);
  This->line_thickness = SCALE_FROM_POINTS(0.5);
  calcul_box(This);
  
  Xc_TRACE(("constructor done"));
  return This;
}

static void dest(This)
c_BG_Box	*This;
{
  Xc_HISTORY(("destructor")); 
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}

static void *copy(This, base)
c_BG_Box	*This;
BaseStd_t	*base;
{
  c_BG_Box	*bg_copy;
  
  if ( (bg_copy = Xc_malloc("BG_Box", sizeof(c_BG_Box)) ) == NULL)
    return NULL;
  memcpy(bg_copy, This, sizeof(c_BG_Box));
  
  if (bg_copy->BaseStd != base)
    BG_CHANGE_BASE(bg_copy)(bg_copy, base);
  return bg_copy;
}

static void change_xinfo(This, xinfo)
c_BG_Box	*This;
x_info		*xinfo;
{
  memcpy(&This->X_info, xinfo, sizeof(x_info));
}

static error change_base(This, new_base)
c_BG_Box	*This;
BaseStd_t	*new_base;
{
  This->BaseStd = new_base;
  return XC_NO_ERROR;
}

static void display_redraw(This, matrix, scale, dx, dy)
c_BG_Box	*This;
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
  int		w, h;
  int		cx, cy;
  int		linet;
  
  display = _display;
  window = _window;
  gc = _gc_inside;
  m = F(matrix).get(matrix, 0);
  
  cx = (int)XcM_X(m, This->point1.dx, This->point1.dy);
  cy = (int)XcM_Y(m, This->point1.dx, This->point1.dy);
  w = (int)XcM_X(m, This->point2.dx, This->point2.dy);
  h = (int)XcM_Y(m, This->point2.dx, This->point2.dy);
  
  if (cx > w)
    SWAP(int, cx, w);
  if (cy > h)
    SWAP(int, cy, h);
  w -= cx;
  h -= cy;
  
  if (w<=0 || h<=0) return;
  
  if (!This->ins_fg_color->transparency)
  {
    val_gc.function = GXcopy;
    val_gc.foreground = This->ins_fg_color->cell.pixel;
    val_gc.background = This->ins_bg_color->cell.pixel;
#if 0
    if (This->ins_bg_color.transparency)
      val_gc.fill_style = FillStipple;
    else
      val_gc.fill_style = FillOpaqueStipple;
#endif  
    XChangeGC(display, gc, GCFunction | GCForeground | GCBackground 
	      /* | GCFillStyle */ ,&val_gc);
    
    XFillRectangle(display, window, gc, dx+cx, dy+cy, w, h); 
  }
  
  if (This->line_thickness)
  {
    val_gc.foreground = This->out_fg_color->cell.pixel;
    val_gc.background = This->out_bg_color->cell.pixel;
    linet = This->line_thickness / scale;
    val_gc.line_width = (linet == 1 ? 0 : linet);
    XChangeGC(display, gc, GCFunction | GCForeground | GCBackground 
	      | GCLineWidth /* | GCFillStyle */ ,&val_gc);
    
    XDrawRectangle(display, window, gc, dx+cx, dy+cy, w, h); 
  }
}

static void display_handles(This, matrix, scale, dx, dy)
c_BG_Box	*This;
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
c_BG_Box  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).addBasicGraph(vectorg, This);
}

static void sub_to_vectorgraph(This, vectorg)
c_BG_Box  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).subBasicGraph(vectorg, This);
}

static bbox_t *get_bounding_box(This)
c_BG_Box *This;
{
  return &(This->bbox);
}

static boolean is_in(This, x, y)
c_BG_Box	*This;
coord_t		x;
coord_t		y;
{
  if (x >= This->bbox.llx && x <= This->bbox.urx &&
      y >= This->bbox.lly && y <= This->bbox.ury)
    return TRUE;
  return FALSE;
}

static void setBG(This, va_alist)
c_BG_Box *This;
va_dcl
{
  va_list ap;
  
  Xc_HISTORY(("set"));

  va_start(ap);
  vset(This, ap);
  va_end(ap);
} 

static void vset(This, ap)
c_BG_Box *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  vector_t	size;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, BG_Box_set_code_t))
    {
    case XcBG_Box_POINT1:
      Xc_TRACE(("XcBG_Box_POINT2"));
      calcul = TRUE;
      This->point1 = va_arg(ap, vector_t);
      break;
    case XcBG_Box_POINT2:
      Xc_TRACE(("XcBG_Box_POINT2"));
      calcul = TRUE;
      This->point1 = va_arg(ap, vector_t);
      break;
    case XcBG_Box_SIZE:
      Xc_TRACE(("XcBG_Box_SIZE"));
      size = va_arg(ap, vector_t);
      This->point2.dx = This->point1.dx + size.dx;
      This->point2.dy = This->point1.dy + size.dy;
      calcul = TRUE;
      break;
    case XcBG_Box_INS_FG_COLOR:
      Xc_TRACE(("XcBG_Box_INS_FG_COLOR"));
      This->ins_fg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Box_INS_BG_COLOR:
      Xc_TRACE(("XcBG_Box_INS_BG_COLOR"));
      This->ins_bg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Box_OUT_FG_COLOR:
      Xc_TRACE(("XcBG_Box_OUT_FG_COLOR"));
      This->out_fg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Box_OUT_BG_COLOR:
      Xc_TRACE(("XcBG_Box_OUT_BG_COLOR"));
      This->out_bg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Box_DASHES:
      Xc_TRACE(("XcBG_Box_DASHES"));
      /* This->dashes = va_arg(ap, c_Dashes *); */
      break;
    case XcBG_Box_PATTERN:
      Xc_TRACE(("XcBG_Box_PATTERN"));
      /* This->pattern = va_arg(ap, c_Pattern *); */
      break;
    case XcBG_Box_LINE_THICKNESS:
      Xc_TRACE(("XcBG_Box_LINE_THICKNESS"));
      This->line_thickness = va_arg(ap, coord_t);
      break;
    case XcBG_Box_END:
      Xc_TRACE(("XcBG_Box_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow BG_Box_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_box(This);
}

static void calcul_box(This)
c_BG_Box	*This;
{
  This->point12.dx = This->point2.dx;
  This->point12.dy = This->point1.dy;
  
  This->point21.dx = This->point1.dx;
  This->point21.dy = This->point2.dy;

  This->bbox.llx = This->point1.dx;
  This->bbox.lly = This->point1.dy;
  This->bbox.urx = This->point2.dx;
  This->bbox.ury = This->point2.dy;
}

/* -------------------------------------------------------------------- **
** ps print, read & write BG_Box					**
** -------------------------------------------------------------------- */ 
static boolean ps_print(this, post)
c_BG_Box *this;
c_PostScript *post;
{
  boolean fill, stroke;

  Xc_HISTORY(("ps_print"));
  
  fill = (this->ins_fg_color->transparency == 0)? TRUE : FALSE;
  stroke = (this->out_fg_color->transparency == 0)? TRUE : FALSE;
  if(!fill && !stroke) return FALSE;

  if(fill && !F(this->ins_fg_color).ps_print(this->ins_fg_color, post, FALSE))
    return FALSE;

  if(stroke != FALSE)
    {
      if(!F(this->out_fg_color).ps_print(this->out_fg_color, post, TRUE))
	return FALSE;
      if(this->line_thickness != post->state.line_width)
	{
	  if(!F(post).putLine(post, "$c w", this->line_thickness))
	    return FALSE;
	  post->state.line_width = this->line_thickness;
	}
    }

  if(post->ai_mode)
    return F(post).putLine(post, "$p m $p L $p L $p L $p L H $s", 
			   this->bbox.llx, this->bbox.lly,
			   this->bbox.urx, this->bbox.lly,
			   this->bbox.urx, this->bbox.ury,
			   this->bbox.llx, this->bbox.ury,
			   this->bbox.llx, this->bbox.lly,
			   (fill? (stroke? "B":"f"):"S"));
  else
    return F(post).putLine(post, "$r re $s", &this->bbox,
			   (fill? (stroke? "B":"f"):"S"));
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
c_BG_Box	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



