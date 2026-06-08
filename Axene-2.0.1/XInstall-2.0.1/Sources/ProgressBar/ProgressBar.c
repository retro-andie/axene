/*
** ProgressBar.c for XInstall in ProgressBar/
** Objet ProgressBar
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Thu Mar 20 22:02:53 1997 Antoine Buat
** Last update Sat Jun 10 17:46:25 2000 Emmanuel Paris
*/

/*--- include ---*/
#include "ProgressBar.h"
#include "Colormap.h"
#include "Xpm.h"

/*--- external ---*/
extern c_Colormap *GlobColormap;

/*--- prototypes ---*/
void *cons_ProgressBar();
void dest_ProgressBar();
void *copy_ProgressBar();

void change_part_ProgressBar();
void change_total_ProgressBar();
void redraw_ProgressBar();
void resize_ProgressBar();

/*--- fill object method ---*/
sf_ProgressBar fc_ProgressBar =
{
 cons_ProgressBar,
 dest_ProgressBar,
 copy_ProgressBar,
 change_part_ProgressBar,
 change_total_ProgressBar,
 redraw_ProgressBar
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
void *cons_ProgressBar(w_Parent, ManageW, Title, Width, Height,
		       pix_on, pix_off)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
int		Width;
int		Height;
char		**pix_on;
char		**pix_off;
{
 c_ProgressBar		*This;
 Arg			argsw[4];
 int			nargs;

 if ((This = (c_ProgressBar *)Xc_malloc("ProgressBar", 
					  sizeof(c_ProgressBar))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_ProgressBar));

 This->f = &fc_ProgressBar;
 This->total = 0;
 This->part = 0;
 This->last_part = 1;
 This->width = Width;
 This->height = Height;
 This->display = NULL;
 This->buf_off = pix_off;
 This->buf_on = pix_on;

 nargs = 0;
 XtSetArg(argsw[nargs], XmNwidth, Width);
 nargs++;
 XtSetArg(argsw[nargs], XmNheight, Height);
 nargs++;
 This->w_This = XmCreateDrawingArea(w_Parent, Title, argsw, nargs);
 if (ManageW)
 {
  F(ManageW).Add_child(ManageW, This->w_This, w_Parent);
 }
 XtAddCallback(This->w_This, XmNexposeCallback, redraw_ProgressBar, 
	       This);
 XtAddCallback(This->w_This, XmNresizeCallback, resize_ProgressBar, 
	       This);
 Xc_TRACE(("Object DrawingArea build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_ProgressBar(This)
c_ProgressBar *This;
{
 if (This->display)
 {
  XFreePixmap(This->display, This->pix_on);
  if (This->pix_on_m)
   XFreePixmap(This->display, This->pix_on_m);
  XFreePixmap(This->display, This->pix_off);
  if (This->pix_off_m)
   XFreePixmap(This->display, This->pix_off_m);
  XFreeGC(This->display, This->gc_on);
  if (This->gc_on_mask)
   XFreeGC(This->display, This->gc_on_mask);
  XFreeGC(This->display, This->gc_off);
  if (This->gc_off_mask)
   XFreeGC(This->display, This->gc_off_mask);
 }
 XtDestroyWidget(This->w_This);
 Xc_free(This);
 Xc_TRACE(("Object ProgressBar destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copier                                                            ** 
** ----------------------------------------------------------------- */
void *copy_ProgressBar(This)
c_ProgressBar *This;
{
 Xc_TRACE(("Object ProgressBar copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** redraw                                                            ** 
** ----------------------------------------------------------------- */
void redraw_ProgressBar(w, This)
Widget		w;
c_ProgressBar	*This;
{
 int		num;
 int		rx;

 if (!This->display)
 {      
  XImage	*xmask;
  XImage	*xsource;
  XImage	*xsource2;
  Pixmap	pmask;
  Pixmap	psource;
  long		bg,fg;
  GC		gc;

  This->center_h = 0;
  This->center_w = 0;

  This->display = XtDisplay(This->w_This);
  This->window = XtWindow(This->w_This);

  if (DefaultDepth(This->display, DefaultScreen(This->display)) == 1)
  {
   fg = WhitePixel(This->display, DefaultScreen(This->display));
   bg = BlackPixel(This->display, DefaultScreen(This->display));
  }
  else
  {
   fg = -1;
   bg = 0;
  }

  xmask = NULL;
  XPMload(This->display, This->buf_off, &xsource,
	  &xmask, FALSE);	/* force mono=FALSE */
  psource = XCreatePixmap(This->display, 
			  DefaultRootWindow(This->display),
			  xsource->width, xsource->height,
			  DefaultDepth(This->display,
				       DefaultScreen(This->display)));
  gc = XCreateGC(This->display, psource, 0, 0);
  XPutImage(This->display, psource, gc, xsource, 0, 0, 0, 0,
	    xsource->width, xsource->height);
  Xc_free(xsource->data);
  XFreeGC(This->display, gc);
  This->pix_off = psource;

  if (xmask)
  {
   pmask = XCreatePixmap(This->display, 
			 DefaultRootWindow(This->display),
			 xmask->width, xmask->height, 
			 DefaultDepth(This->display,
				      DefaultScreen(This->display)));
   gc = XCreateGC(This->display, pmask, 0, 0);
   XSetForeground(This->display, gc, fg);
   XSetBackground(This->display, gc, bg);
   XPutImage(This->display, pmask, gc, xmask, 0, 0, 0, 0,
	     xmask->width, xmask->height);
   Xc_free(xmask->data);
   XFreeGC(This->display, gc);
   XFree((char *)xmask);
   This->pix_off_m = pmask;
   This->gc_off = XCreateGC(This->display, This->window, 0, 0);
   This->gc_off_mask = XCreateGC(This->display, This->window, 0, 0);
   XSetFunction(This->display, This->gc_off_mask, GXand);
   XSetFunction(This->display, This->gc_off, GXor);
  }
  else
  {
   This->pix_off_m = 0;
   This->gc_off = XCreateGC(This->display, This->window, 0, 0);
   XSetFunction(This->display, This->gc_off, GXcopy);
  }

  xmask = NULL;
  XPMload(This->display, This->buf_on, &xsource2,
	  &xmask, FALSE);	/* force mono=FALSE */
  psource = XCreatePixmap(This->display, 
			  DefaultRootWindow(This->display),
			  xsource2->width, xsource2->height,
			  DefaultDepth(This->display,
				       DefaultScreen(This->display)));
  gc = XCreateGC(This->display, psource, 0, 0);
  XPutImage(This->display, psource, gc, xsource2, 0, 0, 0, 0,
	    xsource2->width, xsource2->height);
  Xc_free(xsource2->data);
  XFreeGC(This->display, gc);
  This->pix_on = psource;

  if (xmask)
  {
   pmask = XCreatePixmap(This->display, 
			 DefaultRootWindow(This->display),
			 xmask->width, xmask->height, 
			 DefaultDepth(This->display,
				      DefaultScreen(This->display)));
   gc = XCreateGC(This->display, pmask, 0, 0);
   XSetForeground(This->display, gc, fg);
   XSetBackground(This->display, gc, bg);
   XPutImage(This->display, pmask, gc, xmask, 0, 0, 0, 0,
	     xmask->width, xmask->height);
   Xc_free(xmask->data);
   XFreeGC(This->display, gc);
   XFree((char *)xmask);
   This->pix_on_m = pmask;
   This->gc_on = XCreateGC(This->display, This->window, 0, 0);
   This->gc_on_mask = XCreateGC(This->display, This->window, 0, 0);
   XSetFunction(This->display, This->gc_on_mask, GXand);
   XSetFunction(This->display, This->gc_on, GXor);
  }
  else
  {
   This->pix_on_m = 0;    
   This->gc_on = XCreateGC(This->display, This->window, 0, 0);
   XSetFunction(This->display, This->gc_on, GXcopy);
  }

  if (xsource2->width > xsource->width)
  {
   This->pwidth = xsource2->width;
   This->on_x = 0;
   This->off_x = (xsource2->width - xsource->width) / 2;
  }
  else
  {
   This->pwidth = xsource->width;
   This->off_x = 0;
   This->on_x = (xsource->width - xsource2->width) / 2;
  }

  if (xsource2->height > xsource->height)
  {
   This->pheight = xsource2->height;
   This->on_y = 0;
   This->off_y = (xsource2->height - xsource->height) / 2;
  }
  else
  {
   This->pheight = xsource->height;
   This->off_y = 0;
   This->on_y = (xsource->height - xsource2->height) / 2;
  }
  This->all_y = (This->height-This->pheight) / 2;
  This->pix_num = This->width / This->pwidth;

  This->on_w = xsource2->width;
  This->on_h = xsource2->height;
  This->off_w = xsource->width;
  This->off_h = xsource->height;

  XFree((char*)xsource);
  XFree((char*)xsource2);
 }
 
 if (This->center_w == 0 || This->center_h == 0) return;

 /* redraw all (in case of real redraw) or decrease */
 if (w || (This->part < This->last_part))
 {
  /*XClearWindow(This->display, This->window);*/
  if ((This->total) && (This->part))
  {
   float	factor;

   factor = (float)This->part / (float)This->total;
   num = This->pix_num * factor;
   rx = 0;
   while (num)
   {
    if (This->pix_on_m)
     XCopyArea(This->display, This->pix_on_m, This->window, This->gc_on_mask, 
	       0, 0,
	       This->on_w, This->on_h,
	       This->center_w + rx + This->on_x, 
	       This->center_h + This->on_y + This->all_y);
    XCopyArea(This->display, This->pix_on, This->window, This->gc_on, 
	      0, 0,
	      This->on_w, This->on_h,
	      This->center_w + rx + This->on_x, 
	      This->center_h + This->on_y + This->all_y);

    rx += This->pwidth;
    num --;
   }
   XFlush(This->display);
   
   num = This->pix_num - (int)(This->pix_num * factor);
  }
  else
  {
   num = This->pix_num;
  }
  rx = This->pwidth * (This->pix_num);

  while (num)
  {
   rx -= This->pwidth;
   if (This->pix_off_m)
    XCopyArea(This->display, This->pix_off_m, This->window, This->gc_off_mask, 
	      0, 0,
	      This->off_w, This->off_h,
	      This->center_w + rx + This->off_x, 
	      This->center_h + This->off_y + This->all_y);
   
   XCopyArea(This->display, This->pix_off, This->window, This->gc_off, 
	     0, 0,
	     This->off_w, This->off_h,
	     This->center_w + rx + This->off_x, 
	     This->center_h + This->off_y + This->all_y);
   num --;
  }
  XFlush(This->display);
 }
 else
 {
  if ((This->total) && (This->part))
  {
   float	factor;
   float	factor_last;
   int		skip;
   int		skip_last;

   factor = (float)This->part / (float)This->total;
   factor_last = (float)This->last_part / (float)This->total;
   skip = This->pix_num * factor;
   skip_last = This->pix_num * factor_last;
   rx = This->pwidth * skip_last;
   num = skip - skip_last;
   while (num)
   {
    if (This->pix_on_m)
     XCopyArea(This->display, This->pix_on_m, This->window, This->gc_on_mask, 
	       0, 0,
	       This->on_w, This->on_h,
	       This->center_w+rx+This->on_x, 
	       This->center_h+This->on_y+This->all_y);
    XCopyArea(This->display, This->pix_on, This->window, This->gc_on, 
	      0, 0,
	      This->on_w, This->on_h,
	      This->center_w + rx + This->on_x, 
	      This->center_h + This->on_y + This->all_y);
    
/*
    Zusleep(1);
    XFlush(This->display);
*/    
    rx += This->pwidth;
    num --;
   }
   XFlush(This->display);
  }
 }
 This->last_part = This->part;
}

/* ----------------------------------------------------------------- ** 
** resize                                                            ** 
** ----------------------------------------------------------------- */
void resize_ProgressBar(w, This)
Widget		w;
c_ProgressBar	*This;
{
 Dimension	rwidth,	rheight;

 rwidth = (Dimension)0;
 rheight = (Dimension)0;
 XtVaGetValues(WIDGET(This), XmNwidth, &rwidth, 
	       XmNheight, &rheight, NULL);
 This->center_h = (rheight - This->height)/2;
 This->center_w = (rwidth - This->width)/2;
 redraw_ProgressBar(w, This);
}

/* ----------------------------------------------------------------- ** 
** change total                                                      ** 
** ----------------------------------------------------------------- */
void change_total_ProgressBar(This, size)
c_ProgressBar	*This;
int		size;
{
 This->total = size;
}

/* ----------------------------------------------------------------- ** 
** change part                                                       ** 
** ----------------------------------------------------------------- */
void change_part_ProgressBar(This, size)
c_ProgressBar	*This;
int		size;
{
 This->part = size;
}










