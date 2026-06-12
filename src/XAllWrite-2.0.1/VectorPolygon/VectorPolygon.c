/*
** VectorPolygon.c for XAllWrite in VectorPolygon/
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
** Started on  Tue Jul 18 12:56:37 1995 St�phane Boisson
** Last update Thu Oct 24 19:21:39 1996 Emmanuel Paris
*/


#define NHISTORY
#define NTRACE

#include "VectorPolygon.h"
#include <stdarg.h>

#define BEZIER_SCALEBITS 16
#define BEZIER_SCALE(value) (((long)(value)) << BEZIER_SCALEBITS)
#define BEZIER_HALF (1L << (BEZIER_SCALEBITS - 1))
/*#define BEZIER_DESCALE(value) (((value) + ((value)>0)? BEZIER_HALF: -BEZIER_HALF) >> BEZIER_SCALEBITS)*/
/*#define BEZIER_DESCALE(value) (((value) + ((value)>0)? BEZIER_HALF: 0) >> BEZIER_SCALEBITS)*/
#define BEZIER_LIMIT (1L << (BEZIER_SCALEBITS + 2))


/*--- Standard methods ---*/
static void *constructor();
static void destructor();
static void *copy();

/*--- Standard frame methods ---*/
static error change_base();
static error polyInit();
static void polyRedraw();
static void polyPrint();
static boolean polyRead ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Cadre *frame));
boolean polyWrite ___PROTO((c_VectorPolygon *this, c_DocFile *doc));

/*--- Others methods ---*/
static boolean allocate_chunk ___PROTO((c_VectorPolygon *this));
___INLINE static boolean add_point ___PROTO((c_VectorPolygon *this, 
					     coord_t x, coord_t y,
					     poly_mode_t mode));

static boolean addLineto ___PROTO((c_VectorPolygon *this,
				   coord_t x, coord_t y));
static boolean addMoveto ___PROTO((c_VectorPolygon *this,
				   coord_t x, coord_t y));
static boolean addCurveto ___PROTO((c_VectorPolygon *this,
				    coord_t x, coord_t y,
				    coord_t x2, coord_t y2,
				    coord_t x3, coord_t y3));
static boolean addClosepath ___PROTO((c_VectorPolygon *this));
static boolean set ___PROTO((c_VectorPolygon *this, ...));
static boolean polyStroke ___PROTO((c_VectorPolygon *this));
static boolean polyFill ___PROTO((c_VectorPolygon *this));
static boolean polyClip ___PROTO((c_VectorPolygon *this));
static void getCurrentPoint ___PROTO((c_VectorPolygon *this,
				      coord_t *xp, coord_t *yp));
static void hook_color ___PROTO((c_VectorPolygon *this, int reason,
                                 c_Color *old, c_Color *new));

static XPoint *bezier_step ___PROTO((register XPoint *out,
				     register long xa, register long ya,
				     register long xb, register long yb,
				     register long xc, register long yc));



sf_VectorPolygon fc_VectorPolygon =
{
  constructor, destructor, copy,
  change_base, polyInit, polyRedraw, polyPrint, polyRead, polyWrite, 
  addMoveto, addLineto, addCurveto, addClosepath,
  polyStroke, polyFill, polyClip,
  set,
  getCurrentPoint,
};

static ItemHandle *chunk_handle = NULL;
static ItemHandle *i_handle = NULL;
static long chunk_references = 0;
static long i_references = 0;


/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *constructor()
{
  c_VectorPolygon *this;

  Xc_HISTORY(("Constructor"));
  
  if(i_handle == NULL)
    i_handle = Xc_createItem("VectorPolygons", 16, sizeof(c_VectorPolygon));
  if(i_handle == NULL) return NULL;

  if((this = Xc_new(i_handle)) == NULL) return NULL;
  this->f = &fc_VectorPolygon;
  i_references++;

  this->start = this->end = NULL;
  if(allocate_chunk(this) == FALSE)
    {
      DELETE(c_VectorPolygon)(this);
      return NULL;
    }

  if((this->hook = NEW(c_Hook)(this, 1)) == NULL)
    {
      DELETE(c_VectorPolygon)(this);
      return NULL;
    }

  this->cache = NULL;
  this->fill_color = NULL;
  this->stroke_color = NULL;
  this->draw_type = 0;
  this->end_flag = FALSE;
  this->first_point.dx = this->first_point.dy = COORD_ZERO;
 
  this->first_flag = TRUE;

  Xc_TRACE(("done $%lx", this));
  return this;
}


/* ----------------------------------------------------------------- ** 
** destruction method                                                ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_VectorPolygon *this;
{
  poly_chunk_t *chunk;
  
  Xc_HISTORY(("destructor"));
  Xc_ASSERT(i_references > 0);

  /*--- Free chunks ---*/
  chunk = this->start;
  while(chunk != NULL)
    {
      poly_chunk_t *tmp;

      tmp = chunk->next;
      Xc_delete(chunk_handle, chunk); chunk_references--;
      chunk = tmp;
    }
  Xc_ASSERT(chunk_references >= 0);
  if(chunk_references == 0)
    {
      Xc_destroyItem(chunk_handle);
      chunk_handle = NULL;
    }

  if(this->cache != NULL) Xc_free(this->cache);
  if(this->dash != NULL) Xc_free(this->dash);

  if(this->stroke_color != NULL)
    XcH_UNREGISTER(this->stroke_color, hook_color, this);
  if(this->fill_color != NULL)
    XcH_UNREGISTER(this->fill_color, hook_color, this);

  if(this->hook != NULL) DELETE(c_Hook)(this->hook);

  Xc_delete(i_handle, this);
  if(--i_references == 0)
    {
      Xc_destroyItem(i_handle);
      i_handle = NULL;
    }

  Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- ** 
** Copy method                                                       ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_VectorPolygon *this;
BaseStd_t *base;
{
  c_VectorPolygon *new;
  poly_chunk_t *chunk;
  c_Color *color;

  Xc_HISTORY(("Copy"));  

  if((new = Xc_new(i_handle)) == NULL) return NULL;
  i_references++;

  /*--- Copy data ---*/
  memcpy(new, this, sizeof(c_VectorPolygon));
  new->hook = NULL;
  new->start = new->end = NULL;
  new->stroke_color = new->fill_color = NULL;
  new->cache = NULL;

  /*--- Copy dashes ---*/
  if(new->dash != NULL)
    {
      new->dash = Xc_malloc("dashes", new->dash_count * sizeof(coord_t));
      if(new->dash == NULL)
	{
	  DELETE(c_VectorPolygon)(new);
	  return NULL;
	}
      memcpy(new->dash, this->dash, new->dash_count * sizeof(coord_t));
    }

  /*--- Copy chunks ---*/
  for(chunk = this->start; chunk != NULL; chunk = chunk->next)
    {
      if(allocate_chunk(new) == FALSE)
	{
	  DELETE(c_VectorPolygon)(new);
	  return NULL;
	}
      memcpy(new->end, chunk, sizeof(poly_chunk_t));
      new->end->next = NULL;
    }

  /*--- Hook ---*/
  if((new->hook = NEW(c_Hook)(new, 1)) == NULL)
    {
      DELETE(c_VectorPolygon)(new);
      return NULL;
    }


  /*--- Copy stroke color ---*/
  if(this->stroke_color != NULL)
    {
      color = F(this->stroke_color).virtualCopy(this->stroke_color, base);
      if(color == NULL)
	{
	  DELETE(c_VectorPolygon)(new);
	  return NULL;
	}
      
      if(!XcH_REGISTER(color, hook_color, new))
	{
	  DELETE(c_VectorPolygon)(new);
	  return NULL;
	}
      new->stroke_color = color;
    }

  /*--- Copy fill color ---*/
  if(this->fill_color != NULL)
    {
      color = F(this->fill_color).virtualCopy(this->fill_color, base);
      if(color == NULL)
	{
	  DELETE(c_VectorPolygon)(new);
	  return NULL;
	}
      
      if(!XcH_REGISTER(color, hook_color, new))
	{
	  DELETE(c_VectorPolygon)(new);
	  return NULL;
	}
      new->fill_color = color;
    }

  Xc_TRACE(("copied"));
  return new;
}


/* ----------------------------------------------------------------- ** 
** change_base - BaseStd change                                      ** 
** ----------------------------------------------------------------- */
static error change_base(this, base)
c_VectorPolygon *this;
BaseStd_t *base;
{
  c_Color *color;

  Xc_TRACE(("change_base"));

  if(this->stroke_color != NULL)
    {
      color = F(this->stroke_color).virtualCopy(this->stroke_color, base);
      if(color == NULL) return XC_MEMORY_ALLOCATION;

      if(!XcH_REGISTER(color, hook_color, this))
	{
	  DELETE(c_Color)(color);
	  return XC_MEMORY_ALLOCATION;
	}

      XcH_UNREGISTER(this->stroke_color, hook_color, this);
      this->stroke_color = color;
    }

  if(this->fill_color != NULL)
    {
      color = F(this->fill_color).virtualCopy(this->fill_color, base);
      if(color == NULL) return XC_MEMORY_ALLOCATION;

      if(!XcH_REGISTER(color, hook_color, this))
	{
	  DELETE(c_Color)(color);
	  return XC_MEMORY_ALLOCATION;
	}

      XcH_UNREGISTER(this->fill_color, hook_color, this);
      this->fill_color = color;
    }

  return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** allocate_chunk - Allocate_Chunk a point/operator chunk            ** 
** ----------------------------------------------------------------- */
static boolean allocate_chunk(this)
c_VectorPolygon	*this;
{
  poly_chunk_t *chunk;

  this->count = 0;

  if(chunk_handle == NULL)
    chunk_handle = Xc_createItem("Chunk base", 16, sizeof(poly_chunk_t));
  if(chunk_handle == NULL) return FALSE;

  if((chunk = Xc_new(chunk_handle)) == NULL) return FALSE;
  chunk_references++;

  chunk->next = NULL;

  if(this->start == NULL)
    {
      /*--- Initialisation ---*/
      this->start = this->end = chunk;
      return TRUE;
    }

  /*--- Appending chunk ---*/
  this->end->next = chunk;
  this->end = chunk;

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** add_point - Add a point/operator                                   ** 
** ----------------------------------------------------------------- */
___INLINE static boolean add_point(this, x, y, type)
c_VectorPolygon	*this;
coord_t		x;
coord_t		y;
poly_mode_t	type;
{
  poly_chunk_t *chunk;
  unsigned long index;

  Xc_TRACE(("add_point(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

  chunk = this->end;
  if(this->count >= XcVP_POINT_CHUNK_SIZE)
    {
      if(allocate_chunk(this) == FALSE) return FALSE;
      chunk = this->end;
    }

  index = this->count++;
  chunk->x[index] = x;
  chunk->y[index] = y;
  chunk->type[index] = type;

  if(type != XcVP_RENDER)
    {
      /*--- Update bounding box ---*/
      if(this->first_flag == FALSE)
	{
	  if(this->bbox.llx > x) this->bbox.llx = x;
	  if(this->bbox.lly > y) this->bbox.lly = y;
	  if(this->bbox.urx < x) this->bbox.urx = x;
	  if(this->bbox.ury < y) this->bbox.ury = y;
	}
      else
	{
	  this->bbox.llx = this->bbox.urx = x;
	  this->bbox.lly = this->bbox.ury = y;
	  this->first_flag = FALSE;
	}
    }
  else
    {
      if(this->draw_type & XcVP_STROKE)
	{
	  coord_t delta;

	  delta = this->line_width >> 1;
	  this->bbox.llx -= delta;
	  this->bbox.lly -= delta;
	  this->bbox.urx += delta;
	  this->bbox.ury += delta;
	}

      Xc_TRACE(("bbox(%g/%g, %g/%g)",
		SCALE_TO_POINTS(this->bbox.llx),
		SCALE_TO_POINTS(this->bbox.urx),
		SCALE_TO_POINTS(this->bbox.lly),
		SCALE_TO_POINTS(this->bbox.ury)));
      return TRUE;
    }
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** addMoveto - Add a `moveto' operator                               ** 
** ----------------------------------------------------------------- */
static boolean addMoveto(this, x, y)
c_VectorPolygon *this;
coord_t		x;
coord_t		y;
{
  Xc_HISTORY(("moveto(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

  this->last_point.dx = this->first_point.dx = x;
  this->last_point.dy = this->first_point.dy = y;
  return add_point(this, x, y, XcVP_MOVE);
}


/* ----------------------------------------------------------------- ** 
** addLineto - Add a `lineto' operator                               ** 
** ----------------------------------------------------------------- */
static boolean addLineto(this, x, y)
c_VectorPolygon	*this;
coord_t		x;
coord_t		y;
{
  Xc_HISTORY(("lineto(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

  this->last_point.dx = x, this->last_point.dy = y;
  return add_point(this, x, y, XcVP_LINE);
}


/* ----------------------------------------------------------------- ** 
** addCurveto - Add a `curveto' operator                             ** 
** ----------------------------------------------------------------- */
static boolean addCurveto(this, x, y, x2, y2, x3, y3)
c_VectorPolygon	*this;
coord_t		x;
coord_t		y;
coord_t		x2;
coord_t		y2;
coord_t		x3;
coord_t		y3;
{
  Xc_HISTORY(("curveto(%g, %g, %g, %g, %g, %g)",
	      SCALE_TO_POINTS(x), SCALE_TO_POINTS(y),
	      SCALE_TO_POINTS(x2), SCALE_TO_POINTS(y2),
	      SCALE_TO_POINTS(x3), SCALE_TO_POINTS(y3)));

  this->last_point.dx = x3, this->last_point.dy = y3;
  return(add_point(this, x, y, XcVP_CONTROL) &&
	 add_point(this, x2, y2, XcVP_CONTROL) &&
	 add_point(this, x3, y3, XcVP_CURVE));
}


/* ----------------------------------------------------------------- ** 
** addClosepath - Add a `closepath' operator                         ** 
** ----------------------------------------------------------------- */
static boolean addClosepath(this)
c_VectorPolygon *this;
{
  Xc_HISTORY(("closepath"));

  if((this->last_point.dx == this->first_point.dx) &&
     (this->last_point.dy == this->first_point.dy)) return TRUE;
  this->last_point.dx = this->first_point.dx;
  this->last_point.dy = this->first_point.dy;
  return add_point(this, this->first_point.dx,
		   this->first_point.dy, XcVP_CLOSEPATH);
}


/* ----------------------------------------------------------------- ** 
** polyStroke - Set the polygon to be stroked                        ** 
** ----------------------------------------------------------------- */
static boolean polyStroke(this)
c_VectorPolygon *this;
{
  Xc_HISTORY(("stroke"));
  
  this->draw_type |= XcVP_STROKE;
  if(this->end_flag == FALSE)
    {
      this->end_flag = TRUE;
      return add_point(this, COORD_ZERO, COORD_ZERO, XcVP_RENDER);
    }
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** polyFill - Set the polygon to be filled                           ** 
** ----------------------------------------------------------------- */
static boolean polyFill(this)
c_VectorPolygon *this;
{
  Xc_HISTORY(("fill"));
  
  this->draw_type |= XcVP_FILL;
  if(this->end_flag == FALSE)
    {
      this->end_flag = TRUE;
      return add_point(this, COORD_ZERO, COORD_ZERO, XcVP_RENDER);
    }
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** polyClip - Set the polygon to be clipping mask                    ** 
** ----------------------------------------------------------------- */
static boolean polyClip(this)
c_VectorPolygon *this;
{
  Xc_HISTORY(("clip"));
  
  this->draw_type |= XcVP_CLIP;
  if(this->end_flag == FALSE)
    {
      this->end_flag = TRUE;
      return add_point(this, COORD_ZERO, COORD_ZERO, XcVP_RENDER);
    }
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** getCurrentPoint - Get the last point of the polygon               ** 
** ----------------------------------------------------------------- */
static void getCurrentPoint(this, xp, yp)
c_VectorPolygon *this;
coord_t *xp;
coord_t *yp;
{
  Xc_HISTORY(("getCurrentPoint"));

  *xp = this->last_point.dx;
  *yp = this->last_point.dy;
}


/* ----------------------------------------------------------------- ** 
** bezier_step - Recurse to linearize a bezier curve                 ** 
** ----------------------------------------------------------------- */
static XPoint *bezier_step(out, xA, yA, xB, yB, xC, yC)
register XPoint *out;
register long xA,yA;       /* control point 1 */
register long xB,yB;       /* control point 2 */
register long xC,yC;       /* End */
{
  register long dmax, tmp;

  Xc_TRACE(("bezier_step(%ld, %ld, %ld, %ld, %ld, %ld)",
	    xA, yA, xB, yB, xC, yC));

  /*--- Recursion end test ---*/
#if 1
  dmax = ABS(xA), tmp = ABS(yA);
  if(dmax < tmp) dmax = tmp;
  tmp = ABS(xC - xB);
  if(dmax < tmp) dmax = tmp;
  tmp = ABS(yC - yB);
  if(dmax < tmp) dmax = tmp;

  if(dmax <= BEZIER_LIMIT)
#else
  dmax = MAX(xA, MAX(xB, xC)) - MIN(xA, MAX(xB, xC));
  tmp = MAX(yA, MAX(yB, yC)) - MIN(yA, MAX(yB, yC));
  /*if((dmax < BEZIER_SCALE(1.5)) || (tmp < BEZIER_SCALE(1.5)))*/
#endif
    { 
      out->x = (xC + BEZIER_HALF) >> BEZIER_SCALEBITS;
      out->y = (yC + BEZIER_HALF) >> BEZIER_SCALEBITS;
      return(out + 1);
    }
  else
    {
      long x, y, dmax2, tmp2;

      tmp = xA; tmp2 = yA;
      xA += xB; yA += yB;
      xB += xC; yB += yC;

      dmax = tmp + xA; dmax2 = tmp2 + yA;
      xA += xB; yA += yB;

      x = (dmax + xA) >> 3; y = (dmax2 + yA) >> 3;

      out = bezier_step(out,
			tmp >> 1, tmp2 >> 1,
			dmax >> 2, dmax2 >> 2,
			x, y);

      return bezier_step(out,
			 (xA >> 2) - x, (yA >> 2) - y,
			 (xB >> 1) - x, (yB >> 1) - y,
			 xC - x, yC - y);
    }
} 


/* ----------------------------------------------------------------- ** 
** polyWrite - Write polygon                                        ** 
** ----------------------------------------------------------------- */
boolean polyWrite(this, doc)
c_VectorPolygon *this;
c_DocFile *doc;
{
  Xc_HISTORY(("write"));
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** polyRead - Read polygon                                           ** 
** ----------------------------------------------------------------- */
static boolean polyRead(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
  Xc_HISTORY(("read"));
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** set - Set the polygon parameters                                  ** 
** ----------------------------------------------------------------- */
static boolean set(c_VectorPolygon *this, ...)
{
  boolean exit_flag, rc;
  va_list ap;

  Xc_HISTORY(("set"));

  va_start(ap, this);
  for(exit_flag = FALSE, rc = TRUE; exit_flag == FALSE; )
    switch(va_arg(ap, polygon_set_code_t))
      {
      case XcVP_END:
	Xc_TRACE(("end"));
	exit_flag = TRUE;
	break;

      case XcVP_STROKE_COLOR:
	Xc_TRACE(("stroke color"));
	if(this->stroke_color != NULL)
	  XcH_UNREGISTER(this->stroke_color, hook_color, this);
	this->stroke_color = va_arg(ap, c_Color *);
	if(this->stroke_color != NULL)
	  if(!XcH_REGISTER(this->stroke_color, hook_color, this)) rc = FALSE;
	break;

      case XcVP_FILL_COLOR:
	Xc_TRACE(("fill color"));
	if(this->fill_color != NULL)
	  XcH_UNREGISTER(this->fill_color, hook_color, this);
	this->fill_color = va_arg(ap, c_Color *);
	if(this->fill_color != NULL)
	  if(!XcH_REGISTER(this->fill_color, hook_color, this)) rc = FALSE;
	break;

      case XcVP_LINE_WIDTH:
	Xc_TRACE(("line width"));
	this->line_width = va_arg(ap, coord_t);
	break;

      case XcVP_LINE_DASH:
	Xc_TRACE(("line dash"));
	this->phase = va_arg(ap, coord_t);
	this->dash = va_arg(ap, coord_t *);
	break;

      case XcVP_FLATNESS:
	Xc_TRACE(("flatness"));
	this->flatness = va_arg(ap, coord_t);
	break;

      case XcVP_CAP_STYLE:
	Xc_TRACE(("cap style"));
	this->cap = va_arg(ap, poly_cap_t);
	break;

      case XcVP_JOIN_STYLE:
	Xc_TRACE(("join style"));
	this->join = va_arg(ap, poly_join_t);
	break;

      case XcVP_MITER_LIMIT:
	Xc_TRACE(("miter limit"));
	this->miter_limit = va_arg(ap, coord_t);
	break;

      default:
        Xc_BREAK(("Unknow polygon-set-code"));
      }
  va_end(ap);
  return TRUE;
}



/* ----------------------------------------------------------------- ** 
** polyInit - Polygon X11 cache init                                 ** 
** ----------------------------------------------------------------- */
static error polyInit(this, matrix, scale)
c_VectorPolygon	*this;
c_Matrix *matrix;
scale_t scale;
{
  register poly_chunk_t *chunk;
  matrix_t *m, *mb;
  short old_x, old_y;
  boolean exit_flag;
  XPoint *ptr;
  unsigned long i;
  vector_t last;

  Xc_HISTORY(("polyInit($%lx)", this));

  /*--- Set up matrixes ---*/
  exit_flag = F(matrix).scale(matrix, TRUE,
			      SCALE_FROM_COEF(1 << BEZIER_SCALEBITS),
			      SCALE_FROM_COEF(1 << BEZIER_SCALEBITS));
  if(exit_flag == FALSE) return XC_MEMORY_ALLOCATION;
  m = F(matrix).get(matrix, 1);
  mb = F(matrix).get(matrix, 0);
  XcM_DUMP_MATRIX("scale", m);
  XcM_DUMP_MATRIX("bezier_scale", mb);

  /*--- Setup cache ---*/
  if(this->cache != NULL) Xc_free(this->cache);
  ptr = Xc_malloc("shape cache", sizeof(XPoint) * XcVP_XPOINTS);
  if((this->cache = ptr) == NULL)
    {
      F(matrix).pop(matrix);
      return XC_MEMORY_ALLOCATION;
    }

  chunk = this->start;

  Xc_ASSERT(chunk->type[0] == XcVP_MOVE);

  i = 0;
  old_x = old_y = 0;

  exit_flag = FALSE;
  while(42)
    {
      register short new_x, new_y;
      register coord_t x, y;

      x = chunk->x[i];
      y = chunk->y[i];

      switch(chunk->type[i])
	{
	  /*--- moveto ---*/
	case XcVP_MOVE:
	  last.dx = x;
	  last.dy = y;
	  new_x = (short)XcM_X(m, x, y);
	  new_y = (short)XcM_Y(m, x, y);

	  Xc_TRACE(("move(%d, %d)", (int)new_x, (int)new_y));

	  ptr->x = new_x - old_x;
	  (ptr++)->y = new_y - old_y;
	  old_x = new_x;
	  old_y = new_y;
	  break;

	  /*--- lineto ---*/
	case XcVP_LINE:
	  last.dx = x;
	  last.dy = y;
	  new_x = (short)XcM_X(m, x, y);
	  new_y = (short)XcM_Y(m, x, y);

	  Xc_TRACE(("line(%d, %d)", (int)new_x, (int)new_y));

	  if((new_x != old_x) || (new_y != old_y))
	    {
	      ptr->x = new_x - old_x;
	      (ptr++)->y = new_y - old_y;
	      old_x = new_x;
	      old_y = new_y;
	    }
	  break;

	  /*--- curveto ---*/
	case XcVP_CONTROL:
	  {
	    long x1, y1, x2, y2;

	    Xc_TRACE(("spline"));

	    x2 = (long)XcM_X(mb, last.dx, last.dy);
	    y2 = (long)XcM_Y(mb, last.dx, last.dy);

	    if(++i >= XcVP_POINT_CHUNK_SIZE) chunk = chunk->next, i = 0;
	    Xc_ASSERT(chunk->type[i] == XcVP_CONTROL);

	    x1 = (long)XcM_X(mb, chunk->x[i], chunk->y[i]) - x2;
	    y1 = (long)XcM_Y(mb, chunk->x[i], chunk->y[i]) - y2;

	    if(++i >= XcVP_POINT_CHUNK_SIZE) chunk = chunk->next, i = 0;
	    Xc_ASSERT(chunk->type[i] == XcVP_CURVE);

	    ptr = bezier_step(ptr,
			      (long)XcM_X(mb, x, y) - x2,
			      (long)XcM_Y(mb, x, y) - y2,
			      x1, y1,
			      (long)XcM_X(mb, chunk->x[i], chunk->y[i]) - x2,
			      (long)XcM_Y(mb, chunk->x[i], chunk->y[i]) - y2);
	    
	    last.dx = chunk->x[i];
	    last.dy = chunk->y[i];
	    old_x = (short)XcM_X(m, last.dx, last.dy);
	    old_y = (short)XcM_Y(m, last.dx, last.dy);
	    break;
	  }

	  /*--- closepath ---*/
	case XcVP_CLOSEPATH:
	  Xc_TRACE(("closepath(%g, %g)",
		    SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

	  new_x = (short)XcM_X(m, x, y);
	  new_y = (short)XcM_Y(m, x, y);

	  if((new_x != old_x) || (new_y != old_y))
	    {
	      ptr->x = new_x - old_x;
	      (ptr++)->y = new_y - old_y;
	    }

	  old_x = this->cache->x; old_y = this->cache->y;

	  if((new_x != old_x) || (new_y != old_x))
	    {
	      ptr->x = old_x - new_x;
	      (ptr++)->y = old_y - new_y;
	    }
	  break;

	  /*--- Render ---*/
	case XcVP_RENDER:
	  Xc_TRACE(("render"));

	  this->cache_count = ptr - this->cache;
	  exit_flag = TRUE;
	  break;

	  /*--- Unexpected types ---*/
	default:
	  Xc_BREAK(("Unexpected code"));
	}
      if(exit_flag != FALSE) break;
      if(++i >= XcVP_POINT_CHUNK_SIZE) chunk = chunk->next, i = 0;
    }

  ptr = Xc_realloc("Cache", this->cache, this->cache_count * sizeof(XPoint));
  Xc_ASSERT(ptr != NULL);
  this->cache = ptr;

  if(this->draw_type & XcVP_STROKE)
    {
      this->cache_width = this->line_width * SCALE_TO_COEF(scale);
      if(this->cache_width == 1) this->cache_width = 0;
    }

  F(matrix).pop(matrix);

  Xc_TRACE(("done"));
  return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** polyRedraw - Draw polygon                                         ** 
** ----------------------------------------------------------------- */
static void polyRedraw(this, frame, poly)
c_VectorPolygon	*this;
c_Cadre		*frame;
Polygone	*poly;
{
  Display *display;
  short dx, dy;
  GC gc;

  Xc_HISTORY(("polyRedraw"));

  if((this->draw_type == 0) || (this->cache == NULL))
    {
      Xc_WARNING(("No cache available"));
      return;
    }

  display = frame->X_info.display;
  gc = frame->X_info.gc_inside;

  dx = (frame->sOrig_X + frame->saOrig_X) / frame->sScale;
  dy = (frame->sOrig_Y + frame->saOrig_Y) / frame->sScale;

  this->cache->x += dx;
  this->cache->y += dy;

  if(this->draw_type & XcVP_FILL)
    {
      F(this->fill_color).setGC(this->fill_color, display, gc); 
      XFillPolygon(display,
		   frame->X_info.window, frame->X_info.gc_inside,
		   this->cache, this->cache_count,
		   Complex, CoordModePrevious);

    }

  if(this->draw_type & XcVP_STROKE)
    {
      XSetLineAttributes(display, gc, this->cache_width,
			 LineSolid, CapNotLast, JoinMiter);
      
      F(this->stroke_color).setGC(this->stroke_color, display, gc); 
      XDrawLines(display,
		 frame->X_info.window, frame->X_info.gc_inside,
		 this->cache, this->cache_count,
		 CoordModePrevious);
    }

  this->cache->x -= dx;
  this->cache->y -= dy;
}


/* ----------------------------------------------------------------- ** 
** polyPrint - Print polygon                                         ** 
** ----------------------------------------------------------------- */
static void polyPrint(this, post)
c_VectorPolygon	*this;
c_PostScript	*post;
{
  register coord_t last_x = COORD_ZERO, last_y = COORD_ZERO;
  register poly_chunk_t *chunk;
  boolean exit_flag, close_flag;
  unsigned long i;

  Xc_HISTORY(("ps_print"));

  chunk = this->start;

  Xc_ASSERT(chunk->type[0] == XcVP_MOVE);

  i = 0;
  
#if 0
  if(this->draw_type & XcVP_STROKE)
    F(this->stroke_color).ps_print(this->stroke_color, post, TRUE);
  if(this->draw_type & XcVP_FILL)
    F(this->fill_color).ps_print(this->fill_color, post, FALSE);
  F(post).putLine(post, "$c w", this->line_width);
#endif

  exit_flag = close_flag = FALSE;
  while(42)
    {
      switch(chunk->type[i])
	{
	  /*--- moveto ---*/
	case XcVP_MOVE:
	  last_x = chunk->x[i];
	  last_y = chunk->y[i];
	  if(close_flag != FALSE)
	    {
	      F(post).putLine(post, "h");
	      close_flag = FALSE;
	    }
	  F(post).putLine(post, "$p m", last_x, last_y);
	  break;

	  /*--- lineto ---*/
	case XcVP_LINE:
	  last_x = chunk->x[i];
	  last_y = chunk->y[i];
	  F(post).putLine(post, "$p l", last_x, last_y);
	  break;

	  /*--- curveto ---*/
	case XcVP_CONTROL:
	  {
	    coord_t x0, y0, x1, y1, x2, y2;

	    x0 = chunk->x[i];
	    y0 = chunk->y[i];

	    if(++i >= XcVP_POINT_CHUNK_SIZE) chunk = chunk->next, i = 0;
	    Xc_ASSERT(chunk->type[i] == XcVP_CONTROL);
	    x1 = chunk->x[i];
	    y1 = chunk->y[i];

	    if(++i >= XcVP_POINT_CHUNK_SIZE) chunk = chunk->next, i = 0;
	    Xc_ASSERT(chunk->type[i] == XcVP_CURVE);
	    x2 = chunk->x[i];
	    y2 = chunk->y[i];

	    if((x2 == x1) && (y2 == y1))
	      F(post).putLine(post, "$p $p y", x0, y0, x1, y1);
	    else if((last_x == x0) && (last_y == y0))
	      F(post).putLine(post, "$p $p v", x1, y1, x2, y2);
	    else
	      F(post).putLine(post, "$p $p $p c", x0, y0, x1, y1, x2, y2);

	    last_x = x2;
	    last_y = y2;
	    break;
	  }

	  /*--- closepath ---*/
	case XcVP_CLOSEPATH:
	  close_flag = TRUE;
	  break;

	  /*--- Render ---*/
	case XcVP_RENDER:
	  {
	    static char *op_close[] = {"s", "f", "b",
					 "q s Q W", "q f Q W", "q b Q W"};
	    static char *op_normal[] = {"S", "f", "B",
					  "q S Q W", "q f Q W", "q B Q W"};
	    char **op;
	    int mode;

	    op = (close_flag == FALSE)? op_normal : op_close;
	    mode = 0;
	    if(this->draw_type & XcVP_STROKE) mode |= 1;
	    if(this->draw_type & XcVP_FILL) mode |= 2;
	    if(this->draw_type & XcVP_CLIP) mode |= 4;
	    Xc_ASSERT(mode != 0);

	    F(post).putLine(post, op[mode - 1]);

	    exit_flag = TRUE;
	    break;
	  }

	  /*--- Unexpected types ---*/
	default:
	  Xc_BREAK(("Unexpected code"));
	}
      if(exit_flag != FALSE) break;
      if(++i >= XcVP_POINT_CHUNK_SIZE) chunk = chunk->next, i = 0;
    }

  Xc_TRACE(("printed"));
}


/* ----------------------------------------------------------------- **
** hook_color - Color class hook                                     **
** ----------------------------------------------------------------- */
static void hook_color(this, reason, old, new)
c_VectorPolygon *this;
int reason;
c_Color *old;
c_Color *new;
{
  Xc_TRACE(("hook_color"));

  if(reason == XcH_VALUE_CHANGED)
    {
      if(this->stroke_color == old) this->stroke_color = new;
      if(this->fill_color == old) this->fill_color = new;
    }
  F(this->hook).callback(this->hook, XcH_VALUE_CHANGED, this);
}

