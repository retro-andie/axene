/*
** config.c for Xclamation in Main/
** Functions for loading & saving config file
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
** Started on  Thu Nov 17 18:16:04 1994 Stéphane Boisson
** Last update Fri Oct  9 15:51:14 1998 Emmanuel Paris
*/

#define NTRACE

#include "config.h"
#include "DocFile.h"
#include "VectorFont.h"
#include "Color.h"
#include "TextRuler.h"
#include "TextStyle.h"
#include "ImageCache.h"
#include "RegisterHelp.h"
#include "PlugIn.h"
#include "file.h"

extern c_VectorFont *GlobFontBase;
extern c_Color *GlobColorBase;
extern c_TextRuler *GlobTextRulerBase;
extern c_TextStyle *GlobTextStyleBase;
extern printer_t *GlobPrinterBase;
extern c_ImageCache *GlobImgCache;
extern c_RegisterHelp *GlobRegHelp;

static boolean cb_printer ___PROTO((c_DocFile *doc, char *keyword,
				    long param, printer_t **base));
static boolean cb_getstring ___PROTO((c_DocFile *doc, char *keyword,
					 long param, char **data));
static boolean cb_style ___PROTO((c_DocFile *doc));


/* ----------------------------------------------------------------- ** 
** ReadConfigFile - Read the main config file                        ** 
** ----------------------------------------------------------------- */
boolean ReadConfigFile()
{
  c_DocFile *config_file;
  boolean rc = FALSE;
  char *filename;

  Xc_TRACE(("ReadConfigFile"));
  Xc_mcheck();

  filename = TranslateFilename(NULL, Xc_CONFIG_FILE);
  config_file = NEW(c_DocFile)(XcDF_LOAD, filename, -1);
  Xc_free(filename);

  if(config_file != NULL)
    {
      F(config_file).addCallbacks(config_file,	
				  XcDF_STYLELIST_KEYWORD, cb_style, NULL,
				  XcDF_FONTLIST_KEYWORD,
				  FUNCTION(c_VectorFont, readList),
				  &GlobFontBase,
				  XcDF_COLORLIST_KEYWORD,
				  FUNCTION(c_Color, readList), &GlobColorBase,
				  XcDF_PRINTER_KEYWORD,
				  cb_printer, &GlobPrinterBase,
				  XcDF_RULERLIST_KEYWORD,
				  FUNCTION(c_TextRuler, readList),
				  &GlobTextRulerBase,
				  XcDF_CACHE_CONFIG_KEYWORD,
				  FUNCTION(c_ImageCache, read_config),
				  GlobImgCache,
				  XcDF_HELP_CONFIG_KEYWORD,
				  FUNCTION(c_RegisterHelp, read_config),
				  GlobRegHelp,	
				  NULL);
      rc = F(config_file).expectKeyword(config_file, XcDF_CONFIG_KEYWORD,
					FALSE);
      DELETE(c_DocFile)(config_file);
      Xc_mcheck();
    }
  
  if(GlobColorBase == NULL)
    {
      GlobColorBase = (c_Color *)NEW(c_Color)(&GlobColorBase,"None");
      F(GlobColorBase).set(GlobColorBase, XcC_TRANSPARENCY, 
			   XcC_SCALE_COLOR(1.0), XcC_END);
    }
  
  if(GlobPrinterBase == NULL)
    {
      GlobPrinterBase = Xc_malloc("printer", sizeof(printer_t));
      GlobPrinterBase->name = Xc_malloc("prtFile", strlen("File")+1);
      strcpy(GlobPrinterBase->name, "File");
      GlobPrinterBase->command = NULL;
      GlobPrinterBase->next = NULL;
    }

#if 0
  if(getenv("XC_PLUGIN_TEST") != NULL)
    {
      c_PlugIn *test;

      Xc_HISTORY(("Begin Plug-In Test!"));
      if((test = NEW(c_PlugIn)(XcPLUGIN_TEXT_IN)) != NULL)
	{
	  if(!F(test).add(test, NULL, NULL))
	    Xc_ERROR(("Add plugs-in"));

	  F(test).getByName(test, "GlOUPS");
	  DELETE(c_PlugIn)(test);
	  Xc_mcheck();
	}
      Xc_FATAL(("End Plug-In Test!"));
    }
#endif

  Xc_TRACE(("read"));
  Xc_mcheck();
  return rc;
}


/* ----------------------------------------------------------------- ** 
** ReadUserConfigFile - Read the User config file in $HOME           ** 
** ----------------------------------------------------------------- */
boolean ReadUserConfigFile()
{
  c_DocFile *config_file;
  char *filename;
  boolean rc;

  Xc_TRACE(("ReadUserConfigFile"));

  if((filename = getenv("HOME")) == NULL) filename = "";
  filename = Xc_strconcat(filename, "/", Xc_USER_CONFIG_FILE, NULL);
  if(filename == NULL) return FALSE;
  if(IsFileExist(filename))
    config_file = NEW(c_DocFile)(XcDF_LOAD, filename, -1);
  else
    config_file = NULL;

  Xc_free(filename);

  if(config_file == NULL) return FALSE;

  F(config_file).addCallbacks(config_file,	
			      XcDF_STYLELIST_KEYWORD, cb_style, NULL,
			      XcDF_FONTLIST_KEYWORD,
			      FUNCTION(c_VectorFont, readList),
			      &GlobFontBase,
			      XcDF_COLORLIST_KEYWORD,
			      FUNCTION(c_Color, readList), &GlobColorBase,
			      XcDF_PRINTER_KEYWORD,
			      cb_printer, &GlobPrinterBase,
			      XcDF_RULERLIST_KEYWORD,
			      FUNCTION(c_TextRuler, readList),
			      &GlobTextRulerBase,
			      XcDF_CACHE_CONFIG_KEYWORD,
			      FUNCTION(c_ImageCache, read_config),
			      GlobImgCache,
			      XcDF_HELP_CONFIG_KEYWORD,
			      FUNCTION(c_RegisterHelp, read_config),
			      GlobRegHelp,
			      NULL);
  rc = F(config_file).expectKeyword(config_file, XcDF_CONFIG_KEYWORD, FALSE);
  DELETE(c_DocFile)(config_file);
  Xc_mcheck();
  return rc;
}


/* ----------------------------------------------------------------- ** 
** WriteUserConfigFile - Write the User config file in $HOME         ** 
** ----------------------------------------------------------------- */
boolean WriteUserConfigFile()
{
  printer_t *printer, *ptr;

  Xc_TRACE(("WriteUserConfigFile"));

  for(printer = GlobPrinterBase; printer != NULL; )
    {
      ptr = printer->next;

      Xc_free(printer->name);
      if(printer->command != NULL) Xc_free(printer->command);
      Xc_free(printer);
      printer = ptr;
    }
    
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_printer - Loading callback froprinter                          ** 
** ----------------------------------------------------------------- */
static boolean cb_printer(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
printer_t **base;
{
  printer_t *ptr;

  Xc_TRACE(("cb_printer"));

  if((ptr = Xc_malloc("printer", sizeof(printer_t))) == NULL) return FALSE;
  ptr->name = NULL;
  ptr->command = NULL;

  if(!F(doc).addCallbacks(doc,
			  XcDF_NAME_KEYWORD, cb_getstring, &(ptr->name),
			  XcDF_COMMAND_KEYWORD, cb_getstring, &(ptr->command),
			  NULL)) {Xc_free(ptr); return FALSE; }
  
  if(!F(doc).expectKeyword(doc, NULL, TRUE))
    {
      if(ptr->name != NULL) Xc_free(ptr->name);
      if(ptr->command != NULL) Xc_free(ptr->command);
      Xc_free(ptr);
      return FALSE;
    }

  Xc_TRACE(("printer `%s' (%s)", ptr->name, ptr->command? ptr->command:""));
  
  ptr->next = *base;
  *base = ptr;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_getstring - Loading callback to set a string pointer           ** 
** ----------------------------------------------------------------- */
static boolean cb_getstring(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
char **data;
{
  Xc_TRACE(("cb_getstring"));

  return F(doc).readString(doc, (unsigned char **)data);
}


/* ----------------------------------------------------------------- ** 
** cb_style - Style-list loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_style(doc)
c_DocFile *doc;
{
  textstyle_read_t tmp;
  c_TextStyle *base;

  Xc_TRACE(("cb_style"));

  if(GlobFontBase == NULL)
    {
      Xc_ERROR(("No fonts loaded"));
      return FALSE;
    }

  if(GlobColorBase == NULL)
    {
      Xc_ERROR(("color-list expected before style-list"));
      return FALSE;
    }

  base = NULL;
  tmp.style_base = &base;
  tmp.color_base = GlobColorBase;
  tmp.font_base = GlobFontBase;

  do {
    if(!F(doc).addCallbacks(doc, XcDF_STYLE_KEYWORD,
                            FUNCTION(c_TextStyle, read), &tmp, NULL)) break;
    if(!F(doc).expectKeyword(doc, XcDF_STYLE_KEYWORD, TRUE)) break;
    if(base == NULL) return TRUE;
    F(base).merge(base, &GlobTextStyleBase, TRUE);
    return TRUE;
  } while(0);

  return FALSE;
}
