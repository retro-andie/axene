/*
** print.c for XAllWrite in FrameManager/
** Print methods for FrameManger class
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
** Started on  Mon Jul 24 14:24:45 1995 Stéphane Boisson
** Last update Tue Aug  4 18:30:18 1998 Emmanuel Paris
*/

#include "FrameManagerP.h"

/* ----------------------------------------------------------------- ** 
** printFrameList - Print several Frames                             ** 
** ----------------------------------------------------------------- */
boolean printFrameList(this, post, over_clip)
c_FrMg *this;
c_PostScript *post;
boolean *over_clip;
{
 l_Cadre *ptr;
 l_Cadre *list;

 Xc_HISTORY(("print"));

 *over_clip = FALSE;
 
list = sort_asc_list_cadre(this->ListCadre);
 if(list == NULL) return TRUE;
 
 if(!F(post).pushState(post)) return FALSE;
 
 for(ptr = list ; ptr != NULL; ptr = ptr->NextCadre)
 {
  if ((ptr->cadre->textflow_mode & XcTFM_HOLE_MASK) == XcTFM_OVER)
   *over_clip = TRUE;
  
  if(!F(ptr->cadre).ps_print(ptr->cadre, post)) 
   return FALSE;
 }
 free_list_cadre(list, NO_DEL_CADRE);
 
 return (F(post).popState(post));
}

/* ----------------------------------------------------------------- ** 
** printFrameOver - Print callback over Frames                       ** 
** ----------------------------------------------------------------- */
boolean printFrameOver(this, post, callback, cb_data)
c_FrMg *this;
c_PostScript *post;
void (*callback)();
void *cb_data;
{
 l_Cadre *list, *ptr, *highlist, *lowlist, *tmplist;
 c_Cadre *cadre;
 boolean rect_case = FALSE;

 list = ptr = sort_asc_list_cadre(this->ListCadre);

 while(ptr)
 {
  cadre = ptr->cadre;
  if ((cadre->textflow_mode & XcTFM_HOLE_MASK) == XcTFM_OVER)
  {
   list_cadre_low_high_priority(this, ptr, &lowlist, &highlist, INTERS_CADRE);

   if(!F(post).pushState(post) || !F(post).putLine(post, "q", NULL))
    return FALSE;
   
   if (is_frame_a_rectangle(cadre->ListPoint))
   {
    rect_case = TRUE;
    
    if (!F(post->matrix).translate(post->matrix, TRUE,
				   this->sOrig_X, this->sOrig_Y))
     return FALSE;
    if (!F(post).putLine(post, "$p $v re",
			 cadre->sOrig_X - cadre->thickness / 2, 
			 cadre->sOrig_Y - cadre->thickness / 2,
			 cadre->sWidth + cadre->thickness, 
			 cadre->sHeight + cadre->thickness))
     return FALSE;
    F(post->matrix).pop(post->matrix);
   }
   else
    if (!F(cadre).ps_print_path(cadre, post)) return FALSE;
   
   if (!F(post).putLine(post, "W* n", NULL)) return FALSE;
   
   tmplist = highlist;
   while(tmplist)
   {
    if (!F(post->matrix).translate(post->matrix, TRUE,
				   this->sOrig_X, this->sOrig_Y))
     return FALSE;

    if (rect_case)
    {
     if (!F(post).putLine(post, "$p $v re",
			  cadre->sOrig_X - cadre->thickness / 2, 
			  cadre->sOrig_Y - cadre->thickness / 2,
			  cadre->sWidth + cadre->thickness, 
			  cadre->sHeight + cadre->thickness))
      return FALSE;
    }
    else
    {
     if (!F(post).putLine(post, "$p $v re",
			  cadre->sOrig_X, cadre->sOrig_Y,
			  cadre->sWidth, cadre->sHeight))
      return FALSE;
    }
    F(post->matrix).pop(post->matrix);

    if (!F(tmplist->cadre).ps_print_path(tmplist->cadre, post)) 
     return FALSE;
    
    if (!F(post).putLine(post, "W* n", NULL)) return FALSE;

    tmplist = tmplist->NextCadre;
   }

   free_list_cadre(lowlist, NO_DEL_CADRE);
   free_list_cadre(highlist, NO_DEL_CADRE);

   callback(cb_data, post);
   
   if (!F(post).putLine(post, "Q", NULL) || !F(post).popState(post))
    return FALSE;
  }
  ptr = ptr->NextCadre;
 }
 
 free_list_cadre(list, NO_DEL_CADRE);
 
 return TRUE;
}
