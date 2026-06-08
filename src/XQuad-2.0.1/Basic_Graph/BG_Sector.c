/*
** BG_Sector.c for XQuad in Basic_Graph/
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
** Last update Wed May 29 15:39:50 1996 One of the authors
*/

#define NTRACE
#define NHISTORY

#include "BG_Sector.h"
#include <varargs.h>
#include <math.h>

static void *cons();
static void dest();
static void *copy();
static void change_xinfo ___PROTO((c_BG_Sector *This, x_info *xinfo));
static error change_base ___PROTO((c_BG_Sector *This, BaseStd_t *new_base));
static void display_redraw ___PROTO((c_BG_Sector *This, c_Matrix *matrix,
				     coord_t scale, int dx, int dy));
static void display_handles ___PROTO((c_BG_Sector *This, c_Matrix *matrix,
				      coord_t scale, int dx, int dy));
static void add_to_vectorgraph ___PROTO((c_BG_Sector *This, 
					 c_VectorGraph *vectorg));
static void sub_to_vectorgraph ___PROTO((c_BG_Sector *This, 
					 c_VectorGraph *vectorg));
static bbox_t *get_bounding_box ___PROTO((c_BG_Sector *This));
static boolean is_in ___PROTO((c_BG_Sector *This, coord_t x, coord_t y));
static void setBG (); /* ___PROTO((c_BG_Sector *This, ...)); */
static void vset();
static void calcul_sector ___PROTO((c_BG_Sector *This));
static boolean ps_print ___PROTO((c_BG_Sector *This, c_PostScript *post));
static boolean readBG ___PROTO((c_DocFile *doc,char *keyword,
				long param, BaseStd_t *base));
static boolean writeBG ___PROTO((c_BG_Sector *This, c_DocFile *doc));
static boolean add_arc ___PROTO((c_PostScript *post, boolean first,
				 coord_t radius, coord_t x0, coord_t y0, 
				 coord_t x3, coord_t y3,
				 coord_t xt, coord_t yt));

sf_BG_Sector fc_BG_Sector =
{
  cons, dest, copy,
  change_xinfo, change_base, display_redraw, display_handles,
  add_to_vectorgraph, sub_to_vectorgraph, get_bounding_box, is_in, setBG,
  ps_print, readBG, writeBG
};


/* ----------------------------------------------------------------- ** 
** cons - Constructor                                                ** 
** ----------------------------------------------------------------- */
static void *cons(base)
BaseStd_t	*base;
{
  c_BG_Sector	*This;
  
  Xc_HISTORY(("constructor"));
  
  if ( (This = Xc_malloc("BG_Sector", sizeof(c_BG_Sector)) ) == NULL)
    return NULL;
  This->f = &fc_BG_Sector;
  
  This->type = BG_SECTOR;
  This->BaseStd = base;
  
  This->angle1 = This->angle2 = ANGLE_ZERO;
  This->rayon.dx = BG_AREA_WIDTH / 8 * 3;
  This->rayon.dy = BG_AREA_HEIGHT / 8 * 3;
  This->center.dx = BG_AREA_WIDTH / 2;
  This->center.dy = BG_AREA_HEIGHT / 2;
  This->ins_fg_color = Get_BG_Color(base, BGC_GRAY_LIGHT);
  This->ins_bg_color = Get_BG_Color(base, BGC_GRAY_DARK);
  This->out_fg_color = Get_BG_Color(base, BGC_BLACK);
  This->out_bg_color = Get_BG_Color(base, BGC_WHITE);
  This->line_thickness = SCALE_FROM_POINTS(0.5);
  calcul_sector(This);
  
  Xc_TRACE(("constructor done"));
  return This;
}


/* ----------------------------------------------------------------- ** 
** dest - Destructor                                                 ** 
** ----------------------------------------------------------------- */
static void dest(This)
c_BG_Sector	*This;
{
  Xc_HISTORY(("destructor")); 
  Xc_free(This);
  Xc_TRACE(("destructor done"));
}


/* ----------------------------------------------------------------- ** 
** copy - Copy                                                       ** 
** ----------------------------------------------------------------- */
static void *copy(This, base)
c_BG_Sector	*This;
BaseStd_t	*base;
{
  c_BG_Sector	*bg_copy;
  
  if ( (bg_copy = Xc_malloc("BG_Sector", sizeof(c_BG_Sector)) ) == NULL)
    return NULL;
  memcpy(bg_copy, This, sizeof(c_BG_Sector));
  
  if (bg_copy->BaseStd != base)
    BG_CHANGE_BASE(bg_copy)(bg_copy, base);
  return bg_copy;
}


/* ----------------------------------------------------------------- ** 
** change_xinfo - Change X11 infos                                   ** 
** ----------------------------------------------------------------- */
static void change_xinfo(This, xinfo)
c_BG_Sector	*This;
x_info		*xinfo;
{
  memcpy(&This->X_info, xinfo, sizeof(x_info));
}


/* ----------------------------------------------------------------- ** 
** change_base - Change BaseStd                                      ** 
** ----------------------------------------------------------------- */
static error change_base(This, new_base)
c_BG_Sector	*This;
BaseStd_t	*new_base;
{
  This->BaseStd = new_base;
  return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** display_redraw - Redraw sector on screen                          ** 
** ----------------------------------------------------------------- */
static void display_redraw(This, matrix, scale, dx, dy)
c_BG_Sector	*This;
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
  XPoint	point[3];
  
  display = _display;
  window = _window;
  gc = _gc_inside;
  m = F(matrix).get(matrix, 0);
  
  val_gc.function = GXcopy;
  val_gc.foreground = This->ins_fg_color->cell.pixel;
  val_gc.background = This->ins_bg_color->cell.pixel;
  val_gc.arc_mode = ArcPieSlice;
#if 0
  if (This->ins_bg_color.transparency)
    val_gc.fill_style = FillStipple;
  else
    val_gc.fill_style = FillOpaqueStipple;
#endif  
  XChangeGC(display, gc, GCFunction | GCForeground | GCBackground | 
	    GCArcMode /* | GCFillStyle */ ,&val_gc);
  
  w = (int)XcM_DX(m, This->rayon.dx, This->rayon.dy); 
  h = (int)XcM_DY(m, This->rayon.dx, This->rayon.dy); 
  cx = (int)XcM_X(m, This->center.dx, This->center.dy) - w;
  cy = (int)XcM_Y(m, This->center.dx, This->center.dy) - h;
  XFillArc(display, window, gc, dx+cx, dy+cy, w*2, h*2, 
	   SCALE_TO_XANGLE(This->angle1), 
	   SCALE_TO_XANGLE(This->angle2 - This->angle1));
  
  if (This->line_thickness)
  {
    val_gc.foreground = This->out_fg_color->cell.pixel;
    val_gc.background = This->out_bg_color->cell.pixel;
    linet = This->line_thickness / scale;
    val_gc.line_width = (linet == 1 ? 0 : linet);
    XChangeGC(display, gc, GCForeground | GCBackground | GCLineWidth
	      /* | GCFillStyle */ ,&val_gc);
    
    XDrawArc(display, window, gc, dx+cx, dy+cy, w*2, h*2, 
	     SCALE_TO_XANGLE(This->angle1), 
	     SCALE_TO_XANGLE(This->angle2 - This->angle1));
    point[0].x = (int)XcM_X(m, This->point1.dx, This->point1.dy) + dx;
    point[0].y = (int)XcM_Y(m, This->point1.dx, This->point1.dy) + dy;
    point[1].x = cx + w + dx;
    point[1].y = cy + h + dy;
    point[2].x = (int)XcM_X(m, This->point2.dx, This->point2.dy) + dx;
    point[2].y = (int)XcM_Y(m, This->point2.dx, This->point2.dy) + dy;
    XDrawLines(display, window, gc, point, 3, CoordModeOrigin);
  }
}


/* ----------------------------------------------------------------- ** 
** display_handles - Draw sector handles                             ** 
** ----------------------------------------------------------------- */
static void display_handles(This, matrix, scale, dx, dy)
c_BG_Sector	*This;
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


/* ----------------------------------------------------------------- ** 
** add_to_vectorgraph                                                ** 
** ----------------------------------------------------------------- */
static void add_to_vectorgraph(This, vectorg)
c_BG_Sector  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).addBasicGraph(vectorg, This);
}


/* ----------------------------------------------------------------- ** 
** sub_to_vectorgraph                                                ** 
** ----------------------------------------------------------------- */
static void sub_to_vectorgraph(This, vectorg)
c_BG_Sector  *This;
c_VectorGraph *vectorg;
{
  F(vectorg).subBasicGraph(vectorg, This);
}


/* ----------------------------------------------------------------- ** 
** get_bounding_box - Returns the sector bbox                        ** 
** ----------------------------------------------------------------- */
static bbox_t *get_bounding_box(This)
c_BG_Sector *This;
{
  return &(This->bbox);
}


/* ----------------------------------------------------------------- ** 
** is_in - Test if a point is on sector                              ** 
** ----------------------------------------------------------------- */
static boolean is_in(This, x, y)
c_BG_Sector	*This;
coord_t		x;
coord_t		y;
{
  if (x >= This->bbox.llx && x <= This->bbox.urx &&
      y >= This->bbox.lly && y <= This->bbox.ury)
    return TRUE;
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** setBG - Set sector attributes                                     ** 
** ----------------------------------------------------------------- */
static void setBG(This, va_alist)
c_BG_Sector *This;
va_dcl
{
  va_list ap;
  
  Xc_HISTORY(("set"));

  va_start(ap);
  vset(This, ap);
  va_end(ap);
} 


/* ----------------------------------------------------------------- ** 
** vset - Internal attribute setting                                 ** 
** ----------------------------------------------------------------- */
static void vset(This, ap)
c_BG_Sector *This;
va_list ap;
{
  boolean out_flag;
  boolean calcul;
  
  Xc_TRACE(("vset"));
  
  out_flag = calcul = FALSE;
  do {
    switch(va_arg(ap, BG_Sector_set_code_t))
    {
    case XcBG_Sector_ANGLE1:
      Xc_TRACE(("XcBG_Sector_ANGLE1"));
      calcul = TRUE;
      This->angle1 = SCALE_FROM_VDEGREES( va_arg(ap, vangle_t) );
      break;
    case XcBG_Sector_ANGLE2:
      Xc_TRACE(("XcBG_Sector_ANGLE2"));
      This->angle2 = SCALE_FROM_VDEGREES( va_arg(ap, vangle_t) );
      calcul = TRUE;
      Xc_TRACE(("Angle2= %g", This->angle2));
      break;
    case XcBG_Sector_RAYON:
      Xc_TRACE(("XcBG_Sector_RAYON"));
      This->rayon = va_arg(ap, vector_t);
      calcul = TRUE;
      break;
    case XcBG_Sector_CENTER:
      Xc_TRACE(("XcBG_Sector_CENTER"));
      This->center = va_arg(ap, vector_t);
      calcul = TRUE;
      break;
    case XcBG_Sector_INS_FG_COLOR:
      Xc_TRACE(("XcBG_Sector_INS_FG_COLOR"));
      This->ins_fg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Sector_INS_BG_COLOR:
      Xc_TRACE(("XcBG_Sector_INS_BG_COLOR"));
      This->ins_bg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Sector_OUT_FG_COLOR:
      Xc_TRACE(("XcBG_Sector_OUT_FG_COLOR"));
      This->out_fg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Sector_OUT_BG_COLOR:
      Xc_TRACE(("XcBG_Sector_OUT_BG_COLOR"));
      This->out_bg_color = va_arg(ap, c_Color *);
      break;
    case XcBG_Sector_DASHES:
      Xc_TRACE(("XcBG_Sector_DASHES"));
      /* This->dashes = va_arg(ap, c_Dashes *); */
      break;
    case XcBG_Sector_PATTERN:
      Xc_TRACE(("XcBG_Sector_PATTERN"));
      /* This->pattern = va_arg(ap, c_Pattern *); */
      break;
    case XcBG_Sector_LINE_THICKNESS:
      Xc_TRACE(("XcBG_Sector_LINE_THICKNESS"));
      This->line_thickness = va_arg(ap, coord_t);
      break;
    case XcBG_Sector_END:
      Xc_TRACE(("XcBG_Sector_END"));
      out_flag = TRUE;
      break;
    default:
      Xc_BREAK(("Unknow BG_Sector_set_code"));
      break;
    }
  } while(!out_flag);
  if (calcul)
    calcul_sector(This);
}


/* ----------------------------------------------------------------- ** 
** calcul_sector                                                     ** 
** ----------------------------------------------------------------- */
static void calcul_sector(This)
c_BG_Sector	*This;
{
  real	cs, ss;
  angle_t angle;
  /* this will calcul point1, point2, point12 and point21 */
  
  cs = cos(SCALE_TO_RADIANS(This->angle1));
  ss = sin(SCALE_TO_RADIANS(This->angle1));
  This->point1.dx = This->center.dx + cs * This->rayon.dx;
  This->point1.dy = This->center.dy - ss * This->rayon.dy;

  cs = cos(SCALE_TO_RADIANS(This->angle2));
  ss = sin(SCALE_TO_RADIANS(This->angle2));
  This->point2.dx = This->center.dx + cs * This->rayon.dx;
  This->point2.dy = This->center.dy - ss * This->rayon.dy;

  angle = This->angle1 +  (This->angle2-This->angle1) / 3 ;
  cs = cos(SCALE_TO_RADIANS(angle));
  ss = sin(SCALE_TO_RADIANS(angle));
  This->point12.dx = This->center.dx + cs * This->rayon.dx;
  This->point12.dy = This->center.dy - ss * This->rayon.dy;

  angle = This->angle1 +  (This->angle2-This->angle1) * 2 / 3 ;
  cs = cos(SCALE_TO_RADIANS(angle));
  ss = sin(SCALE_TO_RADIANS(angle));
  This->point21.dx = This->center.dx + cs * This->rayon.dx;
  This->point21.dy = This->center.dy - ss * This->rayon.dy;

  This->bbox.llx = This->bbox.urx = This->center.dx;
  This->bbox.lly = This->bbox.ury = This->center.dy;
  if (This->bbox.llx > This->point1.dx) This->bbox.llx = This->point1.dx;
  if (This->bbox.lly > This->point1.dy) This->bbox.lly = This->point1.dy;
  if (This->bbox.urx < This->point1.dx) This->bbox.urx = This->point1.dx;
  if (This->bbox.ury < This->point1.dy) This->bbox.ury = This->point1.dy;
  if (This->bbox.llx > This->point2.dx) This->bbox.llx = This->point2.dx;
  if (This->bbox.lly > This->point2.dy) This->bbox.lly = This->point2.dy;
  if (This->bbox.urx < This->point2.dx) This->bbox.urx = This->point2.dx;
  if (This->bbox.ury < This->point2.dy) This->bbox.ury = This->point2.dy;
  if (This->bbox.llx > This->point12.dx) This->bbox.llx = This->point12.dx;
  if (This->bbox.lly > This->point12.dy) This->bbox.lly = This->point12.dy;
  if (This->bbox.urx < This->point12.dx) This->bbox.urx = This->point12.dx;
  if (This->bbox.ury < This->point12.dy) This->bbox.ury = This->point12.dy;
  if (This->bbox.llx > This->point21.dx) This->bbox.llx = This->point21.dx;
  if (This->bbox.lly > This->point21.dy) This->bbox.lly = This->point21.dy;
  if (This->bbox.urx < This->point21.dx) This->bbox.urx = This->point21.dx;
  if (This->bbox.ury < This->point21.dy) This->bbox.ury = This->point21.dy;
}


/* ----------------------------------------------------------------- ** 
** ps_print - Print sector                                           ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_BG_Sector *this;
c_PostScript *post;
{
  boolean fill, stroke;
  double sin0, cos0, trad;
  coord_t x0, y0;
  coord_t radius;
  coord_t x3r, y3r;
  angle_t ang1, ang2, adiff;
  boolean first;

  Xc_HISTORY(("ps_print"));

  fill = (this->ins_fg_color->transparency == 0)? TRUE : FALSE;
  stroke = (this->out_fg_color->transparency == 0)? TRUE : FALSE;
  if(!fill && !stroke) return TRUE;

  radius = MIN(this->rayon.dx, this->rayon.dy);
  if(radius <= COORD_ZERO) return TRUE;

  first = TRUE;

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

  if(!F(post).putLine(post, "$p m", this->center.dx, this->center.dy))
    return FALSE;

  ang1 = this->angle1;
  ang2 = this->angle2;
  sin0 = SCALE_TO_POINTS(radius) * sin(SCALE_TO_RADIANS(ang1));
  cos0 = SCALE_TO_POINTS(radius) * cos(SCALE_TO_RADIANS(ang1));
  x0 = this->center.dx + SCALE_FROM_POINTS(cos0);
  y0 = this->center.dy - SCALE_FROM_POINTS(sin0);
  Xc_TRACE(("from %g to %g", SCALE_TO_DEGREES(ang1), SCALE_TO_DEGREES(ang2)));

  /*--- Assume counter clockwise ---*/
  while(ang2 < ang1) ang2 += SCALE_FROM_DEGREES(360.0);
  while((adiff = ang2 - ang1) > SCALE_FROM_DEGREES(90.0))
    {
      double w;

      w = cos0;
      cos0 = -sin0;
      sin0 = w;
      x3r = this->center.dx + SCALE_FROM_POINTS(cos0);
      y3r = this->center.dy - SCALE_FROM_POINTS(sin0);
      if(!add_arc(post, first, radius, x0, y0, x3r, y3r,
		  x0 + SCALE_FROM_POINTS(cos0),
		  y0 - SCALE_FROM_POINTS(sin0))) return FALSE;
      x0 = x3r, y0 = y3r;
      ang1 += SCALE_FROM_DEGREES(90.0);
      first = FALSE;
    }

  /*--- Compute the intersection of the tangents ---*/
  Xc_TRACE(("angle diff= %g", SCALE_TO_DEGREES(adiff)));
  
  trad = tan(SCALE_TO_RADIANS(adiff) / 2.0);
  x3r = this->center.dx + radius * cos(SCALE_TO_RADIANS(ang2));
  y3r = this->center.dy - radius * sin(SCALE_TO_RADIANS(ang2));
  if(!add_arc(post, first, radius, x0, y0, x3r, y3r,
	      x0 - SCALE_FROM_POINTS(trad * sin0),
	      y0 - SCALE_FROM_POINTS(trad * cos0))) return FALSE;

  if(post->ai_mode && 
     !F(post).putLine(post, "$p L", this->center.dx, this->center.dy))
    return FALSE;
  
  return F(post).putLine(post, (fill? (stroke? "b" : "f") : "s"));
}


/* ----------------------------------------------------------------- ** 
** add_arc - Internal routine to add an arc to the path              ** 
** ----------------------------------------------------------------- */
static boolean add_arc(post, first, radius, x0, y0, x3, y3, xt, yt)
c_PostScript *post;
boolean first;
coord_t radius;
coord_t x0;
coord_t y0;
coord_t x3;
coord_t y3;
coord_t xt;
coord_t yt;
{
  double fraction, dx, dy, r;

  Xc_TRACE(("add_arc"));

  dx = SCALE_TO_POINTS(xt - x0); 
  dy = SCALE_TO_POINTS(yt - y0);
  r = SCALE_TO_POINTS(radius);

  /*--- Compute the fraction coefficient for the curve ---*/
  if(fabs(r) < 1.0e-4)
    {
      /*--- Almost zero radius ---*/
      fraction = 0.0;
    }
  else
    {
      fraction = (4.0/3.0) / (1.0 + sqrt(1.0 + (dx * dx + dy * dy) / (r * r)));
    }

  if(first && !F(post).putLine(post, "$p l", x0, y0)) return FALSE;
  return F(post).putLine(post, "$p $p $p c",
			 x0 + (coord_t)((xt - x0) * fraction),
			 y0 + (coord_t)((yt - y0) * fraction),
			 x3 + (coord_t)((xt - x3) * fraction),
			 y3 + (coord_t)((yt - y3) * fraction), x3, y3);
}


/* ----------------------------------------------------------------- ** 
** readBG - Read sector                                              ** 
** ----------------------------------------------------------------- */
static boolean readBG(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
BaseStd_t *base;
{
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeBG - Write sector                                            ** 
** ----------------------------------------------------------------- */
static boolean writeBG(This, doc)
c_BG_Sector	 *This;
c_DocFile	 *doc;
{
  return FALSE;
}



