/*
** Copy_Selected_Cadre.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:27:34 1995 Emmanuel Paris
** Last update Mon Jul 17 20:27:34 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cadres.h"

void copy_selected_cadre_init();
void copy_selected_cadre_abort();
void copy_selected_cadre_done();
void copy_selected_cadre_to_xy();
void copy_selected_cadre_trace();
void copy_selected_cadre_drag();

void copy_selected_cadre_init(cpage,xm,ym)
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
/*  Copy_selected_cadre_to_xy(cpage,xm,ym); */
}

void copy_selected_cadre_abort(cpage)
     c_Page	*cpage;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(source)=FALSE;
  if (d(prems))
  {
    copy_selected_cadre_trace(cpage);
    F(cpage).display_selected_lcadre(cpage);
  }
}

void copy_selected_cadre_done(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*duplcadre;
  l_Cadre	*svglcadre;
  c_Cadre	*dupcadre;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  
  if (d(prems))
  {
    copy_selected_cadre_trace(cpage);
    lcadre=svglcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
    F(cpage).deselect_all_cadre(cpage,FALSE);
    while(lcadre)
    {
      dupcadre=(c_Cadre *)COPY(c_Cadre)(lcadre->cadre,cpage->BaseStd);
      dupcadre->sOrig_X=lcadre->cadre->sOrig_X+d(sxc)-d(sxo);
      dupcadre->sOrig_Y=lcadre->cadre->sOrig_Y+d(syc)-d(syo);
      dupcadre->Forme->Orig_X=dupcadre->sOrig_X/cpage->sScale;
      dupcadre->Forme->Orig_Y=dupcadre->sOrig_Y/cpage->sScale;
      duplcadre=F(cpage).get_copy_cadre(cpage,lcadre,dupcadre);
      F(cpage).draw_cadre(cpage,duplcadre);
      F(cpage).select_cadre(cpage,duplcadre,FALSE);
      lcadre=lcadre->NextCadre;
    }
    free_list_cadre(svglcadre,NO_DEL_CADRE);
    F(cpage).display_selected_lcadre(cpage);
  }
  else
  {
    lcadre=(l_Cadre *)F(cpage).get_cadre_by_xy(cpage,d(xo),d(yo));
    F(cpage).deselect_cadre(cpage,lcadre,TRUE);
  }
  d(source)=FALSE;
}

void copy_selected_cadre_to_xy(cpage,xm,ym)
     c_Page	*cpage;
     int	xm;
     int	ym;
{
  coord_t	sox,soy,sdx,sdy;
  coord_t	svox,svoy,svdx,svdy;
  int		ox,oy,dx,dy;
  int		vox,voy,vdx,vdy;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  
  if (d(prems))
  {
    copy_selected_cadre_trace(cpage);
  }
  else
  {
    F(cpage).undisplay_selected_lcadre(cpage);
    F(CURSOR).set_cursor(CURSOR, C_COPY);
    F(cpage).init_recoordinate_point(cpage);
    d(prems)=TRUE;
  }
  d(xc)=xm;
  d(yc)=ym;
  d(sxc)=d(xc)*cpage->sScale;
  d(syc)=d(yc)*cpage->sScale;

  svox=sox=cpage->select_sX_min+d(sxc)-d(sxo);
  svoy=soy=cpage->select_sY_min+d(syc)-d(syo);
  vox=ox=svox/cpage->sScale;
  voy=oy=svoy/cpage->sScale;
  F(cpage).attract_point(cpage,&sox,&soy,&ox,&oy);

  svdx=sdx=cpage->select_sX_max+d(sxc)-d(sxo);
  svdy=sdy=cpage->select_sY_max+d(syc)-d(syo);
  vdx=dx=svdx/cpage->sScale;
  vdy=dy=svdy/cpage->sScale;
  F(cpage).attract_point(cpage,&sdx,&sdy,&dx,&dy);

  if (ABS(sox-svox)>=ABS(sdx-svdx))
    d(sxc)+=sox-svox;
  else
    d(sxc)+=sdx-svdx;
  
  if (ABS(soy-svoy)>=ABS(sdy-svdy))
    d(syc)+=soy-svoy;
  else
    d(syc)+=sdy-svdy;

  d(xc)=d(sxc)/cpage->sScale;
  d(yc)=d(syc)/cpage->sScale;

  copy_selected_cadre_trace(cpage);
}


void copy_selected_cadre_trace(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  c_Cadre	*cadre;
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
  
  lcadre=cpage->SelectedCadre;
  while(lcadre)
  {
    cadre=lcadre->cadre;
    trace_list_point(cadre->ListPoint,display,window,gc,
		     cadre->Forme->Orig_X+d(xc)-d(xo),
		     cadre->Forme->Orig_Y+d(yc)-d(yo),TRUE);
    lcadre=lcadre->NextCadre;
  }
}

void copy_selected_cadre_drag(cpage,lcadreS)
c_Page	*cpage;
l_Cadre	*lcadreS;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(prems)=FALSE;
  F(cpage).display_selected_lcadre(cpage);
}








