/*
** Context_Menu_Page.c for Xclamation in Page/
** Context Menu For Page
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
** Started on  Tue Mar 21 17:35:00 1995 Emmanuel Paris
** Last update Sat Nov 14 19:38:56 1998 Emmanuel Paris
*/

#include "ContextMenu.h"
#include "Advanced_Editor.h"
#include "Select_Move_Resize_Cadres.h"
#include "TextFrame.h"

extern void LoadImage();
extern void LoadText();
extern void LoadVector();
extern void EditText();

boolean context_menu_page_look();

boolean context_menu_page_look(cpage, event)
c_Page	*cpage;
XEvent	*event;
{
 Display	*display;
 l_Cadre	*lcadre;
 l_Cadre	*selected_cadre;
 int		ret_button;
 int		xm;
 int		ym;
 c_ContextMenu *context;
 void		*object;
 boolean	button = TRUE;
 boolean	retour = FALSE;

 display = cpage->X_info.display;
 xm = event->xbutton.x;
 ym = event->xbutton.y;

 lcadre = (l_Cadre *)F(cpage).get_cadre_by_xy(cpage, xm, ym);
 
 Xc_TRACE(("lcadre: %p xm: %d ym: %d", lcadre, xm, ym));

 if (!lcadre)
 {
  context = (c_ContextMenu *)
   NEW(c_ContextMenu)(display, CM_PAGE_EMPTY, &ret_button, 
		      FALSE, event);
      
  if (context)
  {
   F(context).waitForUnmap(context);
   DELETE(c_ContextMenu)(context);
	  
   switch(ret_button)
   {
   case CM_Pge_CREATE_SQUARE:
    F(cpage).set_function(cpage, CREATE_CADRE_RECTANGLE); 
    break;
   case CM_Pge_CREATE_ELIPSE:
    F(cpage).set_function(cpage, CREATE_CADRE_ELIPSE); 
    break;
   case  CM_Pge_CREATE_POLYGON:
    F(cpage).set_function(cpage, CREATE_CADRE_POLYGONAL); 
    break;
   default:
    break;
   }
  }
  Xc_TRACE(("context menu for no frame"));
  retour = TRUE;
 }
 else
 {
  switch(lcadre->cadre->object_type)
  {
  case CADRE_EMPTY:
  case CADRE_IMAGE_BITMAP:
  case CADRE_VECTOR:

   selected_cadre = cpage->SelectedCadre;
   while(selected_cadre && selected_cadre->cadre != lcadre->cadre)
    selected_cadre = selected_cadre->NextCadre;

   if (!selected_cadre)
   {
    selected_cadre = 
     (l_Cadre *)duplicate_list_cadre(cpage->SelectedCadre);
    
    F(cpage).deselect_all_cadre(cpage, TRUE);
    F(cpage).select_cadre(cpage, lcadre, TRUE);
   }
   else
    selected_cadre = NULL;

   context = (c_ContextMenu *)
    NEW(c_ContextMenu)(display, CM_CADRE_EMPTY, &ret_button, 
		       FALSE, event);
	  
   if (context)
   {
    F(context).waitForUnmap(context);
    DELETE(c_ContextMenu)(context);
	      
    object = lcadre->cadre->object;
    Xc_TRACE(("ret button : %d", ret_button));
    switch(ret_button)
    {
    case CM_Cde_IMPORT_TEXT:
     LoadText((Widget)NULL);
     break;
    case CM_Cde_IMPORT_IMAGE:
     LoadImage((Widget)NULL);
     break;
    case CM_Cde_IMPORT_VECTOR:
     LoadVector((Widget)NULL);
     break;
    case CM_Cde_EDIT_TEXTE:
     EditText(NULL, display);
     break;
    default:
     button = FALSE;
     break;
    }
    if (button && object == lcadre->cadre->object)
     button = FALSE;
   }
   else
    button = FALSE;
	  
   if (!button && selected_cadre)
   {
    F(cpage).deselect_all_cadre(cpage,TRUE);
    lcadre = selected_cadre;
    
    while(lcadre)
    {
     F(cpage).select_cadre(cpage, lcadre, TRUE);
     lcadre = lcadre->NextCadre;
    }
   } 
   if (selected_cadre)
    free_list_cadre(selected_cadre, NO_DEL_CADRE);
   Xc_TRACE(("context menu for frame empty, vertor, text or bitmap"));
   break;
  case CADRE_TEXT:
   selected_cadre = 
    (l_Cadre *)duplicate_list_cadre(cpage->SelectedCadre);
	  
   F(cpage).deselect_all_cadre(cpage, TRUE);
   F(cpage).select_cadre(cpage, lcadre, TRUE);

   context = (c_ContextMenu *)
    NEW(c_ContextMenu)(display, CM_CADRE_TEXT, &ret_button, 
		       FALSE, event);
	  
   if (context)
   {
    F(context).waitForUnmap(context);
    DELETE(c_ContextMenu)(context);
	      
    object = lcadre->cadre->object;
    Xc_TRACE(("ret button : %d", ret_button));
    switch(ret_button)
    {
    case CM_Cdt_EDIT_TEXTE:
     EditText(NULL, display);
     break;
    default:
     button = FALSE;
     break;
    }
    if (button && object == lcadre->cadre->object)
     button = FALSE;
   }
   else
    button = FALSE;
	  
   if (!button)
   {
    F(cpage).deselect_all_cadre(cpage,TRUE);
    lcadre = selected_cadre;
    while(lcadre)
    {
     F(cpage).select_cadre(cpage,lcadre,TRUE);
     lcadre = lcadre->NextCadre;
    }
   }
   free_list_cadre(selected_cadre, NO_DEL_CADRE);
   Xc_TRACE(("context menu for frame text"));
   break;
  }
 }
 return retour;
}






