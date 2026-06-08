/*
** VectorFont.h for Xclamation, XQuad and XAllWrite in VectorFont/
** Definitions of the VectorFont class
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
** Started on  Fri Jun 10 19:57:36 1994 Stéphane Boisson
** Last update Mon Aug  3 19:19:44 1998 Emmanuel Paris
*/

#ifndef __VECTORFONT_H__
#define __VECTORFONT_H__

typedef struct sc_VectorFont c_VectorFont;

#define XcDF_FONTLIST_KEYWORD "FONTLIST"
#define XcDF_FONT_KEYWORD "FONT"
#define XcDF_FAMILY_KEYWORD "FAMILY"
#define XcDF_ALIAS_KEYWORD "ALIAS"
#define XcDF_NORMAL_KEYWORD "NORMAL"
#define XcDF_BOLD_KEYWORD "BOLD"
#define XcDF_ITALIC_KEYWORD "ITALIC"
#define XcDF_BOLD_ITALIC_KEYWORD "BOLD_ITALIC"
#define XcDF_DOWNLOAD_KEYWORD "DOWNLOAD"
#ifndef XcDF_HIDE_KEYWORD
#define XcDF_HIDE_KEYWORD "HIDE"
#endif


#include "xcalibur.h"
#include "vectors.h"
#include "CharMetrics.h"
#include "DocFile.h"
#include "FontEncoding.h"
#include "Hook.h"
#include "PostScript.h"

enum fonttype_e
{
 XcFT_UNKNOW,
 XcFT_POSTSCRIPT_TYPE1,
 XcFT_CALAMUS
};


/*--- Define the methods for the VectorFont Class ---*/
typedef struct 
{
 F_STD;
 boolean (*load) ___PROTO((c_VectorFont *this));

 c_VectorFont *(*getFontByName) ___PROTO((c_VectorFont *this, char *name));

 /*___Loading & Saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long param, c_VectorFont **base));
 boolean (*write) ___PROTO((c_VectorFont *class, c_DocFile *doc));
 boolean (*readList) ___PROTO((c_DocFile *doc, char *keyword,
			       long param, c_VectorFont **base));
 boolean (*writeList) ___PROTO((c_VectorFont *class, c_DocFile *doc));
  
 /*___Printing___*/
 boolean (*ps_print) ___PROTO((c_VectorFont *this, c_PostScript *post));
  
 c_VectorFont *(*getFontByFamilyAndAttribs) ___PROTO((c_VectorFont *this,
						      char *family_name, boolean bold, boolean italic));
  
 c_VectorFont *(*selectDialog) ___PROTO((c_VectorFont *base, char *fontname));

#ifndef NDEBUG
 void (*dump) ___PROTO((c_VectorFont *this));
#endif
} sf_VectorFont;

typedef struct _font_list 
{
 struct _font_list	*Next;
 c_VectorFont		*font;
 boolean		is_normal;
 boolean		is_bold;
 boolean		is_italic;
 boolean		is_bold_italic;
} font_list;

typedef struct _family_tree 
{
 struct _family_tree	*Next;
 char			*name;
 font_list		*list;	
 boolean		can_normal;
 boolean		can_bold;
 boolean		can_italic;
 boolean		can_bold_italic;
} family_tree;

typedef struct _font_alias 
{
 struct _font_alias	*next;
 char			*alias_name;
} font_alias;

typedef struct _garbage_encode
{
 struct _garbage_encode *next;
 char			*encode_name;
} garbage_encode;

/*--- Define the VectorFont Class ---*/
struct sc_VectorFont 
{
 c_VectorFont *previous;
 c_VectorFont *next;
 c_VectorFont **base;

 sf_VectorFont *f;

 enum fonttype_e type;
 char *filename;
 char *metrics_filename;

 boolean resident_flag;
 long id;

 /*___Global Font Information___*/
 char *name;
 char *font_name;
 char *full_name;
 char *family_name;
 char *weight;
 char *version;
 char *notice;
 SHORT mapping_scheme;
 char_t esc_char;
 char *character_set;
 unsigned long characters;
 boolean is_base_font;
 font_encoding_t encoding;

 /*___Character set metrics___*/
 c_CharMetrics *metrics;
 c_Hook *hook;

 /*___Special flags___*/
 boolean download_flag;
 boolean lock_flag;
 boolean hide_flag;

 /*___Family info___*/
 family_tree	*family;
 family_tree	*spec_family;
 font_list	*specific;
  
 /*___Font aliases___*/
 font_alias	*aliases;
 
 /*___garbage encode name malloc___*/
 garbage_encode *garbage;
};


extern sf_VectorFont fc_VectorFont;

extern boolean font_family_filter ___PROTO((family_tree *family));

#endif /* !__VECTORFONT_H__ */



