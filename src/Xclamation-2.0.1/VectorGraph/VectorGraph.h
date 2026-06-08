/*
** VectorGraph.h for Xclamation and XAllWrite in VectorGraph/
** Definition of the class VectorGraph
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
** Started on  Wed Jul 12 14:38:11 1995 Stéphane Boisson
** Last update Sun Jun  8 20:36:35 1997 Emmanuel Paris
*/


#ifndef __XC_VECTOR_GRAPH_H__
#define __XC_VECTOR_GRAPH_H__

typedef struct sc_VectorGraph c_VectorGraph;
typedef struct vector_item_s vector_item_t;
typedef struct vector_state_s vector_state_t;
typedef struct vector_path_s vector_path_t;
/*typedef struct vector_load_s vector_load_t;*/

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "BaseStd.h"
#include "CadreStd.h"
#include "Cadre.h"
#include "VectorPolygon.h"
#include "VectorFont.h"
#include "Matrix.h"
#include "ObjectStd.h"

/*--- Loading & saving keywords ---*/
#define XcDF_VECTOR_KEYWORD "VECTOR"
#define XcDF_TRANSLATE_KEYWORD "TRANSLATE"
#define XcDF_SCALE_KEYWORD "SCALE"
#define XcDF_MODE_KEYWORD "MODE"
#define XcDF_VFLIP_KEYWORD "VFLIP"
#define XcDF_HFLIP_KEYWORD "HFLIP"
#define XcDF_AI_KEYWORD "AI"
#define XcDF_WMF_KEYWORD "WMF"
#define XcDF_XFIG_KEYWORD "XFIG"

/*--- define file type ---*/
#define XcVG_AI		0
#define XcVG_WMF	1
#define XcVG_XFIG	2

/*--- Geometry management types ---*/
typedef enum 
{
 XcVG_NORMAL, XcVG_BEST_ASPECT, XcVG_MAX_ASPECT, XcVG_AUTOSIZE, XcVG_NONE
} vector_mode_t;

/*--- Vector path structure ---*/
struct vector_path_s 
{
 bbox_t bbox;
 unsigned int control_count;
 unsigned int data_count;
 unsigned char *control;
 coord_t *data;
};

/*--- Rendering modes ---*/
#define XcVG_FILL 1
#define XcVG_STROKE 2
#define XcVG_CLIP 4
#define XcVG_STROKE_CLIP (XcVG_CLIP|XcVG_STROKE)
#define XcVG_FILL_CLIP (XcVG_FILL|XcVG_CLIP)
#define XcVG_STROKE_FILL (XcVG_FILL|XcVG_STROKE)
#define XcVG_STROKE_FILL_CLIP (XcVG_FILL|XcVG_STROKE|XcVG_CLIP)


/*--- Methods ---*/
typedef struct
{
 F_STD;
 F_CADRE_STD;

 /*___Dispatch methods___*/
 void (*setMode) ___PROTO((c_VectorGraph *this, vector_mode_t mode,
			   d_object_function *data));
 void (*verticalFlip) ___PROTO((c_VectorGraph *this,
				d_object_function *data));
 void (*horizontalFlip) ___PROTO((c_VectorGraph *this,
				  d_object_function *data));
 void (*center) ___PROTO((c_VectorGraph *this, d_object_function *data));
 void (*textToggle) ___PROTO((c_VectorGraph *this, d_object_function *data));

 /*___Building methods___*/
 boolean (*save) ___PROTO((c_VectorGraph *this));
 boolean (*restore) ___PROTO((c_VectorGraph *this));
 boolean (*translate) ___PROTO((c_VectorGraph *this, coord_t x, coord_t y));
 boolean (*rotate) ___PROTO((c_VectorGraph *this, angle_t angle));
 boolean (*scale) ___PROTO((c_VectorGraph *this, scale_t sx, scale_t sy));
 boolean (*concat) ___PROTO((c_VectorGraph *this, scale_t cxx, scale_t cyx,
			     scale_t cxy, scale_t cyy));
 boolean (*setStrokeColor) ___PROTO((c_VectorGraph *this, c_Color *color));
 boolean (*setFillColor) ___PROTO((c_VectorGraph *this, c_Color *color));
 boolean (*setLineWidth) ___PROTO((c_VectorGraph *this, coord_t width));
 boolean (*setLineJoin) ___PROTO((c_VectorGraph *this, int join_style));
 boolean (*setLineCap) ___PROTO((c_VectorGraph *this, int cap_style));
 boolean (*setMiterLimit) ___PROTO((c_VectorGraph *this, real coef));
 boolean (*setFlatness) ___PROTO((c_VectorGraph *this, int distance));
 boolean (*setLineDash) ___PROTO((c_VectorGraph *this, int count,
				  coord_t phase, coord_t *dashes));

 boolean (*render) ___PROTO((c_VectorGraph *this, int mode));
 boolean (*clip) ___PROTO((c_VectorGraph *this));
 boolean (*newPath) ___PROTO((c_VectorGraph *this));
 boolean (*abortPath) ___PROTO((c_VectorGraph *this));
 boolean (*closePath) ___PROTO((c_VectorGraph *this));
 boolean (*move) ___PROTO((c_VectorGraph *this, coord_t x, coord_t y));
 boolean (*line) ___PROTO((c_VectorGraph *this, coord_t x, coord_t y));
 boolean (*arc) ___PROTO((c_VectorGraph *this,
			  coord_t L, coord_t T,
			  coord_t R, coord_t B,
			  coord_t Ang1, coord_t Ang2));
 boolean (*curve) ___PROTO((c_VectorGraph *this, coord_t x1, coord_t y1,
			    coord_t x2, coord_t y2,coord_t x3, coord_t y3));
 boolean (*curveBegin) ___PROTO((c_VectorGraph *this, coord_t x1, coord_t y1,
				 coord_t x2, coord_t y2));
 boolean (*curveEnd) ___PROTO((c_VectorGraph *this, coord_t x1, coord_t y1,
			       coord_t x2, coord_t y2));
  
 boolean (*getCurrentPoint) ___PROTO((c_VectorGraph *this,
				      coord_t *xp, coord_t *yp));
  
 boolean (*setTextMatrix) ___PROTO((c_VectorGraph *this, matrix_t *matrix));
 boolean (*setTextPoint) ___PROTO((c_VectorGraph *this, coord_t x,coord_t y));
 boolean (*textNewline) ___PROTO((c_VectorGraph *this));
 boolean (*setTextRise) ___PROTO((c_VectorGraph *this, coord_t rise));
 boolean (*setTextMode) ___PROTO((c_VectorGraph *this, int mode));
 boolean (*setTextFont) ___PROTO((c_VectorGraph *this, c_VectorFont *font,
				  coord_t size));
 boolean (*setTextLeading) ___PROTO((c_VectorGraph *this, coord_t leading));
 boolean (*setTextWordSpacing) ___PROTO((c_VectorGraph *this,
					 coord_t spacing));
 boolean (*setTextCharSpacing) ___PROTO((c_VectorGraph *this,
					 coord_t spacing));
 boolean (*setTextHScale) ___PROTO((c_VectorGraph *this, scale_t scale));
 boolean (*setTextKerning) ___PROTO((c_VectorGraph *this, real kerning));
 boolean (*textRender) ___PROTO((c_VectorGraph *this, char_t *string,
				 long length));
 boolean (*end) ___PROTO((c_VectorGraph *this));
 boolean (*load) ___PROTO((c_VectorGraph *this, char *filename));
} sf_VectorGraph;


/*--- Class ---*/
struct sc_VectorGraph
{
 sf_VectorGraph *f;

 char name[XcMAX_NAME_LENGTH];
 char *filename;
 c_Cadre *frame;

 c_VectorGraph *next;
 BaseStd_t *basestd;
 
 /*___Vector parameters___*/
 bbox_t bbox;
 vector_t offset;
 scale_t width_factor;
 scale_t height_factor;
 boolean vflip_flag;
 boolean hflip_flag;
 vector_mode_t mode;

 /*___Frame parameters___*/
 boolean update_flag;
 angle_t angle;
 scale_t scale;
 c_Matrix *matrix;
 boolean drawtext_flag;
 boolean realtime_draw;

 /*___Items___*/
 vector_item_t *base_head;
 vector_item_t *base_tail;

 /*___Handles___*/
 SpoolHandle *spool;

 /*___Building menbers___*/
 vector_item_t	**ptr;
 vector_path_t	path;
 unsigned int	max_path_control;
 unsigned int	max_path_data;
 boolean	building_path;
 vector_t	current_point; 

 /*___File Type___*/
 int	ftype;  /* file type AI, WMF, XFIG ... */
};

extern sf_VectorGraph fc_VectorGraph;

#endif /* __XC_VECTOR_GRAPH_H__ */
