/*
** import.c for Xclamation in TextFrame/
** Text importation
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
** Started on  Wed Nov 23 16:52:00 1994 Stéphane Boisson
** Last update Fri Jun  6 13:48:30 1997 Emmanuel Paris
*/

/*#define NTRACE*/
#define NHISTORY

#include "TextFrame.h"
#include <sys/file.h>
#include "DocFile.h"
/* #include "XMWDocFile.h" */
#include "BaseStd.h"
#include "VectorFont.h"
#include "Text.h"
#include "Color.h"
#include "TextStyle.h"
#include "TextRuler.h"

/*--- Internal structure ---*/
typedef struct
{
 BaseStd_t *base;
 text_read_t text;
 c_Color *color_base;
} text_import_data_t; 

/*--- Prototypes ---*/
boolean readTextFrame ___PROTO((c_DocFile *doc, char *keyword,
				long param, c_Cadre *frame));
boolean importText ___PROTO((c_TextFrame *this, BaseStd_t *base,
			     char *filename));
boolean importASCII ___PROTO((c_TextFrame *this, BaseStd_t *base,
			      char *filename,
			      c_TextStyle *style, c_TextRuler *ruler));
/*boolean importXMW ___PROTO((c_TextFrame *this, BaseStd_t *base,*/
/*char *filename));*/

/*--- Local prototypes ---*/
static boolean cb_color ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_import_data_t *data));
static boolean cb_style ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_import_data_t *data));
static boolean cb_ruler ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_import_data_t *data));
static boolean cb_link ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_TextFrame *this));
extern c_VectorFont *GlobFontBase;
extern void hook_text(); 

/* ----------------------------------------------------------------- ** 
** importText - Entry point                                          ** 
** ----------------------------------------------------------------- */
boolean importText(this, base, filename)
c_TextFrame *this;
BaseStd_t *base;
char *filename;
{
 text_import_data_t data;
 c_DocFile *file;

 Xc_HISTORY(("importText(`%s')", filename));

 if((file = NEW(c_DocFile)(XcDF_LOAD, filename, -1)) == NULL) return FALSE;
 while(42)
 {
  memset(&data, 0, sizeof(text_import_data_t));
  data.base = base;
  data.text.text_base = &base->text_base;

  if(!F(file).addCallbacks(file,
			   XcDF_COLORLIST_KEYWORD, cb_color, &data,
			   XcDF_STYLELIST_KEYWORD, cb_style, &data,
			   XcDF_RULERLIST_KEYWORD, cb_ruler, &data,
			   XcDF_TEXT_KEYWORD,
			   FUNCTION(c_Text,read),&data.text, NULL)) break;

  if(!F(file).expectKeyword(file, XcDF_DOCUMENT_KEYWORD, FALSE) ||
     (file->data_type != XcDF_TEXT_TYPE)) break;

  F(this).attachText(this, file->data_ptr);
  DELETE(c_DocFile)(file);
  Xc_TRACE(("done"));
  return TRUE;
 }

 Xc_TRACE(("loading error"));
 DELETE(c_DocFile)(file);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_color - Color-list loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_color(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_import_data_t *data;
{
 Xc_TRACE(("cb_color"));

 do
 {
  data->color_base = NULL;
  if(!F(doc).addCallbacks(doc,
			  XcDF_COLOR_KEYWORD,
			  FUNCTION(c_Color, read),
			  &data->color_base,
			  NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_COLOR_KEYWORD, TRUE)) break;
  if(data->color_base == NULL) return TRUE;

#if 0
  {
   c_Color *ptr;
      
   for(ptr = data->color_base; ptr != NULL; ptr = ptr->next)
    Xc_TRACE(("color `%s' #%ld", ptr->name, ptr->id));
  }
#endif

  F(data->color_base).merge(data->color_base,
			    &data->base->color_base, TRUE);
  data->color_base = data->base->color_base;

#if 0
  {
   c_Color *ptr;
      
   for(ptr = data->color_base; ptr != NULL; ptr = ptr->next)
    Xc_TRACE(("color `%s' #%ld", ptr->name, ptr->id));
  }
#endif

  return TRUE;
 } while(0);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_style - Style-list loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_style(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_import_data_t *data;
{
 textstyle_read_t tmp;

 Xc_TRACE(("cb_style"));

 if(data->color_base == NULL)
 {
  Xc_ERROR(("color-list expected before style-list"));
  return FALSE;
 }
 if(GlobFontBase == NULL)
 {
  Xc_ERROR(("No fonts loaded"));
  return FALSE;
 }
 data->text.style_base = NULL;
 tmp.style_base = &data->text.style_base;
 tmp.color_base = data->base->color_base;
 tmp.font_base = GlobFontBase;
 do
 {
  if(!F(doc).addCallbacks(doc, XcDF_STYLE_KEYWORD,
			  FUNCTION(c_TextStyle, read), &tmp, NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_STYLE_KEYWORD, TRUE)) break;
  if(data->text.style_base == NULL) return TRUE;
  F(data->text.style_base).merge(data->text.style_base,
				 &data->base->text_style_base, TRUE);
  data->text.style_base = data->base->text_style_base;
  return TRUE;
 } while(0);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_ruler - Rulerlist loading callback                             ** 
** ----------------------------------------------------------------- */
static boolean cb_ruler(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_import_data_t *data;
{
 Xc_TRACE(("cb_ruler"));

 do
 {
  data->text.ruler_base = NULL;
  if(!F(doc).addCallbacks(doc,
			  XcDF_RULER_KEYWORD,
			  FUNCTION(c_TextRuler, read),
			  &data->text.ruler_base,
			  NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_RULER_KEYWORD, TRUE)) break;
  if(data->text.ruler_base == NULL) return TRUE;

  F(data->text.ruler_base).merge(data->text.ruler_base,
				 &data->base->text_ruler_base, TRUE);
  data->text.ruler_base = data->base->text_ruler_base;

  return TRUE;
 } while(0);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** importASCII - Import ASCII                                        ** 
** ----------------------------------------------------------------- */
boolean importASCII(this, base, filename, style, ruler)
c_TextFrame *this;
BaseStd_t *base;
char *filename;
c_TextStyle *style;
c_TextRuler *ruler;
{
 char_t *buffer, *ptr;
 c_Text *text;
 int handle;
 long size, len;

 Xc_HISTORY(("importASCII"));

 if(!style || !ruler) return FALSE;

 if((handle = open(filename, O_RDONLY)) <= 0)
 {
  Xc_SYSERROR((filename));
  return FALSE;
 }
 size = lseek(handle, 0, SEEK_END);
 lseek(handle, 0, SEEK_SET);
 if((buffer = Xc_malloc("text loading buffer", size)) == NULL)
 {
  close(handle);
  return FALSE;
 }
 read(handle, buffer, size);
 close(handle);

 if((text = NEW(c_Text)("TestText", &base->text_base, style, ruler)) == NULL)
 {
  Xc_free(buffer);
  return FALSE;
 }

 Xc_TRACE(("Filter text"));
 for(ptr = buffer, len = size; len--; ptr++)
  if((*ptr < 32) && (*ptr != '\n')) *ptr = 32;

 F(text).insertString(text, buffer, size);
 Xc_free(buffer);

 F(this).attachText(this, text);

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_link - Link loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_link(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_TextFrame *this;
{
 Xc_TRACE(("cb_link"));

 this->id = MAX(param, 0);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readTextFrame - Standard frame read methods                       ** 
** ----------------------------------------------------------------- */
boolean readTextFrame(doc, keyword, param, frame)
c_DocFile *doc;
char *keyword;
long param;
c_Cadre *frame;
{
 c_TextFrame *this;
 c_Text *text;

 Xc_TRACE(("read"));

 if(param < 0) param = 0;
 for(text = frame->BaseStd->text_base; text != NULL; text = text->next)
  if(text->id == param)
  {
   c_TextFrame **buffer;
   unsigned long count;
   boolean rc = FALSE;

   if((this = NEW(c_TextFrame)()) == NULL) return FALSE;
   if(!F(doc).addCallbacks(doc, XcDF_LINK_KEYWORD, cb_link, this, NULL) ||
      !F(doc).expectKeyword(doc, XcDF_LINK_KEYWORD, FALSE) ||
      (this->id < 0))
   {
    DELETE(c_TextFrame)(this);
    return FALSE;
   }

   Xc_TRACE(("new id: %ld", this->id));
   count = F(text->hook).getClientData(text->hook, (void ***)&buffer);
   Xc_ASSERT(count <= 1);
   if(count == 0)
    rc = F(this).attachText(this, text);
   else
   {
    c_TextFrame *ptr;

    Xc_ASSERT((*buffer)->previous == NULL);
    for(ptr = *buffer; ptr != NULL; ptr = ptr->next)
    {
     Xc_TRACE(("id: %ld", ptr->id));
     if(ptr->id > this->id)
     {
      rc = TRUE;
      if ((this->previous = ptr->previous) != NULL) ptr->previous->next = this;
      this->next = ptr; ptr->previous = this;
      this->text = ptr->text;

      if (this->previous == NULL)
      {
       XcH_UNREGISTER(this->text, hook_text, ptr);
       XcH_REGISTER(this->text, hook_text, this);
      }
/*rc = F(this).link(this, ptr, TRUE);*/
      break;
     }
     else if(ptr->next == NULL)
     {
      rc = TRUE;
      this->next = NULL;
      this->previous = ptr; ptr->next = this;
      this->text = ptr->text;

/*rc = F(this).link(this, ptr, FALSE);*/
      break;
     }
    }
    Xc_free(buffer);
    Xc_ASSERT(ptr != NULL);
   }

   if(rc == FALSE)
   {
    DELETE(c_TextFrame)(this);
    return FALSE;
   }
   
   this->end_pos = - this->id - 1;	
   /* hack to make sure that the first  configure will propagate correctly */
   
   frame->object_type = CADRE_TEXT;	
   frame->object = this;
   F(frame).configure_request(frame, TRUE);
   /*F(frame).attachobject(frame, this, CADRE_TEXT);*/
   return TRUE;
  }
 Xc_ERROR(("Bad text #%ld", param));
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** importXMW - Entry point                                           ** 
** ----------------------------------------------------------------- */
/* boolean importXMW(this, base, filename)
   c_TextFrame *this;
   BaseStd_t *base;
   char *filename;
   {
    text_import_data_t	data;
    c_XMWDocFile		*file;
    boolean		ret;

    Xc_HISTORY(("importText(`%s')", filename));

    if((file = NEW(c_XMWDocFile)(filename, -1)) == NULL) 
     return FALSE;

    ret=F(file).readMain(file, base, this);
    DELETE(c_XMWDocFile)(file);
    return ret;
   } */





