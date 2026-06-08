/*
** TableauP.h for XQuad in Tableau/
** Private header for Tableau class
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
** Started on  Wed Apr  5 18:56:57 1995 Stéphane Boisson
** Last update Wed Aug 16 12:26:26 1995 Stéphane Boisson
*/

#ifndef __XC_TABLEAU_P_H__
#define __XC_TABLEAU_P_H__

#include "Tableau.h"

extern c_Color *GlobColorBase;
extern c_VectorFont *GlobFontBase;
extern c_FormatNbr *GlobFormatBase;
extern c_TextStyle *GlobTextStyleBase;

/*--- Read & write methods (docfile.c) ---*/
boolean readTableau ___PROTO((c_DocFile *doc, char *keyword,
			      long param, void *data));
boolean writeTableau ___PROTO((c_Tableau *this, c_DocFile *doc));

/*--- Printing methods (print.c) ---*/
boolean printTableau ___PROTO((c_Tableau *This, c_PostScript *post));


#endif /* !__XC_TABLEAU_P_H__ */
