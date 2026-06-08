/*
** TextRulerP.h for XAllWrite in TextRuler/
** Text ruler private header
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
** Started on  Mon Sep  1 17:00:07 1997 Emmanuel Paris
** Last update Thu Aug 13 17:25:03 1998 Emmanuel Paris
*/

#ifndef _TextRulerP_h_
#define _TextRulerP_h_

#define XcDF_RULER_KEYWORD "RULER"
#define XcDF_RULERLIST_KEYWORD "RULERLIST"
#define XcDF_LEFT_KEYWORD "LEFT"
#define XcDF_RIGHT_KEYWORD "RIGHT"
#define XcDF_CENTER_KEYWORD "CENTER"
#define XcDF_CPERIOD_KEYWORD "CENTER_PERIOD"
#define XcDF_JUSTIFIED_KEYWORD "JUSTIFIED"
#define XcDF_TABLIST_KEYWORD "TABLIST"
#define XcDF_TAB_KEYWORD "TAB"
#ifndef XcDF_HIDE_KEYWORD
#define XcDF_HIDE_KEYWORD "HIDE"
#endif
#define XcDF_TAB_FILLER_KEYWORD "FILLER"
#define XcDF_TABF_POINT_KEYWORD "POINT"
#define XcDF_TABF_DOT_KEYWORD "DOT"
#define XcDF_TABF_HYPHEN_KEYWORD "HYPHEN"
#define XcDF_TABF_USER_KEYWORD "USER"

extern boolean readRuler ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_TextRuler **base));
extern boolean readRulerList ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_TextRuler **base));
extern boolean writeRuler ___PROTO((c_TextRuler *this, c_DocFile *doc));
extern boolean writeRulerList ___PROTO((c_TextRuler *this, c_DocFile *doc));

#endif /* !__XC_TEXT_RULER_H__ */
