/*
** VectorGraphP.h for Xclamation and XAllWrite in VectorGraph/
** Private header for VectorGraph class
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
** Started on  Tue Mar  7 18:41:31 1995 Stéphane Boisson
** Last update Mon May  5 18:07:11 1997 Antoine Buat
*/


#ifndef __XC_VECTOR_GRAPH_P_H__
#define __XC_VECTOR_GRAPH_P_H__

#include <X11/Xlib.h>
#include "VectorGraph.h"
#include "ObjectStd.h"


/*--- Types ---*/
enum vector_type_e 
{
 XcVG_CONTEXT_TYPE,
 XcVG_PATH_TYPE,
 XcVG_RENDER_TYPE, XcVG_CLIP_TYPE,
 XcVG_STROKE_COLOR_TYPE, XcVG_FILL_COLOR_TYPE,
 XcVG_LINE_WIDTH_TYPE, XcVG_LINE_JOIN_TYPE,
 XcVG_LINE_CAP_TYPE, XcVG_LINE_DASH_TYPE,
 XcVG_MITER_LIMIT_TYPE, XcVG_FLATNESS_TYPE,
 XcVG_TEXT_MATRIX_TYPE, XcVG_TEXT_TRANSLATE_TYPE,
 XcVG_TEXT_NEWLINE_TYPE, XcVG_TEXT_RISE_TYPE,
 XcVF_TEXT_MODE_TYPE, XcVG_TEXT_FONT_TYPE,
 XcVG_TEXT_LEADING_TYPE, XcVG_TEXT_WORD_SPACING_TYPE,
 XcVG_TEXT_CHAR_SPACING_TYPE, XcVG_TEXT_HSCALE_TYPE,
 XcVG_TEXT_KERNING_TYPE, XcVG_TEXT_RENDER_TYPE
};


/*--- Path opcodes ---*/
#define XcVG_MOVE_OP 0
#define XcVG_LINE_OP 1
#define XcVG_CURVE_OP 2
#define XcVG_CURVE_BEGIN_OP 3
#define XcVG_CURVE_END_OP 4
#define XcVG_CLOSE_OP 5
#define XcVG_ARC_OP 6


/*--- Path cache structure ---*/
typedef struct 
{
 XPoint *xdata;
 long size;
 long pos;

 /*___Outline___*/
 int outline_size;
 int outline_count;
 struct vector_cache_stroke_s 
 {
  long offset;
  int count;
  XPoint base;
 } *outline;

 /*___Misc___*/
 vector_t last;
 vector_t end;
 XPoint old;
} vector_path_cache_t;


/*--- Graphic state ---*/
struct vector_state_s 
{
 /*___Graphic context___*/
 GC gc;
 Pixmap mask;
 boolean clip_flag;

 /*___Colors___*/
 c_Color *current_color;
 c_Color *stroke_color;
 c_Color *fill_color;
  
 /*___Text attributes___*/
 c_VectorFont *font;
 matrix_t text_matrix;
  
 /*___Path___*/
 vector_path_cache_t path_cache;
};


/*--- Vector item structure ---*/
struct vector_item_s 
{
 vector_item_t *next;
 enum vector_type_e type;

 union 
 {
  /*___Group data___*/
  struct vector_children_data_s 
  {
   vector_item_t *head;
   vector_item_t *tail;
  } children;
    
  /*___Path data___*/
  vector_path_t path;

  /*___Text data___*/
  struct vector_text_data_s 
  {
   bbox_t bbox;
   unsigned int length;
   char_t *string;
  } text;

  /*___Color data___*/
  c_Color *color;

  /*___Font data___*/
  struct vector_font_data_s 
  {
   c_VectorFont *data;
   coord_t size;
  } font;

  /*___Line width data___*/
  coord_t line_width;

  /*___Line join data___*/
  int line_join;

  /*___Line cap data___*/
  int line_cap;

  /*___Line dash data___*/
  struct vector_dash_data_s 
  {
   coord_t phase;
   unsigned int length;
   coord_t *pattern;
  } line_dash;

  /*___Rendering mode___*/
  int render_mode;

  /*___Flatness___*/
  real flatness;

  /*___Miter limit___*/
  real miter_limit;

  /*___Matrix___*/
  matrix_t matrix;
    
  /*___Vector___*/
  vector_t vector;

  /*___Offset___*/
  coord_t offset;

  /*___Scale___*/
  scale_t scale;
    
  /*___Kerning___*/
  real kerning;
 } data;
};


/*--- Global ---*/
void vectorHookColor ___PROTO((c_VectorGraph *this, int reason,
			       c_Color *old, c_Color *new));

/*--- dispatch.c ---*/
void vectorSetMode ___PROTO((c_VectorGraph *this,
			     vector_mode_t mode, d_object_function *data));
void vectorVerticalFlip ___PROTO((c_VectorGraph *this,
				  d_object_function *data));
void vectorHorizontalFlip ___PROTO((c_VectorGraph *this,
				    d_object_function *data));
void vectorCenter ___PROTO((c_VectorGraph *this, d_object_function *data));
void vectorTextToggle ___PROTO((c_VectorGraph *this, d_object_function *data));

/*--- docfile.c ---*/
boolean	vectorWrite ___NPROTO((c_VectorGraph *this, c_DocFile *doc));
boolean vectorRead ___PROTO((c_DocFile *doc, char *keyword,
			     long param, c_Cadre *frame));

/*--- print.c ---*/
boolean vectorPrint ___NPROTO((c_VectorGraph *this, c_PostScript *post));

/*--- redraw.c ---*/
void vectorRedraw ___NPROTO((c_VectorGraph *this, c_Cadre *frame,
			     Polygone *poly));

/*--- path.c ---*/
void vectorPathComputeBBox ___PROTO((vector_path_t *path));
boolean vectorPathCacheInit ___PROTO((vector_path_cache_t *target,
				      vector_path_cache_t *source));
void vectorPathCacheReset ___PROTO((vector_path_cache_t *cache));
void vectorPathCacheFree ___PROTO((vector_path_cache_t *cache));
void vectorPathCacheFill ___PROTO((vector_path_cache_t *cache, int x, int y,
				   Display *display, Drawable win, GC gc));
void vectorPathCacheStroke ___PROTO((vector_path_cache_t *cache, int x, int y,
				     Display *display, Drawable win, GC gc));
boolean vectorPathFlatten ___PROTO((vector_path_t *path, c_Matrix *matrix,
				    vector_path_cache_t *cache));

/*--- commands.c ---*/
extern size_t vectorItemExtraSize[];
boolean vectorSave ___PROTO((c_VectorGraph *this));
boolean vectorRestore ___PROTO((c_VectorGraph *this));

boolean vectorTranslate ___PROTO((c_VectorGraph *this, coord_t x, coord_t y));
boolean vectorRotate ___PROTO((c_VectorGraph *this, angle_t angle));
boolean vectorScale ___PROTO((c_VectorGraph *this, scale_t sx, scale_t sy));
boolean vectorConcat ___PROTO((c_VectorGraph *this,
			       scale_t cxx, scale_t cyx,
			       scale_t cxy, scale_t cyy));

boolean vectorSetStrokeColor ___PROTO((c_VectorGraph *this, c_Color *color));
boolean vectorSetFillColor ___PROTO((c_VectorGraph *this, c_Color *color));
boolean vectorSetLineWidth ___PROTO((c_VectorGraph *this, coord_t width));
boolean vectorSetLineJoin ___PROTO((c_VectorGraph *this, int join_style));
boolean vectorSetLineCap ___PROTO((c_VectorGraph *this, int cap_style));
boolean vectorSetMiterLimit ___PROTO((c_VectorGraph *this, real coef));
boolean vectorSetFlatness ___PROTO((c_VectorGraph *this, int distance));
boolean vectorSetLineDash ___PROTO((c_VectorGraph *this, int count,
				    coord_t phase, coord_t *dashes));

boolean vectorRender ___PROTO((c_VectorGraph *this, int mode));
boolean vectorClip ___PROTO((c_VectorGraph *this));
boolean vectorNewPath ___PROTO((c_VectorGraph *this));
boolean vectorAbortPath ___PROTO((c_VectorGraph *this));
boolean vectorClosePath ___PROTO((c_VectorGraph *this));
boolean vectorMoveTo ___PROTO((c_VectorGraph *this, coord_t x, coord_t y));
boolean vectorLineTo ___PROTO((c_VectorGraph *this, coord_t x, coord_t y));
boolean vectorArcTo ___PROTO((c_VectorGraph *this,
			      coord_t L, coord_t T,
			      coord_t R, coord_t B,
			      coord_t Ang1, coord_t Ang2));
boolean vectorCurveTo ___PROTO((c_VectorGraph *this, coord_t x1, coord_t y1,
				coord_t x2, coord_t y2,
				coord_t x3, coord_t y3));
boolean vectorCurveBegin ___PROTO((c_VectorGraph *this, coord_t x1, coord_t y1,
				   coord_t x2, coord_t y2));
boolean vectorCurveEnd ___PROTO((c_VectorGraph *this, coord_t x1, coord_t y1,
				 coord_t x2, coord_t y2));
boolean vectorGetCurrentPoint ___PROTO((c_VectorGraph *this,
					coord_t *xp, coord_t *yp));
boolean vectorEnd ___PROTO((c_VectorGraph *this));

boolean vectorSetTextMatrix ___PROTO((c_VectorGraph *this, matrix_t *matrix));
boolean vectorSetTextPoint ___PROTO((c_VectorGraph *this,
				     coord_t x, coord_t y));
boolean vectorSetTextMode ___PROTO((c_VectorGraph *this, int mode));
boolean vectorSetTextFont ___PROTO((c_VectorGraph *this, c_VectorFont *font,
				    coord_t size));
boolean vectorSetTextLeading ___PROTO((c_VectorGraph *this, coord_t leading));
boolean vectorSetTextWordSpacing ___PROTO((c_VectorGraph *this,
					   coord_t spacing));
boolean vectorSetTextCharSpacing ___PROTO((c_VectorGraph *this,
					   coord_t spacing));
boolean vectorSetTextRise ___PROTO((c_VectorGraph *this, coord_t rise));
boolean vectorSetTextHScale ___PROTO((c_VectorGraph *this, scale_t scale));
boolean vectorSetTextKerning ___PROTO((c_VectorGraph *this, real kerning));
boolean vectorTextNewline ___PROTO((c_VectorGraph *this));
boolean vectorTextRender ___PROTO((c_VectorGraph *this, char_t *string,
				   long length));

/*--- import.c ---*/
boolean vectorLoad ___PROTO((c_VectorGraph *this, char *filename));

#endif /* !__XC_VECTOR_GRAPH_P_H__ */
