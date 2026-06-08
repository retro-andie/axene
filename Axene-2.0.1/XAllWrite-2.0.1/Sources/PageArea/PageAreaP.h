/*
** PageAreaP.h for XAllWrite in PageArea/
** Private Header for PageArea object
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
** Started on  Sat Mar 22 15:11:09 1997 Emmanuel Paris
** Last update Fri Nov 14 17:45:50 1997 Emmanuel Paris
*/

#ifndef _PageArea_h_
#define _PageArea_h_

#include "PageArea.h"

#define XcDF_PAGEAREA_KEYWORD "PAGELIST"
#define XcDF_DISPLAY_TYPO_MARKS_KEYWORD "DISPLAY_TYPO_MARKS"

extern void changeScaling_PageArea ___PROTO((c_PageArea	*This, scale_t scale,
					     int x, int y, c_Page *pgl));
extern void calculAreaSize_PageArea ___PROTO((c_PageArea *This));
extern c_Page *GetActivePage_PageArea ___PROTO((c_PageArea *This));

/*--- read and write function (defined in docfile.c) ---*/
extern boolean readPageArea ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_PageArea *This));
extern boolean writePageArea ___PROTO((c_PageArea *This, c_DocFile *doc));

/*--- print function (defined in print.c) ---*/
extern boolean ps_printPageArea ___PROTO((c_PageArea *this, 
					  c_PostScript *post));

#endif /* _PageArea_h_ */



