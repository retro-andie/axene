/*
** Fusion_Cadre.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:22:48 1995 Emmanuel Paris
** Last update Mon Jul 17 20:22:48 1995 Emmanuel Paris
*/


#include "Page.h"

void fusion_cadre(cpage)
   c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  l_Cadre	*sortedlcadre;
  int		max_type;
  angle_t	angle;

  F(cpage).undisplay_selected_lcadre(cpage);
  sortedlcadre=lcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
  max_type=0;
  while(lcadre)
  {
    if (lcadre->cadre->object_type>max_type)
      max_type=lcadre->cadre->object_type;
    lcadre=lcadre->NextCadre;
  }
  lcadre=sortedlcadre;
  angle=lcadre->cadre->sAngle;
  svglcadre=NULL;
  while(lcadre->cadre->object_type!=max_type)
  {
    lcadre=lcadre->NextCadre;
  }
  svglcadre=(l_Cadre *)Xc_malloc("addlcadre",sizeof(l_Cadre));
  memcpy(svglcadre,lcadre,sizeof(l_Cadre));
  delete_cadre_in_list(&sortedlcadre,lcadre->cadre);
  lcadre=svglcadre;
  svglcadre=sortedlcadre;
  while(svglcadre)
  {
    F(lcadre->cadre).fusion_cadre(lcadre->cadre,svglcadre->cadre);
    F(cpage).destroy_cadre(cpage,svglcadre,DEL_CADRE);
    svglcadre=svglcadre->NextCadre;
  }
  if (angle!=lcadre->cadre->sAngle &&
      lcadre->cadre->object_type!=CADRE_EMPTY)
    {
      lcadre->cadre->sAngle = angle;
      DISPLAY_CONFIGURE(lcadre->cadre->object)(lcadre->cadre->object,
					       lcadre->cadre,
					       lcadre->cadre->Forme);
    }
  else
    lcadre->cadre->sAngle = angle;    
  F(cpage).redisplay_cadre(cpage,lcadre);
  
  Xc_free(lcadre);
  free_list_cadre(sortedlcadre,NO_DEL_CADRE);
  F(cpage).display_selected_lcadre(cpage);
}


