/*
** Outline_Cadre.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:58:52 1995 Emmanuel Paris
** Last update Thu Oct 24 16:20:15 1996 Emmanuel Paris
*/


#include "Page.h"

void outline_cadre(cpage)
   c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  l_Cadre	*sortedlcadre;
  c_Cadre	*cadre;
  l_Point	*lpoint;
  char 		*name;
  int		priority;
  int		object_type;
  void		*object;
  angle_t	angle = (angle_t)0;

  F(cpage).undisplay_selected_lcadre(cpage);
  sortedlcadre = lcadre =
    (l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);

  svglcadre = sortedlcadre;
  lcadre = sortedlcadre->NextCadre;

  object_type = svglcadre->cadre->object_type;
  object = svglcadre->cadre->object;

  svglcadre->cadre->object_type = CADRE_EMPTY;
  svglcadre->cadre->object = NULL;
  cadre=(c_Cadre *)COPY(c_Cadre)(svglcadre->cadre, cpage->BaseStd);
  cadre->object_type = object_type;
  cadre->object = object;

  while(lcadre)
    {
      lpoint=(l_Point *)SubstractCadre(cadre, lcadre->cadre);
      if (!lpoint)
	{
	  F(cpage).destroy_cadre(cpage, svglcadre, DEL_CADRE);
	  free_list_cadre(sortedlcadre, NO_DEL_CADRE);
	  return;
	}
      cadre->sOrig_X = MIN(cadre->sOrig_X, lcadre->cadre->sOrig_X);
      cadre->sOrig_Y = MIN(cadre->sOrig_Y, lcadre->cadre->sOrig_Y);
      free_list_point(cadre->ListPoint);
      cadre->ListPoint = lpoint;
      lcadre = lcadre->NextCadre;
    }
  XFlush(cpage->X_info.display);

  cadre->Forme->Orig_X = cadre->sOrig_X/cadre->sScale;
  cadre->Forme->Orig_Y = cadre->sOrig_Y/cadre->sScale;
  name = Xc_strdup("dupname",svglcadre->name);
  priority = svglcadre->priority;
  
  F(cpage).destroy_cadre(cpage, svglcadre, DEL_CADRE);
  F(cpage).add_cadre(cpage, name, priority, cadre);
  lcadre = F(cpage).get_cadre_by_name(cpage, name);
  Xc_free(name);
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre, CONTOUR);
  if (cadre->object_type!=CADRE_EMPTY)
    {
      if (cadre->sAngle == angle)
	DISPLAY_CONFIGURE(cadre->object)(cadre->object, cadre, cadre->Forme);
      DISPLAY_CONFIGURE(cadre->object)(cadre->object, cadre, cadre->Forme);
    }
  F(cpage).draw_cadre(cpage, lcadre);
  free_list_cadre(sortedlcadre, NO_DEL_CADRE);

  F(cpage).deselect_all_cadre(cpage, FALSE);
  F(cpage).select_cadre(cpage, lcadre, TRUE); 
}


