/*
** DocumentP.h for XAllWrite in Document/
** Private header for Document class
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
** Last update Thu Nov  6 19:27:24 1997 Emmanuel Paris
*/

#ifndef __XC_DOCUMENT_P_H__
#define __XC_DOCUMENT_P_H__

#include "Document.h"

#define XcDF_DOCUMENT_KEYWORD "DOCUMENT"
#define XcDF_GEOMETRY_KEYWORD "GEOMETRY"


extern c_VectorFont *GlobFontBase;

/*--- Read & write methods (docfile.c) ---*/
boolean readDocument ___PROTO((c_DocFile *doc, char *keyword,
			      long param, void *data));
boolean writeDocument ___PROTO((c_Document *this, c_DocFile *doc));

/*--- Printing methods (print.c) ---*/
boolean printDocument ___PROTO((c_Document *This, c_PostScript *post));


#endif /* !__XC_DOCUMENT_P_H__ */
