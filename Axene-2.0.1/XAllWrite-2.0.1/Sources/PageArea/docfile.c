/*
** docfile.c for XAllWrite in PageArea/
** PageArea read and write functions
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
** Started on  Thu Nov  6 16:58:16 1997 Emmanuel Paris
** Last update Sat Nov  8 17:45:49 1997 Emmanuel Paris
*/

#include "PageAreaP.h"

/*--- reading callback ---*/
static boolean cb_typo_mark ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_PageArea *pagearea));

/* ----------------------------------------------------------------- ** 
** readPageArea : PageArea reading                                   ** 
** ----------------------------------------------------------------- */
boolean readPageArea(doc, keyword, param, This)
c_DocFile *doc;
char *keyword;
long param;
c_PageArea *This;
{
 long ax, ay, vp;
 real factor;

 if (!F(doc).readInteger(doc, &ax) || !F(doc).readInteger(doc, &ay) ||
     !F(doc).readReal(doc, &factor) || !F(doc).readInteger(doc, &vp))
  return FALSE;
 
 This->AreaOrig_X = (int)ax;
 This->AreaOrig_Y = (int)ay;
 
 This->view_page = (view_page_t)vp;
 This->sFactor = SCALE_FROM_COEF(factor);

 if (!F(doc).addCallbacks(doc, 
			  XcDF_DISPLAY_TYPO_MARKS_KEYWORD, cb_typo_mark, This,
			  XcDF_PAGE_KEYWORD, 
			  FUNCTION(c_Page, read), &This->PageList, NULL))
  return FALSE;
 return F(doc).expectKeyword(doc, NULL, TRUE);
}

/* ----------------------------------------------------------------- ** 
** writePageArea : PageArea writing                                  ** 
** ----------------------------------------------------------------- */
boolean writePageArea(This, doc)
c_PageArea *This;
c_DocFile *doc;
{
 c_Page *page;

 if (!F(doc).writeKeyword(doc, XcDF_PAGEAREA_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeInteger(doc, (long)This->AreaOrig_X) ||
     !F(doc).writeInteger(doc, (long)This->AreaOrig_Y) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(This->sFactor)) ||
     !F(doc).writeInteger(doc, (long)This->view_page) ||
     (This->Redraw->spec_chars && 
      !F(doc).writeKeyword(doc, XcDF_DISPLAY_TYPO_MARKS_KEYWORD, -1L)))
  return FALSE;
 
 page = This->PageList;
 while(page)
 {
  F(page).write(page, doc);
  page = page->NextPage;
 }

 if (!F(doc).endSequence(doc)) return FALSE;
 
 return TRUE;
}

static boolean cb_typo_mark(doc, keyword, param, pagearea)
c_DocFile *doc;
char *keyword;
long param;
c_PageArea *pagearea;
{
 Xc_TRACE(("cb_typo_mark"));
 
 pagearea->Redraw->spec_chars = TRUE;
 return TRUE;
}

