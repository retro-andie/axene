/*
 * Copy_Selected_Cadre.c    - definition for Copy_Selected_Cadre' functions
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#include "Select_Move_Resize_Cells.h"

void copy_selected_cadre_init();
void copy_selected_cadre_abort();
void copy_selected_cadre_done();
void copy_selected_cadre_to_xy();
void copy_selected_cadre_trace();
void copy_selected_cadre_drag();

void copy_selected_cadre_init(cfeuille,xm,ym)
     c_Feuille	*cfeuille;
     int	xm;
     int	ym;
{
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  d(xc)=d(xo)=xm;
  d(yc)=d(yo)=ym;
  d(sxc)=d(sxo)=d(xc)*cfrmg->sScale;
  d(syc)=d(syo)=d(yc)*cfrmg->sScale;
  d(prems)=FALSE;
  d(source)=TRUE;
/*  Copy_selected_cadre_to_xy(cfrmg,xm,ym); */
}

void copy_selected_cadre_abort(cfeuille)
     c_Feuille	*cfeuille;
{
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  d(source)=FALSE;
  if (d(prems))
    copy_selected_cadre_trace(cfeuille);
  F(cfrmg).display_selected_lcadre(cfrmg);
}

void copy_selected_cadre_done(cfeuille)
     c_Feuille	*cfeuille;
{
  l_Cadre	*lcadre;
  l_Cadre	*duplcadre;
  l_Cadre	*svglcadre;
  c_Cadre	*dupcadre;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  
  if (d(prems))
  {
    copy_selected_cadre_trace(cfeuille);
    lcadre=svglcadre=(l_Cadre *)sort_asc_list_cadre(cfrmg->SelectedCadre);
    F(cfrmg).deselect_all_cadre(cfrmg,FALSE);
    while(lcadre)
    {
      dupcadre=(c_Cadre *)COPY(c_Cadre)(lcadre->cadre,cfrmg->BaseStd);
      dupcadre->sOrig_X=lcadre->cadre->sOrig_X+d(sxc)-d(sxo);
      dupcadre->sOrig_Y=lcadre->cadre->sOrig_Y+d(syc)-d(syo);
      dupcadre->Forme->Orig_X=dupcadre->sOrig_X/cfrmg->sScale;
      dupcadre->Forme->Orig_Y=dupcadre->sOrig_Y/cfrmg->sScale;
      duplcadre=F(cfrmg).get_copy_cadre(cfrmg,lcadre,dupcadre);
      cfrmg->modify_cadre_callback(cfrmg->callback_data);
      F(cfrmg).draw_cadre(cfrmg,duplcadre);
      F(cfrmg).select_cadre(cfrmg,duplcadre,FALSE);
      lcadre=lcadre->NextCadre;
    }
    free_list_cadre(svglcadre,NO_DEL_CADRE);
    F(cfrmg).display_selected_lcadre(cfrmg);
  }
  else
  {
    lcadre=(l_Cadre *)F(cfrmg).get_cadre_by_xy(cfrmg,d(xo),d(yo));
    F(cfrmg).deselect_cadre(cfrmg,lcadre,TRUE);
  }
  d(source)=FALSE;
}

void copy_selected_cadre_to_xy(cfeuille,xm,ym)
     c_Feuille	*cfeuille;
     int	xm;
     int	ym;
{
  coord_t	sox,soy,sdx,sdy;
  coord_t	svox,svoy,svdx,svdy;
  int		ox,oy,dx,dy;
  int		vox,voy,vdx,vdy;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  
  if (d(prems))
  {
    copy_selected_cadre_trace(cfeuille);
  }
  else
  {
    F(cfrmg).undisplay_selected_lcadre(cfrmg);
    F(CURSOR).set_cursor(CURSOR, C_COPY);
    F(cfrmg).init_recoordinate_point(cfrmg);
    d(prems)=TRUE;
  }
  d(xc)=xm;
  d(yc)=ym;
  d(sxc)=d(xc)*cfrmg->sScale;
  d(syc)=d(yc)*cfrmg->sScale;

  svox=sox=cfrmg->select_sX_min+d(sxc)-d(sxo);
  svoy=soy=cfrmg->select_sY_min+d(syc)-d(syo);
  vox=ox=svox/cfrmg->sScale;
  voy=oy=svoy/cfrmg->sScale;
  F(cfrmg).attract_point(cfrmg,&sox,&soy,&ox,&oy);
  
  svdx=sdx=cfrmg->select_sX_max+d(sxc)-d(sxo);
  svdy=sdy=cfrmg->select_sY_max+d(syc)-d(syo);
  vdx=dx=svdx/cfrmg->sScale;
  vdy=dy=svdy/cfrmg->sScale;
  F(cfrmg).attract_point(cfrmg,&sdx,&sdy,&dx,&dy);
  
  if (ABS(sox-svox)>=ABS(sdx-svdx))
    d(sxc)+=sox-svox;
  else
    d(sxc)+=sdx-svdx;
  
  if (ABS(soy-svoy)>=ABS(sdy-svdy))
    d(syc)+=soy-svoy;
  else
    d(syc)+=sdy-svdy;

  d(xc)=d(sxc)/cfrmg->sScale;
  d(yc)=d(syc)/cfrmg->sScale;

  copy_selected_cadre_trace(cfeuille);
}


void copy_selected_cadre_trace(cfeuille)
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

void copy_selected_cadre_drag(cfeuille,lcadreS)
c_Feuille	*cfeuille;
l_Cadre	*lcadreS;
{
  DATA(Select_Move_Resize_Cell);
  
  GET_DATAF(Select_Move_Resize_Cell);
  d(prems)=FALSE;
  F(cfrmg).display_selected_lcadre(cfrmg);
}








