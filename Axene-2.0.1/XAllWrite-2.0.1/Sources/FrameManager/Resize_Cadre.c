/*
** Resize_Cadre.c for XAllWrite in FrameManager/
** Resize Frame Event loop
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
** Started on  Sat Apr 11 15:44:23 1998 Emmanuel Paris
** Last update Sat Apr 11 15:53:38 1998 Emmanuel Paris
*/

#include "Select_Move_Edit_Text.h"

void Resize_cadre_init();
void Resize_cadre_abort();
void Resize_cadre_done();
void Resize_cadre_to_xy();
void Resize_cadre_trace();

void Resize_cadre_init(cpage,xm,ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 l_Cadre       *lcadre;
 l_Lpoint	*svglistp;
 l_Lpoint	*ssvglistp;
 l_Point       *retpnt;
 boolean	prems;
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 retpnt = F(cfrmg).get_handle_by_xy(cfrmg, xm, ym, NULL, &lcadre);
 F(cfrmg).undisplay_selected_lcadre(cfrmg);
 svglistp = ssvglistp = NULL;
 prems = TRUE;
 if (lcadre)
 {
  d(lcadre) = lcadre;
 }
 else
 {
  lcadre = cfrmg->SelectedCadre;
  d(lcadre) = NULL;
 }
 while(lcadre)
 {
  if (prems)
  {
   svglistp = (l_Lpoint *)Xc_malloc("llpoint",sizeof(l_Lpoint));
   ssvglistp = svglistp;
   prems = FALSE;
  }
  else
  {
   svglistp->NextLpoint =
    (l_Lpoint *)Xc_malloc("llpoint",sizeof(l_Lpoint));
   svglistp = svglistp->NextLpoint;
  }
  svglistp->lpoint = duplicate_list_point(lcadre->cadre->ListPoint);
  F(lcadre->cadre).set_contour_action(lcadre->cadre,
				      GFX_CLEAR);
  F(cfrmg).draw_cadre_contour(cfrmg,lcadre);
  F(lcadre->cadre).set_contour_action(lcadre->cadre,
				      GFX_SET);
  lcadre->cadre->in_work = TRUE;
  if (d(lcadre))
   break;
  lcadre = lcadre->NextCadre;
 }
 if (svglistp)
  svglistp->NextLpoint = NULL;
 d(svgllistp) = ssvglistp;
   
 d(prems) = FALSE;
 d(source) = TRUE;
 d(xorig) = retpnt[0].sX;
 d(yorig) = retpnt[0].sY;
 d(xcent) = retpnt[1].sX;
 d(ycent) = retpnt[1].sY;
 d(change_xfactor) = retpnt[0].cont;
 d(change_yfactor) = retpnt[1].cont;
 F(cfrmg).init_recoordinate_point(cfrmg);
 Resize_cadre_to_xy(cpage, xm, ym);
 Xc_free(retpnt);
}

void Resize_cadre_abort(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
 l_Cadre	*lcadre;
 l_Lpoint	*llpoint;
 l_Lpoint	*llpoint2;
  
 GET_DATAF(Select_Move_Edit_Text);
 Resize_cadre_trace(cpage);
 llpoint = d(svgllistp);
 if (d(lcadre))
  lcadre = d(lcadre);
 else
  lcadre = cfrmg->SelectedCadre;
 while(lcadre)
 {
  llpoint2 = llpoint->NextLpoint;
  free_list_point(lcadre->cadre->ListPoint);
  lcadre->cadre->ListPoint = llpoint->lpoint;
  Xc_free(llpoint);
  llpoint = llpoint2;
  F(cfrmg).draw_cadre_contour(cfrmg, lcadre);
  lcadre->cadre->in_work = FALSE;
  if (d(lcadre)) 
   break;
  lcadre = lcadre->NextCadre;
 }
 F(cfrmg).display_selected_lcadre(cfrmg);
 d(source) = FALSE;
}

void Resize_cadre_done(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
 l_Cadre	*lcadre;
 l_Lpoint	*llpoint;
 l_Lpoint	*llpoint2;
  
 GET_DATAF(Select_Move_Edit_Text);
 Resize_cadre_trace(cpage);
  
 llpoint = d(svgllistp);
 if (d(lcadre))
  lcadre = d(lcadre);
 else
  lcadre = cfrmg->SelectedCadre;
 while(lcadre)
 {
  llpoint2 = llpoint->NextLpoint;
  free_list_point(llpoint->lpoint);
  Xc_free(llpoint);
  llpoint = llpoint2;
  F(cfrmg).redisplay_cadre(cfrmg, lcadre);
  lcadre->cadre->in_work = FALSE;
  if (d(lcadre))
   break;
  lcadre = lcadre->NextCadre;
 }
 F(cfrmg).display_selected_lcadre(cfrmg);
 d(source) = FALSE;
}

void Resize_cadre_to_xy(cpage,xm,ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 l_Cadre	*lcadre;
 l_Lpoint	*llpoint;
 double	cs;
 double	ss;
 double	angle;
 coord_t	sxm,sym;
 coord_t	dx,dy;
 real		sx1,sy1;
 real		sx2,sy2;
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
  
 F(cfrmg).recoordinate_point(cfrmg, &d(xorig), &d(yorig), 0, 0, FALSE);
 F(cfrmg).recoordinate_point(cfrmg, &d(xcent), &d(ycent), 0, 0, TRUE);
  
 sxm = xm * cfrmg->sScale;
 sym = ym * cfrmg->sScale;
 F(cfrmg).attract_point(cfrmg, &sxm, &sym, &xm, &ym);
  
 if (d(prems))
  Resize_cadre_trace(cpage);
  
 if (d(lcadre)) 
  lcadre=d(lcadre);
 else
  lcadre=cfrmg->SelectedCadre;

 if (!d(lcadre))		/* || !d(lcadre)->cadre->sAngle)*/
 {
  if (d(change_xfactor))
   d(xfactor)=((float)(sxm-d(xcent))/((float)(d(xorig)-d(xcent))));
  else
   d(xfactor)=1;
  if (d(change_yfactor))
   d(yfactor)=((float)(sym-d(ycent))/((float)(d(yorig)-d(ycent))));
  else
   d(yfactor)=1;

  if (d(shift)) 
  {
   Xc_TRACE(("xfact: %f, yfact: %f",d(xfactor),d(yfactor)));
   if (d(xfactor)>d(yfactor))
    d(yfactor)=d(xfactor);
   else
    d(xfactor)=d(yfactor);
  }
      
  llpoint=d(svgllistp);
  while(lcadre)
  {
   scale_list_point(lcadre->cadre->ListPoint,
		    llpoint->lpoint,
		    cfrmg->sScale,
		    d(xfactor),d(yfactor),
		    d(xcent)-lcadre->cadre->sOrig_X,
		    d(ycent)-lcadre->cadre->sOrig_Y);
   llpoint=llpoint->NextLpoint;
   if (d(lcadre))
    break;
   lcadre=lcadre->NextCadre;
  }
 }
 else
 {
  angle=SCALE_TO_RADIANS(lcadre->cadre->sAngle);
  cs = cos(angle);
  ss = sin(angle);
  dx = d(xcent)-d(xorig);
  dy = d(ycent)-d(yorig);
  sx1 = -dx*cs+dy*ss;
  sy1 = -dy*cs-dx*ss;
  dx = d(xcent)-sxm;
  dy = d(ycent)-sym;
  sx2 = -dx*cs+dy*ss;
  sy2 = -dy*cs-dx*ss;
  if (d(change_xfactor))
   d(xfactor) = sx2/sx1;
  else
   d(xfactor) = 1;
  if (d(change_yfactor))
   d(yfactor) = sy2/sy1;
  else
   d(yfactor) = 1;
  llpoint = d(svgllistp);
  rotate_list_point(lcadre->cadre->ListPoint,
		    llpoint->lpoint,
		    cfrmg->sScale,
		    -lcadre->cadre->sAngle,
		    d(xcent)-lcadre->cadre->sOrig_X,
		    d(ycent)-lcadre->cadre->sOrig_Y);
  scale_list_point(lcadre->cadre->ListPoint,
		   lcadre->cadre->ListPoint,
		   cfrmg->sScale,
		   d(xfactor),d(yfactor),
		   d(xcent)-lcadre->cadre->sOrig_X,
		   d(ycent)-lcadre->cadre->sOrig_Y);
  rotate_list_point(lcadre->cadre->ListPoint,
		    lcadre->cadre->ListPoint,
		    cfrmg->sScale,
		    lcadre->cadre->sAngle,
		    d(xcent)-lcadre->cadre->sOrig_X,
		    d(ycent)-lcadre->cadre->sOrig_Y);
 }
 Resize_cadre_trace(cpage);
 d(prems) = TRUE;
}

void Resize_cadre_trace(cpage)
c_Page     *cpage;
{
 Display       *display;
 Window        window;
 GC            gc;
 l_Cadre	*lcadre;
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 display = cfrmg->X_info.display;
 window = cfrmg->X_info.window;
 gc = cfrmg->X_info.gc_contour;
 if (d(lcadre))
  lcadre = d(lcadre);
 else
  lcadre = cfrmg->SelectedCadre;
 while(lcadre)
 {
  trace_list_point(lcadre->cadre->ListPoint,display,window,gc,
		   lcadre->cadre->Forme->Orig_X,
		   lcadre->cadre->Forme->Orig_Y,TRUE);
  if (d(lcadre))
   break;
  lcadre = lcadre->NextCadre;
 }
}
