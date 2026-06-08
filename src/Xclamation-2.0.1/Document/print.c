/*
** print.c for Xclamation in Document/
** Document print function
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
** Started on  Tue Apr 22 11:59:30 1997 Emmanuel Paris
** Last update Fri Oct  2 15:32:22 1998 Robin Castanier
*/

#include "VectorGraph.h"
#include "DocumentP.h"

boolean ps_printDocument ___PROTO((c_Document *This, c_PostScript *post));

/* ----------------------------------------------------------------- ** 
** generate a ps file                                                ** 
** ----------------------------------------------------------------- */
boolean ps_printDocument(this, post)
c_Document	*this;
c_PostScript	*post;
{
 c_TextStyle *style_base;
 c_VectorFont *font;
 listpage *tmppage;
 long id;
 print_param_t	*pp;
 c_Page	*page;
 boolean	allow;
  
 Xc_HISTORY(("ps_print"));

 /*--- Reencode fonts ---*/
 style_base = this->BaseStd.text_style_base;
 for(id = 1, font = GlobFontBase; font != NULL; font = font->next)
 {
  font->id = -1;
  if(font->resident_flag != FALSE)
  {
   c_TextStyle *style;
   c_VectorGraph *vector;
   c_Hook *hook;
	  
   Xc_ASSERT(font->hook != NULL);

   hook = font->hook;

   /*--- Check in TextStyle ---*/
   for(style = style_base; style; style = style->next)
    if((XcH_USAGE(style) > 0) && (F(hook).getUsage(hook, style) > 0))
    {
     font->id = id++;
     if(!F(font).ps_print(font, post)) return FALSE;
     break;
    }

   if(font->id == -1)
   {
    /*--- Check in VectorGraph ---*/
    vector = this->BaseStd.vector_base;
    for(; F(hook).getUsage(hook, vector) > 0 ; vector = vector->next)
    {
     font->id = id++;
     if(!F(font).ps_print(font, post)) return FALSE;
     break;
    }
   }
  }
 }
  
 pp = post->print_param;
 for(tmppage = this->ListPage; tmppage != NULL; tmppage = tmppage->NextPage)
 {
  page = tmppage->Page;
  if ((int)page->page_number >= pp->StartPage &&
      (int)page->page_number <= pp->EndPage)
  {
   allow = FALSE;
   switch(page->page_type)
   {
   case Xc_PAGE_LEFT:
    if (pp->Left)
     allow = TRUE;
    break;
   case Xc_PAGE_RIGHT:
    if (pp->Right)
     allow = TRUE;
    break;
   case Xc_PAGE_DOUBLE:
    allow = TRUE;
    break;
   }
   if(allow && !F(page).ps_print(page, post)) return FALSE;
  }
 }
 return TRUE;
}
