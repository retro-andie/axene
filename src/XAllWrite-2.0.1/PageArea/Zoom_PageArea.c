/*
** Zoom_PageArea.c for XAllWrite in PageArea/
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
** Started on  Mon Jul 17 19:50:05 1995 Emmanuel Paris
** Last update Sun Jan 11 15:21:33 1998 Emmanuel Paris
*/

#define _ZPA_PRIVATE_
#include "Zoom_PageArea.h"
#include "Colormap.h"

extern c_Colormap *GlobColormap;

void init_Zoom_Plus_PageArea();
status_t eventlook_Zoom_Plus_PageArea();
void abort_Zoom_Plus_PageArea();
void redraw_Zoom_Plus_PageArea();
void init_Zoom_Minus_PageArea();
status_t eventlook_Zoom_Minus_PageArea();
void abort_Zoom_Minus_PageArea();
void redraw_Zoom_Plus_PageArea();

void zoom_pagearea_fit_in_width();
void zoom_pagearea_fit_in_screen();
void zoom_pagearea_at_real_size();
void zoom_pagearea_at_percent();

/* ----------------------------------------------------------------- ** 
** ZOOM PLUS PAGEAREA FUNCTION                                       ** 
** ----------------------------------------------------------------- */

void init_Zoom_Plus_PageArea(This)
c_PageArea	*This;
{
 This->function_data = (void *)1;
 F(CURSOR).push_and_set_cursor(CURSOR,C_MAG_PLUS);
 XMapRaised(This->display, This->tracing_window);
}

status_t eventlook_Zoom_Plus_PageArea(This,event)
c_PageArea	*This;
XEvent	event;
{
 status_t	retour_func;
 float		factor;
 int		x, y;
  
 retour_func = FUNCTION_NOT_FINISHED;
 if (event.xany.window != This->tracing_window) return retour_func;

 switch(event.type)
 {
 case ButtonPress:
  switch(event.xbutton.button)
  {
  case 1:
   factor = SCALE_TO_PERCENT(This->sFactor);
   if (factor < ZOOM_PLUS_LIMIT)
   {
    XUnmapWindow(This->display, This->tracing_window);

    factor += ZOOM_STEP;
    if (factor < ZOOM_MINUS_LIMIT) factor = (float)ZOOM_MINUS_LIMIT;

    This->GuestPage = NULL;
    x = This->AreaOrig_X + event.xbutton.x;
    y = This->AreaOrig_Y + event.xbutton.y;

    SET_WAIT_MODE_STATIC;
    changeScaling_PageArea(This, SCALE_FROM_PERCENT(factor), x, y, NULL);
    UNSET_WAIT_MODE;
    
    XMapRaised(This->display, This->tracing_window);
   }
   else
   {
    abort_Zoom_Plus_PageArea(This);
    retour_func = FUNCTION_ABORTED;
   }
   break;
  case 3:
   abort_Zoom_Plus_PageArea(This);
   retour_func = FUNCTION_ABORTED;
   break;
  }
 }
 return retour_func;
}

void abort_Zoom_Plus_PageArea(This)
c_PageArea	*This;
{
 This->function_data = NULL;
 XUnmapWindow(This->display, This->tracing_window);
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Zoom_Plus_PageArea(This)
c_PageArea	*This;
{
}

/*                                *
 *    ZOOM MINUS PAGE FUNCTION    *
 *                                */

void init_Zoom_Minus_PageArea(This)
c_PageArea	*This;
{
 This->function_data = (void *)1;
 F(CURSOR).push_and_set_cursor(CURSOR, C_MAG_MINUS);
 XMapRaised(This->display, This->tracing_window);
}

status_t eventlook_Zoom_Minus_PageArea(This, event)
c_PageArea	*This;
XEvent		event;
{
 status_t	retour_func;
 scale_t	factor;
 int		x, y;
 
 retour_func = FUNCTION_NOT_FINISHED;
 if (event.xany.window != This->tracing_window) return retour_func;

 switch(event.type)
 {
 case ButtonPress:
  switch(event.xbutton.button)
  {
  case 1:
   factor = SCALE_TO_PERCENT(This->sFactor);
   if (factor > ZOOM_MINUS_LIMIT)
   {
    XUnmapWindow(This->display, This->tracing_window);

    factor -= ZOOM_STEP;
    if (factor < ZOOM_MINUS_LIMIT) factor = (float)ZOOM_MINUS_LIMIT;

    This->GuestPage = NULL;
    x = This->AreaOrig_X + event.xbutton.x; 
    y = This->AreaOrig_Y + event.xbutton.y;

    SET_WAIT_MODE_STATIC;
    changeScaling_PageArea(This, SCALE_FROM_PERCENT(factor), x, y, NULL);
    UNSET_WAIT_MODE;
    
    XMapRaised(This->display, This->tracing_window);
   }
   else
   {
    abort_Zoom_Minus_PageArea(This);
    retour_func = FUNCTION_ABORTED;
   }
   break;
  case 3:
   abort_Zoom_Minus_PageArea(This);
   retour_func = FUNCTION_ABORTED;
   break;
  case LeaveNotify:
   XUnmapWindow(This->display, This->tracing_window);
   break;
  case EnterNotify:
   XMapRaised(This->display, This->tracing_window);
   break;
  }
 }
 return retour_func;
}

void abort_Zoom_Minus_PageArea(This)
c_PageArea	*This;
{
 This->function_data = NULL;
 XUnmapWindow(This->display, This->tracing_window);
 F(CURSOR).pop_cursor(CURSOR);
}

void redraw_Zoom_Minus_PageArea(This)
c_PageArea	*This;
{
}

/*          
 *             Zoom in Zone               *
 *                                        */

void init_Zoom_PageArea_In_Zone(This)
c_PageArea	*This;
{
 XGCValues	val_gc;
 DATA(Zoom_PageArea_In_Zone);
 
 SET_DATA(Zoom_PageArea_In_Zone);
 
 F(CURSOR).push_and_set_cursor(CURSOR,C_ZOOM_IN_ZONE);
 d(source) = d(prems) = FALSE;
 XMapRaised(This->display, This->tracing_window);
 
 if (GlobColormap->Cblack)
   val_gc.function = GXxor;
 else
  val_gc.function = GXinvert;
 
 val_gc.plane_mask= -1;
 val_gc.background = GlobColormap->Cwhite;
 val_gc.foreground = GlobColormap->Cblack;
 val_gc.line_width = 0;
 val_gc.line_style = LineSolid; 
 val_gc.cap_style = CapNotLast;
 val_gc.join_style = JoinMiter;
 val_gc.subwindow_mode = IncludeInferiors;
 d(gc) = XCreateGC(This->display, This->window, 
		   GCFunction | GCPlaneMask | GCForeground | GCBackground | 
		   GCLineWidth | GCLineStyle | GCCapStyle | GCJoinStyle |
		   GCSubwindowMode, &val_gc);
}

status_t eventlook_Zoom_PageArea_In_Zone(This,event)
c_PageArea	*This;
XEvent	event;
{
 scale_t	new_factor;
 Display	*display;
 Window		window;
 GC		gc;
 int		x, y;
 status_t	retour_func;
 DATA(Zoom_PageArea_In_Zone);
  
 GET_DATA(Zoom_PageArea_In_Zone);
 display = This->display;
 window = This->window;
 gc = d(gc);
  
 retour_func = FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case MotionNotify:
  if (d(source))
  {
   if (d(prems))
    XDrawRectangle(display, window, gc,
		   MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
   d(x2) = event.xmotion.x;
   d(y2) = d(y1) + (d(x2) - d(x1)) * This->Height / This->Width;
   XDrawRectangle(display, window, gc,
		  MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		  ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
   d(prems) = TRUE;
  }
  break;
 case ButtonRelease:
  if (d(source) && event.xbutton.time > d(time) + 600)
  {
   if (d(prems))
    XDrawRectangle(display, window, gc,
		   MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		   ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
	
   XUnmapWindow(This->display, This->tracing_window);

   new_factor = (scale_t)(((double)This->sFactor *
			   (double)This->Width) /
			  (double)ABS(d(x2) - d(x1)));

   if (SCALE_TO_PERCENT(new_factor) > ZOOM_PLUS_LIMIT)
    new_factor = SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT);

   d(prems) = FALSE;
   
   x = This->AreaOrig_X + ((d(x1) +  d(x2)) >> 1);
   y = This->AreaOrig_Y + ((d(y1) +  d(y2)) >> 1);
   
   SET_WAIT_MODE_STATIC;
   changeScaling_PageArea(This, new_factor, x, y, NULL);
   UNSET_WAIT_MODE;

   XFreeGC(display, gc);
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
    if (d(prems))
     XDrawRectangle(display,window,gc,
		    MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		    ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
	  
    XUnmapWindow(This->display, This->tracing_window);

    new_factor = (scale_t)(((double)This->sFactor *
			    (double)This->Width) /
			   (double)ABS(d(x2) - d(x1)));
    if (SCALE_TO_PERCENT(new_factor) > ZOOM_PLUS_LIMIT)
     new_factor = SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT);
    d(prems) = FALSE;

    x = This->AreaOrig_X + ((d(x1) +  d(x2)) >> 1);
    y = This->AreaOrig_Y + ((d(y1) +  d(y2)) >> 1);
    
    changeScaling_PageArea(This, new_factor, x, y, NULL);
    
    XFreeGC(display, gc);
    F(CURSOR).pop_cursor(CURSOR);
    FREE_DATA;
	  
    retour_func = FUNCTION_FINISHED;
   }
   else
   {
    d(time) = event.xbutton.time;
    d(x1) = d(x2) = event.xbutton.x;
    d(y1) = d(y2) = event.xbutton.y;
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

void abort_Zoom_PageArea_In_Zone(This)
c_PageArea	*This;
{
 Display		*display;
 Window		window;
 GC			gc;
 DATA(Zoom_PageArea_In_Zone);
  
 GET_DATA(Zoom_PageArea_In_Zone);
 display = This->display;
 window = This->window;
 gc = d(gc);
  
 if (d(prems))
  XDrawRectangle(display, window, gc,
		 MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));

 XUnmapWindow(This->display, This->tracing_window);

 XFreeGC(display, gc);
 F(CURSOR).pop_cursor(CURSOR);
 FREE_DATA;
}

void redraw_Zoom_PageArea_In_Zone(This)
c_PageArea	*This;
{
 Display	*display;
 Window		window;
 GC		gc;
 DATA(Zoom_PageArea_In_Zone);
  
 GET_DATA(Zoom_PageArea_In_Zone);
  
 display = This->display;
 window = This->window;
 gc = d(gc);

 if (d(prems))
  XDrawRectangle(display, window, gc,
		 MIN(d(x1), d(x2)), MIN(d(y1), d(y2)),
		 ABS(d(x2) - d(x1)), ABS(d(y2) - d(y1)));
}

/*            
 *           Miscellaneous Zoom           *
 *                                        */

void zoom_pagearea_fit_in_width(This, pagel)
c_PageArea	*This;
c_Page		*pagel;
{
 scale_t	new_factor;
 
 if (This->view_page != Xa_VIEW_NORMAL_PAGE)
 {
  This->view_page = Xa_VIEW_NORMAL_PAGE;
  This->ForcedWidth = 0;
  calculAreaSize_PageArea(This);
 }
 
 new_factor = (This->sFactor * (double)This->Width) / (double)This->AreaWidth;

 if (This->sFactor != new_factor)
 {
  changeScaling_PageArea(This, new_factor, 0, 
			 - PA_HMARGIN + PA_VMARGIN, pagel);
 }
}

void zoom_pagearea_fit_in_screen(This, pagel)
c_PageArea	*This;
c_Page		*pagel;
{
 scale_t	new_factor, new_factorX, new_factorY;
 
 if (This->view_page != Xa_VIEW_NORMAL_PAGE)
 {
  This->view_page = Xa_VIEW_NORMAL_PAGE;
  This->ForcedWidth = 0;
  calculAreaSize_PageArea(This);
 }
 
 new_factorX = (This->sFactor * (double)This->Width) / (double)This->AreaWidth;
 new_factorY = (This->sFactor * (double)This->Height) / 
  (double)(pagel->Height + 2 * PA_GUTTER);
 
 new_factor = MIN(new_factorX, new_factorY);

 if (This->sFactor != new_factor)
 {
  changeScaling_PageArea(This, new_factor, 0, -PA_GUTTER, pagel);
 }
}
 
void zoom_pagearea_at_real_size(This, pagel)
c_PageArea	*This;
c_Page		*pagel;
{
 coord_t	sWidth;
 int		Width;
 int		WidthMM;
 scale_t	new_factor;
 
 Width = DisplayWidth(This->display, DefaultScreen(This->display));
 WidthMM = DisplayWidthMM(This->display, DefaultScreen(This->display));
 if (WidthMM > 1000)
  WidthMM = 255;
 sWidth = SCALE_FROM_MILLIMETERS(WidthMM);
 new_factor = SCALE_FROM_COEF( (double)(Width * COORD_DEFAULT)
			      / (double)sWidth);

 This->view_page = Xa_VIEW_NORMAL_PAGE;
 
 if (This->sFactor != new_factor)
 {
  changeScaling_PageArea(This, new_factor, - PA_HMARGIN + PA_VMARGIN, 
			 - PA_HMARGIN + PA_VMARGIN, pagel);
 }
}

void zoom_pagearea_at_percent(This, pagel, percent)
c_PageArea	*This;
c_Page		*pagel;
int		percent;
{
 scale_t scale;
 
 if (percent < 75)
  This->view_page = Xa_VIEW_MULTIPLE_PAGES;
 else
 {
  if (percent < 100)
   This->view_page = Xa_VIEW_TWO_PAGES;
  else
   This->view_page = Xa_VIEW_NORMAL_PAGE;
 }

 scale = SCALE_FROM_PERCENT(percent);
 if (This->sFactor != scale)
 {
  changeScaling_PageArea(This, scale, - PA_HMARGIN + PA_VMARGIN,
			 - PA_HMARGIN + PA_VMARGIN, pagel);
 }
}





