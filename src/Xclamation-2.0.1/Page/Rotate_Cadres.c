/*
** Rotate_Cadres.c for Xclamation in Page/
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
** Started on  Mon Jul 17 19:56:30 1995 Emmanuel Paris
** Last update Thu Feb 22 14:25:35 1996 Emmanuel Paris
*/

#include "Rotate_Cadres.h"

/*				*
 *    ROTATE CADRE FUNCTION     *
 *				*/

void init_Rotate_Cadre();
status_t eventlook_Rotate_Cadre();
void abort_Rotate_Cadre();
void redraw_Rotate_Cadre();
void Rotate_Cadre_Done();
void Rotate_Cadre_Calcul();
void Rotate_Cadre_Trace();
void Rotate_Cadre_Plot_Center();

void init_Rotate_Cadre(cpage)
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
  DATA(Rotate_Cadre);
  
  SET_DATA(Rotate_Cadre);
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
  Rotate_Cadre_Plot_Center(cpage);
  XQueryPointer(cpage->X_info.display,cpage->X_info.window,&root,&fille,
		&xrac,&yrac,&xm,&ym,&mod_button);
  if ( d(xc)-5<xm && d(xc)+5>xm && d(yc)-5<ym && d(yc)+5>ym)
  {
    F(CURSOR).push_and_set_cursor(CURSOR,C_ROTATE_CENTER);
    d(center)=TRUE;
  }
  else
  {
    F(CURSOR).push_and_set_cursor(CURSOR,C_ROTATE_POINTER);
    d(center)=FALSE;
  }
  d(shift) = ((mod_button & ShiftMask)!=0);
  F(cpage).init_recoordinate_point(cpage);
}

status_t eventlook_Rotate_Cadre(cpage,event)
c_Page	*cpage;
XEvent	event;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  int		xm,ym;
  char		tamp_cla[20];
  int		tailtamp=20;
  KeySym	symtouche;
  int		nb_car;
  XComposeStatus	seqtouche;
  status_t	retour_func;
  DATA(Rotate_Cadre);
  
  GET_DATA(Rotate_Cadre);
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
	  Rotate_Cadre_Trace(cpage);
	  d(xm)=xm;
	  d(ym)=ym;
	  d(sxm)=d(xm)*cpage->sScale;
	  d(sym)=d(ym)*cpage->sScale;
	  Rotate_Cadre_Calcul(cpage);
	  Rotate_Cadre_Trace(cpage);
	}
      else
	if (!d(set_center))
	{
	  if ( d(xc)-5<xm && d(xc)+5>xm && d(yc)-5<ym && d(yc)+5>ym)
	  {
	    F(CURSOR).set_cursor(CURSOR,C_ROTATE_CENTER);
	    d(center)=TRUE;
	  }
	  else
	  {
	    F(CURSOR).set_cursor(CURSOR,C_ROTATE_POINTER);
	    d(center)=FALSE;
	  }
	}
      break;
    case KeyPress:
      nb_car=XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			   &symtouche,&seqtouche);
      if (symtouche==XK_Shift_R || symtouche==XK_Shift_L)
	{
	  d(shift) = TRUE;
	  if (d(source))
	    {
	      Rotate_Cadre_Trace(cpage);
	      Rotate_Cadre_Calcul(cpage);
	      Rotate_Cadre_Trace(cpage);
	    }
	}
      break;
    case KeyRelease:
      nb_car=XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			   &symtouche,&seqtouche);
      if (symtouche==XK_Shift_R || symtouche==XK_Shift_L)
	{
	  d(shift) = FALSE;
	  if (d(source))
	    {
	      Rotate_Cadre_Trace(cpage);
	      Rotate_Cadre_Calcul(cpage);
	      Rotate_Cadre_Trace(cpage);
	    }
	}
      break;
    case ButtonRelease:
      if (event.xbutton.time>d(time)+600)
      {
	if (d(source))
	{
	  Rotate_Cadre_Done(cpage);
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
	    Rotate_Cadre_Plot_Center(cpage);
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
	  Rotate_Cadre_Done(cpage);
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
	    Rotate_Cadre_Plot_Center(cpage);
	    d(set_center)=FALSE;
	  }
	  else
	  {
	    if (d(center))
	    {
	      Rotate_Cadre_Plot_Center(cpage);
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
	      Rotate_Cadre_Calcul(cpage);
	      Rotate_Cadre_Trace(cpage);
	      F(CURSOR).set_cursor(CURSOR,C_ROTATE_POINTER);
	    }
	  }
	}
	break;
      case 2:
	if (!d(source) && !d(set_center))
	{
	  xm=event.xbutton.x;
	  ym=event.xbutton.y;
	  d(sxm)=d(xm)*cpage->sScale;
	  d(sym)=d(ym)*cpage->sScale;
	  Rotate_Cadre_Plot_Center(cpage);
	  F(cpage).get_center_list_cadre(cpage->SelectedCadre,
					 &d(xcent),&d(ycent));
	  d(xc)=d(xcent)/cpage->sScale;
	  d(yc)=d(ycent)/cpage->sScale;
	  Rotate_Cadre_Plot_Center(cpage);
	  if ( d(xc)-5<xm && d(xc)+5>xm && d(yc)-5<ym && d(yc)+5>ym)
	  {
	    F(CURSOR).set_cursor(CURSOR,C_ROTATE_CENTER);
	    d(center)=TRUE;
	  }
	  else
	  {
	    F(CURSOR).set_cursor(CURSOR,C_ROTATE_POINTER);
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

void abort_Rotate_Cadre(cpage)
c_Page	*cpage;
{ 
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  l_Lpoint	*llpoint;
  l_Lpoint	*llpoint2;
  DATA(Rotate_Cadre);
  
  GET_DATA(Rotate_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;

  F(cpage).recoordinate_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc),FALSE);
  F(cpage).recoordinate_point(cpage,&d(sxm),&d(sym),&d(xm),&d(ym),TRUE);
  
  Rotate_Cadre_Plot_Center(cpage);
  if (d(source))
  {
    Rotate_Cadre_Trace(cpage);
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

void redraw_Rotate_Cadre(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Rotate_Cadre);
  
  GET_DATA(Rotate_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;

  F(cpage).recoordinate_point(cpage,&d(xcent),&d(ycent),&d(xc),&d(yc),FALSE);
  F(cpage).recoordinate_point(cpage,&d(sxm),&d(sym),&d(xm),&d(ym),TRUE);
  
  Rotate_Cadre_Plot_Center(cpage);
  if (d(source))
  {
    Rotate_Cadre_Trace(cpage);
  }
}

void Rotate_Cadre_Done(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  l_Lpoint	*llpoint;
  l_Lpoint	*llpoint2;
  DATA(Rotate_Cadre);
  
  GET_DATA(Rotate_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  
  Rotate_Cadre_Trace(cpage);
  Rotate_Cadre_Plot_Center(cpage);
  
  llpoint=d(svglistp);
  lcadre=cpage->SelectedCadre;
  while(lcadre)
  {
    llpoint2=llpoint->NextLpoint;
    free_list_point(llpoint->lpoint);
    Xc_free(llpoint);
    llpoint=llpoint2;
    lcadre->cadre->sAngle+=d(angle);
    F(cpage).redisplay_cadre(cpage,lcadre);
    lcadre->cadre->in_work=FALSE;
    lcadre=lcadre->NextCadre;
  }
  F(cpage).display_selected_lcadre(cpage);
}

void Rotate_Cadre_Calcul(cpage)
     c_Page	*cpage;
{
  l_Cadre	*lcadre;
  l_Lpoint	*llpoint;
  int		quartier;
  coord_t	rayon;
  angle_t	anglea;
  real		cs, ss, dx, dy;
  DATA(Rotate_Cadre); 
  
  GET_DATA(Rotate_Cadre);
  d(angle)=SCALE_FROM_RADIANS(atan2((double)(d(ycent)-d(sym)),
				    (double)(d(sxm)-d(xcent))));
  if (d(shift))
    {
      quartier = (SCALE_TO_DEGREES(d(angle)) + 382.5) / 45;
      d(angle) = SCALE_FROM_DEGREES(quartier * 45);
      dx = d(sxm) - d(xcent);
      dy = d(sym) - d(ycent);
      rayon = (coord_t)(sqrt(dx*dx + dy*dy));
      cs = cos(SCALE_TO_RADIANS(d(angle)));
      ss = sin(SCALE_TO_RADIANS(d(angle)));
      d(sxm) = d(xcent) + cs * rayon;
      d(sym) = d(ycent) - ss * rayon;
      d(xm) = d(sxm) / cpage->sScale;
      d(ym) = d(sym) / cpage->sScale;
    }
  lcadre=cpage->SelectedCadre;
  if (!lcadre->NextCadre)
    d(angle) -= lcadre->cadre->sAngle;
  else
    {	/* check if all frames have the same angle */
      anglea = lcadre->cadre->sAngle;
      while(lcadre && lcadre->cadre->sAngle == anglea)
	lcadre = lcadre->NextCadre;
      if (!lcadre)
	d(angle) -= anglea;
      lcadre=cpage->SelectedCadre;
    }
  llpoint=d(svglistp);
  while(lcadre)
  {
    rotate_list_point(lcadre->cadre->ListPoint,
		      llpoint->lpoint,
		      cpage->sScale,d(angle),
		      d(xcent)-lcadre->cadre->sOrig_X,
		      d(ycent)-lcadre->cadre->sOrig_Y);
    llpoint=llpoint->NextLpoint;
    lcadre=lcadre->NextCadre;
  }
}

void Rotate_Cadre_Trace(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre	*lcadre;
  DATA(Rotate_Cadre);
  
  GET_DATA(Rotate_Cadre);
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

void Rotate_Cadre_Plot_Center(cpage)
c_Page	*cpage;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Rotate_Cadre);
  
  GET_DATA(Rotate_Cadre);
  display=cpage->X_info.display;
  window=cpage->X_info.window;
  gc=cpage->X_info.gc_contour;
  XDrawArc(display,window,gc,d(xc)-7,d(yc)-7,14,14,0,23040);
  XDrawLine(display,window,gc,d(xc)-9,d(yc),d(xc)+10,d(yc));
  XDrawLine(display,window,gc,d(xc),d(yc)-9,d(xc),d(yc)+10);
}
