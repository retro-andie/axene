/*
** Context_Menu_Page.c for XAllWrite in Page/
** Context Menu For Page
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
** Started on  Sat Nov 14 16:02:55 1998 Emmanuel Paris
** Last update Sun Nov 15 15:42:31 1998 Emmanuel Paris
*/

#include "ContextMenu.h"
#include "Select_Move_Edit_Text.h"

extern void LoadImage();
extern void LoadVector();

boolean context_menu_page_look(cpage, event)
c_Page	*cpage;
XEvent	*event;
{
 Display	*display;
 l_Cadre	*lcadre;
 l_Cadre	*selected_cadre;
 c_FrMg		*frmg;
 c_ContextMenu	*context;
 void		*object;
 int		ret_button;
 int		xm, ym;
 coord_t	x, y;
 text_line_t	*tl = NULL;
 boolean	button = TRUE;
 boolean	retour = FALSE;

 display = cpage->X_info.display;
 xm = event->xbutton.x;
 ym = event->xbutton.y;
 x = xm * cpage->sScale;
 y = ym * cpage->sScale;
 frmg = cpage->frmg;

 lcadre = (l_Cadre *)F(frmg).get_cadre_by_xy(frmg, xm, ym);
 
 if (!lcadre)
 {
  
  if (F(cpage->Calibration).getTextLineByY(cpage->Calibration, cpage, 
					   y, x,  &tl) && 
      y >= tl->y && y < tl->y + tl->height)
  {
   Xc_TRACE(("context menu for Text selection"));
   
   context = (c_ContextMenu *)
    NEW(c_ContextMenu)(display, CM_TEXT, &ret_button, 
		       FALSE, event);
   
   if (context)
   {
    F(context).waitForUnmap(context);
    DELETE(c_ContextMenu)(context);
	  
    switch(ret_button)
    {
    case CM_Tex_CUT:
     F(cpage->Calibration->PageArea).SetFunction(cpage->Calibration->PageArea,
						 CUT_TEXT); 
     break;
    case CM_Tex_COPY:
     F(cpage->Calibration->PageArea).SetFunction(cpage->Calibration->PageArea,
						 COPY_TEXT); 
     break;
    case CM_Tex_PASTE:
     F(cpage->Calibration->PageArea).SetFunction(cpage->Calibration->PageArea,
						 PASTE_TEXT); 
     break;
    default:
     button = FALSE;
     break;
    }
   }
   else
    button = FALSE;
   retour = button;
  }
  else
  {
   Xc_TRACE(("context menu for no frame"));
   context = (c_ContextMenu *)
    NEW(c_ContextMenu)(display, CM_CREATE_CADRE, &ret_button, 
		       FALSE, event);
      
   if (context)
   {
    F(context).waitForUnmap(context);
    DELETE(c_ContextMenu)(context);
	  
    switch(ret_button)
    {
    case CM_Pge_CREATE_SQUARE:
     F(cpage->Calibration->PageArea).SetFunction(cpage->Calibration->PageArea,
						 CREATE_CADRE_RECTANGLE); 
     break;
    case CM_Pge_CREATE_ELIPSE:
     F(cpage->Calibration->PageArea).SetFunction(cpage->Calibration->PageArea,
						 CREATE_CADRE_ELIPSE); 
     break;
    case  CM_Pge_CREATE_POLYGON:
     F(cpage->Calibration->PageArea).SetFunction(cpage->Calibration->PageArea,
						 CREATE_CADRE_POLYGONAL); 
     break;
    default:
     button = FALSE;
     break;
    }
   }
   else
    button = FALSE;
   retour = button;
  }
 }
 else
 {
  Xc_TRACE(("On Frame Context Menu"));

  if (*cpage->select_type)
  {
   selected_cadre = (l_Cadre *)1;
   F(frmg).select_cadre(frmg, lcadre, TRUE);
  }
  else
  {
   selected_cadre = frmg->SelectedCadre;
   while(selected_cadre && selected_cadre->cadre != lcadre->cadre)
    selected_cadre = selected_cadre->NextCadre;
   
   if (!selected_cadre)
   {
    selected_cadre = 
     (l_Cadre *)duplicate_list_cadre(frmg->SelectedCadre);
    
    F(frmg).deselect_all_cadre(frmg, TRUE);
    F(frmg).select_cadre(frmg, lcadre, TRUE);
   }
   else
    selected_cadre = NULL;
  }
  
  context = (c_ContextMenu *)
   NEW(c_ContextMenu)(display, CM_CADRE_OBJECT, &ret_button, 
		      FALSE, event);
  
  if (context)
  {
   F(context).waitForUnmap(context);
   DELETE(c_ContextMenu)(context);
	  
   object = lcadre->cadre->object;
   Xc_TRACE(("ret button : %d", ret_button));
   switch(ret_button)
   {
   case CM_Cde_IMPORT_IMAGE:
    LoadImage((Widget)NULL);
    break;
   case CM_Cde_IMPORT_VECTOR:
    LoadVector((Widget)NULL);
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
   if (selected_cadre == (l_Cadre *)1)
   {
    F(frmg).deselect_all_cadre(frmg, TRUE);
    F(cpage->Redraw).display_selection(cpage->Redraw, NULL);
    *cpage->select_type = TRUE;
    selected_cadre = NULL;
   }
   else if (selected_cadre)
   {
    F(frmg).deselect_all_cadre(frmg, TRUE);
    lcadre = selected_cadre;
    
    while(lcadre)
    {
     F(frmg).select_cadre(frmg, lcadre, TRUE);
     lcadre = lcadre->NextCadre;
    }
   }
  }
  if (selected_cadre)
   free_list_cadre(selected_cadre, NO_DEL_CADRE);
 }
 
 return retour;
}
