/*
** Copy_Selected_Cadre.c for XAllWrite in FrameManager/
** Copy Selected Cadre event loop
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
** Started on  Sat Apr 11 16:13:39 1998 Emmanuel Paris
** Last update Wed Jul  8 15:21:27 1998 Emmanuel Paris
*/

#include "Select_Move_Edit_Text.h"

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
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 d(xc) = d(xo) = xm;
 d(yc) = d(yo) = ym;
 d(sxc) = d(sxo) = d(xc)*cfrmg->sScale;
 d(syc) = d(syo) = d(yc)*cfrmg->sScale;
 d(prems) = FALSE;
 d(source) = TRUE;
 /*  Copy_selected_cadre_to_xy(cfrmg,xm,ym); */
}

void copy_selected_cadre_abort(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 d(source) = FALSE;
 if (d(prems))
  copy_selected_cadre_trace(cpage);
 F(cfrmg).display_selected_lcadre(cfrmg);
}

void copy_selected_cadre_done(cpage)
c_Page	*cpage;
{
 l_Cadre	*lcadre;
 l_Cadre	*duplcadre;
 l_Cadre	*svglcadre;
 c_Cadre	*dupcadre;
 boolean	recalibrate = FALSE;
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
  
 if (d(prems))
 {
  copy_selected_cadre_trace(cpage);
  lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre(cfrmg->SelectedCadre);
  F(cfrmg).deselect_all_cadre(cfrmg,FALSE);
  while(lcadre)
  {
   dupcadre = (c_Cadre *)COPY(c_Cadre)(lcadre->cadre, cfrmg->BaseStd);
   dupcadre->sOrig_X = lcadre->cadre->sOrig_X + d(sxc) - d(sxo);
   dupcadre->sOrig_Y = lcadre->cadre->sOrig_Y + d(syc) - d(syo);
   dupcadre->Forme->Orig_X = dupcadre->sOrig_X / cfrmg->sScale;
   dupcadre->Forme->Orig_Y = dupcadre->sOrig_Y / cfrmg->sScale;
   duplcadre = F(cfrmg).get_copy_cadre(cfrmg, lcadre, dupcadre);
   F(cfrmg).draw_cadre(cfrmg, duplcadre);
   F(cfrmg).select_cadre(cfrmg, duplcadre, FALSE);

   if ((dupcadre->textflow_mode & XcTFM_HOLE_MASK) > XcTFM_OVER)
    recalibrate = TRUE;

   lcadre = lcadre->NextCadre;
  }

  cfrmg->modify_cadre_callback(cfrmg->callback_data, recalibrate);

  free_list_cadre(svglcadre,NO_DEL_CADRE);
  F(cfrmg).display_selected_lcadre(cfrmg);
 }
 d(source) = FALSE;
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
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
  
 if (d(prems))
 {
  copy_selected_cadre_trace(cpage);
 }
 else
 {
  F(cfrmg).undisplay_selected_lcadre(cfrmg);
  F(CURSOR).set_cursor(CURSOR, C_COPY);
  F(cfrmg).init_recoordinate_point(cfrmg);
  d(prems) = TRUE;
 }
 d(xc) = xm;
 d(yc) = ym;
 d(sxc) = d(xc)*cfrmg->sScale;
 d(syc) = d(yc)*cfrmg->sScale;

 svox = sox = cfrmg->select_sX_min+d(sxc)-d(sxo);
 svoy = soy = cfrmg->select_sY_min+d(syc)-d(syo);
 vox = ox = svox/cfrmg->sScale;
 voy = oy = svoy/cfrmg->sScale;
 F(cfrmg).attract_point(cfrmg,&sox,&soy,&ox,&oy);
  
 svdx = sdx = cfrmg->select_sX_max+d(sxc)-d(sxo);
 svdy = sdy = cfrmg->select_sY_max+d(syc)-d(syo);
 vdx = dx = svdx/cfrmg->sScale;
 vdy = dy = svdy/cfrmg->sScale;
 F(cfrmg).attract_point(cfrmg,&sdx,&sdy,&dx,&dy);
  
 if (ABS(sox-svox) >= ABS(sdx-svdx))
  d(sxc) += sox-svox;
 else
  d(sxc) += sdx-svdx;
  
 if (ABS(soy-svoy) >= ABS(sdy-svdy))
  d(syc) += soy-svoy;
 else
  d(syc) += sdy-svdy;

 d(xc) = d(sxc)/cfrmg->sScale;
 d(yc) = d(syc)/cfrmg->sScale;

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
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 display = cfrmg->X_info.display;
 window = cfrmg->X_info.window;
 gc = cfrmg->X_info.gc_contour;
  
 F(cfrmg).recoordinate_point(cfrmg,&d(sxo),&d(syo),&d(xo),&d(yo),FALSE);
 F(cfrmg).recoordinate_point(cfrmg,&d(sxc),&d(syc),&d(xc),&d(yc),TRUE);
  
 lcadre = cfrmg->SelectedCadre;
 while(lcadre)
 {
  cadre = lcadre->cadre;
  trace_list_point(cadre->ListPoint,display,window,gc,
		   cadre->Forme->Orig_X+d(xc)-d(xo),
		   cadre->Forme->Orig_Y+d(yc)-d(yo),TRUE);
  lcadre = lcadre->NextCadre;
 }
}

void copy_selected_cadre_drag(cpage,lcadreS)
c_Page	*cpage;
l_Cadre	*lcadreS;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 d(prems) = FALSE;
 /*F(cfrmg).display_selected_lcadre(cfrmg);*/
}








