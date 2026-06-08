/*
** DocumentP.h for Xclamation in Document/
** Private header for Document class
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
** Started on  Tue Apr 15 11:56:31 1997 Emmanuel Paris
** Last update Tue Apr 22 12:05:49 1997 Emmanuel Paris
*/

#ifndef __XC_DOCUMENT_P_H__
#define __XC_DOCUMENT_P_H__

#include "Document.h"

extern c_Color *GlobColorBase;
extern c_VectorFont *GlobFontBase;
extern c_TextStyle *GlobTextStyleBase;
extern c_TextRuler *GlobTextRulerBase;

/*--- Read & write methods (docfile.c) ---*/
boolean readDocument ___PROTO((c_DocFile *doc, char *keyword,
			       long param, char *filename));
boolean writeDocument ___PROTO((c_Document *this, c_DocFile *doc));

/*--- Printing methods (print.c) ---*/
boolean ps_printDocument ___PROTO((c_Document *This, c_PostScript *post));


#endif /* !__XC_DOCUMENT_P_H__ */
