/*
** Matrix.c for Xclamation in Matrix/
** Methods for the Matrix class
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
** Started on  Thu Dec  1 13:09:11 1994 Stéphane Boisson
** Last update Wed Apr 24 17:50:37 1996 Stéphane Boisson
*/

#define NHISTORY
#define NTRACE

#include "Matrix.h"

static void *constructor();
static void destructor();
static void *copy();
static matrix_t *pushMatrix ___PROTO((c_Matrix *this));
static matrix_t *getMatrix ___PROTO((c_Matrix *this, int number));
static void popMatrix ___PROTO((c_Matrix *this));
static coord_t convertX ___PROTO((c_Matrix *this, coord_t x, coord_t y));
static coord_t convertY ___PROTO((c_Matrix *this, coord_t x, coord_t y));
static coord_t convertDx ___PROTO((c_Matrix *this, coord_t x, coord_t y));
static coord_t convertDy ___PROTO((c_Matrix *this, coord_t x, coord_t y));
static boolean translate ___PROTO((c_Matrix *this, boolean push_flag,
				   coord_t dx, coord_t dy));
static boolean hardTranslate ___PROTO((c_Matrix *this, boolean push_flag,
				       coord_t dx, coord_t dy));
static boolean scale ___PROTO((c_Matrix *this, boolean push_flag,
			       scale_t sx, scale_t sy));
static boolean rotate ___PROTO((c_Matrix *this, boolean push_flag, angle_t a));
static boolean transform ___PROTO((c_Matrix *this, boolean push_flag,
				   scale_t cxx, scale_t cyx,
				   scale_t cxy, scale_t cyy));
static boolean invert ___PROTO((c_Matrix *this, boolean push_flag));
static boolean identity ___PROTO((c_Matrix *this, boolean push_flag));
static boolean popMatrix2 ___PROTO((c_Matrix *this));
static boolean concatMatrix ___PROTO((c_Matrix *this, boolean push_flag,
				      matrix_t *matrix));
static boolean invertConcatMatrix ___PROTO((c_Matrix *this, boolean push_flag,
					    matrix_t *matrix));

static void matrix_multiply ___PROTO((matrix_t *a, matrix_t *b, matrix_t *c));
static void convertBBox ___PROTO((c_Matrix *this, bbox_t *src, bbox_t *dst));
static boolean vmirror ___PROTO((c_Matrix *this, boolean push_flag));
static boolean hmirror ___PROTO((c_Matrix *this, boolean push_flag));
static boolean initMatrix ___PROTO((c_Matrix *this, boolean push_flag,
				    matrix_t *matrix));


sf_Matrix fc_Matrix =
{
  constructor, destructor, copy,
  getMatrix, popMatrix,
  convertX, convertY,
  translate, scale, rotate, transform, 
  invert, identity,
  convertDx, convertDy,
  popMatrix2,
  concatMatrix,
  convertBBox,
  hardTranslate,
  invertConcatMatrix,
  vmirror, hmirror,
  initMatrix
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor()
{
  c_Matrix *this;

  Xc_HISTORY(("constructor"));
  
  if((this = Xc_malloc("Matrix", sizeof(c_Matrix))) == NULL) return NULL;
  this->f = &fc_Matrix;

  this->base = NULL;
  this->max_pos = 0;
  this->pos = -1;

  if(F(this).identity(this, TRUE) == FALSE)
    {
      Xc_free(this);
      return NULL;
    }
  return this;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Matrix *this;
{
  Xc_HISTORY(("destructor"));
  
  Xc_free(this->base);
  Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_Matrix *this;
{
  c_Matrix *that;

  Xc_HISTORY(("copy"));

  if((that = Xc_malloc("Matrix copy", sizeof(c_Matrix))) == NULL) return NULL;
  that->f = &fc_Matrix;

  that->base = Xc_malloc("Matrix array copy",
			 this->max_pos * sizeof(matrix_t));
  if(that->base == NULL)
    {
      Xc_free(that);
      return NULL;
    }
  memcpy(that->base, this->base, this->max_pos * sizeof(matrix_t));
  that->pos = this->pos;
  that->max_pos = this->max_pos;
  return that;
}


/* ----------------------------------------------------------------- ** 
** matrix_multiply - Matrix multiplication  C = AB (B = ctm)         ** 
** ----------------------------------------------------------------- */
static void matrix_multiply(a, b, c)
matrix_t *a;
matrix_t *b;
matrix_t *c;
{
  register double txx, txy, tyx, tyy;

  XcM_DUMP_MATRIX("A", a);
  XcM_DUMP_MATRIX("B", b);
  txx = (((double)a->M[0][0] * (double)b->M[0][0]) + 
	 ((double)a->M[0][1] * (double)b->M[1][0]));
  tyx = (((double)a->M[0][0] * (double)b->M[0][1]) +
	 ((double)a->M[0][1] * (double)b->M[1][1]));
  txy = (((double)a->M[1][0] * (double)b->M[0][0]) +
	 ((double)a->M[1][1] * (double)b->M[1][0]));
  tyy = (((double)a->M[1][0] * (double)b->M[0][1]) +
	 ((double)a->M[1][1] * (double)b->M[1][1]));
  
  c->M[0][0] = txx;
  c->M[0][1] = tyx;
  c->M[1][0] = txy;
  c->M[1][1] = tyy;
  XcM_DUMP_MATRIX("C = A*B", c);
}


/* ----------------------------------------------------------------- ** 
** pushMatrix - Dup the top matrix                                   ** 
** ----------------------------------------------------------------- */
static matrix_t *pushMatrix(this)
c_Matrix *this;
{
  matrix_t *new;

  Xc_TRACE(("pushMatrix(%d)", this->pos));

  this->pos++;
  if(this->pos == this->max_pos)
    {

      new = Xc_realloc("Matrix array",
		       this->base, (this->max_pos + 1) * sizeof(matrix_t));
      if(new == NULL) return NULL;
      this->max_pos++;
      this->base = new;
    }
  new = this->base + this->pos;
  if(this->pos > 0) memcpy(new, new - 1, sizeof(matrix_t));
  return new; 
}


/* ----------------------------------------------------------------- ** 
** getMatrix - Get a matrix                                          ** 
** ----------------------------------------------------------------- */
static matrix_t *getMatrix(this, number)
c_Matrix *this;
int number;
{
  Xc_HISTORY(("get(%d)", number));

  Xc_ASSERT((number >= 0) && (number <= this->pos));
  XcM_DUMP_MATRIX("ctm", this->base + this->pos - number);
  return(this->base + this->pos - number);
}


/* ----------------------------------------------------------------- ** 
** pop - Pop the top matrix in list                                  ** 
** ----------------------------------------------------------------- */
static void popMatrix(this)
c_Matrix *this;
{
  Xc_HISTORY(("pop(%d)", this->pos));
  
  Xc_ASSERT(this->pos > 0);
  this->pos--;
}


/* ----------------------------------------------------------------- ** 
** convertX - Calculate a X coord                                    ** 
** ----------------------------------------------------------------- */
static coord_t convertX(this, x, y)
c_Matrix *this;
coord_t x;
coord_t y;
{
  matrix_t *m;

  Xc_TRACE(("convertX"));
  
  m = this->base + this->pos;
  return(ROUNDFLOAT((m->M[0][0] * x) + (m->M[1][0] * y)) + m->v.dx);
}


/* ----------------------------------------------------------------- ** 
** convertY - Calculate a Y coord                                    ** 
** ----------------------------------------------------------------- */
static coord_t convertY(this, x, y)
c_Matrix *this;
coord_t x;
coord_t y;
{
  matrix_t *m;

  Xc_TRACE(("convertY"));
  
  m = this->base + this->pos;
  return(ROUNDFLOAT((m->M[0][1] * x) + (m->M[1][1] * y)) + m->v.dy);
}


/* ----------------------------------------------------------------- ** 
** convertX - Calculate a X coord                                    ** 
** ----------------------------------------------------------------- */
static coord_t convertDx(this, x, y)
c_Matrix *this;
coord_t x;
coord_t y;
{
  matrix_t *m;

  Xc_TRACE(("convertDx"));
  
  m = this->base + this->pos;
  return ROUNDFLOAT((m->M[0][0] * x) + (m->M[1][0] * y));
}


/* ----------------------------------------------------------------- ** 
** convertY - Calculate a Y coord                                    ** 
** ----------------------------------------------------------------- */
static coord_t convertDy(this, x, y)
c_Matrix *this;
coord_t x;
coord_t y;
{
  matrix_t *m;

  Xc_TRACE(("convertDy"));
  
  m = this->base + this->pos;
  return ROUNDFLOAT((m->M[0][1] * x) + (m->M[1][1] * y));
}


/* ----------------------------------------------------------------- ** 
** hardTranslate - Direct translation                                ** 
** ----------------------------------------------------------------- */
static boolean hardTranslate(this, push_flag, dx, dy)
c_Matrix *this;
boolean push_flag;
coord_t dx;
coord_t dy;
{
  matrix_t *m;

  Xc_HISTORY(("hardTranslate(%g, %g)",
	      SCALE_TO_POINTS(dx), SCALE_TO_POINTS(dy)));
  
  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  m->v.dx += dx;
  m->v.dy += dy;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** translate - Translation                                           ** 
** ----------------------------------------------------------------- */
static boolean translate(this, push_flag, dx, dy)
c_Matrix *this;
boolean push_flag;
coord_t dx;
coord_t dy;
{
  matrix_t *m;
  double oldx, oldy;

  Xc_HISTORY(("translate(%g, %g)", SCALE_TO_POINTS(dx), SCALE_TO_POINTS(dy)));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  if((dx == COORD_ZERO) && (dy == COORD_ZERO)) return TRUE;

  oldx = (double)m->v.dx;
  oldy = (double)m->v.dy;
  m->v.dx = ROUNDFLOAT(((double)m->M[0][0] * (double)dx) +
		       ((double)m->M[1][0] * (double)dy) + (double)oldx);
  m->v.dy = ROUNDFLOAT(((double)m->M[0][1] * (double)dx) +
		       ((double)m->M[1][1] * (double)dy) + (double)oldy);
  Xc_TRACE(("(%g, %g) + (%g %g)",
	    oldx / COORD_SCALEVALUE,
	    oldy / COORD_SCALEVALUE,
	    ((double)m->v.dx - oldx) / COORD_SCALEVALUE,
	    ((double)m->v.dy - oldy) / COORD_SCALEVALUE));
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** scale - Scaling                                                   ** 
** ----------------------------------------------------------------- */
static boolean scale(this, push_flag, sx, sy)
c_Matrix *this;
boolean push_flag;
scale_t sx;
scale_t sy;
{
  matrix_t *m, tmp;

  Xc_HISTORY(("scale(%g, %g)", SCALE_TO_COEF(sx), SCALE_TO_COEF(sy)));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  if((sx == SCALE_ONE) && (sy == SCALE_ONE)) return TRUE;

  tmp.M[0][0] = SCALE_TO_COEF(sx);
  tmp.M[1][1] = SCALE_TO_COEF(sy);
  tmp.M[1][0] = tmp.M[0][1] = 0.0;
  tmp.v.dx = tmp.v.dy = COORD_ZERO;

  matrix_multiply(&tmp, m, m);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** rotate - Rotation                                                 ** 
** ----------------------------------------------------------------- */
static boolean rotate(this, push_flag, a)
c_Matrix *this;
boolean push_flag;
angle_t a;
{
  matrix_t *m, tmp;
  double angle;

  Xc_HISTORY(("rotate(%g°)", SCALE_TO_DEGREES(a)));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  if(a == ANGLE_ZERO) return TRUE;

  angle = SCALE_TO_RADIANS(a);
  tmp.M[0][0] = tmp.M[1][1] = cos(angle);
  tmp.M[1][0] = -(tmp.M[0][1] = sin(angle)); 
  tmp.v.dx = tmp.v.dy = COORD_ZERO;

  matrix_multiply(&tmp, m, m);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** transform - Transformation                                        ** 
** ----------------------------------------------------------------- */
static boolean transform(this, push_flag, cxx, cyx, cxy, cyy)
c_Matrix *this;
boolean push_flag;
scale_t cxx;
scale_t cyx;
scale_t cxy;
scale_t cyy;
{
  matrix_t *m, tmp;

  Xc_HISTORY(("transform(%g, %g, %g, %g)",
	      SCALE_TO_COEF(cxx), SCALE_TO_COEF(cyx),
	      SCALE_TO_COEF(cxy), SCALE_TO_COEF(cyy)));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  tmp.M[0][0] = SCALE_TO_COEF(cxx);
  tmp.M[0][1] = SCALE_TO_COEF(cyx);
  tmp.M[1][0] = SCALE_TO_COEF(cxy);
  tmp.M[1][1] = SCALE_TO_COEF(cyy);
  tmp.v.dx = tmp.v.dy = COORD_ZERO;

  matrix_multiply(&tmp, m, m);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** invert                                                            ** 
** ----------------------------------------------------------------- */
static boolean invert(this, push_flag)
c_Matrix *this;
boolean push_flag;
{
  matrix_t *m;
  real d;
  real txx,txy,tyx,tyy;

  Xc_HISTORY(("invert"));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;
 
  txx = m->M[0][0];
  txy = m->M[1][0];
  tyx = m->M[0][1];
  tyy = m->M[1][1];

  d = tyy * txx - txy * tyx;
  if(d == 0.0)
    {
      Xc_WARNING(("Matrix inversion impossible"));
      return FALSE;
    }

  m->M[0][0] = tyy / d;
  m->M[1][0] = -txy / d;
  m->M[0][1] = -tyx / d;
  m->M[1][1] = txx / d;
  m->v.dx = -m->v.dx;
  m->v.dy = -m->v.dy;

  XcM_DUMP_MATRIX("A", m);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** identity                                                          ** 
** ----------------------------------------------------------------- */
static boolean identity(this, push_flag)
c_Matrix *this;
boolean push_flag;
{
  matrix_t *m;

  Xc_HISTORY(("identity"));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  m->M[0][0] = m->M[1][1] = 1.0;
  m->M[1][0] = m->M[0][1] = 0.0;
  m->v.dx = m->v.dy = COORD_ZERO;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** push - Push matrix interface                                      ** 
** ----------------------------------------------------------------- */
static boolean popMatrix2(this)
c_Matrix *this;
{
  Xc_HISTORY(("push(%d)", this->pos));

  return((pushMatrix(this) == NULL)? FALSE : TRUE);
}


/* ----------------------------------------------------------------- ** 
** concat - Concat two matrixes                                      ** 
** ----------------------------------------------------------------- */
static boolean concatMatrix(this, push_flag, source)
c_Matrix *this;
boolean push_flag;
matrix_t *source;
{
  register double txx, txy, tyx, tyy;
  matrix_t *target;

  Xc_HISTORY(("concat"));

  if(push_flag == FALSE)
    target = this->base + this->pos;
  else if((target = pushMatrix(this)) == NULL)
    return FALSE;

  XcM_DUMP_MATRIX("A", source);
  XcM_DUMP_MATRIX("B", target);
  txx = (((double)source->M[0][0] * (double)target->M[0][0]) + 
	 ((double)source->M[0][1] * (double)target->M[1][0]));
  tyx = (((double)source->M[0][0] * (double)target->M[0][1]) +
	 ((double)source->M[0][1] * (double)target->M[1][1]));
  txy = (((double)source->M[1][0] * (double)target->M[0][0]) +
	 ((double)source->M[1][1] * (double)target->M[1][0]));
  tyy = (((double)source->M[1][0] * (double)target->M[0][1]) +
	 ((double)source->M[1][1] * (double)target->M[1][1]));
  
  target->v.dx += ((source->v.dx * (double)target->M[0][0]) + 
		   (source->v.dy * (double)target->M[1][0]));
  target->v.dy += ((source->v.dx * (double)target->M[0][1]) + 
		   (source->v.dy * (double)target->M[1][1]));
  target->M[0][0] = txx;
  target->M[0][1] = tyx;
  target->M[1][0] = txy;
  target->M[1][1] = tyy;
  XcM_DUMP_MATRIX("C = A*B", target);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** concat - Concat two matrixes                                      ** 
** ----------------------------------------------------------------- */
static boolean invertConcatMatrix(this, push_flag, source)
c_Matrix *this;
boolean push_flag;
matrix_t *source;
{
  register double txx, txy, tyx, tyy;
  matrix_t *target;

  Xc_HISTORY(("concat"));

  if(push_flag == FALSE)
    target = this->base + this->pos;
  else if((target = pushMatrix(this)) == NULL)
    return FALSE;

  XcM_DUMP_MATRIX("A", target);
  XcM_DUMP_MATRIX("B", source);
  txx = (((double)target->M[0][0] * (double)source->M[0][0]) + 
	 ((double)target->M[0][1] * (double)source->M[1][0]));
  tyx = (((double)target->M[0][0] * (double)source->M[0][1]) +
	 ((double)target->M[0][1] * (double)source->M[1][1]));
  txy = (((double)target->M[1][0] * (double)source->M[0][0]) +
	 ((double)target->M[1][1] * (double)source->M[1][0]));
  tyy = (((double)target->M[1][0] * (double)source->M[0][1]) +
	 ((double)target->M[1][1] * (double)source->M[1][1]));
  
  target->v.dx += ((target->v.dx * (double)source->M[0][0]) + 
		   (target->v.dy * (double)source->M[1][0]));
  target->v.dy += ((target->v.dx * (double)source->M[0][1]) + 
		   (target->v.dy * (double)source->M[1][1]));
  target->M[0][0] = txx;
  target->M[0][1] = tyx;
  target->M[1][0] = txy;
  target->M[1][1] = tyy;
  XcM_DUMP_MATRIX("C = A*B", target);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** convertBBox - Convert a bounding box                              ** 
** ----------------------------------------------------------------- */
static void convertBBox(this, src, dst)
c_Matrix *this;
bbox_t *src;
bbox_t *dst;
{
  matrix_t *m;
  bbox_t tmp;
  coord_t v;

/*  Xc_HISTORY(("convertBBox")); */
  
  if(dst == src)
    {
      Xc_BBOX_COPY(&tmp, src);
      src = &tmp;
    }

  m = this->base + this->pos;
  dst->llx = dst->urx = (coord_t)XcM_DX(m, src->llx, src->lly);

  v = (coord_t)XcM_DX(m, src->urx, src->ury);
  if(v < dst->llx) dst->llx = v;
  if(v > dst->urx) dst->urx = v; 

  v = (coord_t)XcM_DX(m, src->llx, src->ury);
  if(v < dst->llx) dst->llx = v;
  if(v > dst->urx) dst->urx = v; 

  v = (coord_t)XcM_DX(m, src->urx, src->lly);
  if(v < dst->llx) dst->llx = v;
  if(v > dst->urx) dst->urx = v; 

  dst->lly = dst->ury = (coord_t)XcM_DY(m, src->llx, src->lly);

  v = (coord_t)XcM_DY(m, src->urx, src->ury); 
  if(v < dst->lly) dst->lly = v;
  if(v > dst->ury) dst->ury = v; 

  v = (coord_t)XcM_DY(m, src->llx, src->ury);
  if(v < dst->lly) dst->lly = v;
  if(v > dst->ury) dst->ury = v;

  v = (coord_t)XcM_DY(m, src->urx, src->lly);
  if(v < dst->lly) dst->lly = v;
  if(v > dst->ury) dst->ury = v;
}


/* ----------------------------------------------------------------- ** 
** vmirror - Make a vertical mirror                                  ** 
** ----------------------------------------------------------------- */
static boolean vmirror(this, push_flag)
c_Matrix *this;
boolean push_flag;
{
  matrix_t *m;

  Xc_HISTORY(("vmirror"));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  m->M[0][1] = -m->M[0][1];
  m->M[1][1] = -m->M[1][1];
  
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** hmirror - Make a horizontal mirror                                ** 
** ----------------------------------------------------------------- */
static boolean hmirror(this, push_flag)
c_Matrix *this;
boolean push_flag;
{
  matrix_t *m;

  Xc_HISTORY(("hmirror"));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;

  m->M[1][0] = -m->M[1][0];
  m->M[0][0] = -m->M[0][0];
  
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** init - Init matrix                                                ** 
** ----------------------------------------------------------------- */
static boolean initMatrix(this, push_flag, matrix)
c_Matrix *this;
boolean push_flag;
matrix_t *matrix;
{
  matrix_t *m;

  Xc_HISTORY(("init"));

  if(push_flag == FALSE)
    m = this->base + this->pos;
  else if((m = pushMatrix(this)) == NULL)
    return FALSE;
    
  XcM_COPY_MATRIX(m, matrix);
  
  return TRUE;
}
