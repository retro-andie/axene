/*
** TimeIndicator.c for XAllWrite in TimeIndicator/
** Object Loading Document Icons
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
** Last update Wed Jun  4 16:54:54 1997 Emmanuel Paris
*/

#include "TimeIndicator.h"
#include "../Pixmaps/led1.zpm"
#include "../Pixmaps/led2.zpm"
#include "../Pixmaps/led3.zpm"
#include "../Pixmaps/led4.zpm"
#include "../Pixmaps/led5.zpm"

static char **led_sequence[NB_LED_IMAGE] =
{
 led1_xpm, led2_xpm, led3_xpm, led4_xpm, led5_xpm
};

static void *cons_TimeIndicator();
static void dest_TimeIndicator();
static void *copy_TimeIndicator();
static void get_indicator_geometry();
static void display_indicator();
static void expose_indicator();
static void advance_indicator();

sf_TimeIndicator fc_TimeIndicator=
{
 cons_TimeIndicator,
 dest_TimeIndicator,
 copy_TimeIndicator,
 get_indicator_geometry,
 display_indicator,
 expose_indicator,
 advance_indicator,
};

static void *cons_TimeIndicator(display, nb_led)
Display	*display;
int	nb_led;
{
 c_TimeIndicator	*This;
 XImage	*xsource;
 XImage	*xmask;
 Pixmap	psource;
 Pixmap	pmask;
 char		**image;
 GC		gc;
 long		bg,fg;
 int		i;

 if ((This=(c_TimeIndicator *)Xc_malloc("TimeIndicator",sizeof(c_TimeIndicator)))==NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f= &fc_TimeIndicator;  

 This->display = display;
 This->window = 0;
 This->nb_led = nb_led;
 This->current_led = 0;

 if (DefaultDepth(display, DefaultScreen(display)) == 1)
 {
  fg = WhitePixel(display, DefaultScreen(display));
  bg = BlackPixel(display, DefaultScreen(display));
 }
 else
 {
  fg = -1;
  bg = 0;
 }
  
 for(i = 0; i < NB_LED_IMAGE; i++)
 {
  image = led_sequence[i];
  XPMload(display, image, &xsource, &xmask, FALSE);
  psource = XCreatePixmap(display, 
			  DefaultRootWindow(display),
			  xsource->width, xsource->height,
			  DefaultDepth(display, DefaultScreen(display)));
  gc = XCreateGC(display, psource, 0, 0);
  XPutImage(display, psource, gc, xsource, 0, 0, 0, 0,
	    xsource->width, xsource->height);
  Xc_free(xsource->data);
  This->led_width  = xsource->width;
  This->led_height = xsource->height;
  XFree((char*)xsource);
  XFreeGC(display, gc);
  This->pixmap_led[i] = psource;
  if (xmask)
  {
   pmask = XCreatePixmap(display, 
			 DefaultRootWindow(display),
			 xmask->width, xmask->height,
			 DefaultDepth(display, DefaultScreen(display)));
   gc = XCreateGC(display, pmask, 0, 0);
   XSetForeground(display, gc, fg);
   XSetBackground(display, gc, bg);
   XPutImage(display, pmask, gc, xmask, 0, 0, 0, 0,
	     xmask->width, xmask->height);
   Xc_free(xmask->data);
   XFree((char *)xmask);
   XFreeGC(display, gc);
   This->pixmap_led_mask[i] = pmask;
  }
  else
   This->pixmap_led_mask[i] = (Pixmap)NULL;
 }

 This->gc = This->gc_mask = (GC)0;
  
 return This;
}

static void dest_TimeIndicator(This)
c_TimeIndicator   *This;
{
 int i;

 for(i = 0; i < NB_LED_IMAGE; i++)
 {
  XFreePixmap(This->display, This->pixmap_led[i]);
  if (This->pixmap_led_mask[i])
   XFreePixmap(This->display, This->pixmap_led_mask[i]);
 }
 if (This->gc)
  XFreeGC(This->display, This->gc); 
 if (This->gc_mask)
  XFreeGC(This->display, This->gc_mask);
 Xc_free(This);
}

static void *copy_TimeIndicator(This)
c_TimeIndicator   This;
{
 Xc_TRACE(("copy TimeIndicator function not done.normally not usefull."));
 return NULL;
}


static void get_indicator_geometry(This, width, height)
c_TimeIndicator *This;
int *width;
int *height;
{
 if (width)
  *width = This->nb_led*This->led_width + (This->nb_led-1)*LED_SEPARATOR;
 if (height)
  *height = This->led_height;
}

static void display_indicator(This, window, orig_x, orig_y)
c_TimeIndicator *This;
Window window;
int    orig_x;
int    orig_y;
{
 boolean	need_mask = FALSE;
 int i;
  
 This->window = window;
 This->orig_x = orig_x;
 This->orig_y = orig_y;
 This->gc = XCreateGC(This->display, window, 0, 0);
  
 for(i = 0; i < NB_LED_IMAGE; i++)
 {
  if (This->pixmap_led_mask[i])
  {
   need_mask = TRUE;
   break;
  }
 }
 if (need_mask)
 {
  This->gc_mask = XCreateGC(This->display, window, 0, 0);
  XSetFunction(This->display, This->gc_mask, GXand);
  XSetFunction(This->display, This->gc, GXor);
 }
 else
  XSetFunction(This->display, This->gc, GXcopy);
}

static void expose_indicator(w, This)
Widget	w;
c_TimeIndicator	*This;
{
 int i, x, y;
  
 XClearWindow(This->display, This->window);
 x = This->orig_x;
 y = This->orig_y;
 for(i=0; i<This->nb_led; i++)
 {
  if (i>=This->current_led)
  {
   if (This->pixmap_led_mask[0])
    XCopyArea(This->display, This->pixmap_led_mask[0], This->window,
	      This->gc_mask, 0, 0, This->led_width, This->led_height, 
	      x, y); 
   XCopyArea(This->display, This->pixmap_led[0],
	     This->window, This->gc, 0, 0, This->led_width,
	     This->led_height, x, y);
  }
  else
  {
   if (This->pixmap_led_mask[NB_LED_IMAGE - 1])
    XCopyArea(This->display, This->pixmap_led_mask[NB_LED_IMAGE - 1],
	      This->window, This->gc_mask, 0, 0,
	      This->led_width, This->led_height, x, y); 
   XCopyArea(This->display, This->pixmap_led[NB_LED_IMAGE - 1],
	     This->window, This->gc, 0, 0,
	     This->led_width, This->led_height, x, y);
  }
  x += This->led_width + LED_SEPARATOR;
 }  
}

static void advance_indicator(This)
c_TimeIndicator *This;
{
 int	x, y, i;
 XEvent event, ret_event;
  
 if (!This->window)
  return;
 Xc_TRACE(("avance indicator to position: %d", This->current_led));
  
 while(XCheckTypedEvent(This->display, Expose, &ret_event) == True)
 {
  XNextEvent(This->display, &event);
  if (event.xany.window == This->window && event.type == Expose)
   expose_indicator(NULL, This);
  XtDispatchEvent(&ret_event);
 }
  
 if (This->current_led < This->nb_led)
 {
  x = This->orig_x;
  y = This->orig_y;
  x += This->current_led * (This->led_width + LED_SEPARATOR);
      
  for (i = 1; i < NB_LED_IMAGE; i++)
  {
   XFlush(This->display);
   if (This->pixmap_led_mask[i])
    XCopyArea(This->display, This->pixmap_led_mask[i], This->window,
	      This->gc_mask, 0, 0, This->led_width, This->led_height, 
	      x, y); 
   XCopyArea(This->display, This->pixmap_led[i], This->window, This->gc,
	     0, 0, This->led_width, This->led_height, x, y);
   XFlush(This->display);
   Zusleep(40000);
  }
  This->current_led++;
 }
 else
 {
  Xc_WARNING(("Too many advance indicator"));
 }
}
