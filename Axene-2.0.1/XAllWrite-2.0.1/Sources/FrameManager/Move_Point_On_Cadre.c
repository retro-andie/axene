/*
** Move_Point_On_Cadre.c for XAllWrite in FrameManager/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jul 17 20:00:21 1995 Emmanuel Paris
** Last update Thu Jun 25 19:19:14 1998 Emmanuel Paris
*/

#include "Select_Move_Edit_Text.h"

void move_point_on_cadre_init();
void move_point_on_cadre_abort();
void move_point_on_cadre_done();
void move_point_on_cadre_to_xy();
void move_point_on_cadre_trace();

void move_point_on_cadre_init(cpage, xm, ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 l_Cadre	*lcadre;
 l_Point	**retpnt;
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 lcadre = F(cfrmg).get_cadre_and_point_by_xy(cfrmg, cfrmg->SelectedCadre, 
					   &retpnt, xm, ym);
 d(lcadre) = lcadre;
 d(retpnt) = retpnt;
 d(prems) = FALSE;
 d(source) = TRUE;
 F(cfrmg).undisplay_selected_lcadre(cfrmg);
 lcadre->cadre->in_work = TRUE;
 F(cfrmg).init_recoordinate_point(cfrmg);
 move_point_on_cadre_to_xy(cpage, xm, ym);
}

void move_point_on_cadre_abort(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 d(lcadre)->cadre->in_work = FALSE;
 move_point_on_cadre_trace(cpage);
 Xc_free(d(retpnt));
 F(cfrmg).display_selected_lcadre(cfrmg);
 d(source) = FALSE;
}

void move_point_on_cadre_done(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 move_point_on_cadre_trace(cpage);

 d(retpnt)[0]->sX = d(sxc) - d(lcadre)->cadre->sOrig_X;
 d(retpnt)[0]->sY = d(syc) - d(lcadre)->cadre->sOrig_Y;
 d(retpnt)[0]->X = d(retpnt)[0]->sX / cfrmg->sScale;
 d(retpnt)[0]->Y = d(retpnt)[0]->sY / cfrmg->sScale;

 F(cfrmg).redisplay_cadre(cfrmg, d(lcadre));
 Xc_free(d(retpnt));
 d(lcadre)->cadre->in_work = FALSE;
 F(cfrmg).display_selected_lcadre(cfrmg);

 d(source) = FALSE;
}

void move_point_on_cadre_to_xy(cpage, xm, ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 if (d(prems))
 {
  move_point_on_cadre_trace(cpage);
 }
 d(xc) = xm;
 d(yc) = ym;
 d(sxc) = d(xc) * cfrmg->sScale;
 d(syc) = d(yc) * cfrmg->sScale;
 F(cfrmg).attract_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc));
 move_point_on_cadre_trace(cpage);
 d(prems) = TRUE;
}

void move_point_on_cadre_trace(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window	window;
 GC		gc;
 int		ox, oy;
 DATA(Select_Move_Edit_Text);
  
 GET_DATAF(Select_Move_Edit_Text);
 display = cfrmg->X_info.display;
 window = cfrmg->X_info.window;
 gc = cfrmg->X_info.gc_contour;
  
 F(cfrmg).recoordinate_point(cfrmg, &d(sxc), &d(syc), &d(xc), &d(yc), TRUE);
  
 ox = d(lcadre)->cadre->Forme->Orig_X;
 oy = d(lcadre)->cadre->Forme->Orig_Y;
  
 if (d(retpnt)[1])
  XDrawLine(display, window, gc, d(xc), d(yc), 
	    d(retpnt)[1]->X + ox, d(retpnt)[1]->Y + oy);
 if (d(retpnt)[2])
  XDrawLine(display, window, gc, 
	    d(retpnt)[2]->X + ox, d(retpnt)[2]->Y + oy, d(xc), d(yc));
}




