/*
** docfile.c for XQuad in Tableau/
** Loading & saving methods for Tableau class
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
** Started on  Wed Apr  5 19:00:54 1995 Stéphane Boisson
** Last update Wed Feb  4 00:59:53 1998 Emmanuel Paris
*/

#include "TableauP.h"
#include "version.h"
#include "osname.h"
#include <time.h>

/*--- Loading ---*/
static boolean cb_filename ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Tableau *this));
static boolean cb_version ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_Tableau *this));
static boolean cb_osname ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Tableau *this));
static boolean cb_creator ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_Tableau *this));
static boolean cb_started_on ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_Tableau *this));
static boolean cb_last_update ___PROTO((c_DocFile *doc, char *keyword,
					long param, c_Tableau *this));
static boolean cb_color ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Tableau *tableau));
static boolean cb_style ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Tableau *tableau));
static boolean cb_format ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Tableau *tableau));


/* ----------------------------------------------------------------- ** 
** writeTableau - Saving method                                      ** 
** ----------------------------------------------------------------- */
boolean writeTableau(this, doc)
c_Tableau *this;
c_DocFile *doc;
{
 c_CellStyle *cellstyles;
 c_TextStyle *styles;
 c_FormatNbr *format;
 c_Color *colors;

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

 /*--- Write color list ---*/
 Xc_TRACE(("write colors"));
 if((colors = this->BaseStd.color_base) != NULL)
  if(F(colors).writeList(colors, doc) == FALSE) return FALSE;

 /*--- Write text style list ---*/
 Xc_TRACE(("text styles"));
 if((styles = this->BaseStd.text_style_base) != NULL)
  if(F(styles).writeList(styles, doc) == FALSE) return FALSE;

 /*--- Write format list ---*/
 Xc_TRACE(("formats"));
 if((format = this->BaseStd.format_base) != NULL)
  if(F(format).writeList(format, doc) == FALSE) return FALSE;

 /*--- Write CellStyles ---*/
 Xc_TRACE(("cell styles"));
 if((cellstyles = this->BaseStd.cell_style_base) != NULL)
  if(F(cellstyles).writeList(cellstyles, doc) == FALSE) return FALSE;

 /*--- Write sheets ---*/
 if(!F(this->Feuille).write(this->Feuille, doc)) return FALSE;
  
 Xc_TRACE(("done"));
 this->modified = FALSE;
  
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readTableau - Loading method                                      ** 
** ----------------------------------------------------------------- */
boolean readTableau(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
void *data;
{
 c_Tableau *tableau;
 c_CellStyle *cstyle;

 Xc_TRACE(("read"));

 if((tableau = NEW(c_Tableau)()) == NULL) return FALSE;
  
 /*--- Destroy the bases ---*/
 Xc_TRACE(("destroy CellStyle base"));
  
 cstyle = tableau->BaseStd.cell_style_base;
 while(cstyle->nb_style > 1)
  F(cstyle).RemoveCellStyleFromBase(cstyle, cstyle->cell_style);
 cstyle->nb_style = 0;
 F(cstyle).DeleteCellStyle(cstyle->cell_style);
 cstyle->default_cstyle = cstyle->cell_style = NULL;
 Xc_TRACE(("delete CellStyle base"));
  
 Xc_TRACE(("destroy TextStyle base"));
 while(tableau->BaseStd.text_style_base != NULL)
  DELETE(c_TextStyle)(tableau->BaseStd.text_style_base);

 Xc_TRACE(("destroy Format base"));
 while(tableau->BaseStd.format_base != NULL)
  DELETE(c_FormatNbr)(tableau->BaseStd.format_base);

 Xc_TRACE(("destroy Color base"));
 while(tableau->BaseStd.color_base != NULL)
  DELETE(c_Color)(tableau->BaseStd.color_base);
    
 do
 {
  unsigned char *name;

  if(!F(doc).addCallbacks(doc,
			  XcDF_FILENAME_KEYWORD, cb_filename, tableau,
			  XcDF_VERSION_KEYWORD, cb_version, tableau,
			  XcDF_OSNAME_KEYWORD, cb_osname, tableau,
			  XcDF_CREATOR_KEYWORD, cb_creator, tableau,
			  XcDF_STARTED_ON_KEYWORD, cb_started_on, tableau,
			  XcDF_LAST_UPDATE_KEYWORD, cb_last_update, tableau,
			  XcDF_CELLSTYLELIST_KEYWORD,
			  FUNCTION(c_CellStyle, readList), &tableau->BaseStd,
			  XcDF_SHEET_KEYWORD,
			  FUNCTION(c_Feuille, read), tableau->Feuille,
			  XcDF_COLORLIST_KEYWORD, cb_color, tableau,
			  XcDF_FORMATLIST_KEYWORD, cb_format, tableau,
			  XcDF_STYLELIST_KEYWORD, cb_style, tableau,
			  NULL)) break;

  /*--- Read document name ---*/
  if(!F(doc).readString(doc, &name)) break;;
  if(name != NULL)
  {
   Xc_TRACE(("name: `%s'", name));
   strncpy(tableau->name, name, XcMAX_NAME_LENGTH - 1);
   Xc_free(name);
  }
    
  /*--- Return doc ---*/
  doc->data_type = XcDF_DOCUMENT_TYPE;
  doc->data_ptr = tableau;

  Xc_TRACE(("done"));
  return TRUE;

 } while(0);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_filename                                                       ** 
** ----------------------------------------------------------------- */
static boolean cb_filename(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Tableau *this;
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
c_Tableau *this;
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
c_Tableau *this;
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
c_Tableau *this;
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
c_Tableau *this;
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
c_Tableau *this;
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
** cb_color - Color-list loading callback                            **
** ----------------------------------------------------------------- */
static boolean cb_color(doc, keyword, param, tableau)
c_DocFile *doc;
char *keyword;
long param;
c_Tableau *tableau;
{
 Xc_TRACE(("cb_color"));

 do
 {
  if(!F(doc).addCallbacks(doc,
			  XcDF_COLOR_KEYWORD, FUNCTION(c_Color, read),
			  &tableau->BaseStd.color_base,
			  NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_COLOR_KEYWORD, TRUE)) break;

  Xc_TRACE(("cb_color ok"));
  return TRUE;
 } while(0);
 return FALSE;
}


/* ----------------------------------------------------------------- **
** cb_format - format-list loading callback                          **
** ----------------------------------------------------------------- */
static boolean cb_format(doc, keyword, param, tableau)
c_DocFile *doc;
char *keyword;
long param;
c_Tableau *tableau;
{
 format_read_t data;

 Xc_TRACE(("cb_format"));

 if(tableau->BaseStd.color_base == NULL)
 {
  Xc_ERROR(("color-list expected before format-list"));
  return FALSE;
 }

 if(tableau->BaseStd.format_base != NULL) return TRUE;
  
 data.format_base = &tableau->BaseStd.format_base;
 data.color_base = &tableau->BaseStd.color_base;

 return (FUNCTION(c_FormatNbr,readList)(doc, keyword, 0L, &data));
 /*  if(!F(doc).addCallbacks(doc,
     XcDF_FORMAT_KEYWORD, FUNCTION(c_FormatNbr, read),
     &data, NULL)) return FALSE;
     return F(doc).expectKeyword(doc, XcDF_FORMAT_KEYWORD, TRUE); */
}


/* ----------------------------------------------------------------- ** 
** cb_style - Loading callback for text style                        ** 
** ----------------------------------------------------------------- */
static boolean cb_style(doc, keyword, param, tableau)
c_DocFile *doc;
char *keyword;
long param;
c_Tableau *tableau;
{
 textstyle_read_t tmp;

 Xc_TRACE(("cb_style"));

 if(GlobFontBase == NULL)
 {
  Xc_ERROR(("No fonts loaded"));
  return FALSE;
 }

 if(tableau->BaseStd.color_base == NULL)
 {
  Xc_ERROR(("color-list expected before style-list"));
  return FALSE;
 }

 tmp.style_base = &tableau->BaseStd.text_style_base;
 tmp.color_base = tableau->BaseStd.color_base;
 tmp.font_base = GlobFontBase;

 do
 {
  if(!F(doc).addCallbacks(doc, XcDF_STYLE_KEYWORD,
			  FUNCTION(c_TextStyle, read), &tmp, NULL)) break;
  if(!F(doc).expectKeyword(doc, XcDF_STYLE_KEYWORD, TRUE)) break;
  Xc_TRACE(("cb_style ok"));
  return TRUE;
 } while(0);
 return FALSE;
}
