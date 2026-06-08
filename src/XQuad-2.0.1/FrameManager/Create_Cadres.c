/*
** Create_Cadres.c for XQuad in FrameManager/
** define Dispatch to FrameManager function
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
** Started on  Tue Mar 28 13:43:16 1995 Emmanuel Paris
** Last update Mon Jan  8 11:18:34 1996 
*/

#include "Create_Cadres.h"

void init_Create_Cadre_Rectangle();
status_t eventlook_Create_Cadre_Rectangle();
void abort_Create_Cadre_Rectangle();
void redraw_Create_Cadre_Rectangle();
void Create_Cadre_Rectangle_Done();
static void calcul_rectangle();

/*
 *   CREATE CADRE RECTANGLE FUNCTION    *
 *                               	*/

void init_Create_Cadre_Rectangle(cfrmg)
c_FrMg	*cfrmg;
{
  Window	root;
  Window	fille;
  int		xrac,yrac;
  int		xm,ym;
  unsigned int	mod_button;
  DATA(Create_Cadre_Rectangle);
  
  SET_DATA(Create_Cadre_Rectangle);
  
  F(CURSOR).push_and_set_cursor(CURSOR,C_CREATE_RECTANGLE);
  F(GlobColormap).use_color(GlobColormap,cfrmg->X_info.gc_contour,BLACK);
  d(source)=d(prems)=FALSE;
    XQueryPointer(cfrmg->X_info.display,cfrmg->X_info.window,&root,&fille,
		&xrac,&yrac,&xm,&ym,&mod_button);
  d(shift)=((mod_button & ShiftMask)!=0);
  F(cfrmg).init_recoordinate_point(cfrmg);
}

status_t eventlook_Create_Cadre_Rectangle(cfrmg,event)
c_FrMg	*cfrmg;
XEvent	event;
{
  char			tamp_cla[20];
  int			tailtamp=20;
  KeySym		symtouche;
  int			nb_car;
  XComposeStatus	seqtouche;
  Display		*display;
  Window		window;
  GC			gc;
  status_t		retour_func;
  DATA(Create_Cadre_Rectangle);
  
  GET_DATA(Create_Cadre_Rectangle);
  display=cfrmg->X_info.display;
  window=cfrmg->X_info.window;
  gc=cfrmg->X_info.gc_contour;
  
  F(cfrmg).recoordinate_point(cfrmg,&d(sx1),&d(sy1),&d(x1),&d(y1),FALSE);
  F(cfrmg).recoordinate_point(cfrmg,&d(sx2),&d(sy2),&d(x2),&d(y2),TRUE);
  
  retour_func=FUNCTION_NOT_FINISHED;
  switch(event.type)
  {
  case KeyPress:
    nb_car=XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			 &symtouche,&seqtouche);
    if (symtouche==XK_Shift_R || symtouche==XK_Shift_L)
    {
      d(shift)=TRUE;
      F(CURSOR).set_cursor(CURSOR,C_CREATE_SQUARE);
      if (d(source))
      {    
	XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		       ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
	calcul_rectangle(cfrmg);
	XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		       ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
      }
    }
    break;
  case KeyRelease:
    nb_car=XLookupString((XKeyEvent *)&event,tamp_cla,tailtamp,
			 &symtouche,&seqtouche);
    if (symtouche==XK_Shift_R || symtouche==XK_Shift_L)
    {
      d(shift)=FALSE;
      F(CURSOR).set_cursor(CURSOR,C_CREATE_RECTANGLE);
      if (d(source))
      {
	XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		       ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
	calcul_rectangle(cfrmg);
	XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		       ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
      }
    }
    break;
  case MotionNotify:
    if (d(source))
    {
      if (d(prems))
      {
	XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		       ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1)));
      }
      d(x2)=d(xm)=event.xmotion.x;
      d(y2)=d(ym)=event.xmotion.y;
      calcul_rectangle(cfrmg);
      XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		       ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
      d(prems)=TRUE;
    }
    break;
  case ButtonRelease:
    if (d(source) && event.xbutton.time>d(time)+600)
      {
	Create_Cadre_Rectangle_Done(cfrmg);
	F(CURSOR).pop_cursor(CURSOR);
	FREE_DATA;
	
	retour_func=FUNCTION_FINISHED;
      }
    break;
  case ButtonPress:
    if (event.xbutton.button==1)
      {
	if (d(source))
	  {
	    Create_Cadre_Rectangle_Done(cfrmg);
	    F(CURSOR).pop_cursor(CURSOR);
	    FREE_DATA;
	    
	    retour_func=FUNCTION_FINISHED;
	  }
	else
	  {
	    d(time)=event.xbutton.time;
	    d(x1)=d(x2)=d(xm)=event.xbutton.x;
	    d(y1)=d(y2)=d(ym)=event.xbutton.y;
	    d(sx1)=d(sx2)=d(x1)*cfrmg->sScale;
	    d(sy1)=d(sy2)=d(y1)*cfrmg->sScale;
	    F(cfrmg).attract_point(cfrmg,&d(sx1),&d(sy1),&d(x1),&d(y1)); 
	    d(source)=TRUE;
	  }
      }
    else
      {
	if (d(source))
	  retour_func=FUNCTION_ABORTED;
	else
	  retour_func=FUNCTION_REINIT;
      }
    break;
  }
  return retour_func;
}

void abort_Create_Cadre_Rectangle(cfrmg)
c_FrMg	*cfrmg;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Create_Cadre_Rectangle);
  
  GET_DATA(Create_Cadre_Rectangle);
  display=cfrmg->X_info.display;
  window=cfrmg->X_info.window;
  gc=cfrmg->X_info.gc_contour;
  
  F(cfrmg).recoordinate_point(cfrmg,&d(sx1),&d(sy1),&d(x1),&d(y1),FALSE);
  F(cfrmg).recoordinate_point(cfrmg,&d(sx2),&d(sy2),&d(x2),&d(y2),TRUE);
  
  if (d(source))
    XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		   ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
  FREE_DATA;
  
  F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Create_Cadre_Rectangle(cfrmg)
c_FrMg	*cfrmg;
{
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Create_Cadre_Rectangle);
  
  GET_DATA(Create_Cadre_Rectangle);
  display=cfrmg->X_info.display;
  window=cfrmg->X_info.window;
  gc=cfrmg->X_info.gc_contour;
  
  F(cfrmg).recoordinate_point(cfrmg,&d(sx1),&d(sy1),&d(x1),&d(y1),FALSE);
  F(cfrmg).recoordinate_point(cfrmg,&d(sx2),&d(sy2),&d(x2),&d(y2),TRUE);
  
  if (d(source))
    XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		   ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1))); 
}

void Create_Cadre_Rectangle_Done(cfrmg)
c_FrMg	*cfrmg;
{
  Display	*display;
  Window	window;
  GC		gc;
  l_Cadre       *lcadre;
  c_Cadre	*cadre;
  DATA(Create_Cadre_Rectangle);
  
  GET_DATA(Create_Cadre_Rectangle);
  display=cfrmg->X_info.display;
  window=cfrmg->X_info.window;
  gc=cfrmg->X_info.gc_contour;
  
  XDrawRectangle(display,window,gc,MIN(d(x1),d(x2)),MIN(d(y1),d(y2)),
		 ABS(d(x2)-d(x1)),ABS(d(y2)-d(y1)));
  if (d(sx1)==d(sx2))
    d(sx2)+=3;
  if (d(sy1)==d(sy2))
    d(sy2)+=3;
  lcadre=F(cfrmg).get_new_cadre(cfrmg);
  if (!lcadre)
  {
    Xc_WARNING(("Can't create cadre for an unknow reason"));
  }
  else
  {
    cadre=lcadre->cadre;
    cadre->thickness = SCALE_FROM_POINTS(.5);
    F(cadre).addspoint(cadre,MIN(d(sx1),d(sx2)),MIN(d(sy1),d(sy2)),FALSE);
    F(cadre).addspoint(cadre,MIN(d(sx1),d(sx2)),MAX(d(sy1),d(sy2)),TRUE);
    F(cadre).addspoint(cadre,MAX(d(sx1),d(sx2)),MAX(d(sy1),d(sy2)),TRUE);
    F(cadre).addspoint(cadre,MAX(d(sx1),d(sx2)),MIN(d(sy1),d(sy2)),TRUE);
    F(cadre).calculorigine(cadre);
    F(cadre).calculforme(cadre,CONTOUR);
    F(cadre).set_X_info(cadre,cfrmg->X_info.display,
			cfrmg->X_info.window);
    F(cfrmg).deselect_all_cadre(cfrmg,TRUE);
    
    cfrmg->modify_cadre_callback(cfrmg->callback_data);
    
    F(cfrmg).draw_cadre(cfrmg,lcadre);
    F(cfrmg).select_cadre(cfrmg,lcadre,TRUE);
    cfrmg->verrou = FALSE;
  }
}

static void calcul_rectangle(cfrmg)
c_FrMg	*cfrmg;
{ 
  int	dx,dy;
  DATA(Create_Cadre_Rectangle);
  
  GET_DATA(Create_Cadre_Rectangle);
  
  d(x2)=d(xm);
  d(y2)=d(ym);
  d(sx2)=d(x2)*cfrmg->sScale;
  d(sy2)=d(y2)*cfrmg->sScale;
  F(cfrmg).attract_point(cfrmg,&d(sx2),&d(sy2),&d(x2),&d(y2));
  if (d(shift))
  {
    dx = ABS(d(x1)-d(x2));
    dy = ABS(d(y1)-d(y2));
    if ( dx > dy )
    {
      if (d(y1) > d(y2))
	d(y2) = d(y1) - dx;
      else
	d(y2) = d(y1) + dx;
    }
    else
    {
      if (d(x1) > d(x2))
	d(x2) = d(x1) - dy;
      else
	d(x2) = d(x1) + dy;
    }
  }
  d(sx2)=d(x2)*cfrmg->sScale;
  d(sy2)=d(y2)*cfrmg->sScale;
}
