/*
** VectorGraph.h for XQuad in VectorGraph/
** Definition of the class VectorGraph
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
** Started on  Sometime during     1995 One of the authors
** Last update Thu Jun 20 15:46:00 1996 One of the authors
*/

#ifndef _VectorGraph_h
#define _VectorGraph_h

typedef struct sc_VectorGraph c_VectorGraph;

#include <X11/Xlib.h>
#include "xcalibur.h"
#include "CadreStd.h"
#include "Cadre.h"
#include "Matrix.h"
#include "ObjectStd.h"
#include "High_Graph.h"
#include "BasicGraphStd.h"

#define XcDF_VECTOR_KEYWORD "VECTOR"
#define XcDF_TRANSLATE_KEYWORD "TRANSLATE"
#define XcDF_SCALE_KEYWORD "SCALE"
#define XcDF_MODE_KEYWORD "MODE"
#define XcDF_VFLIP_KEYWORD "VFLIP"
#define XcDF_HFLIP_KEYWORD "HFLIP"

#define XcDF_AI_KEYWORD "AI"


/*--- Internal types ---*/
typedef struct vector_item_s vector_item_t;
struct vector_item_s {
  void			*basic_graph;
  struct vector_item_s	*next;
};

typedef void (*XcLoadVectorProc) ___PROTO((c_VectorGraph *vector,
					   void *cb_data));
     
struct vector_load_s {
  char *name;
  char *copyright;
  char *id;
  int extension_count;
  char **extensions;
  boolean (*test) ___PROTO((char *filename));
  error (*getHeader) ___PROTO((c_VectorGraph *image));
  error (*load) ___PROTO((c_VectorGraph *image, XcLoadVectorProc cb_func,
			  void *cb_data));
};

typedef enum {
  XcVG_NORMAL, XcVG_BEST_ASPECT, XcVG_MAX_ASPECT, XcVG_AUTOSIZE
} vector_mode_t;


/*--- Methods ---*/
typedef struct
{
  F_STD;
  F_CADRE_STD;

  /*___Building methods___*/
  boolean (*addBasicGraph) ___PROTO((c_VectorGraph *this,
				     void *basic_graph));
  boolean (*subBasicGraph) ___PROTO((c_VectorGraph *this,
				     void *basic_graph));
  
  /*___Dispatch methods___*/
  void (*setMode) ___PROTO((c_VectorGraph *this,
			    vector_mode_t mode,
			    d_object_function *data));
  void (*verticalFlip) ___PROTO((c_VectorGraph *this,
				 d_object_function *data));
  void (*horizontalFlip) ___PROTO((c_VectorGraph *this,
				   d_object_function *data));
  void (*center) ___PROTO((c_VectorGraph *this,
			   d_object_function *data));
} sf_VectorGraph;


/*--- Class ---*/
struct sc_VectorGraph
{
 sf_VectorGraph *f;

 char name[XcMAX_NAME_LENGTH];
 char *filename;

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

 /*___Children___*/
 vector_item_t *list;

 /*___Handles___*/
 SpoolHandle *spool;

 c_HighGraph	*highg;
 c_Cadre 	*frame;
 vector_item_t	*last_item;
};

extern sf_VectorGraph fc_VectorGraph;

#endif
