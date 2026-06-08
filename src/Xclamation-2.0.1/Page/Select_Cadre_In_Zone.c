/*
** Select_Cadre_In_Zone.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:53:59 1995 Emmanuel Paris
** Last update Mon Jul 17 19:53:59 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cadres.h"

void select_cadre_in_zone_init();
void select_cadre_in_zone_abort();
void select_cadre_in_zone_done();
void select_cadre_in_zone_to_xy();
void select_cadre_in_zone_trace();

void select_cadre_in_zone_init(cpage,xm,ym)
     c_Page	*cpage;
     int	xm;
     int	ym;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(xc)=d(xo)=xm;
  d(yc)=d(yo)=ym;
  d(sxc)=d(sxo)=d(xc)*cpage->sScale;
  d(syc)=d(syo)=d(yc)*cpage->sScale;
  d(prems)=FALSE;
  d(source)=TRUE;
  F(cpage).init_recoordinate_point(cpage);
  select_cadre_in_zone_to_xy(cpage,xm,ym); 
}

void select_cadre_in_zone_abort(cpage)
     c_Page	*cpage;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(source)=FALSE;
  select_cadre_in_zone_trace(cpage);
}

void select_cadre_in_zone_done(cpage)
     c_Page	*cpage;
{
  int min_x;
  int min_y;
  int max_x;
  int max_y;
  l_Cadre	*lcadre;
  Polygone	*polyg;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(source)=FALSE;
  select_cadre_in_zone_trace(cpage);
  min_x=MIN(d(xo),d(xc));
  min_y=MIN(d(yo),d(yc));
  max_x=MAX(d(xo),d(xc));
  max_y=MAX(d(yo),d(yc));
  F(cpage).undisplay_selected_lcadre(cpage);
  lcadre=cpage->ListCadre;
  while(lcadre)
  {
    if (!lcadre->cadre->selected)
    {
      polyg=lcadre->cadre->Forme;
      if (min_x<=(polyg->Orig_X+polyg->x_min) &&
	  min_y<=(polyg->Orig_Y+polyg->y_min) &&
	  max_x>=(polyg->Orig_X+polyg->x_max) &&
	  max_y>=(polyg->Orig_Y+polyg->y_max))
      {
	F(cpage).select_cadre(cpage,lcadre,FALSE);
	cpage->verrou|=lcadre->cadre->verrou;
      }
    }
    lcadre=lcadre->NextCadre;
  }
  F(cpage).display_selected_lcadre(cpage);
}

void select_cadre_in_zone_to_xy(cpage,xm,ym)
     c_Page	*cpage;
     int	xm;
     int	ym;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  if(d(prems))
    select_cadre_in_zone_trace(cpage);
  d(xc)=xm;
  d(yc)=ym;
  d(sxc)=d(xc)*cpage->sScale;
  d(syc)=d(yc)*cpage->sScale;
  select_cadre_in_zone_trace(cpage);
  d(prems)=TRUE;
}

void select_cadre_in_zone_trace(cpage)
     c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  F(cpage).recoordinate_point(cpage,&d(sxo),&d(syo),&d(xo),&d(yo),FALSE);
  F(cpage).recoordinate_point(cpage,&d(sxc),&d(syc),&d(xc),&d(yc),TRUE);
  
  XDrawRectangle(display,window,gc,MIN(d(xo),d(xc)),MIN(d(yo),d(yc)),
		 (d(xo)>d(xc)?d(xo)-d(xc):d(xc)-d(xo)),
		 (d(yo)>d(yc)?d(yo)-d(yc):d(yc)-d(yo)));
  
}


