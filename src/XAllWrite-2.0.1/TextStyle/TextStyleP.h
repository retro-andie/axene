/*
** TextStyleP.h for Xclamation, XQuad and XAllWrite in TextStyle/
** loading callbacks definitions for TextStyle class
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
** Started on  Sun Jun 12 02:09:46 1994 Stéphane Boisson
** Last update Fri Apr 10 18:35:16 1998 Emmanuel Paris
*/

#ifndef _TextStyleP_h_
#define _TextStyleP_h_

#define XcDF_STYLE_KEYWORD "STYLE"
#define XcDF_STYLELIST_KEYWORD "STYLELIST"
#define XcDF_BGCOLOR_KEYWORD "BGCOLOR"
#define XcDF_ITALIC_KEYWORD "ITALIC"
#define XcDF_UNDERLINE_KEYWORD "UNDERLINE"
#define XcDF_SUBSCRIPT_KEYWORD "SUBSCRIPT"
#define XcDF_SUPSCRIPT_KEYWORD "SUPERSCRIPT"
#define XcDF_OUTLINE_KEYWORD "OUTLINE"
#define XcDF_STRIKEOUT_KEYWORD "STRIKEOUT"
#define XcDF_WIDTH_KEYWORD "WIDTH"
#define XcDF_SHADOW_KEYWORD "SHADOW"
#define XcDF_SMALL_CAPS_KEYWORD "SMALLCAPS"
#define XcDF_BIG_CAPS_KEYWORD "BIGCAPS"
#define XcDF_INHERIT_KEYWORD "INHERIT"
#ifndef XcDF_HIDE_KEYWORD
#define XcDF_HIDE_KEYWORD "HIDE"
#endif

typedef struct
{
 c_TextStyle *style;
 c_TextStyle **style_base;
 c_Color *color_base;
 c_VectorFont *font_base;
} textstyle_read_t;

extern boolean readStyle ___PROTO((c_DocFile *doc, char *keyword,
                                   long param, textstyle_read_t *data));
extern boolean writeStyle ___PROTO((c_TextStyle *this, c_DocFile *doc));
extern boolean writeStyleList ___PROTO((c_TextStyle *this, c_DocFile *doc));

#endif /* !__XC_TEXT_STYLE_H__ */

