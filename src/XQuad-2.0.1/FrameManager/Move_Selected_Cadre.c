/*
 * Move_Selected_Cadre.c    - definition for Move_Selected_Cadre' functions
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#include "Select_Move_Resize_Cells.h"

void move_selected_cadre_init();
void move_selected_cadre_abort();
void move_selected_cadre_done();
void move_selected_cadre_to_xy();
void move_selected_cadre_trace();
void move_selected_cadre_drag();

void move_selected_cadre_init(cfeuille,xm,ym)
c_Feuille	*cfeuille;
int	xm;
int	ym;
{
  l_Cadre	*lcadre;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  d(xc)=d(xo)=xm;
  d(yc)=d(yo)=ym;
  d(sxc)=d(sxo)=d(xc)*cfrmg->sScale;
  d(syc)=d(syo)=d(yc)*cfrmg->sScale;
  d(prems)=FALSE;
  d(source)=TRUE;
  F(cfrmg).undisplay_selected_lcadre(cfrmg);
  lcadre=cfrmg->SelectedCadre;
  while(lcadre)
    {
      F(lcadre->cadre).set_contour_action(lcadre->cadre,GFX_CLEAR);
      F(cfrmg).draw_cadre_contour(cfrmg,lcadre);
      F(lcadre->cadre).set_contour_action(lcadre->cadre,GFX_SET);
      lcadre->cadre->in_work=TRUE;
      lcadre=lcadre->NextCadre;
    }
  F(CURSOR).set_cursor(CURSOR,C_MOVE);
  F(cfrmg).init_recoordinate_point(cfrmg);
  move_selected_cadre_to_xy(cfeuille,xm,ym);
}

void move_selected_cadre_abort(cfeuille)
     c_Feuille	*cfeuille;
{
  l_Cadre	*lcadre;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  d(source)=FALSE;
  if (d(prems))
    {
      move_selected_cadre_trace(cfeuille);
    }
  lcadre=cfrmg->SelectedCadre;
  while(lcadre)
    {
      F(cfrmg).draw_cadre_contour(cfrmg,lcadre);
      lcadre->cadre->in_work=FALSE;
      lcadre=lcadre->NextCadre;
    }
  F(cfrmg).display_selected_lcadre(cfrmg);
}

void move_selected_cadre_done(cfeuille)
     c_Feuille	*cfeuille;
{
  l_Cadre	*lcadre;
  l_Cadre	*svglcadre;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  
  move_selected_cadre_trace(cfeuille);
  lcadre=svglcadre=(l_Cadre *)sort_asc_list_cadre(cfrmg->SelectedCadre);
  while(lcadre)
    {
      F(cfrmg).move_cadre_to_xy(cfrmg,lcadre,
				lcadre->cadre->sOrig_X+d(sxc)-d(sxo),
				lcadre->cadre->sOrig_Y+d(syc)-d(syo));
      lcadre->cadre->in_work=FALSE;
      lcadre=lcadre->NextCadre;
    }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
  F(cfrmg).display_selected_lcadre(cfrmg);
  d(source)=FALSE;
}

void move_selected_cadre_to_xy(cfeuille,xm,ym)
     c_Feuille	*cfeuille;
     int	xm;
     int	ym;
{
  coord_t	sox,soy,sdx,sdy,snx,sny,smx,smy;
  coord_t	svox,svoy,svdx,svdy,svnx,svny,svmx,svmy;
  int		ox,oy,dx,dy,nx,ny,mx,my;
  int		vox,voy,vdx,vdy,vnx,vny,vmx,vmy;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  
  if (d(prems))
  {
    move_selected_cadre_trace(cfeuille);
  }
  d(xc)=xm;
  d(yc)=ym;
  d(sxc)=d(xc)*cfrmg->sScale;
  d(syc)=d(yc)*cfrmg->sScale;

  svox=sox=svnx=snx=cfrmg->select_sX_min+d(sxc)-d(sxo);
  svoy=soy=svmy=smy=cfrmg->select_sY_min+d(syc)-d(syo);
  vox=ox=vnx=nx=svox/cfrmg->sScale;
  voy=oy=vmy=my=svoy/cfrmg->sScale;
  svdx=sdx=svmx=smx=cfrmg->select_sX_max+d(sxc)-d(sxo);
  svdy=sdy=svny=sny=cfrmg->select_sY_max+d(syc)-d(syo);
  vdx=dx=vmx=mx=svdx/cfrmg->sScale;
  vdy=dy=vny=ny=svdy/cfrmg->sScale;
  
  F(cfrmg).attract_point(cfrmg,&sox,&soy,&ox,&oy);
  F(cfrmg).attract_point(cfrmg,&sdx,&sdy,&dx,&dy);
  F(cfrmg).attract_point(cfrmg,&snx,&sny,&nx,&ny);
  F(cfrmg).attract_point(cfrmg,&smx,&smy,&mx,&my);
  
#ifdef ___PSEUDO_COMPOSE_MACRO
#define ML_DELTA(p) ABS(s##p-sv##p)
#else
#ifdef ___GNUC_COMPOSE_MACRO
#define ML_DELTA(p) ABS(s##p##-sv##p##)
#else
#define ML_DELTA(p) ABS(s/**/p/**/-sv/**/p/**/)
#endif
#endif
  
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

  d(xc)=d(sxc)/cfrmg->sScale;
  d(yc)=d(syc)/cfrmg->sScale;
  
  move_selected_cadre_trace(cfeuille);
  d(prems)=TRUE;
}


void move_selected_cadre_trace(cfeuille)
     c_Feuille	*cfeuille;
{
  l_Cadre	*lcadre;
  c_Cadre	*cadre;
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  display=cfrmg->X_info.display;
  window=cfrmg->X_info.window;
  gc=cfrmg->X_info.gc_contour;
  
  F(cfrmg).recoordinate_point(cfrmg,&d(sxo),&d(syo),&d(xo),&d(yo),FALSE);
  F(cfrmg).recoordinate_point(cfrmg,&d(sxc),&d(syc),&d(xc),&d(yc),TRUE);
  
  lcadre=cfrmg->SelectedCadre;
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

void move_selected_cadre_drag(cfeuille,lcadreS)
c_Feuille	*cfeuille;
l_Cadre	*lcadreS;	
{
  l_Cadre	*lcadre;
  l_Cadre	*duplcadre;
  l_Cadre	*svglcadre;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  
  lcadre=svglcadre=(l_Cadre *)sort_asc_list_cadre(lcadreS);
  cfrmg->SelectedCadre=(l_Cadre *)duplicate_list_cadre(lcadreS);
  while(lcadre)
    {
      lcadre->cadre->in_work=FALSE;
      duplcadre=F(cfrmg).get_cadre_by_name(cfrmg,lcadre->name);
/*duplcadre->name=Xc_strdup("name del",lcadre->name);*/
      F(cfrmg).destroy_cadre(cfrmg,lcadre, NO_DEL_CADRE);
      lcadre=lcadre->NextCadre;
    }
  free_list_cadre(svglcadre,NO_DEL_CADRE);
  if (!cfrmg->SelectedCadre)
    cfrmg->deselect_cadre_callback(cfrmg->callback_data);    
  d(source)=d(prems)=FALSE;
}


