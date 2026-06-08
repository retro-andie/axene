/*
** AutoScroll.c for XQuad in AutoScroll/
** Object For Auto-Scrolling
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
** Started on  Wed Feb 22 16:58:39 1995 Emmanuel Paris
** Last update Sun Mar 15 17:54:23 1998 Emmanuel Paris
*/
#include "AutoScroll.h"

#define LOOPTIME 10
#define WTIME 2000
#define WTIME2 150000
#define MIN_WTIME2 30000

void *cons_AutoScroll();
void dest_AutoScroll();
void *copy_AutoScroll();
void eventlook_AutoScroll();
static boolean scroll_workarea();

sf_AutoScroll fc_AutoScroll =
{
 cons_AutoScroll,
 dest_AutoScroll,
 copy_AutoScroll,
 eventlook_AutoScroll,
};

void *cons_AutoScroll(display,x_min,y_min,x_max,y_max,
		      ScrollBarH, ScrollBarV, function, data)
Display	*display;
int	x_min;  
int	y_min;  
int	x_max;  
int	y_max;
c_ScrollBarX *ScrollBarH;
c_ScrollBarX *ScrollBarV;
void		(*function)();
void		*data;
{
 c_AutoScroll *This;
  
 if ((This=(c_AutoScroll *)Xc_malloc("AutoScroll",sizeof(c_AutoScroll)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_AutoScroll;  
 This->display = display;
 This->x_min = x_min;
 This->y_min = y_min;
 This->x_max = x_max;
 This->y_max = y_max;
 This->ScrollBarH = ScrollBarH;
 This->ScrollBarV = ScrollBarV;

 This->Callback = function;
 This->Callback_data = data;

 This->recurs = FALSE;
 return This;
}

void dest_AutoScroll(This)
c_AutoScroll   *This;
{
 Xc_free(This);
}

void *copy_AutoScroll(This)
c_AutoScroll   This;
{
 Xc_TRACE(("copy AutoScroll function not done.normally not usefull."));
 return NULL;
} 

void eventlook_AutoScroll(This,event)
c_AutoScroll	*This;
XEvent	*event;
{
 boolean 	ret;
 boolean	sortie;
 int		wtime2;
 int		nb_event;
 XEvent	event2;
 int		xm, ym, xr, yr, mdb;
 Window	fr, ff;
 int		deltax, deltay;
 double	rate;
  
 if (This->recurs)
  return;
 switch(event->type)
 {
 case Expose:
 case NoExpose:
 case GraphicsExpose:
 case LeaveNotify:
  XQueryPointer(This->display, event->xany.window, &fr, &ff,
		&xr, &yr, &xm, &ym, &mdb);
  event->type = MotionNotify;
  event->xmotion.x = xm;
  event->xmotion.y = ym;
 case ButtonPress:
 case MotionNotify:
  This->x = event->xmotion.x;
  This->y = event->xmotion.y;
  ret = scroll_workarea(This, event);
  break;
 default:
  ret = FALSE;
  break;
 }
 if (ret)
 {
  sortie = FALSE;
  wtime2 = WTIME2;
  do
  {
   if (XCheckMaskEvent(This->display, EnterWindowMask, &event2) == True)
   { sortie = TRUE; nb_event = 0; }
   else
    nb_event = XEventsQueued(This->display, QueuedAfterReading);
   while(nb_event && !sortie)
   {
    XPeekEvent(This->display, &event2);
    switch(event2.type)
    {
    case GraphicsExpose:
    case NoExpose:
    case Expose:
     XNextEvent(This->display, &event2);
     break;
    case MotionNotify:
     XNextEvent(This->display, &event2);
     *event = event2;
     This->x = event->xmotion.x;
     This->y = event->xmotion.y;
     break;
    default:
     sortie = TRUE;
     break;
    }
    nb_event--;
   }
   if (!sortie)
   {
    if (wtime2 > MIN_WTIME2)
     wtime2 -= WTIME;
	  
    This->recurs = TRUE;
    This->Callback(This->Callback_data,*event);
    This->recurs = FALSE;
    sortie = scroll_workarea(This, event) != TRUE;
    XFlush(This->display);
	  
    if (XCheckMaskEvent(This->display, EnterWindowMask, &event2) == True)
     sortie = TRUE;
	  
    deltax = deltay = 0;
    if (This->x < This->x_min)
     deltax = This->x_min - This->x;
    if (This->x > This->x_max)
     deltax = This->x - This->x_max;
    if (This->y < This->y_min)
     deltay = This->y_min - This->y;
    if (This->y > This->y_max)
     deltay = This->y - This->y_max;
	  
    rate =  1.0 + MAX( deltax, deltay) / 20.0;
    if (rate > 10.0) rate = 10.0;
	  
    Zusleep(wtime2 / rate);
   }
  } while (!sortie);
 } 
}

static boolean scroll_workarea(This, event)
c_AutoScroll	*This;
XEvent	*event;
{
  
 int	xm;
 int	ym;
 c_ScrollBarX	*ScrollB1;
 c_ScrollBarX	*ScrollB2;
 boolean direct1 = SCROLL_RIGHT;
 boolean direct2 = SCROLL_DOWN;
  
 xm = event->xmotion.x;
 ym = event->xmotion.y;
 ScrollB1 = ScrollB2 = (c_ScrollBarX *)NULL;
 if (xm < This->x_min && This->ScrollBarH)
 {
  ScrollB1=This->ScrollBarH;
  direct1 = SCROLL_LEFT;
 }
 if (xm > This->x_max && This->ScrollBarH)
 {
  ScrollB1 = This->ScrollBarH;
  direct1 = SCROLL_RIGHT;
 }
 if (ym < This->y_min && This->ScrollBarV)
 {
  if (ScrollB1)
  {
   ScrollB2 = This->ScrollBarV;
   direct2 = SCROLL_UP;
  }
  else
  {
   ScrollB1 = This->ScrollBarV;
   direct1 = SCROLL_UP;
  }
 }
 if (ym>This->y_max && This->ScrollBarV)
 {
  if (ScrollB1)
  {
   ScrollB2 = This->ScrollBarV;
   direct2 = SCROLL_DOWN;
  }
  else
  {
   ScrollB1 = This->ScrollBarV;
   direct1 = SCROLL_DOWN;
  }
 }
 if (ScrollB1)
 {
  F(ScrollB1).RequestMove(ScrollB1, direct1, ScrollB2, direct2);
  return TRUE;
 }
 return FALSE;
}



