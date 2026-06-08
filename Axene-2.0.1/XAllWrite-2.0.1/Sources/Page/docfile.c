/*
** docfile.c for XAllWrite in Page/
** load and save functions for Page object
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
** Started on  Tue Mar 18 17:17:50 1997 Emmanuel Paris
** Last update Mon Sep  7 14:55:43 1998 Emmanuel Paris
*/

#include "Page.h"

#define XcDF_MARGINS_KEYWORD "MARGINS"

/*--- load function callbacks ---*/
static boolean cb_format ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Page *page));
static boolean cb_type ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Page *page));
static boolean cb_orientation ___PROTO((c_DocFile *doc, char *keyword,
					long param, c_Page *page));
static boolean cb_margins ___PROTO((c_DocFile *doc, char *keyword,
				    long param, c_Page *page));

/* ----------------------------------------------------------------- ** 
** readPage - Page loading                                           ** 
** ----------------------------------------------------------------- */
boolean readPage(doc, keyword, page_number, pagelist)
c_DocFile       *doc;
char            *keyword;
long            page_number;
c_Page	        **pagelist;
{
 c_Page		*page;
 int		i;
 Xc_HISTORY(("read page: %d", page_number));

 page = *pagelist;
 if (page->page_number != (unsigned int)page_number)
 {
  page = (c_Page *)NEW(c_Page)(pagelist, page->BaseStd, page->Calibration,
			       page->Redraw, page->select_type);
 }
 
 do
 {
  if (page->page_number != (unsigned int)page_number) break;

  /*--- Page formats ---*/
  for(i = Xc_Count_Page_Format; i-- > 0; )
   if(!F(doc).addCallbacks(doc, Xc_Table_Page_Format[i].keyword,
			   cb_format, page, NULL)) return FALSE;
  
  if(!F(doc).addCallbacks(doc,
			  /*___Page type___*/
			  XcDF_LEFT_KEYWORD, cb_type, page,
			  XcDF_RIGHT_KEYWORD, cb_type, page,
			  XcDF_DOUBLE_KEYWORD, cb_type, page,
			  /*___Page orientation___*/
			  XcDF_PORTRAIT_KEYWORD, cb_orientation, page,
			  XcDF_LANDSCAPE_KEYWORD, cb_orientation, page,
			  /*___Page margins___*/
			  XcDF_MARGINS_KEYWORD, cb_margins, page,
			  /*___FramaManager___*/
			  XcDF_FRAMELIST_KEYWORD, FUNCTION(c_FrMg, read), page,
			  NULL))
   break;
  
  if(F(doc).expectKeyword(doc, NULL, TRUE) == FALSE) break;
  
  return TRUE;
 }
 while(1);
 Xc_TRACE(("loading error"));
 DELETE(c_Page)(page);
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** writePage - Page saving                                           ** 
** ----------------------------------------------------------------- */
boolean writePage(this, doc)
c_Page *this;
c_DocFile *doc;
{
 Xc_HISTORY(("write(%d)", this->page_number));

 if(!F(doc).writeKeyword(doc, XcDF_PAGE_KEYWORD, this->page_number) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeKeyword(doc, this->page_format->keyword,
			 this->page_format->param)) return FALSE;
  
 /*--- Write page dimension if custom format ---*/
 Xc_TRACE(("page dimension"));
 if (this->page_format == &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT])
 {
  if (this->page_orient == Xc_LANDSCAPE)
  {
   if (!F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sHeight)) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sWidth)) ||
       !F(doc).endSequence(doc)) return FALSE;
  }
  else
  {
   if (!F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sWidth)) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sHeight)) ||
       !F(doc).endSequence(doc)) return FALSE;
  }
 }

 /*--- Write page type ---*/
 Xc_TRACE(("page type"));
 if ((this->page_type & (~Xc_PAGE_DOUBLE)) == Xc_PAGE_LEFT)
 {
  if(!F(doc).writeKeyword(doc, XcDF_LEFT_KEYWORD, -1L))
   return FALSE;
 }
 else
 {
  if(!F(doc).writeKeyword(doc, XcDF_RIGHT_KEYWORD, -1L))
   return FALSE;
 }
 
 if ((this->page_type & Xc_PAGE_DOUBLE) == Xc_PAGE_DOUBLE)
 {
  if(!F(doc).writeKeyword(doc, XcDF_DOUBLE_KEYWORD, -1L))
   return FALSE;
 }
  
 /*--- Write page orientation ---*/
 Xc_TRACE(("orientation"));
 if(!F(doc).writeKeyword(doc, (this->page_orient != Xc_LANDSCAPE)?
			 XcDF_PORTRAIT_KEYWORD : XcDF_LANDSCAPE_KEYWORD,
			 -1L)) return FALSE;
  
 /*--- Write page margins ---*/
 Xc_TRACE(("page margins"));
 if(!F(doc).writeKeyword(doc, XcDF_MARGINS_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->left_margin)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->right_margin)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->top_margin)) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->bottom_margin)) ||
    !F(doc).endSequence(doc)) return FALSE;

 Xc_TRACE(("done"));
 
 return (F(this->frmg).write(this->frmg, doc) &&
	 F(doc).endSequence(doc));
}

/* ----------------------------------------------------------------- ** 
** cb_format - Loading callback for page format                      ** 
** ----------------------------------------------------------------- */
static boolean cb_format(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
 int i;

 Xc_TRACE(("cb_format(`%s')", keyword));

 for(i = 0; i < Xc_Count_Page_Format; i++)
  if((param == Xc_Table_Page_Format[i].param) &&
     !strcmp(Xc_Table_Page_Format[i].keyword, keyword))
  {
   page->page_format = &Xc_Table_Page_Format[i];
   if(i == Xc_CUSTOM_FORMAT)
   {
    real w, h;

    if(!F(doc).readReal(doc, &w)|| !F(doc).readReal(doc, &h))
     return FALSE;
    page->sWidth = SCALE_FROM_POINTS(w);
    page->sHeight = SCALE_FROM_POINTS(h);
    Xc_TRACE(("custom format: %gx%g",
	      SCALE_TO_CENTIMETERS(w), SCALE_TO_CENTIMETERS(h)));
   }
   else
   {
    Xc_TRACE(("format: `%s'", Xc_Table_Page_Format[i].name));
   }

   return TRUE;
  }
 Xc_BREAK(("Format not found"));
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_type - Loading callback for page type                          ** 
** ----------------------------------------------------------------- */
static boolean cb_type(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
  Xc_TRACE(("cb_type(`%s')", keyword));

  if(!strcmp(keyword, XcDF_DOUBLE_KEYWORD))
   page->page_type |= Xc_PAGE_DOUBLE;
  else if(!strcmp(keyword, XcDF_RIGHT_KEYWORD))
   page->page_type |= Xc_PAGE_RIGHT;
  else
   page->page_type &= ~Xc_PAGE_RIGHT;
    
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_orientation - Loading callback for orientation                 ** 
** ----------------------------------------------------------------- */
static boolean cb_orientation(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
  Xc_TRACE(("cb_orientation(`%s')", keyword));

  page->page_orient = (strcmp(keyword, XcDF_LANDSCAPE_KEYWORD)?
		       Xc_PORTRAIT : Xc_LANDSCAPE);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_margins - Loading callbcak for page margins                    ** 
** ----------------------------------------------------------------- */
static boolean cb_margins(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
  real left, right, top, bottom;

  Xc_TRACE(("cb_margins"));

  if(!F(doc).readReal(doc, &left)|| !F(doc).readReal(doc, &right) ||
     !F(doc).readReal(doc, &top)|| !F(doc).readReal(doc, &bottom))
    return FALSE;

  page->left_margin = SCALE_FROM_POINTS(left);
  page->right_margin = SCALE_FROM_POINTS(right);
  page->top_margin = SCALE_FROM_POINTS(top);
  page->bottom_margin = SCALE_FROM_POINTS(bottom);

  return TRUE;
}

