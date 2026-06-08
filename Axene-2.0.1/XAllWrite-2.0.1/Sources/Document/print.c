/*
** print.c for XAllWrite in Document/
** Printing methods
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
** Last update Fri Nov 14 17:46:38 1997 Emmanuel Paris
*/

#include "DocumentP.h"

/* ----------------------------------------------------------------- ** 
** printDocument - Print object                                      ** 
** ----------------------------------------------------------------- */
boolean printDocument(this, post)
c_Document	*this;
c_PostScript	*post;
{
 c_TextStyle *style_base;
 c_VectorFont *font;
 long	      id;
 
 Xc_HISTORY(("ps_print"));

 /*--- Reencode fonts ---*/
 style_base = this->BaseStd.text_style_base;
 for(id = 1, font = GlobFontBase; font != NULL; font = font->next)
 {
  font->id = -1;
  if(font->resident_flag != FALSE)
  {
   c_TextStyle *style;
   c_Hook *hook;
   
   Xc_ASSERT(font->hook != NULL);

   hook = font->hook;
   /*--- Check in TextStyle ---*/
   for(style = style_base; style; style = style->next)
    if(F(hook).getUsage(hook, style) > 0 && style->used_count > 0)
    {
     font->id = id++;
     if(!F(font).ps_print(font, post)) return FALSE;
     break;
    }
  }
 }
 
 return F(this->PageArea).ps_print(this->PageArea, post);
}






