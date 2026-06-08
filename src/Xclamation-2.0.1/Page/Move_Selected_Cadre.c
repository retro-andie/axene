/*
** Move_Selected_Cadre.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:59:38 1995 Emmanuel Paris
** Last update Tue Oct  3 13:13:18 1995 Stéphane Boisson
*/

#include "Select_Move_Resize_Cadres.h"

void move_selected_cadre_init();
void move_selected_cadre_abort();
void move_selected_cadre_done();
void move_selected_cadre_to_xy();
void move_selected_cadre_trace();
void move_selected_cadre_drag();

void move_selected_cadre_init(cpage,xm,ym)
c_Page	*cpage;
int	xm;
int	ym;
{
  l_Cadre	*lcadre;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(xc)=d(xo)=xm;
  d(yc)=d(yo)=ym;
  d(sxc)=d(sxo)=d(xc)*cpage->sScale;
  d(syc)=d(syo)=d(yc)*cpage->sScale;
  d(prems)=FALSE;
  d(source)=TRUE;
  F(cpage).undisplay_selected_lcadre(cpage);
  lcadre=cpage->SelectedCadre;
  while(lcadre)
    {
      F(lcadre->cadre).set_contour_action(lcadre->cadre,GFX_CLEAR);
      F(cpage).draw_cadre_contour(cpage,lcadre);
      F(lcadre->cadre).set_contour_action(lcadre->cadre,GFX_SET);
      lcadre->cadre->in_work=TRUE;
      lcadre=lcadre->NextCadre;
    }
  F(CURSOR).set_cursor(CURSOR,C_MOVE);
  F(cpage).init_recoordinate_point(cpage);
  move_selected_cadre_to_xy(cpage,xm,ym);
}

void move_selected_cadre_abort(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(source)=FALSE;
  if (d(prems))
    {
      move_selected_cadre_trace(cpage);
    }
  lcadre=cpage->SelectedCadre;
  while(lcadre)
    {
      F(cpage).draw_cadre_contour(cpage,lcadre);
      lcadre->cadre->in_work=FALSE;
      lcadre=lcadre->NextCadre;
    }
  F(cpage).display_selected_lcadre(cpage);
}

void move_selected_cadre_done(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  
  move_selected_cadre_trace(cpage);
  lcadre=svglcadre=(l_Cadre *)sort_asc_list_cadre(cpage->SelectedCadre);
  while(lcadre)
    {
      F(cpage).move_cadre_to_xy(cpage,lcadre,
				lcadre->cadre->sOrig_X+d(sxc)-d(sxo),
				lcadre->cadre->sOrig_Y+d(syc)-d(syo));
      lcadre->cadre->in_work=FALSE;
      lcadre=lcadre->NextCadre;
    }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
  F(cpage).display_selected_lcadre(cpage);
  d(source)=FALSE;
}

void move_selected_cadre_to_xy(cpage,xm,ym)
     c_Page	*cpage;
     int	xm;
     int	ym;
{
  coord_t	sox,soy,sdx,sdy,snx,sny,smx,smy;
  coord_t	svox,svoy,svdx,svdy,svnx,svny,svmx,svmy;
  int		ox,oy,dx,dy,nx,ny,mx,my;
  int		vox,voy,vdx,vdy,vnx,vny,vmx,vmy;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  
  if (d(prems))
  {
    move_selected_cadre_trace(cpage);
  }
  d(xc)=xm;
  d(yc)=ym;
  d(sxc)=d(xc)*cpage->sScale;
  d(syc)=d(yc)*cpage->sScale;

  svox=sox=svnx=snx=cpage->select_sX_min+d(sxc)-d(sxo);
  svoy=soy=svmy=smy=cpage->select_sY_min+d(syc)-d(syo);
  vox=ox=vnx=nx=svox/cpage->sScale;
  voy=oy=vmy=my=svoy/cpage->sScale;
  svdx=sdx=svmx=smx=cpage->select_sX_max+d(sxc)-d(sxo);
  svdy=sdy=svny=sny=cpage->select_sY_max+d(syc)-d(syo);
  vdx=dx=vmx=mx=svdx/cpage->sScale;
  vdy=dy=vny=ny=svdy/cpage->sScale;
  
  F(cpage).attract_point(cpage,&sox,&soy,&ox,&oy);
  F(cpage).attract_point(cpage,&sdx,&sdy,&dx,&dy);
  F(cpage).attract_point(cpage,&snx,&sny,&nx,&ny);
  F(cpage).attract_point(cpage,&smx,&smy,&mx,&my);
  
#ifdef ___PSEUDO_COMPOSE_MACRO
#define ML_DELTA(p) ABS(s##p-sv##p)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define ML_DELTA(p) ABS(s##p##-sv##p##)
#else
#define ML_DELTA(p) ABS(s/**/p/**/-sv/**/p/**/)
#endif
#endif /* PSEUDO */
  
  if (ML_DELTA(ox) >= ML_DELTA(dx) && ML_DELTA(ox) >= ML_DELTA(nx) &&
      ML_DELTA(ox) >= ML_DELTA(mx))
    d(sxc)+=sox-svox;
  else
    if (ML_DELTA(dx) >= ML_DELTA(ox) && ML_DELTA(dx) >= ML_DELTA(nx) &&
	ML_DELTA(dx) >= ML_DELTA(mx))
      d(sxc)+=sdx-svdx;
    else
      if (ML_DELTA(nx) >= ML_DELTA(ox) && ML_DELTA(nx) >= ML_DELTA(dx) &&
	  ML_DELTA(nx) >= ML_DELTA(mx))
	d(sxc)+=snx-svnx;
      else
	d(sxc)+=smx-svmx;
  
  if (ML_DELTA(oy) >= ML_DELTA(dy) && ML_DELTA(oy) >= ML_DELTA(ny) &&
      ML_DELTA(oy) >= ML_DELTA(my))
    d(syc)+=soy-svoy;
  else
    if (ML_DELTA(dy) >= ML_DELTA(oy) && ML_DELTA(dy) >= ML_DELTA(ny) &&
	ML_DELTA(dy) >= ML_DELTA(my))
      d(syc)+=sdy-svdy;
    else
      if (ML_DELTA(ny) >= ML_DELTA(oy) && ML_DELTA(ny) >= ML_DELTA(dy) &&
	  ML_DELTA(ny) >= ML_DELTA(my))
	d(syc)+=sny-svny;
      else
	d(syc)+=smy-svmy;
  
#undef ML_DELTA

  d(xc)=d(sxc)/cpage->sScale;
  d(yc)=d(syc)/cpage->sScale;
  
  move_selected_cadre_trace(cpage);
  d(prems)=TRUE;
}


void move_selected_cadre_trace(cpage)
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

/*             Drag And Drop: Drop for source             */

void move_selected_cadre_drag(cpage,lcadreS)
c_Page	*cpage;
l_Cadre	*lcadreS;	
{
  l_Cadre	*lcadre;
  l_Cadre	*duplcadre;
  l_Cadre	*svglcadre;
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  
  lcadre=svglcadre=(l_Cadre *)sort_asc_list_cadre(lcadreS);
  cpage->SelectedCadre=(l_Cadre *)duplicate_list_cadre(lcadreS);
  while(lcadre)
    {
      lcadre->cadre->in_work=FALSE;
      duplcadre=F(cpage).get_cadre_by_name(cpage,lcadre->name);
/*duplcadre->name=Xc_strdup("name del",lcadre->name);*/
      F(cpage).destroy_cadre(cpage,lcadre, NO_DEL_CADRE);
      lcadre=lcadre->NextCadre;
    }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
  d(source)=d(prems)=FALSE;
}


