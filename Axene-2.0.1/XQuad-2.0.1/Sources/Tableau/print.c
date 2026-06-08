/*
** print.c for XQuad in Tableau/
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
** Started on  Fri Jun  2 14:45:58 1995 Stéphane Boisson
** Last update Mon Mar 23 14:56:15 1998 Emmanuel Paris
*/

#include "TableauP.h"

/* ----------------------------------------------------------------- ** 
** printTableau - Print object                                       ** 
** ----------------------------------------------------------------- */
boolean printTableau(this, post)
c_Tableau	*this;
c_PostScript	*post;
{
 c_TextStyle *style_base;
 c_TextStyle *style_base2;
 c_VectorFont *font;
 long id;

 Xc_HISTORY(("print"));

 /*--- Reencode fonts ---*/
 style_base = this->BaseStd.text_style_base;
 style_base2 = this->BaseStd.BG_text_style_base;
 for(id = 1, font = GlobFontBase; font != NULL; font = font->next)
 {
  font->id = -1;
  if(font->resident_flag != FALSE)
  {
   c_TextStyle *style;
   c_Hook *hook;
      
   Xc_ASSERT(font->hook != NULL);
      
   Xc_TRACE(("resident font `%s' #%ld",
	     font->font_name, (long)XcH_USAGE(font)));
      
   hook = font->hook;
   for(style = style_base; style; style = style->next)
    if((XcH_USAGE(style) > 0) && (F(hook).getUsage(hook, style) > 0))
    {
     font->id = id++;
     if(!F(font).ps_print(font, post)) return FALSE;
     break;
    }
   if (font->id == -1)
   {
    for(style = style_base2; style; style = style->next)
     if((XcH_USAGE(style) > 0) && style->font == font)
     {
      font->id = id++;
      if(!F(font).ps_print(font, post)) return FALSE;
      break;
     }
   }
  }
 }
  
 /*--- Title font encoding ---*/
 if(this->Feuille->print_setup->rowcol_f)
 {
  font = this->Feuille->orig_style->font;
  if(font->id == -1)
  {
   font->id = id;
   if(!F(font).ps_print(font, post)) return FALSE;
  }
 }

 return F(this->Feuille).ps_print(this->Feuille, post);
}






