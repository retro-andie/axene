/*
** Move_Line_On_Cadre.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:01:23 1995 Emmanuel Paris
** Last update Fri Sep 26 19:27:53 1997 Emmanuel Paris
*/

#include "Select_Move_Resize_Cadres.h"

void move_line_on_cadre_init();
void move_line_on_cadre_abort();
void move_line_on_cadre_done();
void move_line_on_cadre_to_xy();
void move_line_on_cadre_trace();

void move_line_on_cadre_init(cpage,xm,ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 l_Cadre	*lcadre;
 l_Point	**retpnt;
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 lcadre = F(cpage).get_cadre_and_point_by_line(cpage,cpage->SelectedCadre,
					     &retpnt, xm, ym);
 d(point_d) = F(lcadre->cadre).getpreviouspoint(lcadre->cadre, retpnt[0]);
 d(point_e) = F(lcadre->cadre).getnextpoint(lcadre->cadre, retpnt[1]);
 d(xc) = d(xo) = xm;
 d(yc) = d(yo) = ym;
 d(sxc) = d(sxo) = d(xc)*cpage->sScale;
 d(syc) = d(syo) = d(yc)*cpage->sScale;
 d(lcadre) = lcadre;
 d(retpnt) = retpnt;
 d(prems) = FALSE;
 d(source) = TRUE;
 F(cpage).undisplay_selected_lcadre(cpage);
 lcadre->cadre->in_work = TRUE;
 F(cpage).init_recoordinate_point(cpage);
 move_line_on_cadre_to_xy(cpage, xm, ym);
}

void move_line_on_cadre_abort(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 move_line_on_cadre_trace(cpage);
 Xc_free(d(retpnt));
 d(lcadre)->cadre->in_work = FALSE;
 F(cpage).display_selected_lcadre(cpage);
 d(source) = FALSE;
}

void move_line_on_cadre_done(cpage)
c_Page	*cpage;
{
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 move_line_on_cadre_trace(cpage);
  
 d(retpnt)[0]->sX += d(sxc) - d(sxo);
 d(retpnt)[0]->sY += d(syc) - d(syo);
 d(retpnt)[1]->sX += d(sxc) - d(sxo);
 d(retpnt)[1]->sY += d(syc) - d(syo);
  
 d(retpnt)[0]->X = d(retpnt)[0]->sX / d(lcadre)->cadre->sScale;
 d(retpnt)[0]->Y = d(retpnt)[0]->sY / d(lcadre)->cadre->sScale;
 d(retpnt)[1]->X = d(retpnt)[1]->sX / d(lcadre)->cadre->sScale;
 d(retpnt)[1]->Y = d(retpnt)[1]->sY / d(lcadre)->cadre->sScale;
  
 F(cpage).redisplay_cadre(cpage, d(lcadre));
 Xc_free(d(retpnt));
 d(lcadre)->cadre->in_work = FALSE;
 F(cpage).display_selected_lcadre(cpage);
 d(source) = FALSE;
}

void move_line_on_cadre_to_xy(cpage, xm, ym)
c_Page	*cpage;
int	xm;
int	ym;
{
 coord_t	sox, soy, sdx, sdy;
 coord_t	svox, svoy, svdx, svdy;
 int		ox, oy, dx, dy;
 int		vox, voy, vdx, vdy;
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 if (d(prems))
 {
  move_line_on_cadre_trace(cpage);
 }
 d(xc) = xm;
 d(yc) = ym;
 d(sxc) = d(xc) * cpage->sScale;
 d(syc) = d(yc) * cpage->sScale;
  
 svox = sox = d(retpnt)[0]->sX + d(lcadre)->cadre->sOrig_X + d(sxc) - d(sxo);
 svoy = soy = d(retpnt)[0]->sY + d(lcadre)->cadre->sOrig_Y + d(syc) - d(syo);
 vox = ox = svox / cpage->sScale;
 voy = oy = svoy / cpage->sScale;
 F(cpage).attract_point(cpage, &sox, &soy, &ox, &oy);

 svdx = sdx = d(retpnt)[1]->sX + d(lcadre)->cadre->sOrig_X + d(sxc) - d(sxo);
 svdy = sdy = d(retpnt)[1]->sY + d(lcadre)->cadre->sOrig_Y + d(syc) - d(syo);
 vdx = dx = svdx / cpage->sScale;
 vdy = dy = svdy / cpage->sScale;
 F(cpage).attract_point(cpage, &sdx, &sdy, &dx, &dy);

 if (ABS(sox - svox) >= ABS(sdx - svdx))
  d(sxc) += sox - svox;
 else
  d(sxc) += sdx - svdx;
  
 if (ABS(soy - svoy) >= ABS(sdy - svdy))
  d(syc) += soy - svoy;
 else
  d(syc) += sdy - svdy;

 d(xc) = d(sxc) / cpage->sScale;
 d(yc) = d(syc) / cpage->sScale;
  
 move_line_on_cadre_trace(cpage);
 d(prems) = TRUE;
}

void move_line_on_cadre_trace(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window	window;
 GC		gc;
 int		ox,oy;
 int		xa,ya;
 int		xb,yb;
 DATA(Select_Move_Resize_Cadre);
  
 GET_DATA(Select_Move_Resize_Cadre);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sxo), &d(syo), &d(xo), &d(yo), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sxc), &d(syc), &d(xc), &d(yc), TRUE);
  
 ox = d(lcadre)->cadre->Forme->Orig_X;
 oy = d(lcadre)->cadre->Forme->Orig_Y;
  
 xa = d(retpnt)[0]->X + ox + d(xc) - d(xo);
 ya = d(retpnt)[0]->Y + oy + d(yc) - d(yo);
 if (d(point_d) && d(point_d) != d(retpnt)[1])
  XDrawLine(display, window, gc, xa, ya, d(point_d)->X+ox, d(point_d)->Y+oy);
 xb = d(retpnt)[1]->X + ox + d(xc) - d(xo);
 yb = d(retpnt)[1]->Y + oy + d(yc) - d(yo);
 if (d(point_e) && d(point_e) != d(retpnt)[0])
  XDrawLine(display, window, gc, xb, yb, d(point_e)->X + ox, d(point_e)->Y+oy);
 XDrawLine(display, window, gc, xa, ya, xb, yb);
}




