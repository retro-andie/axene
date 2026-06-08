/*
** docfile.c for XAllWrite in Document/
** Loading & saving methods for Document class
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Wed Feb  4 00:59:06 1998 Emmanuel Paris
*/

#include "DocumentP.h"
#include "TextP.h"
#include "PageAreaP.h"
#include "version.h"
#include "osname.h"
#include <time.h>

/*--- Loading ---*/
static boolean cb_filename ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Document *this));
static boolean cb_version ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_Document *this));
static boolean cb_osname ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Document *this));
static boolean cb_creator ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_Document *this));
static boolean cb_started_on ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_Document *this));
static boolean cb_last_update ___PROTO((c_DocFile *doc, char *keyword,
					long param, c_Document *this));
static boolean cb_geometry ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Document *this));
static boolean cb_color ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Document *this));
static boolean cb_style ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Document *this));
static boolean cb_ruler ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Document *this));

/* ----------------------------------------------------------------- ** 
** writeDocument - Saving method                                     ** 
** ----------------------------------------------------------------- */
boolean writeDocument(this, doc)
c_Document *this;
c_DocFile *doc;
{ 
 c_TextRuler *rulers;
 c_TextStyle *styles;
 c_Color *colors;
 c_Text *texts;

 Xc_HISTORY(("write"));
 
 /* update document information */
 {
  time_t now = time(NULL);
  char	*lastcr;

  sprintf(this->version, "%d.%d.%d", MAJOR, MINOR, PL);
  sprintf(this->osname, "%.30s", Xc_SYSTEM_NAME);
  sprintf(this->last_update_date, "%.30s", ctime(&now));
  if ((lastcr = strrchr(this->last_update_date, '\n')) != NULL)
   *lastcr = '\0';

  strncpy(this->creator, Xc_getgecos, XcMAX_NAME_LENGTH);
 }

 if(!F(doc).writeKeyword(doc, XcDF_DOCUMENT_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).write(doc, this->name, strlen(this->name))) return FALSE;
 
 if (!F(doc).writeKeyword(doc, XcDF_FILENAME_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->filename, strlen(this->filename)) ||
     !F(doc).endSequence(doc))
  return FALSE;   
 
 if (!F(doc).writeKeyword(doc, XcDF_VERSION_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->version, strlen(this->version)) ||
     !F(doc).endSequence(doc))
  return FALSE;   

 if (!F(doc).writeKeyword(doc, XcDF_OSNAME_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->osname, strlen(this->osname)) ||
     !F(doc).endSequence(doc))
  return FALSE;   

 if (!F(doc).writeKeyword(doc, XcDF_CREATOR_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->creator, strlen(this->creator)) ||
     !F(doc).endSequence(doc))
  return FALSE;   

 if (!F(doc).writeKeyword(doc, XcDF_STARTED_ON_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->started_on_date, strlen(this->started_on_date))||
     !F(doc).endSequence(doc))
  return FALSE;   
 
 if (!F(doc).writeKeyword(doc, XcDF_LAST_UPDATE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->last_update_date, 
		   strlen(this->last_update_date)) ||
     !F(doc).endSequence(doc))
  return FALSE;   

 if (!this->Maximized &&
     (!F(doc).writeKeyword(doc, XcDF_GEOMETRY_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeInteger(doc, (long)this->PosX) ||
      !F(doc).writeInteger(doc, (long)this->PosY) ||
      !F(doc).writeInteger(doc, (long)this->Width) ||
      !F(doc).writeInteger(doc, (long)this->Height) ||
      !F(doc).endSequence(doc)))
  return FALSE; 

 for(texts = this->BaseStd.text_base; texts != NULL; texts = texts->next)
  F(texts).purge_changed_items(texts);
 
 /*--- Write color list ---*/
 Xc_TRACE(("write colors"));
 if ((colors = this->BaseStd.color_base) != NULL)
  if (F(colors).writeList(colors, doc) == FALSE) return FALSE;

 /*--- Write text ruler list ---*/
 Xc_TRACE(("text rulers"));
 if ((rulers = this->BaseStd.text_ruler_base) != NULL)
  if (F(rulers).writeList(rulers, doc) == FALSE) return FALSE;

 /*--- Write text style list ---*/
 Xc_TRACE(("text styles"));
 if ((styles = this->BaseStd.text_style_base) != NULL)
  if (F(styles).writeList(styles, doc) == FALSE) return FALSE;

 /*--- Write text list ---*/
 Xc_TRACE(("texts"));
 if ((texts = this->BaseStd.text_base) != NULL)
  if (F(texts).writeList(texts, doc) == FALSE) return FALSE;
 
 if (F(this->PageArea).writePageArea(this->PageArea, doc) == FALSE)
  return FALSE;

 Xc_TRACE(("done"));
 this->modified = FALSE;
 
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readDocument - Loading method                                     ** 
** ----------------------------------------------------------------- */
boolean readDocument(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
void *data;
{
 c_Document *Document;
 
 Xc_TRACE(("read"));
 
 if((Document = NEW(c_Document)()) == NULL) return FALSE;

 /*--- Destroy the document database ---*/
 if (Document->BaseStd.text_base != NULL)
  DELETE(c_Text)(Document->BaseStd.text_base);
 while(Document->BaseStd.text_style_base != NULL)
  DELETE(c_TextStyle)(Document->BaseStd.text_style_base);
 while(Document->BaseStd.text_ruler_base != NULL)
  DELETE(c_TextRuler)(Document->BaseStd.text_ruler_base);
 while(Document->BaseStd.color_base != NULL)
  DELETE(c_Color)(Document->BaseStd.color_base);
 
 do
 {
  unsigned char *name;
   
  if(!F(doc).addCallbacks(doc,
			  XcDF_FILENAME_KEYWORD, cb_filename, Document,
			  XcDF_VERSION_KEYWORD, cb_version, Document,
			  XcDF_OSNAME_KEYWORD, cb_osname, Document,
			  XcDF_CREATOR_KEYWORD, cb_creator, Document,
			  XcDF_STARTED_ON_KEYWORD, cb_started_on, Document,
			  XcDF_LAST_UPDATE_KEYWORD, cb_last_update, Document,
			  XcDF_GEOMETRY_KEYWORD, cb_geometry, Document,
			  XcDF_COLORLIST_KEYWORD, cb_color, Document,
			  XcDF_RULERLIST_KEYWORD, cb_ruler, Document,
			  XcDF_STYLELIST_KEYWORD, cb_style, Document,
			  XcDF_TEXTLIST_KEYWORD, FUNCTION(c_Text, readList), 
			  &Document->BaseStd,
			  XcDF_PAGEAREA_KEYWORD, 
			  FUNCTION(c_PageArea, readPageArea), 
			  Document->PageArea,
			  NULL)) break;
   
  /*--- Read document name ---*/
  if(!F(doc).readString(doc, &name)) break;
  if(name != NULL)
  {
   Xc_TRACE(("name: `%s'", name));
   strncpy(Document->name, name, XcMAX_NAME_LENGTH - 1);
   Xc_free(name);
  }

  /*--- Return doc ---*/
  doc->data_type = XcDF_DOCUMENT_TYPE;
  doc->data_ptr = Document;
  
  Xc_TRACE(("done"));
  return TRUE;
  
 } while(0);
 Xc_TRACE(("loading error"));
 DELETE(c_Document)(Document);
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_filename                                                       ** 
** ----------------------------------------------------------------- */
static boolean cb_filename(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 unsigned char *name;
 
 Xc_TRACE(("cb_true_name"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  Xc_HISTORY(("cb_filename: %s", name));
  Xc_free(name);
 }
 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_version                                                        ** 
** ----------------------------------------------------------------- */
static boolean cb_version(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 unsigned char *name;
 
 Xc_TRACE(("cb_version"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  sprintf(this->version, "%.9s", name);
  Xc_free(name);
 }
 
 Xc_HISTORY(("cb_version: %s", this->version));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_osname                                                         ** 
** ----------------------------------------------------------------- */
static boolean cb_osname(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 unsigned char *name;
 
 Xc_TRACE(("cb_osname"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  sprintf(this->osname, "%.30s", name);
  Xc_free(name);
 }
 
 Xc_HISTORY(("cb_osname: %s", this->osname));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_creator                                                        ** 
** ----------------------------------------------------------------- */
static boolean cb_creator(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 unsigned char *name;
 
 Xc_TRACE(("cb_creator"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strncpy(this->creator, name, XcMAX_NAME_LENGTH);
  Xc_free(name);
 }
 
 Xc_HISTORY(("cb_creator: %s", this->creator));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_started_on                                                     ** 
** ----------------------------------------------------------------- */
static boolean cb_started_on(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 unsigned char *name;
 
 Xc_TRACE(("cb_started_on"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  sprintf(this->started_on_date, "%.30s", name);
  Xc_free(name);
 }
 Xc_HISTORY(("cb_started_on: %s", this->started_on_date));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_last_update                                                    ** 
** ----------------------------------------------------------------- */
static boolean cb_last_update(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 unsigned char *name;
 
 Xc_TRACE(("cb_last_update"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  sprintf(this->last_update_date, "%.30s", name);
  Xc_free(name);
 }
 Xc_HISTORY(("cb_last_update: %s", this->last_update_date));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_geometry: read document geometry                               ** 
** ----------------------------------------------------------------- */
static boolean cb_geometry(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 long px, py, w, h;
 
 Xc_TRACE(("cb_geometry"));
 if (!F(doc).readInteger(doc, &px) ||
     !F(doc).readInteger(doc, &py) ||
     !F(doc).readInteger(doc, &w) ||
     !F(doc).readInteger(doc, &h))
  return FALSE;
 
 this->PosX = px; this->PosY = py;
 this->Width = w; this->Height = h;
 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_color - Color-list loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_color(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 Xc_TRACE(("cb_color"));

 do
 {
  if(!F(doc).addCallbacks(doc, XcDF_COLOR_KEYWORD,
			  FUNCTION(c_Color, read), &this->BaseStd.color_base,
			  NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_COLOR_KEYWORD, TRUE)) break;
  return TRUE;
 } while(0);
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_style - Style-list loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_style(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 textstyle_read_t tmp;

 Xc_TRACE(("cb_style"));

 if(GlobFontBase == NULL)
 {
  Xc_ERROR(("No fonts loaded"));
  return FALSE;
 }
 if(this->BaseStd.color_base == NULL)
 {
  Xc_ERROR(("color-list expected before style-list"));
  return FALSE;
 }
 tmp.style_base = &this->BaseStd.text_style_base;
 tmp.color_base = this->BaseStd.color_base;
 tmp.font_base = GlobFontBase;
 do {
     if(!F(doc).addCallbacks(doc, XcDF_STYLE_KEYWORD,
			     FUNCTION(c_TextStyle, read), &tmp, NULL)) break;
     if(!F(doc).expectKeyword(doc, XcDF_STYLE_KEYWORD, TRUE)) break;
     return TRUE;
    } while(0);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_ruler - Rulerlist loading callback                             ** 
** ----------------------------------------------------------------- */
static boolean cb_ruler(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Document *this;
{
 Xc_TRACE(("cb_ruler"));

 do
 {
  if(!F(doc).addCallbacks(doc,
			  XcDF_RULER_KEYWORD,
			  FUNCTION(c_TextRuler, read),
			  &this->BaseStd.text_ruler_base,
			  NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_RULER_KEYWORD, TRUE)) break;
  return TRUE;
 } while(0);
 Xc_TRACE(("loading error"));
 return FALSE;
}
