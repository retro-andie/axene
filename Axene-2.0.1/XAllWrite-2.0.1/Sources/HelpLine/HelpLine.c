/*
** HelpLine.c for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice in 
** 	HelpLine/
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
** Started on  Mon Jul 17 17:45:41 1995 Emmanuel Paris
** Last update Sat Jan 29 17:27:15 2000 Emmanuel Paris
*/

#define NTRACE

#define _HELPLINE_PRIVATE_
#include "HelpLine.h"
#include "Colormap.h"
#include "Resource.h"

#ifdef HAVE_XSHAPE
#include <X11/extensions/shape.h>
extern boolean xshape_flag;
#endif

#define WAIT_COUNT 10
#define WAIT_DELAY 15000
#define CUT_SIZE 180
#define WIDTH_SPACING  4
#define HEIGHT_SPACING 3
#define Y_OFFSET 20

static void *cons_HelpLine();
static void dest_HelpLine();
static void *copy_HelpLine();
static void enable_helpline ___PROTO((c_HelpLine *This, Widget widget, 
				      int message, int func));
static void disable_helpline ___PROTO((c_HelpLine *This, Widget widget));
static void change_function_message ___PROTO((c_HelpLine *This, int message,
					      int function));
static int  get_helpline_message ___PROTO((c_HelpLine *This, Widget widget));

static void eventlook_HelpLine();
static void popup_bubble();
static void popdown_bubble();

extern c_Colormap *GlobColormap;
extern c_Resource *GlobResources;
extern void XcHandleEvent ___PROTO((XEvent *event));

sf_HelpLine fc_HelpLine =
{
 cons_HelpLine,
 dest_HelpLine,
 copy_HelpLine,
 enable_helpline,
 disable_helpline,
 change_function_message,
 get_helpline_message
};

static char *hl_colors[] = 
{ "black", "gray50", "gray75", "gray85", "white" };
static char *hl_colors_bw[] = 
{ "black", "white", "white", "white", "black" };

static void *cons_HelpLine(type, w_Parent, ManageW, Title)
hl_type_t type;
Widget w_Parent;
c_ManageWidget *ManageW;
char   *Title;
{
 int		nargs;
 Arg      	args[10];
 char		*bubble;
 c_HelpLine	*This;
  
 if ((This = (c_HelpLine *)Xc_malloc("HelpLine", sizeof(c_HelpLine))) 
     == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_HelpLine;
 
 switch(type)
 {
 case  XcHL_NO_LABEL:
  This->Label = NULL;
  This->w_This = (Widget)0;
  This->display = (Display *)w_Parent;
  break;
 case XcHL_LABEL_GIVEN:
  This->Label = NULL;
  This->w_This = w_Parent;
  This->display = XtDisplay(w_Parent);
  break;
 case XcHL_BUILD_LABEL:
  This->Label = (c_Label *)NEW(c_Label)(w_Parent, ManageW, Title);
  This->w_This = WIDGET(This->Label);
  This->display = XtDisplay(This->w_This);

  nargs = 0;
  XtSetArg(args[nargs], XmNalignment, XmALIGNMENT_CENTER);
  nargs++;
  XtSetValues( This->w_This, args, nargs);
  break;
 }  
 
 This->base = NULL;
 This->hl_handle = Xc_createItem("hl_item", 100, sizeof(helpline_t));

 This->message = XcR_NONE;
 This->function = XcR_NONE;
 This->in_window = FALSE;
 This->popup = FALSE;

 This->bubbles = XcHL_CLASSIC_BUBBLE;
 bubble = F(GlobResources).getString(GlobResources, XcR_helpBubbles);
 if (bubble == NULL || Xstrcmp(bubble, "Normal") || Xstrcmp(bubble, "Classic"))
  This->bubbles = XcHL_CLASSIC_BUBBLE;
 else if (Xstrcmp(bubble, "No") || Xstrcmp(bubble, "Off"))
  This->bubbles = XcHL_NO_BUBBLE;
 else if (Xstrcmp(bubble, "Fun") || Xstrcmp(bubble, "Smart"))
  This->bubbles = XcHL_FUN_BUBBLE;
  
 if (This->bubbles != XcHL_NO_BUBBLE)
 {
  Display	*display = This->display;
  color_cells_t	realcolor;
  int i;
  char **colorlist;
  
  if (DefaultDepth(display, DefaultScreen(display)) == 1)
   colorlist = hl_colors_bw;
  else
   colorlist = hl_colors;

  for(i = 0; i < 5; i++)
  {
   F(GlobColormap).allocate_named_color(GlobColormap, colorlist[i], 
					&realcolor);
   This->color[i] = realcolor.pixel;
  }
  This->font = F(GlobResources).getXFontStruct(GlobResources,
					       XcR_bubbleFont);
 }

 Xc_HISTORY(("Object HelpLine build"));
 return This;
}

static void dest_HelpLine(This)
c_HelpLine *This;
{
 if (This->Label)
  DELETE(c_Label)(This->Label);

 Xc_destroyItem(This->hl_handle);
 Xc_HISTORY(("Object HelpLine destroyed"));
 Xc_free(This);
}

static void *copy_HelpLine(This)
c_HelpLine *This;
{
 Xc_WARNING(("Copy object HelpLine useless"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** enable_helpline - register a helpline message for widget          ** 
** ----------------------------------------------------------------- */
static void enable_helpline(This, widget, message, func)
c_HelpLine *This;
Widget widget;
int message;
int func;
{
 helpline_t *item;

 item = (helpline_t *)Xc_new(This->hl_handle);
 item->This = This;
 item->widget = widget;
 item->message = message;
 item->function = func;
 item->next = This->base;

 This->base = item;

 XtAddEventHandler(widget, EnterWindowMask | LeaveWindowMask | 
		   ButtonPressMask | ButtonReleaseMask | PointerMotionMask, 
		   False, eventlook_HelpLine, (void *)item);
}

/* ----------------------------------------------------------------- ** 
** disable_helpline - unregister a helpline message for widget       ** 
** ----------------------------------------------------------------- */
static void disable_helpline(This, widget)
c_HelpLine *This;
Widget widget;
{
 helpline_t *item, *item2;
 
 item = item2 = This->base;
 while(item && item->widget != widget)
 {
  item2 = item;
  item = item->next;
 }

 if (item)
 {
  if (item2 != This->base)
   item2->next = item->next;
  else
   This->base = item->next;

  Xc_delete(This->hl_handle, item);
  change_function_message(This, XcR_NONE, XcR_NONE);  
  XtRemoveEventHandler(widget, EnterWindowMask | LeaveWindowMask | 
		       ButtonPressMask | PointerMotionMask,
		       False, eventlook_HelpLine, (void *)item);
 }
}

/* ----------------------------------------------------------------- ** 
** change_function_message - change bottom bar label                 ** 
** ----------------------------------------------------------------- */
static void change_function_message(This, message, function)
c_HelpLine	*This;
int		message;
int		function;
{
 Arg      	args[1];
 XmString	xms;
 int msg;

 if (This->w_This == (Widget)0) return;
 
 if (message != XcR_NONE)
 {
  if (message == XcR_DEFAULT)
  {
   if (function != XcR_NONE && function != XcR_DEFAULT)
    This->function = function;
   return;
  }
  if (message == This->message) return;
  msg = message;
 }
 else
 {
  if (function == XcR_NONE)
   msg = This->function;
  else
  {
   if (This->function == function) return;
   msg = function;
   This->function = function;
  }
  if (msg == This->message) return;
 }
 
 if (msg == XcR_NONE) return;
 
 This->message = msg;
 if (msg == XcR_DEFAULT)
  xms = XmStringCreate(DEFAULT_FUNC, XmSTRING_DEFAULT_CHARSET);
 else
 {
  char *mess, *mess2, *mess3;
  
  mess = F(GlobResources).getString(GlobResources, msg);
  if ((mess2 = strchr(mess, '|')) != NULL)
  {
   mess3 = (char *)Xc_malloc("mess", mess2 - mess + 1);
   memcpy(mess3, mess, mess2 - mess);
   mess3[mess2 - mess] = '\0';
   xms = XmStringCreate(mess3, XmSTRING_DEFAULT_CHARSET);
   Xc_free(mess3);
  }
  else
   xms = XmStringCreate(mess, XmSTRING_DEFAULT_CHARSET);
 }
 XtSetArg(args[0], XmNlabelString, xms);
 XtSetValues(This->w_This, args, 1);
 XmStringFree(xms);
}

/* ----------------------------------------------------------------- ** 
** get_helpline_message - retrieve the helpline message for widget   ** 
** ----------------------------------------------------------------- */
static int  get_helpline_message(This, widget)
c_HelpLine *This;
Widget widget;
{
 helpline_t *item;

 item = This->base;
 while(item && item->widget != widget)
  item = item->next;

 if (item) 
  return item->message;

 return -1;
}

static void eventlook_HelpLine(w, item, event)
Widget	w;
helpline_t *item;
XEvent	*event;
{
 c_HelpLine *This;
 int i;

 This = item->This;
 
 switch(event->type)
 {
 case EnterNotify:
  if (This->in_window)
  {
   Xc_TRACE(("pointer still in Icon"));
   break;
  }
  This->in_window = TRUE;
  change_function_message(This, item->message, item->function);
  
  Xc_TRACE(("Entering Icon 0x%p", w));
  break;
 case LeaveNotify:
  if (!This->in_window)
  {
   Xc_TRACE(("pointer has ever leave this Icon"));
   break;
  }
  This->in_window = FALSE;
  change_function_message(This, XcR_NONE, item->function);
  if (This->popup)
  {
   popdown_bubble(This);
   This->popup = FALSE;
  }
  Xc_TRACE(("Leaving Icon 0x%p", w));
  break;
 case MotionNotify:
  if (This->popup || This->bubbles == XcHL_NO_BUBBLE) break;
  i = 0;
  while(i < WAIT_COUNT)
  {
   if (XEventsQueued(XtDisplay(w), QueuedAfterReading) > 0) break;
   Zusleep(WAIT_DELAY);
   i++;
  }
  if (i == WAIT_COUNT)
  {
   if (item->message != XcR_NONE)
   {
    popup_bubble(This, event, item->message);
    This->popup = TRUE;
   }
  }
  break;
 case ButtonPress:
 case ButtonRelease:
  if (This->popup)
  {
   popdown_bubble(This);
   This->popup = FALSE;
  }
  break;
 default:
  Xc_WARNING(("unknow event type: %d", event->type));
  break;
 }
}

static void popup_bubble(This, event, message)
c_HelpLine *This;
XEvent *event;
int message;
{
 char *msg, *ptr, *sptr, *eptr;
 char **msgrows;
 int x, y, tx, i, nbrow;
 int width, mwidth, maxwidth, twidth;
 int height, rheight;
 int swidth, sheight, dummy;
 GC  gc;
 boolean border;
 Window root;
 XGCValues xgcv;
 XSetWindowAttributes attribs;

 msg = F(GlobResources).getString(GlobResources, message);
 ptr = strchr(msg, '|');
 if (ptr)
  msg = ptr+1;

 msgrows = (char **)Xc_malloc("mrows", sizeof(char *) * 4);
 nbrow = 0;
 width = XTextWidth(This->font, msg, strlen(msg));

 ptr = sptr = msg; eptr = msg + strlen(msg);
 maxwidth = width / (width / CUT_SIZE + 1) + CUT_SIZE / 4;
 while(width > maxwidth && *ptr && nbrow < 3)
 {
  while(*ptr && XTextWidth(This->font, sptr, ptr - sptr) <= maxwidth)
  {
   if (*ptr == ' ') eptr = ptr;
   ptr++;
  }
  if (!*ptr && XTextWidth(This->font, sptr, ptr - sptr) <= maxwidth)
   eptr = ptr;
  else if (*eptr != ' ' || eptr == sptr)
  {
   while(*ptr && *ptr != ' ') ptr++;
   eptr = ptr;
  }
  msgrows[nbrow] = (char *)Xc_malloc("row", eptr - sptr + 1);
  memcpy(msgrows[nbrow], sptr, eptr - sptr);
  msgrows[nbrow++][eptr - sptr] = '\0';
  if (*eptr == ' ') eptr++;
  sptr = ptr = eptr; 
 }
 
 if (*sptr)
 {
  msgrows[nbrow] = (char *)Xc_malloc("row", eptr - sptr + 1);
  memcpy(msgrows[nbrow], sptr, eptr - sptr);
  msgrows[nbrow++][eptr - sptr] = '\0';
 }
 
 maxwidth = 0;
 for(i = 0; i < nbrow; i++)
 {
  width = XTextWidth(This->font, msgrows[i], strlen(msgrows[i]));
  if (width > maxwidth) maxwidth = width;
 }
 
 rheight = This->font->ascent + This->font->descent + HEIGHT_SPACING / 2;
 height = rheight * nbrow + HEIGHT_SPACING + (HEIGHT_SPACING + 1) / 2;

#ifdef HAVE_XSHAPE
 if (xshape_flag && This->bubbles == XcHL_FUN_BUBBLE)
 {
  mwidth = (height + 1) / 2;
  border = FALSE;
  width = maxwidth + ((1 + nbrow) * rheight) / 2 + 
   (1 + nbrow) * WIDTH_SPACING + HEIGHT_SPACING;
 }
 else
 {
  mwidth = WIDTH_SPACING;
  border = TRUE;
  width = mwidth * 2 + maxwidth;
 }
#else
 mwidth = WIDTH_SPACING;
 border = TRUE;
 width = mwidth * 2 + maxwidth;
#endif
 

 XGetGeometry(This->display, DefaultRootWindow(This->display), &root,
	      &dummy, &dummy, &swidth, &sheight, &dummy, &dummy);
 
 x = event->xmotion.x_root;
 if (x + width > swidth)
  x -= width + 2;
 y = event->xmotion.y_root;
 if (border)
 {
  if (y + Y_OFFSET + height > sheight)
   y -= height + 4; else y += Y_OFFSET;
 }
 else
 {
  if (y + height > sheight)
   y -= height + 4;
 }
 if (border)
 {
  attribs.background_pixel = This->color[0];
  attribs.border_pixel = This->color[1];
 }
 else
 {
  attribs.background_pixel = This->color[2];
  attribs.border_pixel = This->color[0];
 }
 attribs.override_redirect = TRUE;
 This->window = XCreateWindow(This->display, DefaultRootWindow(This->display),
			      x, y, width, height, (int)border,
			      CopyFromParent, InputOutput, CopyFromParent, 
			      CWBackPixel| CWBorderPixel | CWOverrideRedirect,
			      &attribs);
 
 if (border)
 {
  XMapRaised(This->display, This->window);

  xgcv.font = F(GlobResources).getFont(GlobResources, XcR_bubbleFont);
  xgcv.foreground = This->color[3];
  gc = XCreateGC(This->display, This->window, GCForeground | GCFont, &xgcv);
  
  x = mwidth;
  y = HEIGHT_SPACING + This->font->ascent;
  for(i = 0; i < nbrow; i++)
  {
   twidth = XTextWidth(This->font, msgrows[i], strlen(msgrows[i]));
   tx = x + (maxwidth - twidth) / 2;

   XDrawString(This->display, This->window, gc, tx, y, 
	       msgrows[i], strlen(msgrows[i]));
   y += rheight;
  }
  XFreeGC(This->display, gc);
 }
#ifdef HAVE_XSHAPE
 else
 {
  Pixmap shape_mask ;
  XPoint xpnt[4];
  int ty;

  shape_mask = XCreatePixmap(This->display, This->window, width, height, 1);
  
  xgcv.function = GXcopy;
  xgcv.foreground = 0;
  
  gc = XCreateGC(This->display, shape_mask, GCFunction | GCForeground, &xgcv);
  XFillRectangle(This->display, shape_mask, gc, 0, 0, width, height);
  
  xpnt[0].y = xpnt[1].y = 0;
  xpnt[2].y = xpnt[3].y = height;
  xpnt[3].x = 0; xpnt[0].x = mwidth;
  xpnt[1].x = width - 1; xpnt[2].x = width - 1 - mwidth;
  
  XSetForeground(This->display, gc, 1);
  XFillPolygon(This->display, shape_mask, gc, xpnt, 4, Convex, 
	       CoordModeOrigin);
  
  XShapeCombineMask(This->display, This->window, ShapeBounding, 0, 0,
		    shape_mask, ShapeSet);
  XShapeCombineMask(This->display, This->window, ShapeClip, 0, 0,
		    shape_mask, ShapeSet);
  XFreePixmap(This->display, shape_mask);
  XFreeGC(This->display, gc);

  XMapRaised(This->display, This->window);

  xgcv.font = F(GlobResources).getFont(GlobResources, XcR_bubbleFont);
  xgcv.foreground = This->color[0];
  gc = XCreateGC(This->display, This->window, GCForeground | GCFont, &xgcv);

  x = mwidth + WIDTH_SPACING;
  y = ty = HEIGHT_SPACING + This->font->ascent;
  for(i = 0; i < nbrow; i++)
  {
   twidth = XTextWidth(This->font, msgrows[i], strlen(msgrows[i]));
   tx = x + (maxwidth - twidth) / 2 - ((ty - y) / 2);
   
   XDrawString(This->display, This->window, gc, tx, ty, 
	       msgrows[i], strlen(msgrows[i]));
   ty += rheight;
  }

  XSetLineAttributes(This->display, gc, 3, LineSolid, CapButt, JoinMiter);

  XSetForeground(This->display, gc, This->color[0]);
  XDrawLine(This->display, This->window, gc, width - mwidth - 1, height - 1,
	     width - 1, 0);
  XSetForeground(This->display, gc, This->color[4]);
  XDrawLine(This->display, This->window, gc, 0, height - 1, mwidth, 0);

  XSetLineAttributes(This->display, gc, 1, LineSolid, CapButt, JoinMiter);
  
  XSetForeground(This->display, gc, This->color[0]);
  XDrawLine(This->display, This->window, gc, 2, height -1, width, height - 1);

  XSetForeground(This->display, gc, This->color[4]);
  XDrawLine(This->display, This->window, gc, 0, 0, width - 2, 0);

  XSetForeground(This->display, gc, This->color[1]);
  XDrawLine(This->display, This->window, gc, width - 2 - mwidth, height - 2,
	     width - 3, 0);

  XSetForeground(This->display, gc, This->color[3]);
  XDrawLine(This->display, This->window, gc, 2, height - 1, mwidth + 1, 0);

  XFreeGC(This->display, gc);
 }
#endif

 for(i = 0; i < nbrow; i++)
  Xc_free(msgrows[i]);
 Xc_free(msgrows);

}

static void popdown_bubble(This)
c_HelpLine *This;
{
 XUnmapWindow(This->display, This->window);
 XDestroyWindow(This->display, This->window);
}
