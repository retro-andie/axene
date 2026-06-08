/*
** Delete_Selected_Cadre.c for XQuad and XAllWrite in FrameManager/
** function definition for deleting cadre
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
** Started on  Mon Jul 17 20:25:27 1995 Emmanuel Paris
** Last update Wed Aug  5 17:44:20 1998 Emmanuel Paris
*/

#include "FrameManager.h"

void delete_selected_cadre(cfrmg)
c_FrMg	*cfrmg;
{
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
  
 F(cfrmg).undisplay_selected_lcadre(cfrmg);
 lcadre = (l_Cadre *)sort_desc_list_cadre(cfrmg->SelectedCadre);
 svglcadre = lcadre;
 while(lcadre)
 {
  F(cfrmg).destroy_cadre(cfrmg, lcadre, DEL_CADRE);
  lcadre = lcadre->NextCadre;
 }
 free_list_cadre(svglcadre, NO_DEL_CADRE);
 F(cfrmg).display_selected_lcadre(cfrmg);
  
 if (!cfrmg->SelectedCadre)
  cfrmg->deselect_cadre_callback(cfrmg->callback_data);    
}

void delete_selected_object(cfrmg)
c_FrMg	*cfrmg;
{
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
  
 F(cfrmg).undisplay_selected_lcadre(cfrmg);
 lcadre=(l_Cadre *)sort_desc_list_cadre(cfrmg->SelectedCadre);
 svglcadre=lcadre;
 while(lcadre)
 {
  if (lcadre->cadre->object_type != CADRE_EMPTY)
  {
   F(lcadre->cadre).detachobject(lcadre->cadre);
   F(cfrmg).draw_cadre(cfrmg, lcadre);
  }
  else
   F(cfrmg).destroy_cadre(cfrmg, lcadre, DEL_CADRE);
  lcadre=lcadre->NextCadre;
 }
 free_list_cadre(svglcadre, NO_DEL_CADRE);
 F(cfrmg).display_selected_lcadre(cfrmg);

 if (!cfrmg->SelectedCadre)
  cfrmg->deselect_cadre_callback(cfrmg->callback_data);    
}



