/*
** Scale_Cadres.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:55:35 1995 Emmanuel Paris
** Last update Mon Jul 17 19:55:35 1995 Emmanuel Paris
*/

#include "Scale_Cadres.h"

/*				*
 *    Scale CADRE FUNCTION     *
 *				*/

void init_Scale_Cadre();
status_t eventlook_Scale_Cadre();
void abort_Scale_Cadre();
void redraw_Scale_Cadre();
void Scale_Cadre_Done();
void Scale_Cadre_Calcul();
void Scale_Cadre_Trace();
void Scale_Cadre_Plot_Center();

void init_Scale_Cadre(cpage)
c_Page	*cpage;
{
  boolean	prems;
  l_Cadre	*lcadre;
  l_Lpoint	*svglistp;
  l_Lpoint	*ssvglistp;
  Window	root;
  Window	fille;
  int		xrac,yrac;
  int		xm,ym;
  unsigned int	mod_button;
  DATA(Scale_Cadre);
  
  SET_DATA(Scale_Cadre);
  F(GlobColormap).use_color(GlobColormap,cpage->X_info.gc_contour,BLACK);
  d(set_center)=d(source)=FALSE;
  prems=TRUE;
  svglistp=ssvglistp=NULL;
  lcadre=cpage->SelectedCadre;
  while(lcadre)
    {
      if (prems)
	{
	  svglistp=(l_Lpoint *)Xc_malloc("llpoint",sizeof(l_Lpoint));
	  ssvglistp=svglistp;
	  prems=FALSE;
	}
      else
	{
	  svglistp->NextLpoint=
	    (l_Lpoint *)Xc_malloc("llpoint",sizeof(l_Lpoint));
	  svglistp=svglistp->NextLpoint;
	}
      svglistp->lpoint=duplicate_list_point(lcadre->cadre->ListPoint);
      lcadre=lcadre->NextCadre;
    }
  if (svglistp)
    svglistp->NextLpoint=NULL;
  d(svglistp)=ssvglistp;
  
  F(cpage).get_center_list_cadre(cpage->SelectedCadre,&d(xcent),&d(ycent));
  d(xc)=d(xcent)/cpage->sScale;
  d(yc)=d(ycent)/cpage->sScale;
  Scale_Cadre_Plot_Center(cpage);
  XQueryPointer(cpage->X_info.display,cpage->X_info.window,&root,&fille,
		&xrac,&yrac,&xm,&ym,&mod_button);
  if ( d(xc)-7<xm && d(xc)+7>xm && d(yc)-7<ym && d(yc)+7>ym)
  {
    F(CURSOR).push_and_set_cursor(CURSOR,C_SCALE);
    d(center)=TRUE;
  }
  else
  {
    F(CURSOR).push_and_set_cursor(CURSOR,C_SCALE_POINTER);
    d(center)=FALSE;
  }
  F(cpage).init_recoordinate_point(cpage);
}

status_t eventlook_Scale_Cadre(cpage,event)
c_Page	*cpage;
XEvent	event;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  int		xm,ym;
  status_t	retour_func;
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  F(cpage).recoordinate_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc),FALSE);
  F(cpage).recoordinate_point(cpage,&d(sxm),&d(sym),&d(xm),&d(ym),TRUE);
  
  retour_func=FUNCTION_NOT_FINISHED;
  switch(event.type)
    {
    case MotionNotify:
      xm=event.xmotion.x;
      ym=event.xmotion.y;
      if (d(source))
      {
	Scale_Cadre_Trace(cpage);
	d(xm)=xm;
	d(ym)=ym;
	d(sxm)=d(xm)*cpage->sScale;
	d(sym)=d(ym)*cpage->sScale;
	Scale_Cadre_Calcul(cpage);
	Scale_Cadre_Trace(cpage);
      }
      else
	if (!d(set_center))
	{
	  if ( d(xc)-7<xm && d(xc)+7>xm && d(yc)-7<ym && d(yc)+7>ym)
	  {
	    F(CURSOR).set_cursor(CURSOR,C_SCALE);
	    d(center)=TRUE;
	  }
	  else
	  {
	    F(CURSOR).set_cursor(CURSOR,C_SCALE_POINTER);
	    d(center)=FALSE;
	  }
	}
      break;
    case ButtonRelease:
      if (event.xbutton.time>d(time)+600)
      {
	if (d(source))
	{
	  Scale_Cadre_Done(cpage);
	  F(CURSOR).pop_cursor(CURSOR);
	  FREE_DATA;
	  
	  retour_func=FUNCTION_FINISHED;
	}
	else
	  if (d(set_center))
	  {
	    d(xc)=event.xbutton.x;
	    d(yc)=event.xbutton.y;
	    d(xcent)=d(xc)*cpage->sScale;
	    d(ycent)=d(yc)*cpage->sScale;
	    F(cpage).attract_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc));
	    Scale_Cadre_Plot_Center(cpage);
	    d(set_center)=FALSE;
	  }  
      }
      break;
    case ButtonPress:
      switch(event.xbutton.button)
      {
      case 1:
	if (d(source))
	{
	  Scale_Cadre_Done(cpage);
	  F(CURSOR).pop_cursor(CURSOR);
	  FREE_DATA;
	  
	  retour_func=FUNCTION_FINISHED;
	}
	else
	{
	  if (d(set_center))
	  {
	    d(xc)=event.xbutton.x;
	    d(yc)=event.xbutton.y;
	    d(xcent)=d(xc)*cpage->sScale;
	    d(ycent)=d(yc)*cpage->sScale;
	    F(cpage).attract_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc));
	    Scale_Cadre_Plot_Center(cpage);
	    d(set_center)=FALSE;
	  }
	  else
	  {
	    if (d(center))
	    {
	      Scale_Cadre_Plot_Center(cpage);
	      d(set_center)=TRUE;
	      d(time)=event.xbutton.time;
	    }
	    else
	    {
	      d(time)=event.xbutton.time;
	      d(source)=TRUE;
	      d(xm)=event.xbutton.x;
	      d(ym)=event.xbutton.y;
	      d(sxm)=d(xm)*cpage->sScale;
	      d(sym)=d(ym)*cpage->sScale;
	      F(cpage).undisplay_selected_lcadre(cpage);
	      lcadre=cpage->SelectedCadre;
	      while(lcadre)
	      {	
		F(lcadre->cadre).set_contour_action(lcadre->cadre,
						    GFX_CLEAR);
		F(cpage).draw_cadre_contour(cpage,lcadre);
		F(lcadre->cadre).set_contour_action(lcadre->cadre,
						    GFX_SET);
		lcadre->cadre->in_work=TRUE;
		lcadre=lcadre->NextCadre;
	      }
	      Scale_Cadre_Calcul(cpage);
	      Scale_Cadre_Trace(cpage);
	      F(CURSOR).set_cursor(CURSOR,C_SCALE_POINTER);
	    }
	  }
	}
	break;
      case 2:
	if (!d(source) && !d(set_center))
	{
	  xm=event.xbutton.x;
	  ym=event.xbutton.y;
	  Scale_Cadre_Plot_Center(cpage);
	  F(cpage).get_center_list_cadre(cpage->SelectedCadre,
					 &d(xcent),&d(ycent));
	  d(xc)=d(xcent)/cpage->sScale;
	  d(yc)=d(ycent)/cpage->sScale;
	  Scale_Cadre_Plot_Center(cpage);
	  if ( d(xc)-7<xm && d(xc)+7>xm && d(yc)-7<ym && d(yc)+7>ym)
	  {
	    F(CURSOR).set_cursor(CURSOR,C_SCALE);
	    d(center)=TRUE;
	  }
	  else
	  {
	    F(CURSOR).set_cursor(CURSOR,C_SCALE_POINTER);
	    d(center)=FALSE;
	  }
	}
	break;
      case 3:
	if (d(source))
	  retour_func=FUNCTION_ABORTED;
	else
	  retour_func=FUNCTION_REINIT;
	break;
      }
      break;
    }
  return retour_func;
}

void abort_Scale_Cadre(cpage)
     c_Page	*cpage;
{ 
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  l_Lpoint	*llpoint;
  l_Lpoint	*llpoint2;
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  F(cpage).recoordinate_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc),FALSE);
  F(cpage).recoordinate_point(cpage,&d(sxm),&d(sym),&d(xm),&d(ym),TRUE);
  
  Scale_Cadre_Plot_Center(cpage);
  if (d(source))
  {
    Scale_Cadre_Trace(cpage);
  }    
  llpoint=d(svglistp);
  lcadre=cpage->SelectedCadre;
  while(lcadre)
  {
    llpoint2=llpoint->NextLpoint;
    free_list_point(lcadre->cadre->ListPoint);
    lcadre->cadre->ListPoint=llpoint->lpoint;
    Xc_free(llpoint);
    llpoint=llpoint2;
    if (d(source))
      F(cpage).draw_cadre_contour(cpage,lcadre);
    lcadre->cadre->in_work=FALSE;
    lcadre=lcadre->NextCadre;
  }
  if (d(source))
    F(cpage).display_selected_lcadre(cpage);
  FREE_DATA;
  F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Scale_Cadre(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  F(cpage).recoordinate_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc),FALSE);
  F(cpage).recoordinate_point(cpage,&d(sxm),&d(sym),&d(xm),&d(ym),TRUE);
  
  Scale_Cadre_Plot_Center(cpage);
  if (d(source))
  {
      Scale_Cadre_Trace(cpage);
  }
}

void Scale_Cadre_Done(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  l_Lpoint	*llpoint;
  l_Lpoint	*llpoint2;
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  Scale_Cadre_Trace(cpage);
  Scale_Cadre_Plot_Center(cpage);
  
  llpoint=d(svglistp);
  lcadre=cpage->SelectedCadre;
  while(lcadre)
  {
    llpoint2=llpoint->NextLpoint;
    free_list_point(llpoint->lpoint);
    Xc_free(llpoint);
    llpoint=llpoint2;
    F(cpage).redisplay_cadre(cpage,lcadre);
    lcadre->cadre->in_work=FALSE;
    lcadre=lcadre->NextCadre;
  }
  F(cpage).display_selected_lcadre(cpage);
}

void Scale_Cadre_Calcul(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Lpoint	*llpoint;
  real		xp2,yp2;
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
/*Xc_TRACE(("AVANT calcul"));*/
  xp2=(d(sxm)-d(xcent))/cpage->sScale;
  yp2=(d(ycent)-d(sym))/cpage->sScale;
  d(factor)=sqrt(yp2*yp2+xp2*xp2)/100.0;
/*Xc_TRACE(("APRES calcul....%f",d(factor)));*/
  lcadre=cpage->SelectedCadre;
  llpoint=d(svglistp);
  while(lcadre)
  {
    scale_list_point(lcadre->cadre->ListPoint,
		      llpoint->lpoint,
		      cpage->sScale,d(factor),d(factor),
		      d(xcent)-lcadre->cadre->sOrig_X,
		      d(ycent)-lcadre->cadre->sOrig_Y);
    llpoint=llpoint->NextLpoint;
    lcadre=lcadre->NextCadre;
  }
}

void Scale_Cadre_Trace(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;

  XDrawLine(display,window,gc,d(xc),d(yc),d(xm),d(ym));
  lcadre=cpage->SelectedCadre;
  while(lcadre)
    {
      trace_list_point(lcadre->cadre->ListPoint,display,window,gc,
                   lcadre->cadre->Forme->Orig_X,
                   lcadre->cadre->Forme->Orig_Y,TRUE);
      lcadre=lcadre->NextCadre;
    }
}

void Scale_Cadre_Plot_Center(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  XSegment	segment[14];
  DATA(Scale_Cadre);
  
  GET_DATA(Scale_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  segment[0].x1=d(xc)-7;segment[0].y1=d(yc)-7;
  segment[0].x2=d(xc)+8;segment[0].y2=d(yc)+8;
  segment[1].x1=d(xc)-6;segment[1].y1=d(yc)-7;
  segment[1].x2=d(xc)+8;segment[1].y2=d(yc)+7;
  segment[2].x1=d(xc)-7;segment[2].y1=d(yc)-6;
  segment[2].x2=d(xc)+7;segment[2].y2=d(yc)+8;
  segment[3].x1=d(xc)-7;segment[3].y1=d(yc)+7;
  segment[3].x2=d(xc)+8;segment[3].y2=d(yc)-8;
  segment[4].x1=d(xc)-6;segment[4].y1=d(yc)+7;
  segment[4].x2=d(xc)+8;segment[4].y2=d(yc)-7;
  segment[5].x1=d(xc)-7;segment[5].y1=d(yc)+6;
  segment[5].x2=d(xc)+7;segment[5].y2=d(yc)-8;
  segment[6].x1=d(xc)-5;segment[6].y1=d(yc)-7;
  segment[6].x2=d(xc)-2;segment[6].y2=d(yc)-7;
  segment[7].x1=d(xc)-7;segment[7].y1=d(yc)-5;
  segment[7].x2=d(xc)-7;segment[7].y2=d(yc)-2;
  segment[8].x1=d(xc)+5;segment[8].y1=d(yc)-7;
  segment[8].x2=d(xc)+2;segment[8].y2=d(yc)-7;
  segment[9].x1=d(xc)+7;segment[9].y1=d(yc)-5;
  segment[9].x2=d(xc)+7;segment[9].y2=d(yc)-2;
  segment[10].x1=d(xc)-5;segment[10].y1=d(yc)+7;
  segment[10].x2=d(xc)-2;segment[10].y2=d(yc)+7;
  segment[11].x1=d(xc)-7;segment[11].y1=d(yc)+5;
  segment[11].x2=d(xc)-7;segment[11].y2=d(yc)+2;
  segment[12].x1=d(xc)+5;segment[12].y1=d(yc)+7;
  segment[12].x2=d(xc)+2;segment[12].y2=d(yc)+7;
  segment[13].x1=d(xc)+7;segment[13].y1=d(yc)+5;
  segment[13].x2=d(xc)+7;segment[13].y2=d(yc)+2;
  
  XDrawSegments(display,window,gc,segment,14);
  
}
