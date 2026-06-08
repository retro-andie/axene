/*
** Create_Cadres.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:26:52 1995 Emmanuel Paris
** Last update Tue Jun 30 19:20:04 1998 Emmanuel Paris
*/


#include "Create_Cadres.h"

void init_Create_Cadre_Rectangle();
status_t eventlook_Create_Cadre_Rectangle();
void abort_Create_Cadre_Rectangle();
void redraw_Create_Cadre_Rectangle();
void Create_Cadre_Rectangle_Done();
static void calcul_rectangle();

void init_Create_Cadre_Elipse();
status_t eventlook_Create_Cadre_Elipse();
void abort_Create_Cadre_Elipse();
void redraw_Create_Cadre_Elipse();
void Create_Cadre_Elipse_Done();
static void calcul_elipse();

void init_Create_Cadre_Polygonal();
status_t eventlook_Create_Cadre_Polygonal();
void abort_Create_Cadre_Polygonal();
void redraw_Create_Cadre_Polygonal();
void Create_Cadre_Polygonal_Done();


/*					**
**   CREATE CADRE RECTANGLE FUNCTION    **
**                               	*/

void init_Create_Cadre_Rectangle(cpage)
c_Page	*cpage;
{
 Window		root;
 Window		fille;
 int		xrac, yrac;
 int		xm, ym;
 unsigned int	mod_button;
 DATA(Create_Cadre_Rectangle);
  
 SET_DATA(Create_Cadre_Rectangle);
  
 F(CURSOR).push_and_set_cursor(CURSOR, C_CREATE_RECTANGLE);
 F(GlobColormap).use_color(GlobColormap, cpage->X_info.gc_contour, BLACK);
 d(source) = d(prems) = FALSE;
 XQueryPointer(cpage->X_info.display, cpage->X_info.window, &root, &fille,
	       &xrac, &yrac, &xm, &ym, &mod_button);
 d(shift) = ((mod_button & ShiftMask)!=0);
 F(cpage).init_recoordinate_point(cpage);
}

status_t eventlook_Create_Cadre_Rectangle(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 char		tamp_cla[20];
 int		tailtamp = 20;
 KeySym		symtouche;
 int		nb_car;
 XComposeStatus	seqtouche;
 Display	*display;
 Window		window;
 GC		gc;
 status_t	retour_func;
 DATA(Create_Cadre_Rectangle);
  
 GET_DATA(Create_Cadre_Rectangle);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2), TRUE);
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case KeyPress:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
		       &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   d(shift) = TRUE;
   F(CURSOR).set_cursor(CURSOR, C_CREATE_SQUARE);
   if (d(source))
   {    
    XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
    calcul_rectangle(cpage);
    XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
   }
  }
  break;
 case KeyRelease:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
		       &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   d(shift) = FALSE;
   F(CURSOR).set_cursor(CURSOR, C_CREATE_RECTANGLE);
   if (d(source))
   {
    XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
		   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
    calcul_rectangle(cpage);
    XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
   }
  }
  break;
 case MotionNotify:
  if (d(source))
  {
   if (d(prems))
   {
    XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)));
   }
   d(x2) = d(xm) = event.xmotion.x;
   d(y2) = d(ym) = event.xmotion.y;
   calcul_rectangle(cpage);
   XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		  ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
   d(prems) = TRUE;
  }
  break;
 case ButtonRelease:
  if (d(source) && event.xbutton.time > d(time) + 600)
  {
   Create_Cadre_Rectangle_Done(cpage);
   F(CURSOR).pop_cursor(CURSOR);
   FREE_DATA;
	
   retour_func = FUNCTION_FINISHED;
  }
  break;
 case ButtonPress:
  if (event.xbutton.button == 1)
  {
   if (d(source))
   {
    Create_Cadre_Rectangle_Done(cpage);
    F(CURSOR).pop_cursor(CURSOR);
    FREE_DATA;
	    
    retour_func = FUNCTION_FINISHED;
   }
   else
   {
    d(time) = event.xbutton.time;
    d(x1) = d(x2) = d(xm) = event.xbutton.x;
    d(y1) = d(y2) = d(ym) = event.xbutton.y;
    d(sx1) = d(sx2) = d(x1) * cpage->sScale;
    d(sy1) = d(sy2) = d(y1) * cpage->sScale;
    F(cpage).attract_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1));
    d(source) = TRUE;
   }
  }
  else
  {
   if (d(source))
    retour_func = FUNCTION_ABORTED;
   else
    retour_func = FUNCTION_REINIT;
  }
  break;
 }
 return retour_func;
}

void abort_Create_Cadre_Rectangle(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Rectangle);
  
 GET_DATA(Create_Cadre_Rectangle);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2), TRUE);
  
 if (d(source))
  XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
 FREE_DATA;
  
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Create_Cadre_Rectangle(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Rectangle);
  
 GET_DATA(Create_Cadre_Rectangle);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2), TRUE);
  
 if (d(source))
  XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1))); 
}

void Create_Cadre_Rectangle_Done(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 l_Cadre	*lcadre;
 c_Cadre	*cadre;
 DATA(Create_Cadre_Rectangle);
  
 GET_DATA(Create_Cadre_Rectangle);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 if (d(source))
 {
  XDrawRectangle(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)));
  d(source) = FALSE;
 }
 if (d(sx1) == d(sx2))
  d(sx2)+=3;
 if (d(sy1) == d(sy2))
  d(sy2)+=3;
 lcadre = F(cpage).get_new_cadre(cpage);
 if (!lcadre)
 {
  Xc_WARNING(("Can't create cadre for an unknow reason"));
 }
 else
 {
  cadre = lcadre->cadre;
  F(cadre).addspoint(cadre, MIN(d(sx1), d(sx2)), MIN(d(sy1), d(sy2)), FALSE);
  F(cadre).addspoint(cadre, MIN(d(sx1), d(sx2)), MAX(d(sy1), d(sy2)), TRUE);
  F(cadre).addspoint(cadre, MAX(d(sx1), d(sx2)), MAX(d(sy1), d(sy2)), TRUE);
  F(cadre).addspoint(cadre, MAX(d(sx1), d(sx2)), MIN(d(sy1), d(sy2)), TRUE);
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre, CONTOUR);
  F(cadre).set_X_info(cadre, cpage->X_info.display,
		      cpage->X_info.window);
  XSetForeground(cadre->X_info.display, 
		 cadre->X_info.gc_inside,
		 F(GlobColormap).get_spec_color(GlobColormap,
						cpage->nb_cadre));
  F(cpage).deselect_all_cadre(cpage, TRUE);
  F(cpage).draw_cadre(cpage, lcadre);
  F(cpage).select_cadre(cpage, lcadre, TRUE);
  cpage->verrou = FALSE;
 }
}

static void calcul_rectangle(cpage)
c_Page	*cpage;
{ 
 int	dx, dy;
 DATA(Create_Cadre_Rectangle);
  
 GET_DATA(Create_Cadre_Rectangle);
  
 d(x2) = d(xm);
 d(y2) = d(ym);
 d(sx2) = d(x2) * cpage->sScale;
 d(sy2) = d(y2) * cpage->sScale;
 F(cpage).attract_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2));
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
 d(sx2) = d(x2) * cpage->sScale;
 d(sy2) = d(y2) * cpage->sScale;
}

/*				     **	
**   CREATE CADRE ELIPSE FUNCTION    **
**				     */

void init_Create_Cadre_Elipse(cpage)
c_Page	*cpage;
{
 Window		root;
 Window		fille;
 int		xrac, yrac;
 int		xm, ym;
 unsigned int	mod_button;
 DATA(Create_Cadre_Elipse);
  
 SET_DATA(Create_Cadre_Elipse);
  
 F(CURSOR).push_and_set_cursor(CURSOR, C_CREATE_ELIPSE);
 F(GlobColormap).use_color(GlobColormap, cpage->X_info.gc_contour, BLACK);
 d(source) = d(prems) = FALSE;
 XQueryPointer(cpage->X_info.display, cpage->X_info.window, &root, &fille,
	       &xrac, &yrac, &xm, &ym, &mod_button);
 d(shift) = ((mod_button & ShiftMask)!=0);
 F(cpage).init_recoordinate_point(cpage);
}

status_t eventlook_Create_Cadre_Elipse(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 char		tamp_cla[20];
 int		tailtamp = 20;
 KeySym		symtouche;
 int		nb_car;
 XComposeStatus	seqtouche;
 Display	*display;
 Window		window;
 GC		gc;
 status_t	retour_func;
 DATA(Create_Cadre_Elipse);
  
 GET_DATA(Create_Cadre_Elipse);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2), TRUE);
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case KeyPress:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
		       &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   d(shift) = TRUE;
   F(CURSOR).set_cursor(CURSOR, C_CREATE_CIRCLE);
   if (d(source))
   {
    XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
	     ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040); 
    calcul_elipse(cpage);
    XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
	     ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040); 
   }
  }
  break;
 case KeyRelease:
  nb_car = XLookupString((XKeyEvent *)&event, tamp_cla, tailtamp,
		       &symtouche, &seqtouche);
  if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
  {
   d(shift) = FALSE;
   F(CURSOR).set_cursor(CURSOR, C_CREATE_ELIPSE);
   if (d(source))
   {
    XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	     ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040); 
    calcul_elipse(cpage);
    XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	     ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040); 
   }
  }
  break;
 case MotionNotify:
  if (d(source))
  {
   if (d(prems))
   {
    XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	     ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040);
   }
   d(xm) = event.xmotion.x;
   d(ym) = event.xmotion.y;
   calcul_elipse(cpage);
   XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	    ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040);
      
   d(prems) = TRUE;
  }
  break;
 case ButtonRelease:
  if (d(source) && event.xbutton.time > d(time) + 600)
  {
   Create_Cadre_Elipse_Done(cpage);
   F(CURSOR).pop_cursor(CURSOR);
   FREE_DATA;
	
   retour_func = FUNCTION_FINISHED;
  }
  break;
 case ButtonPress:
  if (event.xbutton.button == 1)
  {
   if (d(source))
   {
    Create_Cadre_Elipse_Done(cpage);
    F(CURSOR).pop_cursor(CURSOR);
    FREE_DATA;
	    
    retour_func = FUNCTION_FINISHED;
   }
   else
   {
    d(time) = event.xbutton.time;
    d(x1) = d(x2) = d(xm) = event.xbutton.x;
    d(y1) = d(y2) = d(ym) = event.xbutton.y;
    d(sx1) = d(sx2) = d(x1) * cpage->sScale;
    d(sy1) = d(sy2) = d(y1) * cpage->sScale;
    F(cpage).attract_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1));
    d(source) = TRUE;
   }
  }
  else
  {
   if (d(source))
    retour_func = FUNCTION_ABORTED;
   else
    retour_func = FUNCTION_REINIT;
  }
  break;
 }
 return retour_func;
}

void abort_Create_Cadre_Elipse(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Elipse);
  
 GET_DATA(Create_Cadre_Elipse);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2), TRUE);
  
 if (d(source))
 {
  XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040);
 }
 FREE_DATA;
  
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Create_Cadre_Elipse(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Elipse);
  
 GET_DATA(Create_Cadre_Elipse);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sx1), &d(sy1), &d(x1), &d(y1), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2), TRUE);
  
 if (d(source))
 {
  XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040);
 }
}

void Create_Cadre_Elipse_Done(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 c_Cadre	*cadre;
 l_Cadre	*lcadre;
 boolean	prems;
 coord_t	rayonx;
 coord_t	rayony;
 int		i;
 double	angle;
 DATA(Create_Cadre_Elipse);
  
 GET_DATA(Create_Cadre_Elipse);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 if (d(source))
 {
  XDrawArc(display, window, gc, MIN(d(x1), d(x2)), MIN(d(y1), d(y2)), 
	   ABS(d(x2)-d(x1)), ABS(d(y2)-d(y1)), 0, 23040);
  d(source) = FALSE;
 }
 lcadre = F(cpage).get_new_cadre(cpage);
 if (!lcadre)
 {
  Xc_WARNING(("Can't create cadre %s for an unknow reason", lcadre->name));
 }
 else
 {
  prems = FALSE;
  cadre = lcadre->cadre;
  rayonx = ABS(d(sx1)-d(sx2));
  if (rayonx < COORD_SCALEVALUE)
   rayonx = COORD_SCALEVALUE;
  rayony = ABS(d(sy1)-d(sy2));
  if (rayony < COORD_SCALEVALUE)
   rayony = COORD_SCALEVALUE;
  for(i = 0;i < 40;i++)
  {
   angle = (double)i / 40 * (2 * M_PI);
   F(cadre).addspoint(cadre, 
		      (coord_t)( (rayonx * cos(angle)) / 2 + 
				MIN(d(sx1), d(sx2)) + 
				(double)rayonx / 2), 
		      (coord_t)( (rayony * sin(angle)) / 2 + 
				MIN(d(sy1), d(sy2)) + 
				(double)rayony / 2), 
		      prems);
   prems = TRUE;
  }
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre, CONTOUR);
  F(cadre).set_X_info(cadre, cpage->X_info.display,
		      cpage->X_info.window);
  XSetForeground(cadre->X_info.display,
		 cadre->X_info.gc_inside,
		 F(GlobColormap).get_spec_color(GlobColormap,
						cpage->nb_cadre));
  F(cpage).deselect_all_cadre(cpage, TRUE);
  F(cpage).draw_cadre(cpage, lcadre);
  F(cpage).select_cadre(cpage, lcadre, TRUE);
  cpage->verrou = FALSE;
 }
}

static void calcul_elipse(cpage)
c_Page	*cpage;
{ 
 int	dx, dy;
 DATA(Create_Cadre_Elipse);
  
 GET_DATA(Create_Cadre_Elipse);
  
 d(x2) = d(xm);
 d(y2) = d(ym);
 d(sx2) = d(x2) * cpage->sScale;
 d(sy2) = d(y2) * cpage->sScale;
 F(cpage).attract_point(cpage, &d(sx2), &d(sy2), &d(x2), &d(y2));
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
 d(sx2) = d(x2) * cpage->sScale;
 d(sy2) = d(y2) * cpage->sScale;
}

/*					**
**   CREATE CADRE POLYGONAL FUNCTION    **
**                                      */

void init_Create_Cadre_Polygonal(cpage)
c_Page	*cpage;
{
 DATA(Create_Cadre_Polygonal);
  
 SET_DATA(Create_Cadre_Polygonal);
  
 F(CURSOR).push_and_set_cursor(CURSOR, C_CREATE_POLYGONE);
 F(GlobColormap).use_color(GlobColormap, cpage->X_info.gc_contour, BLACK);
 d(lcadre) = F(cpage).get_new_cadre(cpage);
 if (!d(lcadre))
 {
  Xc_WARNING(("Can't create cadre for an unknow reason"));
 }
 else
 {
  d(cadre) = d(lcadre)->cadre;
 }  
 d(cadre)->Forme->Orig_X = d(cadre)->Forme->Orig_Y = 0;
 d(lp_loop) = (l_Point *)NULL;
 d(source) = d(cont) = d(prems) = d(connected) = d(ended) = FALSE;
 F(cpage).init_recoordinate_point(cpage);
}

status_t eventlook_Create_Cadre_Polygonal(cpage, event)
c_Page	*cpage;
XEvent	event;
{
 int		xm, ym;
 int		px, py;
 l_Point   	*lpoint;
 Display	*display;
 Window		window;
 GC		gc;
 l_Point	*rlpoint;
 status_t	retour_func;
 DATA(Create_Cadre_Polygonal);
  
 GET_DATA(Create_Cadre_Polygonal);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sxo), &d(syo), &d(xo), &d(yo), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sxc), &d(syc), &d(xc), &d(yc), TRUE);
 px = d(cadre)->Forme->Orig_X;
 py = d(cadre)->Forme->Orig_Y;
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case MotionNotify:
  if (d(cont))
  {
   if (d(prems))
   {
    XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
   }
   d(xc) = event.xmotion.x;
   d(yc) = event.xmotion.y;
   d(sxc) = d(xc) * cpage->sScale;
   d(syc) = d(yc) * cpage->sScale;
   F(cpage).attract_point(cpage, &d(sxc), &d(syc), &d(xc), &d(yc));
   XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
   if (d(lp_loop)->X + px < d(xc) + HANDLE_X_ACT && 
       d(lp_loop)->X + px > d(xc)-HANDLE_X_ACT &&
       d(lp_loop)->Y + py < d(yc) + HANDLE_Y_ACT && 
       d(lp_loop)->Y + py > d(yc)-HANDLE_Y_ACT )
   {
    F(CURSOR).set_cursor(CURSOR, C_CONNECT_POINT);
    d(connected) = TRUE;
   }
   else
   {
    F(CURSOR).set_cursor(CURSOR, C_CREATE_POLYGONE);
    d(connected) = FALSE;
   }
   d(prems) = TRUE;
  }
  else
  {
   if (d(lp_loop) && 
       d(lp_loop)->X + px < event.xmotion.x + HANDLE_X_ACT && 
       d(lp_loop)->X + px > event.xmotion.x-HANDLE_X_ACT &&
       d(lp_loop)->Y + py < event.xmotion.y + HANDLE_Y_ACT && 
       d(lp_loop)->Y + py > event.xmotion.y-HANDLE_Y_ACT )
   {
    F(CURSOR).set_cursor(CURSOR, C_END_MARK);
    d(ended) = TRUE;
   }
   else
   {
    F(CURSOR).set_cursor(CURSOR, C_CREATE_POLYGONE);
    d(ended) = FALSE;
   }
  }      
  break;
 case ButtonPress:
  switch(event.xbutton.button)
  {
  case 1:
   xm = event.xbutton.x;
   ym = event.xbutton.y;
   if (d(cont))
   {
    if (d(connected))
    {
     XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
     if (d(xo) == d(lp_loop)->X + px && d(yo) == d(lp_loop)->Y + py)
     {
      XDrawPoint(display, window, gc, d(xo), d(yo));
     }
     else
     {
      rlpoint = F(d(cadre)).getnextpoint(d(cadre), d(lp_loop));
      if (d(xo)!=rlpoint->X || d(yo)!=rlpoint->Y)
      {
       XDrawLine(display, window, gc, d(xo), d(yo), 
		 d(lp_loop)->X + px, d(lp_loop)->Y + py);
      }
     }
     F(CURSOR).set_cursor(CURSOR, C_END_MARK);
     d(ended) = TRUE;
     d(cont) = d(prems) = FALSE;
    }
    else
    {
     if (d(prems))
      XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
     d(xc) = d(xo); d(yc) = d(yo);
     d(xo) = xm;
     d(yo) = ym;
     d(sxo) = d(xo) * cpage->sScale;
     d(syo) = d(yo) * cpage->sScale;
     F(cpage).attract_point(cpage, &d(sxo), &d(syo), &d(xo), &d(yo));
     F(d(cadre)).addspoint(d(cadre), 
			   d(sxo)-px * cpage->sScale, 
			   d(syo)-py * cpage->sScale, TRUE);
     d(xo) = d(sxo) / cpage->sScale;
     d(yo) = d(syo) / cpage->sScale;
     XDrawLine(display, window, gc, d(xo), d(yo), d(xc), d(yc));
	  
     d(prems) = FALSE;
    }
   }
   else
   {
    if (d(ended))
    {
     d(prems) = FALSE;
     Create_Cadre_Polygonal_Done(cpage);
     F(CURSOR).pop_cursor(CURSOR);
     FREE_DATA;
	  
     retour_func = FUNCTION_FINISHED;
    }
    else
    {
     d(xo) = xm;
     d(yo) = ym;
     d(sxo) = d(xo) * cpage->sScale;
     d(syo) = d(yo) * cpage->sScale;
     F(cpage).attract_point(cpage, &d(sxo), &d(syo), &d(xo), &d(yo));
     F(d(cadre)).addspoint(d(cadre), d(sxo), d(syo), FALSE);
     d(xo) = d(sxo) / cpage->sScale;
     d(yo) = d(syo) / cpage->sScale;
     lpoint = d(cadre)->ListPoint;
     while(lpoint->NextPoint)
      lpoint = lpoint->NextPoint;
     d(lp_loop) = lpoint;
     d(source) = d(cont) = TRUE;
     d(prems) = FALSE;
    }
   }
   break;
  case 3:
   if (!d(cadre)->ListPoint)
   {
    F(cpage).delete_cadre(cpage, d(lcadre), DEL_CADRE);
    FREE_DATA;
	
    F(CURSOR).pop_cursor(CURSOR);
    retour_func = FUNCTION_REINIT;
   }
   else
   {
    retour_func = FUNCTION_ABORTED;
   }
   break;
  case 2:
   if (!d(cadre)->ListPoint)
   {
    retour_func = FUNCTION_REINIT;
   }
   else
   {
    if (d(prems))
     XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
    d(prems) = TRUE;
    Create_Cadre_Polygonal_Done(cpage);
    F(CURSOR).pop_cursor(CURSOR);
    FREE_DATA;
	
    retour_func = FUNCTION_FINISHED;
   }
   break;
  }
  break;
 }
 return retour_func;
}


void abort_Create_Cadre_Polygonal(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Polygonal);
  
 GET_DATA(Create_Cadre_Polygonal);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sxo), &d(syo), &d(xo), &d(yo), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sxc), &d(syc), &d(xc), &d(yc), TRUE);
  
 if (d(source))
 {
  if (d(prems))
  {
   XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
  }
  trace_list_point(d(cadre)->ListPoint, display, window, gc, 
		   d(cadre)->Forme->Orig_X, d(cadre)->Forme->Orig_Y, !d(cont));
 }
 F(cpage).delete_cadre(cpage, d(lcadre), DEL_CADRE);
 FREE_DATA;
  
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Create_Cadre_Polygonal(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Polygonal);

 GET_DATA(Create_Cadre_Polygonal);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;
  
 F(cpage).recoordinate_point(cpage, &d(sxo), &d(syo), &d(xo), &d(yo), FALSE);
 F(cpage).recoordinate_point(cpage, &d(sxc), &d(syc), &d(xc), &d(yc), TRUE);
  
 if (d(source))
 {
  trace_list_point(d(cadre)->ListPoint, display, window, gc, 
		   d(cadre)->Forme->Orig_X, d(cadre)->Forme->Orig_Y, 
		   !d(cont));
  if (d(cont))
   XDrawLine(display, window, gc, d(xc), d(yc), d(xo), d(yo));
 }
}

void Create_Cadre_Polygonal_Done(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Create_Cadre_Polygonal);
  
 GET_DATA(Create_Cadre_Polygonal);
 display = cpage->X_info.display;
 window = cpage->X_info.window;
 gc = cpage->X_info.gc_contour;

 if (d(source))
 {
  trace_list_point(d(cadre)->ListPoint, display, window, gc, 
		   d(cadre)->Forme->Orig_X, d(cadre)->Forme->Orig_Y, 
		   !d(prems));
  d(source) = FALSE;
 }

 F(d(cadre)).calculorigine(d(cadre));
 F(d(cadre)).calculforme(d(cadre), CONTOUR);
 F(d(cadre)).set_X_info(d(cadre), cpage->X_info.display, 
			cpage->X_info.window);
 XSetForeground(d(cadre)->X_info.display, 
		d(cadre)->X_info.gc_inside, 
		F(GlobColormap).get_spec_color(GlobColormap, 
					       cpage->nb_cadre));
 F(cpage).deselect_all_cadre(cpage, TRUE);
 F(cpage).draw_cadre(cpage, d(lcadre));
 F(cpage).display_selected_lcadre(cpage);
 F(cpage).select_cadre(cpage, d(lcadre), TRUE);
 cpage->verrou = FALSE;
}



