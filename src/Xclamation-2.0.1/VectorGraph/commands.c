/*
** commands.c for Xclamation and XAllWrite in VectorGraph/
** Commands methods for VectorGraph class
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
** Started on  Wed Dec 13 16:12:44 1995 Stéphane Boisson
** Last update Fri Dec 31 16:08:56 1999 Emmanuel Paris
*/

/*#define NHISTORY*/
#define NTRACE

#include "VectorGraphP.h"
#include <varargs.h>


/*--- Constants ---*/
#define XcVG_TMP_CONTROL_CHUNK 128
#define XcVG_TMP_DATA_CHUNK 128


/*--- Local prototypes ---*/
static vector_item_t *allocItem ___PROTO((c_VectorGraph *this,
					  enum vector_type_e type));
static boolean addPathControl ___PROTO((c_VectorGraph *this,
					unsigned char op));
static boolean addPathData ___NPROTO((c_VectorGraph *this, int count, ...));
static boolean packPath ___PROTO((c_VectorGraph *this));


/*--- Local variables ---*/
size_t vectorItemExtraSize[]=
{
 sizeof(struct vector_children_data_s),/* XcVG_CONTEXT_TYPE		*/
 sizeof(vector_path_t),		/* XcVG_PATH_TYPE		*/
 sizeof(int),			/* XcVG_RENDER_TYPE		*/
 0,				/* XcVG_CLIP_TYPE		*/
 sizeof(c_Color *),		/* XcVG_STROKE_COLOR_TYPE	*/
 sizeof(c_Color *),		/* XcVG_FILL_COLOR_TYPE		*/
 sizeof(coord_t),		/* XcVG_LINE_WIDTH_TYPE		*/
 sizeof(int),			/* XcVG_LINE_JOIN_TYPE		*/
 sizeof(int),			/* XcVG_LINE_CAP_TYPE		*/
 sizeof(struct vector_dash_data_s),/* XcVG_LINE_DASH_TYPE		*/
 sizeof(real),			/* XcVG_MITER_LIMIT_TYPE	*/
 sizeof(int),			/* XcVG_FLATNESS_TYPE		*/

 /*___Text___*/
 sizeof(matrix_t),		/* XcVG_TEXT_MATRIX_TYPE	*/
 sizeof(vector_t),		/* XcVG_TEXT_TRANSLATE_TYPE	*/
 0,				/* XcVG_TEXT_NEWLINE_TYPE	*/
 sizeof(coord_t),		/* XcVG_TEXT_RISE_TYPE		*/
 sizeof(int),			/* XcVF_TEXT_MODE_TYPE		*/
 sizeof(struct vector_font_data_s),/* XcVG_TEXT_FONT_TYPE		*/
 sizeof(coord_t),		/* XcVG_TEXT_LEADING_TYPE	*/
 sizeof(coord_t),		/* XcVG_TEXT_WORD_SPACING_TYPE	*/
 sizeof(coord_t),		/* XcVG_TEXT_CHAR_SPACING_TYPE	*/
 sizeof(scale_t),		/* XcVG_TEXT_HSCALE_TYPE	*/
 sizeof(real),			/* XcVG_TEXT_KERNING_TYPE	*/
 sizeof(struct vector_text_data_s)/* XcVG_TEXT_RENDER_TYPE	*/
};

#ifndef NTRACE
static char *debug_type[] = {
			     "CONTEXT", "PATH", "RENDER", "CLIP",
			     "STROKE_COLOR", "FILL_COLOR",
			     "LINE_WIDTH", "LINE_JOIN", "LINE_CAP", "LINE_DASH",
			     "MITER_LIMIT", "FLATNESS",
			     "TEXT_MATRIX", "TEXT_TRANSLATE", "TEXT_NEWLINE", "TEXT_RISE",
			     "TEXT_MODE", "TEXT_FONT", "TEXT_LEADING",
			     "TEXT_WORD_SPACING", "TEXT_CHAR_SPACING",
			     "TEXT_HSCALE", "TEXT_KERNING",
			     "TEXT_RENDER"
			    };
#endif


/* ----------------------------------------------------------------- ** 
** allocItem - Allocate vector item                                  ** 
** ----------------------------------------------------------------- */
static vector_item_t *allocItem(this, type)
c_VectorGraph *this;
enum vector_type_e type;
{
 vector_item_t *ptr;
 size_t size;

 Xc_TRACE(("allocItem(%s)", debug_type[(int)type - (int)XcVG_CONTEXT_TYPE]));

#if 1
 size = (offsetof(vector_item_t, data) + 
	 vectorItemExtraSize[(int)type - (int)XcVG_CONTEXT_TYPE]);
#else
 size = sizeof(vector_item_t);
#endif

 /*Xc_TRACE((" size=%ld", size));*/
 if((ptr = Xc_smalloc(this->spool, size)) == NULL) return NULL;

 ptr->type = type;
 ptr->next = *this->ptr;
 *this->ptr = ptr;
 this->ptr = &ptr->next;

 Xc_TRACE(("  item=$%lx", (long)ptr));
 return ptr;
}


/* ----------------------------------------------------------------- ** 
** addPathControl - Add control op to current path                   ** 
** ----------------------------------------------------------------- */
static boolean addPathControl(this, op)
c_VectorGraph *this;
unsigned char op;
{
 Xc_TRACE(("addPathControl(%d)", op));

 if(this->max_path_control == this->path.control_count)
 {
  unsigned char *ptr;

  Xc_TRACE((" grow path control"));
  this->max_path_control += XcVG_TMP_CONTROL_CHUNK;
  ptr = Xc_realloc("path control", this->path.control,
		   this->max_path_control * sizeof(unsigned char));
  if(ptr == NULL)
  {
   this->max_path_control -= XcVG_TMP_CONTROL_CHUNK;
   return FALSE;
  }
  this->path.control = ptr;
 }

 this->path.control[this->path.control_count++] = op;
 this->building_path = TRUE;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** addPathData - Add data coord to current path                      ** 
** ----------------------------------------------------------------- */
static boolean addPathData(this, count, va_alist)
c_VectorGraph *this;
int count;
va_dcl
{
 va_list ap;

 Xc_TRACE(("addPathData(%d)", count));

 if(this->max_path_data < (this->path.data_count + count))
 {
  coord_t *ptr;

  Xc_TRACE((" grow path_data"));
  this->max_path_data += MAX(count, XcVG_TMP_DATA_CHUNK);
  ptr = Xc_realloc("path data", this->path.data,
		   this->max_path_data * sizeof(coord_t));
  if(ptr == NULL)
  {
   this->max_path_data -= MAX(count, XcVG_TMP_DATA_CHUNK);
   return FALSE;
  }
  this->path.data = ptr;
 }

 va_start(ap);
 while(count--)
 {
  this->path.data[this->path.data_count++] = va_arg(ap, coord_t);
  /*Xc_TRACE(("  %g", SCALE_TO_POINTS(this->path.data[this->path.data_count - 1])));*/
 }
 va_end(ap);

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** packPath - Pack temp path                                         ** 
** ----------------------------------------------------------------- */
static boolean packPath(this)
c_VectorGraph *this;
{
 vector_item_t *ptr;
 size_t size;

 Xc_TRACE(("packPath(%d op, %d coords)", this->path.control_count,
	   this->path.data_count));
 Xc_ASSERT(this->building_path == TRUE);

 /*if(this->path.control_count == 1) return F(this).abortPath(this);*/
  
 if((ptr = allocItem(this, XcVG_PATH_TYPE)) == NULL)
  return FALSE;

 ptr->data.path.control_count = this->path.control_count;
 ptr->data.path.data_count = this->path.data_count;

 size = ((this->path.control_count * sizeof(unsigned char)) +
	 (this->path.data_count * sizeof(coord_t)));
  
 if(size != 0)
 {
  if((ptr->data.path.data = Xc_smalloc(this->spool, size)) == NULL)
   return FALSE;
      
  memcpy(ptr->data.path.data, this->path.data,
	 this->path.data_count * sizeof(coord_t));
  ptr->data.path.control = (unsigned char *)(ptr->data.path.data +
					     this->path.data_count);
  memcpy(ptr->data.path.control, this->path.control,
	 this->path.control_count * sizeof(unsigned char));
  vectorPathComputeBBox(&ptr->data.path);
 }
 else
 {
  ptr->data.path.control = NULL;
  ptr->data.path.data = NULL;
  Xc_BBOX_INIT(&ptr->data.path.bbox, COORD_ZERO, COORD_ZERO);
 }

 this->path.control_count = this->path.data_count = 0;
 this->building_path = FALSE;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** save - Save context                                               ** 
** ----------------------------------------------------------------- */
boolean vectorSave(this)
c_VectorGraph *this;
{
 vector_item_t *ptr;

 Xc_HISTORY(("save"));

 if((this->building_path == TRUE) && !packPath(this)) return FALSE;
 if((ptr = allocItem(this, XcVG_CONTEXT_TYPE)) == NULL)
  return FALSE;
 ptr->data.children.head = ptr->data.children.tail = NULL;
 this->ptr = &ptr->data.children.head;

 return F(this->matrix).push(this->matrix);
}


/* ----------------------------------------------------------------- ** 
** restore - Restore saved context                                   ** 
** ----------------------------------------------------------------- */
boolean vectorRestore(this)
c_VectorGraph *this;
{
 vector_item_t *ptr, *parent;

 Xc_HISTORY(("restore"));

 if((this->building_path == TRUE) && !F(this).abortPath(this)) return FALSE;
 F(this->matrix).pop(this->matrix);

 ptr = this->base_head;
 parent = NULL;
 while(ptr != NULL)
 {
  Xc_TRACE(("  type %s ($%lx) next=$%lx",
	    debug_type[(int)ptr->type - (int)XcVG_CONTEXT_TYPE],
	    (long)ptr, (long)ptr->next));
      
  if(this->ptr == &ptr->next)
  {
   Xc_ASSERT(ptr->next == NULL);
   Xc_ASSERT(parent != NULL);
   parent->data.children.tail = ptr;
   ptr->next = parent;
   this->ptr = &parent->next;
   Xc_TRACE(("restore done"));
   return TRUE;
  }

  if(ptr->type != XcVG_CONTEXT_TYPE)
  {
   vector_item_t *next;
	  
   next = ptr->next;
   if(next->type == XcVG_CONTEXT_TYPE)
   {
    if(this->ptr == &next->data.children.head)
    {
     /*--- Trash empty context ---*/
     Xc_TRACE(("empty context"));
     this->ptr = &ptr->next;
     ptr->next = next->next;
     return TRUE;
    }
    Xc_TRACE(("   end of context"));
    if(next->data.children.tail == ptr) next = next->next;
   }
   ptr = next;
  }
  else
  {
   Xc_TRACE(("   head=$%lx tail=$%lx",
	     (long)ptr, (long)ptr->next,
	     (long)ptr->data.children.head,
	     (long)ptr->data.children.tail));
   if(ptr->data.children.tail == NULL) parent = ptr;
   ptr = ptr->data.children.head;
  }
 }

 Xc_TRACE(("restore error"));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** translate - Translate matrix origin                               ** 
** ----------------------------------------------------------------- */
boolean vectorTranslate(this, x, y)
c_VectorGraph *this;
coord_t x;
coord_t y;
{
 Xc_HISTORY(("translate(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

 return F(this->matrix).translate(this->matrix, FALSE, x, y);
}


/* ----------------------------------------------------------------- ** 
** rotate - Rotate matrix                                            ** 
** ----------------------------------------------------------------- */
boolean vectorRotate(this, angle)
c_VectorGraph *this;
angle_t angle;
{
 Xc_HISTORY(("rotate(%g)", SCALE_TO_DEGREES(angle)));

 return F(this->matrix).rotate(this->matrix, FALSE, angle);
}


/* ----------------------------------------------------------------- ** 
** scale - Scale matrix                                              ** 
** ----------------------------------------------------------------- */
boolean vectorScale(this, sx, sy)
c_VectorGraph *this;
scale_t sx;
scale_t sy;
{
 Xc_HISTORY(("scale(%g%%, %g%%)", SCALE_TO_PERCENT(sx),
	     SCALE_TO_PERCENT(sy)));

 return F(this->matrix).scale(this->matrix, FALSE, sx, sy);
}


/* ----------------------------------------------------------------- ** 
** concat - Concat matrix                                            ** 
** ----------------------------------------------------------------- */
boolean vectorConcat(this, cxx, cyx, cxy, cyy)
c_VectorGraph *this;
scale_t cxx;
scale_t cyx;
scale_t cxy;
scale_t cyy;
{
 Xc_HISTORY(("concat(%g, %g, %g, %g)",
	     SCALE_TO_COEF(cxx), SCALE_TO_COEF(cyx),
	     SCALE_TO_COEF(cxy), SCALE_TO_COEF(cyy)));

 return F(this->matrix).transform(this->matrix, FALSE, cxx, cyx, cxy, cyy);
}


/* ----------------------------------------------------------------- ** 
** setStrokeColor - Set stroke rendering color                       ** 
** ----------------------------------------------------------------- */
boolean vectorSetStrokeColor(this, color)
c_VectorGraph *this;
c_Color *color;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setStrokeColor(`%s')", color->name));

 if(!XcH_REGISTER(color, vectorHookColor, this)) return FALSE;
 if((ptr = allocItem(this, XcVG_STROKE_COLOR_TYPE)) == NULL)
 {
  XcH_UNREGISTER(color, vectorHookColor, this);
  return FALSE;
 }
 ptr->data.color = color;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setFillColor - Set fill rendering color                           ** 
** ----------------------------------------------------------------- */
boolean vectorSetFillColor(this, color)
c_VectorGraph *this;
c_Color *color;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setFillColor(`%s')", color->name));

 if(!XcH_REGISTER(color, vectorHookColor, this)) return FALSE;
 if((ptr = allocItem(this, XcVG_FILL_COLOR_TYPE)) == NULL)
 {
  XcH_UNREGISTER(color, vectorHookColor, this);
  return FALSE;
 }
 ptr->data.color = color;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setFlatness - Set flatness                                        ** 
** ----------------------------------------------------------------- */
boolean vectorSetFlatness(this, distance)
c_VectorGraph *this;
int distance;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setFlatness(%d)", distance));

 if((ptr = allocItem(this, XcVG_FLATNESS_TYPE)) == NULL)
  return FALSE;
 ptr->data.flatness = MAX(0, MIN(distance, 100));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setMiterLimit - Set the miter limit                               ** 
** ----------------------------------------------------------------- */
boolean vectorSetMiterLimit(this, coef)
c_VectorGraph *this;
real coef;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setMiterLimit(`%g')", (double)coef));

 if((ptr = allocItem(this, XcVG_MITER_LIMIT_TYPE)) == NULL)
  return FALSE;
 ptr->data.miter_limit = MAX(coef, 1.0);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setLineWidth - Set stroke width                                   ** 
** ----------------------------------------------------------------- */
boolean vectorSetLineWidth(this, width)
c_VectorGraph *this;
coord_t width;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setLineWidth(%g)", SCALE_TO_POINTS(width)));

 if((ptr = allocItem(this, XcVG_LINE_WIDTH_TYPE)) == NULL)
  return FALSE;
 ptr->data.line_width = MAX(width, COORD_ZERO);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setLineJoin - Set line joining style                              ** 
** ----------------------------------------------------------------- */
boolean vectorSetLineJoin(this, join_style)
c_VectorGraph *this;
int join_style;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setLineJoin(%d)", join_style));

 if((ptr = allocItem(this, XcVG_LINE_JOIN_TYPE)) == NULL)
  return FALSE;
 ptr->data.line_join = MAX(0, MIN(2, join_style));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setLineCap - Set line cap style                                   ** 
** ----------------------------------------------------------------- */
boolean vectorSetLineCap(this, cap_style)
c_VectorGraph *this;
int cap_style;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setLineCap(%d)", cap_style));

 if((ptr = allocItem(this, XcVG_LINE_CAP_TYPE)) == NULL)
  return FALSE;
 ptr->data.line_cap = MAX(0, MIN(2, cap_style));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setLineDash - Set line dash style                                 ** 
** ----------------------------------------------------------------- */
boolean vectorSetLineDash(this, count, phase, dashes)
c_VectorGraph *this;
int count;
coord_t phase;
coord_t *dashes;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setLineDash"));

 if((ptr = allocItem(this, XcVG_LINE_DASH_TYPE)) == NULL)
  return FALSE;

 if(count < 0) count = 0;
 ptr->data.line_dash.phase = phase;
 if((ptr->data.line_dash.length = count) != 0)
 {
  count *= sizeof(coord_t);
  ptr->data.line_dash.pattern = Xc_smalloc(this->spool, count);
  if(ptr->data.line_dash.pattern == NULL) return FALSE;
  memcpy(ptr->data.line_dash.pattern, dashes, count);
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** clip - Add current path to clipping path                          ** 
** ----------------------------------------------------------------- */
boolean vectorClip(this)
c_VectorGraph *this;
{
 Xc_HISTORY(("clip"));

 if(this->building_path && !packPath(this)) return FALSE;
 return((allocItem(this, XcVG_CLIP_TYPE) == NULL)? FALSE : TRUE);
}


/* ----------------------------------------------------------------- ** 
** render - Render current path                                      ** 
** ----------------------------------------------------------------- */
boolean vectorRender(this, mode)
c_VectorGraph *this;
int mode;
{
 vector_item_t *ptr;

 Xc_HISTORY(("render(%d)", mode));

 if(this->building_path)
 {
  if(((this->path.control_count == 1) &&
      (*this->path.control != XcVG_CLOSE_OP)) || !this->path.control_count)
   F(this).abortPath(this);
  else if(!packPath(this))
   return FALSE;
 }
 if((ptr = allocItem(this, XcVG_RENDER_TYPE)) == NULL)
  return FALSE;
 ptr->data.render_mode = mode;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** newPath - Create new path                                         ** 
** ----------------------------------------------------------------- */
boolean vectorNewPath(this)
c_VectorGraph *this;
{
 vector_item_t *ptr;

 Xc_HISTORY(("newPath"));

 if(!F(this).abortPath(this)) return FALSE;
  
 if((ptr = allocItem(this, XcVG_RENDER_TYPE)) == NULL)
  return FALSE;
 ptr->data.render_mode = 0;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** abortPath - Abort path creation                                   ** 
** ----------------------------------------------------------------- */
boolean vectorAbortPath(this)
c_VectorGraph *this;
{
 Xc_HISTORY(("abortPath"));
  
 this->building_path = FALSE;
 this->path.control_count = this->path.data_count = 0;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** closePath - Close a path                                          ** 
** ----------------------------------------------------------------- */
boolean vectorClosePath(this)
c_VectorGraph *this;
{
 Xc_HISTORY(("closePath"));

 return addPathControl(this, XcVG_CLOSE_OP);
}


/* ----------------------------------------------------------------- ** 
** move - Move current point                                         ** 
** ----------------------------------------------------------------- */
boolean vectorMoveTo(this, x, y)
c_VectorGraph *this;
coord_t x;
coord_t y;
{
 matrix_t *m;
 Xc_HISTORY(("move(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));
  
 this->current_point.dx = x;
 this->current_point.dy = y;

 m = F(this->matrix).get(this->matrix, 0);
 return(addPathControl(this, XcVG_MOVE_OP) &&
	addPathData(this, 2,
		    (coord_t)XcM_X(m, x, y), (coord_t)XcM_Y(m, x, y)));
}


/* ----------------------------------------------------------------- ** 
** line - Add line to path                                           ** 
** ----------------------------------------------------------------- */
boolean vectorLineTo(this, x, y)
c_VectorGraph *this;
coord_t x;
coord_t y;
{
 matrix_t *m;

 Xc_HISTORY(("line(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));

 this->current_point.dx = x;
 this->current_point.dy = y;

 m = F(this->matrix).get(this->matrix, 0);
 return(addPathControl(this, XcVG_LINE_OP) &&
	addPathData(this, 2,
		    (coord_t)XcM_X(m, x, y), (coord_t)XcM_Y(m, x, y)));
}

/* ----------------------------------------------------------------- ** 
** arc - Add an arc to current path                                  ** 
** ----------------------------------------------------------------- */
boolean vectorArcTo(this, Cx, Cy, Rx, Ry, Ang1, Ang2)
c_VectorGraph *this;
coord_t Cx;
coord_t Cy;
coord_t Rx;
coord_t Ry;
coord_t Ang1;
coord_t Ang2;
{
 matrix_t *m;

 this->current_point.dx = Cx+Rx*cos(SCALE_TO_RADIANS(Ang2));
 this->current_point.dy = Cy+Ry*sin(SCALE_TO_RADIANS(Ang2));

 m = F(this->matrix).get(this->matrix, 0);
 return(addPathControl(this, XcVG_ARC_OP) &&
	addPathData(this, 6,
		    (coord_t)XcM_X(m, Cx, Cy), (coord_t)XcM_Y(m, Cx, Cy),
		    (coord_t)XcM_DX(m, Rx, Ry),
		    (coord_t)XcM_DY(m, Rx, Ry),
		    Ang1, Ang2));
}

/* ----------------------------------------------------------------- ** 
** curve - Add a curve to current path                               ** 
** ----------------------------------------------------------------- */
boolean vectorCurveTo(this, x1, y1, x2, y2, x3, y3)
c_VectorGraph *this;
coord_t x1;
coord_t y1;
coord_t x2;
coord_t y2;
coord_t x3;
coord_t y3;
{
 matrix_t *m;

 Xc_HISTORY(("curve(%g, %g, %g, %g, %g, %g)",
	     SCALE_TO_POINTS(x1), SCALE_TO_POINTS(y1),
	     SCALE_TO_POINTS(x2), SCALE_TO_POINTS(y2),
	     SCALE_TO_POINTS(x3), SCALE_TO_POINTS(y3)));
   
 this->current_point.dx = x3;
 this->current_point.dy = y3;

 m = F(this->matrix).get(this->matrix, 0);
 return(addPathControl(this, XcVG_CURVE_OP) &&
	addPathData(this, 6,
		    (coord_t)XcM_X(m, x1, y1), (coord_t)XcM_Y(m, x1, y1),
		    (coord_t)XcM_X(m, x2, y2), (coord_t)XcM_Y(m, x2, y2),
		    (coord_t)XcM_X(m, x3, y3), (coord_t)XcM_Y(m, x3, y3)));
}

/* ----------------------------------------------------------------- ** 
** curveEnd - Add a curve to current path at end                     ** 
** ----------------------------------------------------------------- */
boolean vectorCurveEnd(this, x1, y1, x2, y2)
c_VectorGraph *this;
coord_t x1;
coord_t y1;
coord_t x2;
coord_t y2;
{
 matrix_t *m;

 Xc_HISTORY(("curveEnd(%g, %g)", SCALE_TO_POINTS(x1), SCALE_TO_POINTS(y1),
	     SCALE_TO_POINTS(x2), SCALE_TO_POINTS(y2)));
   
 this->current_point.dx = x2;
 this->current_point.dy = y2;

 m = F(this->matrix).get(this->matrix, 0);
 return(addPathControl(this, XcVG_CURVE_END_OP) &&
	addPathData(this, 4,
		    (coord_t)XcM_X(m, x1, y1), (coord_t)XcM_Y(m, x1, y1),
		    (coord_t)XcM_X(m, x2, y2), (coord_t)XcM_Y(m, x2, y2)));
}


/* ----------------------------------------------------------------- ** 
** curveStart - Add a curve to current path at start                 ** 
** ----------------------------------------------------------------- */
boolean vectorCurveBegin(this, x1, y1, x2, y2)
c_VectorGraph *this;
coord_t x1;
coord_t y1;
coord_t x2;
coord_t y2;
{
 matrix_t *m;

 Xc_HISTORY(("curveBegin(%g, %g)", SCALE_TO_POINTS(x1), SCALE_TO_POINTS(y1),
	     SCALE_TO_POINTS(x2), SCALE_TO_POINTS(y2)));
   
 this->current_point.dx = x2;
 this->current_point.dy = y2;

 m = F(this->matrix).get(this->matrix, 0);
 return(addPathControl(this, XcVG_CURVE_BEGIN_OP) &&
	addPathData(this, 4,
		    (coord_t)XcM_X(m, x1, y1), (coord_t)XcM_Y(m, x1, y1),
		    (coord_t)XcM_X(m, x2, y2), (coord_t)XcM_Y(m, x2, y2)));
}


/* ----------------------------------------------------------------- ** 
** getCurrentPoint - Returns current point                           ** 
** ----------------------------------------------------------------- */
boolean vectorGetCurrentPoint(this, x, y)
c_VectorGraph *this;
coord_t *x;
coord_t *y;
{
 Xc_HISTORY(("getCurrentPoint"));

 /*--- Don't work when last opcode was ClosePath ---*/
 if(x != NULL) *x = this->current_point.dx;
 if(y != NULL) *y = this->current_point.dy;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextMatrix - New text matrix                                   ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextMatrix(this, matrix)
c_VectorGraph *this;
matrix_t *matrix;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextMatrix"));

 XcM_DUMP_MATRIX("text matrix", matrix);
 if((ptr = allocItem(this, XcVG_TEXT_MATRIX_TYPE)) == NULL)
  return FALSE;
 XcM_COPY_MATRIX(&ptr->data.matrix, matrix);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextPoint - Translate text matrix                              ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextPoint(this, x, y)
c_VectorGraph *this;
coord_t x;
coord_t y;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextPoint(%g, %g)", SCALE_TO_POINTS(x), SCALE_TO_POINTS(y)));
  
 if((ptr = allocItem(this, XcVG_TEXT_TRANSLATE_TYPE)) == NULL)
  return FALSE;
 ptr->data.vector.dx = x;
 ptr->data.vector.dy = y;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** textNewline - Jump next line                                      ** 
** ----------------------------------------------------------------- */
boolean vectorTextNewline(this)
c_VectorGraph *this;
{
 return((allocItem(this, XcVG_TEXT_NEWLINE_TYPE) == NULL)? FALSE : TRUE);
}


/* ----------------------------------------------------------------- ** 
** setTextRise - Set text rise                                       ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextRise(this, rise)
c_VectorGraph *this;
coord_t rise;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextRize(%g)", SCALE_TO_POINTS(rise)));

 if((ptr = allocItem(this, XcVG_TEXT_RISE_TYPE)) == NULL)
  return FALSE;
 ptr->data.offset = rise;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextMode - text rendering mode                                 ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextMode(this, mode)
c_VectorGraph *this;
int mode;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextMode(%d)", mode));

 if((ptr = allocItem(this, XcVF_TEXT_MODE_TYPE)) == NULL)
  return FALSE;
 ptr->data.render_mode = mode;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextFont - Set text font and size                              ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextFont(this, font, size)
c_VectorGraph *this;
c_VectorFont *font;
coord_t size;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextFont(%s, %g)", font->font_name, SCALE_TO_POINTS(size)));

 if(!F(font).load(font) || !XcH_REGISTER(font, NULL, this)) return FALSE;
 if((ptr = allocItem(this, XcVG_TEXT_FONT_TYPE)) == NULL)
 {
  XcH_UNREGISTER(font, NULL, this);
  return FALSE;
 }
 ptr->data.font.data = font;
 ptr->data.font.size = size;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextLeading - Text leading                                     ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextLeading(this, leading)
c_VectorGraph *this;
coord_t leading;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextLeading(%g)", SCALE_TO_POINTS(leading)));

 if((ptr = allocItem(this, XcVG_TEXT_LEADING_TYPE)) == NULL)
  return FALSE;
 ptr->data.offset = leading;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextWordSpacing - Set word spacing                             ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextWordSpacing(this, spacing)
c_VectorGraph *this;
coord_t spacing;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextWordSpacing(%g)", spacing));

 if((ptr = allocItem(this, XcVG_TEXT_WORD_SPACING_TYPE)) == NULL)
  return FALSE;
 ptr->data.offset = spacing;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextCharSpacing - Set character spacing                        ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextCharSpacing(this, spacing)
c_VectorGraph *this;
coord_t spacing;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextCharSpacing(%g)", spacing));

 if((ptr = allocItem(this, XcVG_TEXT_CHAR_SPACING_TYPE)) == NULL)
  return FALSE;
 ptr->data.offset = spacing;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextHScale - Set horizontal scaling                            ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextHScale(this, scale)
c_VectorGraph *this;
scale_t scale;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextHScale(%g%%)", SCALE_TO_PERCENT(scale)));

 if((ptr = allocItem(this, XcVG_TEXT_HSCALE_TYPE)) == NULL)
  return FALSE;
 ptr->data.scale = scale;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setTextKerning - Set text kerning                                 ** 
** ----------------------------------------------------------------- */
boolean vectorSetTextKerning(this, kerning)
c_VectorGraph *this;
real kerning;
{
 vector_item_t *ptr;

 Xc_HISTORY(("setTextkerning(%g)", (double)kerning));

 if((ptr = allocItem(this, XcVG_TEXT_KERNING_TYPE)) == NULL)
  return FALSE;
 ptr->data.kerning = kerning;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** textRender - Render text string                                   ** 
** ----------------------------------------------------------------- */
boolean vectorTextRender(this, string, length)
c_VectorGraph *this;
char_t *string;
long length;
{
 vector_item_t *ptr;

 Xc_HISTORY(("textRender(`%*s')", length, string));

 if((this->building_path == TRUE) && !packPath(this)) return FALSE;
    
 if((ptr = allocItem(this, XcVG_TEXT_RENDER_TYPE)) == NULL)
  return FALSE;

 Xc_BBOX_INIT(&ptr->data.text.bbox, COORD_ZERO, COORD_ZERO);
 ptr->data.text.length = length;

 ptr->data.text.string = Xc_smalloc(this->spool, length * sizeof(char_t));
 if(ptr->data.text.string == NULL) return FALSE;
 memcpy(ptr->data.text.string, string, length * sizeof(char_t));

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** end - End vector description                                      ** 
** ----------------------------------------------------------------- */
boolean vectorEnd(this)
c_VectorGraph *this;
{
 vector_item_t *item, *last_item;
 boolean first_flag;
 coord_t	dx;
 coord_t	dy;
 coord_t	font_size;
 c_VectorFont	*font;
 float		angle;
 
 Xc_HISTORY(("end"));
 angle = 0.0;
 
 if(this->building_path && !F(this).abortPath(this)) return FALSE;
 if(this->path.control != NULL)
 {
  Xc_free(this->path.control);
  this->path.control = NULL;
 }
 if(this->path.data != NULL)
 {
  Xc_free(this->path.data);
  this->path.data = NULL;
 }

 /*--- find last text pos, text matrix, textfont ---*/

 /*--- Compute global bbox ---*/
 first_flag = TRUE;
 for(item = this->base_head, last_item = NULL; item != NULL; )
 {
  switch(item->type)
  {
  case XcVG_CONTEXT_TYPE:
   {
    Xc_ASSERT(item->data.children.tail != NULL);
    if(last_item != item->data.children.tail)
    {
     /*--- Push ---*/
     Xc_ASSERT(item->data.children.head != NULL);
     last_item = item;
     item = item->data.children.head;
     continue;
    }
   }
   break;
  case  XcVG_PATH_TYPE:
   if (item->data.path.control_count > 1)
   {
    Xc_DUMP_BBOX("path bbox", &item->data.path.bbox);
    if(first_flag != FALSE)
    {
     Xc_BBOX_COPY(&this->bbox, &item->data.path.bbox);
     first_flag = FALSE;
    }
    else
     Xc_BBOX_UNION(&this->bbox, &item->data.path.bbox);
   }
   break;
  case XcVG_TEXT_MATRIX_TYPE:
   /* deduce angle and dx dy */
   angle = acos(item->data.matrix.M[0][0]);
   if (item->data.matrix.M[0][1] < 0)
    angle += M_PI;
   break;
  case XcVG_TEXT_TRANSLATE_TYPE:
   dx = item->data.vector.dx;
   dy = item->data.vector.dy;
   break;
  case XcVG_TEXT_FONT_TYPE:
   font_size = item->data.font.size;
   font = item->data.font.data;
   break;
  case XcVG_TEXT_RENDER_TYPE:
   {
#if 0
    c_CharMetrics	*cm;
    int			t_length;
    int			t_dx;
    int			t_dy;

    t_dx = cos(angle)*dx + -sin(angle)*dy;
    t_dy = sin(angle)*dx + cos(angle)*dy;
    if(first_flag != FALSE)
    {
     Xc_BBOX_INIT(&this->bbox, t_dx, t_dy);
     first_flag = FALSE;
    }
    else
     Xc_BBOX_UPDATE(&this->bbox, t_dx, t_dy);

    cm = COPY(c_CharMetrics)(font->metrics,
			     NULL, font_size,
			     SCALE_FROM_COEF(1.0),
			     SCALE_FROM_DEGREES(0.0),
			     0, TRUE);
    if(cm != NULL)
    {
     t_length = F(cm).textWidth(cm, item->data.text.string, 
				item->data.text.length);
     DELETE(c_CharMetrics)(cm);
    }
    else
     t_length = 0;
    t_dx += t_length*cos(angle) - font_size*sin(angle);
    t_dy += t_length*sin(angle) + font_size*cos(angle);
/*t_dy = dy - 300 + t_length*sin(angle);*/ /*add popup*/
    Xc_BBOX_UPDATE(&this->bbox, t_dx, t_dy);
#endif
   }
  default:
   break;
  }
  last_item = item;
  item = item->next;
 }
 Xc_DUMP_BBOX("global bbox", &this->bbox);

 /*--- Get tail ---*/
 if(this->base_head != NULL)
{
 for(item = this->base_head; item->next != NULL; item = item->next);
 this->base_tail = item;
}
 else
  this->base_tail = NULL;

 return TRUE;
}

