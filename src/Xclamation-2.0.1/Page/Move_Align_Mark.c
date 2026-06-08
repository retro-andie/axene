/*
** Move_Align_Mark.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:02:22 1995 Emmanuel Paris
** Last update Mon Jul 17 20:02:22 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cadres.h"

void move_align_mark_init();
void move_align_mark_abort();
void move_align_mark_done();
void move_align_mark_to_xy();
void move_align_mark_trace();

void move_align_mark_init(cpage,xm,ym)
     c_Page	*cpage;
     int	xm;
     int	ym;
{ 
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  d(vhline)=F(cpage).get_page_align_mark(cpage,xm,ym,&d(orient));
  Xc_TRACE(("ORIENTATION OF ALIGN MARK: %s",d(orient)==ALIGN_MARK_HORIZONTAL?
	    "horizontal":"vertical"));
  d(prems)=FALSE;
  d(source)=TRUE;
  F(cpage->RegleV).Clear_Repere(cpage->RegleV, REGLE_REPERE_2);
  F(cpage->RegleH).Clear_Repere(cpage->RegleH, REGLE_REPERE_2);
  cpage->RegleV->rep1_actif=TRUE;
  cpage->RegleH->rep1_actif=TRUE;
  move_align_mark_to_xy(cpage,xm,ym);
}

void move_align_mark_abort(cpage)
c_Page	*cpage;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  if (d(prems))
    move_align_mark_trace(cpage);
  cpage->RegleV->rep1_actif=FALSE;
  cpage->RegleH->rep1_actif=FALSE;
  F(cpage->RegleV).Draw_Repere(cpage->RegleV, REGLE_REPERE_2,d(yc));
  F(cpage->RegleH).Draw_Repere(cpage->RegleH, REGLE_REPERE_2,d(xc));
  d(source)=FALSE;
}

void move_align_mark_done(cpage)
c_Page	*cpage;
{
  coord_t	sxc;
  coord_t	syc;

  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);
  
  if (d(prems))
    move_align_mark_trace(cpage);
  d(source)=FALSE;
  
  sxc = d(xc)*cpage->sScale+cpage->sOrig_X;
  syc = d(yc)*cpage->sScale+cpage->sOrig_Y;
  
  if (d(orient)==ALIGN_MARK_HORIZONTAL)
    {  
      if (d(yc)>=0 && d(yc)<cpage->Height)
	F(cpage).move_page_align_mark(cpage, d(vhline), d(orient), sxc, syc);
      else
	F(cpage).remove_page_align_mark(cpage,d(vhline),d(orient));
    }
  else
    { 
      if (d(xc)>=0 && d(xc)<cpage->Width)
	F(cpage).move_page_align_mark(cpage, d(vhline), d(orient), sxc, syc);
      else
	F(cpage).remove_page_align_mark(cpage,d(vhline),d(orient));
    }
  cpage->RegleV->rep1_actif=FALSE;
  cpage->RegleH->rep1_actif=FALSE;
  F(cpage->RegleV).Draw_Repere(cpage->RegleV, REGLE_REPERE_2,d(yc));
  F(cpage->RegleH).Draw_Repere(cpage->RegleH, REGLE_REPERE_2,d(xc));
}

void move_align_mark_to_xy(cpage,xm,ym)
c_Page	*cpage;
int	xm;
int	ym;
{
  DATA(Select_Move_Resize_Cadre);
  
  GET_DATA(Select_Move_Resize_Cadre);

  if (d(prems))
    move_align_mark_trace(cpage);  
  d(xc)=xm;
  d(yc)=ym;
  d(prems)=FALSE;
  move_align_mark_trace(cpage);
}

void move_align_mark_trace(cpage)
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

  if (d(orient)==ALIGN_MARK_HORIZONTAL)
    {
      if (d(prems))
	{
	  F(cpage->RegleV).Clear_Repere(cpage->RegleV, REGLE_REPERE_1);
	  XDrawLine(display, window, gc, 0, d(yc),
		    cpage->RegleH->Size, d(yc));
	}
      else
	{
	  if (d(yc)>=0 && d(yc)<cpage->Height)
	    {
	      F(cpage->RegleV).Draw_Repere(cpage->RegleV,
					   REGLE_REPERE_1, d(yc));
	      XDrawLine(display, window, gc, 0, d(yc),
			cpage->RegleH->Size,d(yc));
	      d(prems)=TRUE;
	    }
	  else
	    d(prems)=FALSE;
	}
    }
  else
    {
      if (d(prems))
	{
	  F(cpage->RegleH).Clear_Repere(cpage->RegleH, REGLE_REPERE_1);
	  XDrawLine(display, window, gc, d(xc), 0,
		    d(xc), cpage->RegleV->Size);
	}
      else
	{
	  if (d(xc)>=0 && d(xc)<cpage->Width)
	    {
	      F(cpage->RegleH).Draw_Repere(cpage->RegleH,
					   REGLE_REPERE_1, d(xc));
	      XDrawLine(display, window, gc, d(xc), 0,
			d(xc), cpage->RegleV->Size);
	      d(prems)=TRUE;
	    }
	  else
	    d(prems)=FALSE;
	}
    }
}

