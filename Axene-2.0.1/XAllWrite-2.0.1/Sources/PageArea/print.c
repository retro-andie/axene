/*
** print.c for XAllWrite in PageArea/
** Print function for PageArea
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
** Started on  Fri Nov 14 17:26:39 1997 Emmanuel Paris
** Last update Mon Sep  7 14:55:01 1998 Emmanuel Paris
*/

#include "PageAreaP.h"

/* ----------------------------------------------------------------- ** 
** printPageArea - launch page print                                 ** 
** ----------------------------------------------------------------- */
boolean ps_printPageArea(This, post)
c_PageArea *This;
c_PostScript *post;
{
 c_Page *page;
 print_param_t	*pp;

 pp = post->print_param;
 page = This->PageList;

 while(page && (int)page->page_number < pp->StartPage)
  page = page->NextPage;
 
 if (!page) return FALSE;
 
 while(page && (int)page->page_number <= pp->EndPage)
 {
  if (!pp->Left && (page->page_type & (~Xc_PAGE_DOUBLE)) == Xc_PAGE_LEFT)
  {
   page = page->NextPage;
   continue;
  }

  if (!pp->Right && (page->page_type & (~Xc_PAGE_DOUBLE)) == Xc_PAGE_RIGHT)
  {
   page = page->NextPage;
   continue;
  }
  
  if (!F(page).ps_print(page, post))
   return FALSE;

  page = page->NextPage;
 }

 return TRUE;
}
