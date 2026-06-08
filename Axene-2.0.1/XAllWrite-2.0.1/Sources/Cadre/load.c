/*
** load.c for Xclamation, XQuad and XAllWrite in Cadre/
** Methods for frame loading
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
** Started on  Tue Dec 27 12:49:44 1994 Stéphane Boisson
** Last update Mon Jun  8 16:53:16 1998 Emmanuel Paris
*/

#define NTRACE

#include "VectorGraph.h"
#ifdef Xc_XCLAMATION
#include "TextFrame.h"
#endif
#if defined(Xc_XCLAMATION) || defined(Xc_XALLWRITE)
#include "Image.h"
#endif
#include "CadreP.h"

static struct 
{
 char *keyword;
 XcFrameStdReadProc *callback;
} frame_contents[] = 
{
#ifdef Xc_XCLAMATION
 {XcDF_TEXT_KEYWORD, &fc_TextFrame.read},
#endif
#if defined(Xc_XCLAMATION) || defined(Xc_XALLWRITE)
 {XcDF_IMAGE_KEYWORD, &fc_Image.read},
#endif
 {XcDF_VECTOR_KEYWORD, &fc_VectorGraph.read},
 {NULL, NULL}
};

static boolean cb_name ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Cadre *frame));
static boolean cb_selected ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Cadre *frame));
static boolean cb_shape ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Cadre *frame));
static boolean cb_point ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Cadre *frame));
static boolean cb_line ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Cadre *frame));
static boolean cb_background ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_Cadre *frame));
static boolean cb_outline ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_Cadre *frame));
static boolean cb_textflow ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Cadre *frame));

/* ----------------------------------------------------------------- ** 
** readCadre - Cadre loading                                         ** 
** ----------------------------------------------------------------- */
boolean readCadre(doc, keyword, factor, base)
c_DocFile *doc;
char *keyword;
scale_t factor;
BaseStd_t *base;
{
 c_Cadre *Cadre;
 int i;

 Xc_TRACE(("read"));

 if((Cadre = NEW(c_Cadre)(base)) == NULL) return FALSE;

 Cadre->sFactor = factor;
 Cadre->sScale  = (coord_t)(COORD_DEFAULT / factor);
  
 for(i = 0; frame_contents[i].keyword != NULL; i++)
  if(!F(doc).addCallbacks(doc, frame_contents[i].keyword,
			  *frame_contents[i].callback, Cadre,
			  NULL)) { DELETE(c_Cadre)(Cadre); return FALSE; }

 do 
 {
  real x, y;
    
  if(!F(doc).addCallbacks(doc,
			  XcDF_NAME_KEYWORD, cb_name, Cadre,
			  XcDF_COLOR_KEYWORD, cb_background, Cadre,
			  XcDF_FRAME_SELECTED_KEYWORD, cb_selected, Cadre,
			  XcDF_TEXTFLOW_KEYWORD, cb_textflow, Cadre,
			  XcDF_SHAPE_KEYWORD, cb_shape, Cadre,
			  NULL)) break;
    
  if(!F(doc).readReal(doc, &x) ||
     !F(doc).readReal(doc, &y)) break;
  Cadre->sOrig_X = SCALE_FROM_POINTS(x);
  Cadre->sOrig_Y = SCALE_FROM_POINTS(y);
    
  if(!F(doc).readReal(doc, &x) ||
     !F(doc).readReal(doc, &y)) break;
  Cadre->sWidth = SCALE_FROM_POINTS(x);
  Cadre->sHeight = SCALE_FROM_POINTS(y);
    
  if(!F(doc).readReal(doc, &x)) break;
  Cadre->sAngle = SCALE_FROM_DEGREES(x);
    
  F(Cadre).calculorigine(Cadre);

  if(!F(doc).readBoolean(doc, &(Cadre->verrou)) ||
     !F(doc).expectKeyword(doc, NULL, TRUE)) break;

  doc->data_type = XcDF_CADRE_TYPE;
  doc->data_ptr = Cadre;
  Xc_TRACE(("done"));
  return TRUE;

 } while(0);

 Xc_TRACE(("loading error"));
 DELETE(c_Cadre)(Cadre);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_name - Loading callback for frame name                         ** 
** ----------------------------------------------------------------- */
static boolean cb_name(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 unsigned char *name;

 Xc_TRACE(("cb_name"));

 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strncpy(frame->name, name, XcMAX_NAME_LENGTH - 1);
  Xc_free(name);
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_selected - Loading callback for frame selection flag           ** 
** ----------------------------------------------------------------- */
static boolean cb_selected(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 Xc_TRACE(("cb_selected"));

 frame->selected = TRUE;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_shape - Loading callback for a frame shape                     ** 
** ----------------------------------------------------------------- */
static boolean cb_shape(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 real thickness;

 Xc_TRACE(("cb_shape"));

 if(!F(doc).addCallbacks(doc,
			 XcDF_POINT_KEYWORD, cb_point, frame,
			 XcDF_LINE_KEYWORD, cb_line, frame,
			 XcDF_COLOR_KEYWORD, cb_outline, frame,
			 NULL)) return FALSE;
 if(!F(doc).readReal(doc, &thickness)) return FALSE;
 frame->thickness = SCALE_FROM_POINTS(thickness);
 return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_point - Loading callback for a point in shape                  ** 
** ----------------------------------------------------------------- */
static boolean cb_point(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 real x, y;

 Xc_TRACE(("cb_point"));

 if(!F(doc).readReal(doc, &x) || !F(doc).readReal(doc, &y)) return FALSE;
 F(frame).addspoint(frame, SCALE_FROM_POINTS(x),
		    SCALE_FROM_POINTS(y), FALSE);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_line - Loading callback for a line in shape                    ** 
** ----------------------------------------------------------------- */
static boolean cb_line(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 real x, y;

 Xc_TRACE(("cb_line"));

 if(!F(doc).readReal(doc, &x) || !F(doc).readReal(doc, &y)) return FALSE;
 if (!frame->ListPoint) return FALSE;
 F(frame).addspoint(frame, SCALE_FROM_POINTS(x),
		    SCALE_FROM_POINTS(y), TRUE);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_background - Loading callback for background color             ** 
** ----------------------------------------------------------------- */
static boolean cb_background(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 c_Color *ptr;

 Xc_TRACE(("cb_background"));

 if(param < 0) param = 0;
 for(ptr = frame->BaseStd->color_base; ptr != NULL; ptr = ptr->next)
  if(ptr->id == param) return F(frame).setBackgroundColor(frame, ptr);

 Xc_ERROR(("Bad color #%ld", param));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_outline - Loading callback for outline color                   ** 
** ----------------------------------------------------------------- */
static boolean cb_outline(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 c_Color *ptr;

 Xc_TRACE(("cb_outline"));

 if(param < 0) param = 0;
 for(ptr = frame->BaseStd->color_base; ptr != NULL; ptr = ptr->next)
  if(ptr->id == param) return F(frame).setOutlineColor(frame, ptr);

 Xc_ERROR(("Bad color #%ld", param));
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_textflow - Loading callback for textflow mode                  ** 
** ----------------------------------------------------------------- */
static boolean cb_textflow(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 Xc_TRACE(("cb_textflow"));
 
 frame->textflow_mode = (textflow_t)param;
 return TRUE;
}
