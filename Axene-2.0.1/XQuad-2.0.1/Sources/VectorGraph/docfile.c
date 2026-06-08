/*
** docfile.c for XQuad in VectorGraph/
** Loading method for the VectorGraph class
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
** Started on  Wed Mar 15 14:51:08 1995 Stéphane Boisson
** Last update Tue Aug 29 15:10:14 1995 Stéphane Boisson
*/

#include "VectorGraphP.h"

/*--- Internal data structure ---*/
typedef struct {
  c_VectorGraph *vector;
  BaseStd_t *base_std;
  char *name;

  /*___Mode___*/
  vector_mode_t mode;

  /*___Scale factors___*/
  scale_t sx;
  scale_t sy;

  /*___translation___*/
  vector_t offset;

  /*___flip flags___*/
  boolean vflip_flag;
  boolean hflip_flag;
} vector_load_t;


/*--- Prototypes ---*/
static boolean cb_translate ___PROTO((c_DocFile *doc, char *keyword,
				      long param, vector_load_t *data));
static boolean cb_scale ___PROTO((c_DocFile *doc, char *keyword,
				  long param, vector_load_t *data));
static boolean cb_mode ___PROTO((c_DocFile *doc, char *keyword,
				 long param, vector_load_t *data));
static boolean cb_flip ___PROTO((c_DocFile *doc, char *keyword,
				 long param, vector_load_t *data));


/* ----------------------------------------------------------------- ** 
** writeVector - Standard frame write method                         ** 
** ----------------------------------------------------------------- */
boolean writeVectorGraph(this, doc)
c_VectorGraph *this;
c_DocFile *doc;
{
  Xc_HISTORY(("write"));

  if(!F(doc).writeKeyword(doc, XcDF_VECTOR_KEYWORD, -1L) ||
     !F(doc).startSequence(doc)) return FALSE;

  /*--- Write mode & flips ---*/
  if(!F(doc).writeKeyword(doc, XcDF_MODE_KEYWORD, (long)this->mode) ||
     ((this->vflip_flag == TRUE) &&
      !F(doc).writeKeyword(doc, XcDF_VFLIP_KEYWORD, -1L)) ||
     ((this->hflip_flag == TRUE) &&
      !F(doc).writeKeyword(doc, XcDF_HFLIP_KEYWORD, -1L))) return FALSE;
  
  if(this->mode != XcVG_NORMAL)
    {
      /*--- Write offset ---*/
      if(!F(doc).writeKeyword(doc, XcDF_TRANSLATE_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc) ||
	 !F(doc).writeReal(doc, SCALE_TO_POINTS(this->offset.dx)) ||
	 !F(doc).writeReal(doc, SCALE_TO_POINTS(this->offset.dy)) ||
	 !F(doc).endSequence(doc)) return FALSE;
      
      /*--- Write scale ---*/
      if((this->width_factor != SCALE_ONE) ||
	 (this->height_factor != SCALE_ONE))
	if(!F(doc).writeKeyword(doc, XcDF_SCALE_KEYWORD, -1L) ||
	   !F(doc).startSequence(doc) ||
	   !F(doc).writeReal(doc, SCALE_TO_COEF(this->width_factor)) ||
	   !F(doc).writeReal(doc, SCALE_TO_COEF(this->height_factor)) ||
	   !F(doc).endSequence(doc)) return FALSE;
    }
  
  
  /*--- write vector info ---*/
  if((this->highg != NULL) && !F(this->highg).write(this->highg, doc))
    return FALSE;

  return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readVector - Standard frame read method                           ** 
** ----------------------------------------------------------------- */
boolean readVectorGraph(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
  vector_load_t data;

  Xc_TRACE(("read"));

  if((data.vector = NEW(c_VectorGraph)()) == NULL) return FALSE;
  data.vector->frame = frame;

  /*--- formats ---*/
  if(!F(doc).addCallbacks(doc, XcDF_GRAPH_KEYWORD,
			  FUNCTION(c_HighGraph, read), NULL, NULL))
     {
      DELETE(c_VectorGraph)(data.vector);
      return FALSE;
    }

  /*--- Init parameters ---*/
  data.base_std = frame->BaseStd;
  data.mode = data.vector->mode;
  data.offset.dx = data.vector->offset.dx;
  data.offset.dy = data.vector->offset.dy;
  data.vflip_flag = data.vector->vflip_flag;
  data.hflip_flag = data.vector->hflip_flag;
  data.sx = data.vector->width_factor;
  data.sy = data.vector->height_factor;
  data.mode = data.vector->mode;

  while(42)
    {
      if(!F(doc).addCallbacks(doc,
			      XcDF_TRANSLATE_KEYWORD, cb_translate, &data,
			      XcDF_SCALE_KEYWORD, cb_scale, &data,
			      XcDF_MODE_KEYWORD, cb_mode, &data,
			      XcDF_VFLIP_KEYWORD, cb_flip, &data,
			      XcDF_HFLIP_KEYWORD, cb_flip, &data,
			      XcDF_GRAPH_KEYWORD,
			      FUNCTION(c_HighGraph, read), data.vector,
			      NULL)) break;
      
      if(!F(doc).expectKeyword(doc, NULL, TRUE)) break;
      
      /*--- Set parameters ---*/
      data.vector->mode = data.mode;
      data.vector->offset.dx = data.offset.dx;
      data.vector->offset.dy = data.offset.dy;
      data.vector->vflip_flag = data.vflip_flag;
      data.vector->hflip_flag = data.hflip_flag;
      data.vector->width_factor = data.sx;
      data.vector->height_factor = data.sy;
      data.vector->mode = data.mode;
      
#if 1
      frame->object_type = CADRE_VECTOR;
      frame->object = data.vector;
      F(frame).configure_request(frame, FALSE);
#else
      F(frame).attachobject(frame, data.vector, CADRE_VECTOR);
#endif

      Xc_TRACE(("done"));
      return TRUE;
    }
  
  Xc_TRACE(("loading error"));
  DELETE(c_VectorGraph)(data.vector);

  /*--- Return TRUE -> keep on loading ---*/
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_translate - loading callback for translation                   ** 
** ----------------------------------------------------------------- */
static boolean cb_translate(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
  real x, y;

  Xc_TRACE(("cb_translate"));

  if(!F(doc).readReal(doc, &x) || !F(doc).readReal(doc, &y)) return FALSE;
  data->offset.dx = SCALE_FROM_POINTS(x);
  data->offset.dy = SCALE_FROM_POINTS(y);

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_scale - Loading callback for scale factors                     ** 
** ----------------------------------------------------------------- */
static boolean cb_scale(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
  real x, y;

  Xc_TRACE(("cb_scale"));

  if(!F(doc).readReal(doc, &x) || !F(doc).readReal(doc, &y)) return FALSE;
  data->sx = SCALE_FROM_COEF(x);
  data->sy = SCALE_FROM_COEF(y);

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_mode - Loading callback for mode                               ** 
** ----------------------------------------------------------------- */
static boolean cb_mode(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
  Xc_TRACE(("cb_mode"));

  if(param > XcVG_AUTOSIZE)
    data->mode = XcVG_NORMAL;
  else
    data->mode = (vector_mode_t)param;

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** Loading callback for flips                                        ** 
** ----------------------------------------------------------------- */
static boolean cb_flip(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
  Xc_TRACE(("cb_flip"));

  if(*keyword == 'V')
    data->vflip_flag = TRUE;
  else
    data->hflip_flag = TRUE;

  return TRUE;
}
