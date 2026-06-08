/*
** Delete_Selected_Cadre.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:25:27 1995 Emmanuel Paris
** Last update Mon Jul 17 20:25:27 1995 Emmanuel Paris
*/

#include "Page.h"

void delete_selected_cadre(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  
  F(cpage).undisplay_selected_lcadre(cpage);
  lcadre=(l_Cadre *)sort_desc_list_cadre(cpage->SelectedCadre);
  svglcadre=lcadre;
  while(lcadre)
  {
    F(cpage).destroy_cadre(cpage,lcadre,DEL_CADRE);
    lcadre=lcadre->NextCadre;
  }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
  F(cpage).display_selected_lcadre(cpage);
}

void delete_selected_object(cpage)
c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  
  F(cpage).undisplay_selected_lcadre(cpage);
  lcadre=(l_Cadre *)sort_desc_list_cadre(cpage->SelectedCadre);
  svglcadre=lcadre;
  while(lcadre)
  {
    if (lcadre->cadre->object_type!=CADRE_EMPTY)
      {
	F(lcadre->cadre).detachobject(lcadre->cadre);
	F(cpage).draw_cadre(cpage,lcadre);
      }
    else
      F(cpage).destroy_cadre(cpage,lcadre,DEL_CADRE);
    lcadre=lcadre->NextCadre;
  }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
  F(cpage).display_selected_lcadre(cpage);
}

