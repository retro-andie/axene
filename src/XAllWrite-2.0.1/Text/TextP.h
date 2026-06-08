/*
** TextP.h for XAllWrite in Text/
** Text read/write/print private header
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Tue Nov  4 18:16:18 1997 Emmanuel Paris
** Last update Mon Jul 20 15:30:25 1998 Emmanuel Paris
*/

#ifndef _TextP_h_
#define _TextP_h_

#include "Text.h"

#define XcDF_TEXT_KEYWORD "TEXT"
#define XcDF_TEXTLIST_KEYWORD "TEXTLIST"

#define XcDF_TT_BODY_KEYWORD		"BODY"
#define XcDF_TT_CLIPBOARD_KEYWORD	"CLIPBOARD"
#define XcDF_TT_HEADER_KEYWORD		"HEADER"
#define XcDF_TT_FOOTER_KEYWORD		"FOOTER"
#define XcDF_TT_FOOTNOTES_KEYWORD	"FOOTNOTES"

#define XcDF_XA_BEGIN_KEYWORD "BEGIN"
#define XcDF_XA_END_KEYWORD "END"
#define XcDF_XA_TEXT_KEYWORD "TXT"
#define XcDF_XA_LINEFEED_KEYWORD "RC"
#define XcDF_XA_TAB_KEYWORD "TAB"
#define XcDF_XA_PAGEBRK_KEYWORD "PB"

#define XcDF_SELECTION_KEYWORD "SELECTION"

/*--- Internal structures ---*/
typedef struct
{
 c_Text *text;
 BaseStd_t   *base;
 c_TextStyle *style;
 c_TextRuler *ruler;
 xa_item_t   *item;
 int	     sstart, send;
} text_read_t;

extern boolean readText ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
extern boolean writeText ___PROTO((c_Text *this, c_DocFile *doc));
extern boolean readTextList ___PROTO((c_DocFile *doc, char *keyword,
				  long param, BaseStd_t *base));
extern boolean writeTextList ___PROTO((c_Text *this, c_DocFile *doc));

#endif /* _TextP_h_ */

