/*
** docfile.c for Xclamation and XAllWrite in VectorGraph/
** Loading method for the VectorGraph class
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
** Started on  Wed Mar 15 14:51:08 1995 Stéphane Boisson
** Last update Sun Jun  8 19:39:08 1997 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "VectorGraphP.h"
#include "AIDocFile.h"
#include "WMFDocFile.h"
#include "XFIGDocFile.h"

/*--- Internal data structure ---*/
typedef struct
{
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
static boolean cb_ai ___PROTO((c_DocFile *doc, char *keyword,
			       long param, vector_load_t *data));
static boolean cb_wmf ___PROTO((c_DocFile *doc, char *keyword,
				long param, vector_load_t *data));
static boolean cb_xfig ___PROTO((c_DocFile *doc, char *keyword,
				 long param, vector_load_t *data));

/* ----------------------------------------------------------------- ** 
** vectorWrite - Standard frame write method                         ** 
** ----------------------------------------------------------------- */
boolean vectorWrite(this, doc)
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

 /*--- write vector info ---*/
 switch (this->ftype)
 {
 case XcVG_XFIG:
  if(!F(doc).writeKeyword(doc, XcDF_XFIG_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeFilename(doc, this->filename) || 
     !F(doc).endSequence(doc)) return FALSE;
  break;
 case XcVG_WMF:
  if(!F(doc).writeKeyword(doc, XcDF_WMF_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeFilename(doc, this->filename) || 
     !F(doc).endSequence(doc)) return FALSE;
  break;
 case XcVG_AI:
 default:
  if(!F(doc).writeKeyword(doc, XcDF_AI_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeFilename(doc, this->filename) || 
     !F(doc).endSequence(doc)) return FALSE;
  break;
 }
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readVector - Standard frame read method                           ** 
** ----------------------------------------------------------------- */
boolean vectorRead(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 vector_load_t data;

 Xc_TRACE(("read"));

 if((data.vector = NEW(c_VectorGraph)(frame->BaseStd)) == NULL) return FALSE;
  
 /*--- formats ---*/
 if(!F(doc).addCallbacks(doc, XcDF_AI_KEYWORD, cb_ai, &data, NULL))
 {
  DELETE(c_VectorGraph)(data.vector);
  return FALSE;
 }

 if(!F(doc).addCallbacks(doc, XcDF_WMF_KEYWORD, cb_wmf, &data, NULL))
 {
  DELETE(c_VectorGraph)(data.vector);
  return FALSE;
 }

 if(!F(doc).addCallbacks(doc, XcDF_XFIG_KEYWORD, cb_xfig, &data, NULL))
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
  F(frame).configure_request(frame, TRUE);
#else
  F(frame).attachobject(frame, data.vector, CADRE_VECTOR);
#endif
  data.vector->update_flag = TRUE;

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


/* ----------------------------------------------------------------- ** 
** cb_ai - Loading callback for AI filename                          ** 
** ----------------------------------------------------------------- */
static boolean cb_ai(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
 c_AIDocFile *ai;
 unsigned char *filename;
 boolean rc;

 Xc_TRACE(("cb_ai"));

 if(!F(doc).readFilename(doc, &filename) || !filename) return FALSE;
 Xc_TRACE(("file: `%s'", filename));

 data->vector->ftype = XcVG_AI;
 ai = NEW(c_AIDocFile)(XcAIDF_LOAD, filename, -1, data->vector);
 Xc_free(filename);
 if(ai == NULL) return FALSE;

 rc = F(ai).readMain(ai);
 DELETE(c_AIDocFile)(ai);
 return rc;
}

/* ----------------------------------------------------------------- ** 
** cb_wmf - Loading callback for WMF filename                        ** 
** ----------------------------------------------------------------- */
static boolean cb_wmf(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
 c_WMFDocFile *wmf;
 unsigned char *filename;
 boolean rc;

 Xc_TRACE(("cb_wmf"));
 
 if(!F(doc).readFilename(doc, &filename) || !filename) return FALSE;
 Xc_TRACE(("file: `%s'", filename));

 data->vector->ftype = XcVG_WMF;
 wmf = NEW(c_WMFDocFile)(filename, data->vector);
 Xc_free(filename);
 if(wmf == NULL) 
  return FALSE;
 rc = F(wmf).readMain(wmf);
 DELETE(c_WMFDocFile)(wmf);
 return rc;
}

/* ----------------------------------------------------------------- ** 
** cb_xfig - Loading callback for XFIG filename                      ** 
** ----------------------------------------------------------------- */
static boolean cb_xfig(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
vector_load_t *data;
{
 c_XFIGDocFile *xfig;
 unsigned char *filename;
 boolean rc;

 Xc_TRACE(("cb_xfig"));
 
 if(!F(doc).readFilename(doc, &filename) || !filename) return FALSE;
 Xc_TRACE(("file: `%s'", filename));

 data->vector->ftype = XcVG_XFIG;
 xfig = NEW(c_XFIGDocFile)(filename, data->vector);
 Xc_free(filename);
 if(xfig == NULL) 
  return FALSE;
 rc = F(xfig).readMain(xfig);
 DELETE(c_XFIGDocFile)(xfig);
 return rc;
}
