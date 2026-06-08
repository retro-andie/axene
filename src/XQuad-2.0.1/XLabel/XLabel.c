/*
** XLabel.c for Xclamation, XQuad, XAllWrite and XInstall in XLabel/
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
** Started on  Mon Jul 17 19:01:33 1995 Robin Castanier
** Last update Sun Jan  9 16:57:12 2000 Emmanuel Paris
*/

#include "XLabel.h"
#include "RegisterHelp.h"
#include "WInterface.h"
#include "Resource.h"

extern c_Resource              *GlobResources;

static void *cons_XLabel();
static void dest_XLabel();
static void *copy_XLabel();
static void set_string ___PROTO((c_XLabel *This, char *string)); 

static void cb_event ___PROTO((Widget w, c_XLabel *This, XEvent *event));
static void draw_string ___PROTO((c_XLabel *This));

sf_XLabel fc_XLabel =
{
 cons_XLabel,
 dest_XLabel,
 copy_XLabel,
 set_string
};

static void *cons_XLabel(w_Parent, ManageW, Title, alignment, fontres)
Widget w_Parent;
c_ManageWidget *ManageW;
char   *Title;
int alignment;
int fontres;
{
 c_XLabel	*This;
 Arg		argsw[1];
 XmString	xms;
 
 if ((This = (c_XLabel *)Xc_malloc("XLabel", sizeof(c_XLabel))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_XLabel;
  
 This->w_This = XmCreateLabel(w_Parent, Title, NULL, 0);
 Xc_REGISTER_HELP(This->w_This);
 
 This->display = XtDisplay(This->w_This);
 This->window = (Window)0;
 This->gc = (GC)0;
 This->mapped = FALSE;

 XtSetArg(argsw[0], XmNlabelString, &xms);
 XtGetValues(This->w_This, argsw, 1);

 if (XmStringGetLtoR(xms, XmSTRING_DEFAULT_CHARSET, &This->string) == False)
  This->string = NULL;
 XmStringFree(xms);
 This->oldstring = This->string = NULL;

 xms =  XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(This->w_This, argsw, 1);
 XmStringFree(xms);
 
 This->alignment = alignment;
 This->fontres = fontres;

 XtAddEventHandler(This->w_This, SubstructureNotifyMask | ExposureMask, False,
		   (void *)cb_event, (void *)This);

 if (ManageW)
  F(ManageW).Add_child(ManageW, This->w_This, w_Parent);  
 Xc_TRACE(("Object XLabel build"));
 return This;
}

static void dest_XLabel(This)
c_XLabel *This;
{
 XtDestroyWidget(This->w_This);
 if (This->gc != (GC)0)
  XFreeGC(This->display, This->gc);
 if (This->string)
  Xc_free(This->string);
 if (This->oldstring)
  Xc_free(This->oldstring);
 Xc_free(This);
 Xc_TRACE(("Object XLabel destroyed"));
}

static void *copy_XLabel(This)
c_XLabel *This;
{
 Xc_TRACE(("Object XLabel copy function not done"));
 return NULL;
}

static void set_string(This, string)
c_XLabel *This;
char *string;
{
 if (This->oldstring) Xc_free(This->oldstring);
 
 This->oldstring = This->string;
 This->string = Xc_strdup("ls", string);
 if (This->mapped && This->string)
 {
  draw_string(This);
  XFlush(This->display);
 }
}

static void cb_event(w, This, event)
Widget w;
c_XLabel *This;
XEvent *event;
{ 
 if (XtWindow(w) != event->xany.window) return;

 switch(event->type)
 {
 case MapNotify:
  This->mapped = TRUE;
    
  if (!This->window)
  {
   Arg		argsw[2];
   int		none;
   long		foreg;
   long		backg;
   XGCValues 	xgcv;
   XCharStruct	chstr;
   Window	wdummy;
   int		idummy;
   unsigned int	width, height, udummy;

  
   This->window = event->xmap.window; 

   XtSetArg(argsw[0], XmNbackground, &backg);
   XtSetArg(argsw[1], XmNforeground, &foreg);
   XtGetValues(w, argsw, 2);
   xgcv.foreground = foreg;
   xgcv.background = backg;
   xgcv.font = F(GlobResources).getFont(GlobResources, This->fontres);
   This->gc = XCreateGC(This->display, This->window, 
			GCForeground | GCBackground | GCFont,
			&xgcv);
   This->font_struct = XQueryFont(This->display, XGContextFromGC(This->gc));
   XTextExtents(This->font_struct, "Mg", 2, &none, 
		&This->font_height, &none, &chstr);

   XGetGeometry(event->xmap.display, This->window, &wdummy, &idummy, &idummy,
		&width, &height, &udummy, &udummy);

   This->width = width;
   This->height = height;
  }

  break;
 case UnmapNotify:
  This->mapped = FALSE;
  break;
 case ConfigureNotify:
  This->width = event->xconfigure.width;
  This->height = event->xconfigure.height;
  if (This->mapped)
   XClearWindow(This->display, This->window);
  else
   break;
 case Expose:
 case GraphicsExpose:
  if (This->string)
   draw_string(This);
  break;
 }
}


static void draw_string(This)
c_XLabel *This;
{
 int y, width, owidth;
 
 width = XTextWidth(This->font_struct, This->string, strlen(This->string));
 if (This->oldstring)
  owidth = XTextWidth(This->font_struct, This->oldstring, 
		      strlen(This->oldstring));
 else
  owidth = This->width;

 y = ((This->height + This->font_height) >> 1) - 2;
 
 switch(This->alignment)
 {
 case XlALIGNMENT_BEGINNING:
  XDrawImageString(This->display, This->window, This->gc, 
		   0, y, This->string, strlen(This->string));
  if (owidth > width)
   XClearArea(This->display, This->window, width, 0, 
	      owidth - width, This->height, False);
  break;
 case XlALIGNMENT_CENTER:
  width = XTextWidth(This->font_struct, This->string, strlen(This->string));
  XDrawImageString(This->display, This->window, This->gc, 
		   (This->width - width) >> 1, y,
		   This->string, strlen(This->string));
    
  if (owidth > width)
  {
   int mwidth = (owidth - width + 1) >> 1;

   XClearArea(This->display, This->window, (This->width - owidth) >> 1, 0, 
	      mwidth, This->height, False);

   mwidth = (owidth - width) >> 1;
   if (mwidth)
    XClearArea(This->display, This->window, 
	       ((This->width - width) >> 1) + width, 0, 
	       mwidth, This->height, False);
  }
  break;
 case XlALIGNMENT_END:
  XDrawImageString(This->display, This->window, This->gc, 
		   (This->width - width) - 2, y,
		   This->string, strlen(This->string));
  if (owidth > width)
   XClearArea(This->display, This->window, (This->width - owidth) - 2, 0, 
	      owidth - width, This->height, False);
  break;
 }
}







