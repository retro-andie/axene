/*
** Box_language.c for XInstall in Box_language/
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
** Started on  Thu Jan 16 00:49:53 1997 Antoine Buat
** Last update Sat Jan 15 19:37:41 2000 Emmanuel Paris
*/

/*--- includes ---*/
#include "Box_language.h"
#include "Actions.h"
#include "Cursor.h"
#include <time.h>
#include <sys/times.h>

/*--- define ---*/
#define BL_ANIM_DELAY 14L

/*--- include pixmap ---*/
#include "../Pixmaps/led5.zpm"
#include "../Pixmaps/select_lang.h"
#include "../Pixmaps/select_lang2.h"

/*--- functions declaration ---*/
static void *cons_Box_language();
static int BLwaitForUnMap();
static void dest_Box_language();
static void *copy_Box_language();

static void redraw_Image();
static void resize_Image();
static void test_Input();
static void restore_ball();
static void draw_ball();
static void create_pixmap();
static void anim_ball();

/*--- externals ---*/
extern image_load_t	Load_Gif;
extern c_GCursor	*GlobCursor;
extern boolean		GlobLangOpt;

/*--- externals functions ---*/
extern void XcHandleEvent ___PROTO((XEvent *event));

/*--- global ---*/
static char *lang_cancel[] =
{
 "Cancel",
 "Cancel",
 "Annuler",
 "Abbrechen",
 "Cancelar",
};
static XmString lang_cancelXm[5];

static char *lang_next[] =
{
 "Next >>>",
 "Next >>>",
 "Suivant >>>",
 "Nächst >>>",
 "Siguiente >>>",
};
static XmString lang_nextXm[5];

/*--- methods ---*/
sf_Box_language fc_Box_language =
{
 cons_Box_language,
 dest_Box_language,
 copy_Box_language,
 BLwaitForUnMap
};

/* ----------------------------------------------------------------- ** 
** constructeur                                                      ** 
** ----------------------------------------------------------------- */
static void *cons_Box_language(w_Parent, pszTitle, zone, sel_tongue)
Widget		w_Parent;
char		*pszTitle;
click_zone	*zone;
{
 c_Box_language	*This;
 Display	*display;
 Dimension	h1, h2;
 int		i;

 /*--- no zone defined answer 0 ---*/
 if (zone == NULL)
  return NULL;

 if((This = (c_Box_language *)Xc_malloc("Box_language",
					sizeof(c_Box_language))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_Box_language));

 This->f = &fc_Box_language;
 
 if (GlobLangOpt) return This;

 This->app = XtWidgetToApplicationContext(w_Parent);
 XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL); 
 
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle, h1, h2,
						      XcCD_HORIZONTAL);
 This->w_This = WIDGET(This->Dialog);
 allow_ba_action(This->w_This);
 This->display = display = XtDisplay(WIDGET(This->Dialog));

 create_pixmap(This, display);

 i = 0;
 while (i < 5)
 {
  lang_cancelXm[i] = XmStringCreateLtoR(lang_cancel[i],
					XmSTRING_DEFAULT_CHARSET);
  lang_nextXm[i] =  XmStringCreateLtoR(lang_next[i],
				       XmSTRING_DEFAULT_CHARSET);
  i++;
 }

 This->all_zone = zone;
 This->sel_zone = zone;
 This->sel_num = 0;
 while (This->sel_zone->ret_code != sel_tongue)
 {
  This->sel_zone = This->sel_zone + 1; 
  This->sel_num++;
 }
 This->angle = 0.0;
 This->lock = FALSE;

 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxLanguage", TRUE);

 This->FrameIcon = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					   MANAGER(This->Dialog), 
					   NULL, 0);
 /*--- grid son ---*/
 SET_WAIT_MODE_STATIC;
 SET_WAIT_MODE_ANIM;

 This->ImageStream = Xc_NewStream(select_lang_gif, select_lang_gif_size);
 This->Image = (c_Image *)NEW(c_Image)
  ((char *)This->ImageStream, &Load_Gif);

 This->ImageStream2 = Xc_NewStream(select_lang2_gif, select_lang2_gif_size);
 This->Image2 = (c_Image *)NEW(c_Image)
  ((char *)This->ImageStream2, &Load_Gif);

 UNSET_WAIT_MODE_ANIM;

 F(This->Image).CalculImage(This->Image, 0, 0);
 F(This->Image2).CalculImage(This->Image2, 0, 0);


 This->Drawing = XmCreateDrawingArea(WIDGET(This->FrameIcon),
				     "BKDrawingGIF", NULL, 0);
 F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				    This->Drawing,
				    WIDGET(This->FrameIcon));
    
 XtAddCallback(This->Drawing, XmNexposeCallback, redraw_Image, 
	       This);
 XtAddCallback(This->Drawing, XmNresizeCallback, resize_Image, 
	       This);
 XtAddCallback(This->Drawing, XmNinputCallback, test_Input, 
	       This);

 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->FrameIcon), XcG_MAX_SIZE,
			 NULL);
 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_CANCEL | XcCD_NEXT,
				  XcCD_NEXT,
				  NULL, NULL);
  
 XtVaSetValues(This->Dialog->w_button[XcCDn_CANCEL], XmNlabelString, 
	       lang_cancelXm[This->sel_zone->ret_code-1], NULL);
 XtVaSetValues(This->Dialog->w_button[XcCDn_NEXT], XmNlabelString, 
	       lang_nextXm[This->sel_zone->ret_code-1], NULL);


 This->wdraw = 0;
 F(This->Dialog).map(This->Dialog, TRUE);
 
 Xc_TRACE(("Object Box_language build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BLwaitForUnMap(This, sel_tongue)
c_Box_language	*This;
int		*sel_tongue;
{
 int	rc;
 XEvent		event;
 XtAppContext	app;

 if (GlobLangOpt) return XcCD_NEXT;

 while(!This->wdraw)
 {
  app = XtWidgetToApplicationContext(WIDGET(This));
  XSync(XtDisplay(WIDGET(This)), 0);
  XtAppNextEvent(app, &event);
  if(event.type == Expose)
   XcHandleEvent(&event);
  else
   XtDispatchEvent(&event);
 }
 
 rc = 0;
 This->id = XtAppAddTimeOut(This->app, BL_ANIM_DELAY, anim_ball, This);
 rc = F(This->Dialog).waitForUnmap(This->Dialog);
 XtRemoveTimeOut(This->id);

 if (TEST_WAIT_MODE) UNSET_WAIT_MODE;
 *sel_tongue = This->sel_zone->ret_code;
 return rc;

/*
 app = XtWidgetToApplicationContext(WIDGET(This));
 while (This->Dialog->mapped_flag == TRUE)
 {
  float		angle;
  int		sizex;
  int		sizey;
  int		sx;
  int		sy;

  angle = 0;
  while ((angle < 2*3.14) && (This->Dialog->mapped_flag == TRUE))
  {
   sizex = (This->sel_zone->width/2);
   sizey = (This->sel_zone->height/2);
   sx = This->x_start+sizex+This->sel_zone->x;
   sy = This->y_start+sizey+This->sel_zone->y;
   if (XEventsQueued(This->display, QueuedAlready))
   {
    XtAppNextEvent(app, &event);
    if(event.type == Expose)
     XcHandleEvent(&event);
    else
     XtDispatchEvent(&event);
   }
   XSync(This->display, 0);
   restore_ball(This, This->wdraw, 3);
   restore_ball(This, This->wdraw, 2);
   restore_ball(This, This->wdraw, 1);
   restore_ball(This, This->wdraw, 0);
   draw_ball(This, This->wdraw,
	     (int)(sizex*cos(2*angle))+sx,
	     (int)(sizey*sin(2*angle))+sy, 0);
   draw_ball(This, This->wdraw,
	     (int)(sizex*cos(2*angle+3.14))+sx,
	     (int)(sizey*sin(2*angle+3.14))+sy, 1);
   draw_ball(This, This->wdraw,
	     (int)(sizex*cos(angle+1.57))+sx,
	     (int)(sizey*sin(2*(angle+1.57)))+sy, 2);
   draw_ball(This, This->wdraw,
	     (int)(sizex*cos(-(angle+4.71)))+sx,
	     (int)(sizey*sin(-2*(angle+4.71)))+sy, 3);
   angle+=3.14/50;
   usleep(1);
   if (XEventsQueued(This->display, QueuedAlready))
   {
    XFlush(This->display);
   } 
  }
 }

 *sel_tongue = This->sel_zone->ret_code;
 return This->Dialog->result;
*/
}

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
static void dest_Box_language(This)
c_Box_language *This;
{
 int	i;

 if (GlobLangOpt) { Xc_free(This); return; }

 XtDestroyWidget(This->Drawing);
 for (i = 0; i < 4; i++)
 {
  XFreePixmap(XtDisplay(WIDGET(This)), This->ball[i].image);
  if (This->ball[i].mask)
   XFreePixmap(XtDisplay(WIDGET(This)),This->ball[i].mask);
 }

 DELETE(c_Image)(This->Image);
 Xc_DeleteStream(This->ImageStream);
 
 DELETE(c_Image)(This->Image2);
 Xc_DeleteStream(This->ImageStream2);

 if (This->IconTitle)
  DELETE(c_Icon)(This->IconTitle);
 DELETE(c_Frame)(This->FrameIcon);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 i = 0;
 while (i < 5)
 {
  XmStringFree(lang_cancelXm[i]);
  XmStringFree(lang_nextXm[i]);
  i++;
 }
 

 Xc_free(This);
 Xc_TRACE(("Object Box_language destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
static void *copy_Box_language(This)
c_Box_language *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** redraw image                                                      ** 
** ----------------------------------------------------------------- */
static void redraw_Image(wid, This)
Widget		wid;
c_Box_language	*This;
{
 Display	*display;
 Window		window;
 GC		gc;
 Dimension	w, h;

 XtVaGetValues(This->Drawing, XmNwidth, &w, XmNheight, &h, NULL); 
 This->x_start = ((int)w - This->Image->width) / 2;
 This->y_start = ((int)h - This->Image->height) / 2;
 display = XtDisplay(WIDGET(This->Dialog));
 This->wdraw = window = XtWindow(This->Drawing);
 gc = XCreateGC(display, window, 0, 0);
 XSetFunction(display, gc, GXcopy);

 if (!This->gc_mask)
 {
  XGCValues	xgcv;
  
  xgcv.function = GXand;
  xgcv.graphics_exposures = False;
  This->gc_mask = XCreateGC(This->display, This->wdraw, 
			    GCFunction|GCGraphicsExposures, &xgcv);
  xgcv.function = GXor;
  This->gc_image = XCreateGC(This->display, This->wdraw, 
			     GCFunction|GCGraphicsExposures, &xgcv);
  xgcv.function = GXcopy;
  This->gc_save = XCreateGC(This->display, This->wdraw, 
			    GCFunction|GCGraphicsExposures, &xgcv);
  F(This->Image).savePart(This->Image, This->wdraw, This->gc_save, 
			  This->sel_zone->x, This->sel_zone->y,
			  This->sel_zone->width, This->sel_zone->height);
  F(This->Image).drawputPart(This->Image, This->Image2, 
			     This->wdraw, This->gc_save,
			     This->sel_zone->x, This->sel_zone->y,
			     This->sel_zone->width,
			     This->sel_zone->height,
			     This->sel_zone->x, This->sel_zone->y,
			     This->x_start, This->y_start);
 }

 restore_ball(This, This->wdraw, 3);
 restore_ball(This, This->wdraw, 2);
 restore_ball(This, This->wdraw, 1);
 restore_ball(This, This->wdraw, 0);

 F(This->Image).draw(This->Image, window, gc, w, h);

 draw_ball(This, This->wdraw, 0, -20, 0);
 draw_ball(This, This->wdraw, 0, -20, 1);
 draw_ball(This, This->wdraw, 0, -20, 2);
 draw_ball(This, This->wdraw, 0, -20, 3);
}

/* ----------------------------------------------------------------- ** 
** resize image                                                      ** 
** ----------------------------------------------------------------- */
static void resize_Image(wid, This)
Widget		wid;
c_Box_language	*This;
{
 Display	*display;
 Window		window;
 GC		gc;
 Dimension	w, h;

 display = XtDisplay(WIDGET(This->Dialog));
 This->wdraw = window = XtWindow(This->Drawing);

 if (!This->gc_mask)
 {
  XGCValues	xgcv;
   
  xgcv.function = GXand;
  xgcv.graphics_exposures = False;
  This->gc_mask = XCreateGC(This->display, This->wdraw, 
			    GCFunction|GCGraphicsExposures, &xgcv);
  xgcv.function = GXor;
  This->gc_image = XCreateGC(This->display, This->wdraw, 
			     GCFunction|GCGraphicsExposures, &xgcv);
  xgcv.function = GXcopy;
  This->gc_save = XCreateGC(This->display, This->wdraw, 
			    GCFunction|GCGraphicsExposures, &xgcv);
  F(This->Image).savePart(This->Image, This->wdraw, This->gc_save, 
			  This->sel_zone->x, This->sel_zone->y,
			  This->sel_zone->width, This->sel_zone->height);
  F(This->Image).drawputPart(This->Image, This->Image2, 
			     This->wdraw, This->gc_save,
			     This->sel_zone->x, This->sel_zone->y,
			     This->sel_zone->width,
			     This->sel_zone->height,
			     This->sel_zone->x, This->sel_zone->y,
			     This->x_start, This->y_start);
 }

 restore_ball(This, This->wdraw, 3);
 restore_ball(This, This->wdraw, 2);
 restore_ball(This, This->wdraw, 1);
 restore_ball(This, This->wdraw, 0);

 XClearWindow(display, window);
 XtVaGetValues(This->Drawing, XmNwidth, &w, XmNheight, &h, NULL); 
 This->x_start = ((int)w - This->Image->Dwidth) / 2;
 This->y_start = ((int)h - This->Image->Dheight) / 2;
 gc = XCreateGC(display, window, 0, 0);
 XSetFunction(display, gc, GXcopy);
 F(This->Image).draw(This->Image, window, gc, w, h);

 draw_ball(This, This->wdraw, 0, -20, 0);
 draw_ball(This, This->wdraw, 0, -20, 1);
 draw_ball(This, This->wdraw, 0, -20, 2);
 draw_ball(This, This->wdraw, 0, -20, 3);
}


/* ----------------------------------------------------------------- ** 
** test input                                                        ** 
** ----------------------------------------------------------------- */
static void test_Input(wid, This, cbs)
Widget				wid;
c_Box_language			*This;
XmDrawingAreaCallbackStruct	*cbs;
{
 This->lock = TRUE;

 if (!This->gc_mask)
 {
  XGCValues	xgcv;

  xgcv.function = GXand;
  This->gc_mask = XCreateGC(This->display, XtWindow(wid), GCFunction, &xgcv);
  xgcv.function = GXor;
  This->gc_image = XCreateGC(This->display, XtWindow(wid), GCFunction, &xgcv);
  xgcv.function = GXcopy;
  This->gc_save = XCreateGC(This->display, XtWindow(wid), GCFunction, &xgcv);
  F(This->Image).savePart(This->Image, XtWindow(wid), This->gc_save, 
			  This->sel_zone->x, This->sel_zone->y,
			  This->sel_zone->width, This->sel_zone->height);
  F(This->Image).drawputPart(This->Image, This->Image2, 
			     This->wdraw, This->gc_save,
			     This->sel_zone->x, This->sel_zone->y,
			     This->sel_zone->width,
			     This->sel_zone->height,
			     This->sel_zone->x, This->sel_zone->y,
			     This->x_start, This->y_start);
 }

 switch (cbs->event->type)
 {
 case ButtonPress:
  {
   XButtonEvent *bevent;
   int		i;

   bevent = (XButtonEvent *)cbs->event;
   if ((bevent->button == Button1) && (bevent->type == ButtonPress))
   {
    int	x;
    int	y;
#ifdef WANT_CLICK_INFO
    printf("receive input %d %d\n",
	   bevent->x - This->x_start,
	   bevent->y - This->y_start);
#endif
    /*--- test if in a zone ---*/
    x = bevent->x - This->x_start;
    y = bevent->y - This->y_start;
    i = 0;
    while (This->all_zone[i].ret_code)
    {
     if ((x >= This->all_zone[i].x) && 
	 (x <= This->all_zone[i].x + This->all_zone[i].width) &&
	 (y >= This->all_zone[i].y) && 
	 (y <= This->all_zone[i].y + This->all_zone[i].height))
      break;
     i++;
    }
    if (This->all_zone[i].ret_code)
    {
     if (This->all_zone+i != This->sel_zone)
     {
      /*--- restore all ball ---*/
      restore_ball(This, This->wdraw, 3);
      restore_ball(This, This->wdraw, 2);
      restore_ball(This, This->wdraw, 1);
      restore_ball(This, This->wdraw, 0);
      /*--- then restore image ---*/
      F(This->Image).restorePart(This->Image, This->wdraw, This->gc_save, 
				 This->x_start, This->y_start);
      /*--- then save new ---*/
      F(This->Image).savePart(This->Image, XtWindow(wid), This->gc_save, 
			      This->all_zone[i].x, This->all_zone[i].y,
			      This->all_zone[i].width,
			      This->all_zone[i].height);
      /*--- then put image ---*/
      F(This->Image).drawputPart(This->Image, This->Image2, 
				 This->wdraw, This->gc_save,
				 This->all_zone[i].x, This->all_zone[i].y,
				 This->all_zone[i].width,
				 This->all_zone[i].height,
				 This->all_zone[i].x, This->all_zone[i].y,
				 This->x_start, This->y_start);
      This->sel_zone = This->all_zone+i;
      This->sel_num = i;
      draw_ball(This, This->wdraw, 0, -20, 0);
      draw_ball(This, This->wdraw, 0, -20, 1);
      draw_ball(This, This->wdraw, 0, -20, 2);
      draw_ball(This, This->wdraw, 0, -20, 3);
      XtVaSetValues(This->Dialog->w_button[XcCDn_CANCEL], XmNlabelString, 
		    lang_cancelXm[This->sel_zone->ret_code-1], NULL);
      XtVaSetValues(This->Dialog->w_button[XcCDn_NEXT], XmNlabelString, 
		    lang_nextXm[This->sel_zone->ret_code-1], NULL);
     }
    }    
   }
  }
  break;
 case KeyPress:
  {
   XKeyEvent	*kevent;
   KeySym	symb;
   kevent = (XKeyEvent *)cbs->event;
   symb = XKeycodeToKeysym(This->display, kevent->keycode, 0);
   if (symb == XK_Up)
   {
    if (!This->sel_zone[1].ret_code)
     This->sel_zone = This->all_zone;
    else
     This->sel_zone = This->sel_zone+1;
      
    /*--- restore all ball ---*/
    restore_ball(This, This->wdraw, 3);
    restore_ball(This, This->wdraw, 2);
    restore_ball(This, This->wdraw, 1);
    restore_ball(This, This->wdraw, 0);
    /*--- then restore image ---*/
    F(This->Image).restorePart(This->Image, This->wdraw, This->gc_save, 
			       This->x_start, This->y_start);
    /*--- then save new ---*/
    F(This->Image).savePart(This->Image, XtWindow(wid), This->gc_save, 
			    This->sel_zone->x, This->sel_zone->y,
			    This->sel_zone->width,
			    This->sel_zone->height);
    /*--- then put image ---*/
    F(This->Image).drawputPart(This->Image, This->Image2, 
			       This->wdraw, This->gc_save,
			       This->sel_zone->x, This->sel_zone->y,
			       This->sel_zone->width,
			       This->sel_zone->height,
			       This->sel_zone->x, This->sel_zone->y,
			       This->x_start, This->y_start);
    draw_ball(This, This->wdraw, 0, -20, 0);
    draw_ball(This, This->wdraw, 0, -20, 1);
    draw_ball(This, This->wdraw, 0, -20, 2);
    draw_ball(This, This->wdraw, 0, -20, 3);
    XtVaSetValues(This->Dialog->w_button[XcCDn_CANCEL], XmNlabelString, 
		  lang_cancelXm[This->sel_zone->ret_code-1], NULL);
    XtVaSetValues(This->Dialog->w_button[XcCDn_NEXT], XmNlabelString, 
		  lang_nextXm[This->sel_zone->ret_code-1], NULL);
   }
  }
  break;
 }
 This->lock = FALSE;
}

/* ----------------------------------------------------------------- ** 
** restore ball                                                      ** 
** ----------------------------------------------------------------- */
static void restore_ball(This, window, num)
c_Box_language	*This;
Window		window;
int		num;
{
 if (This->ball[num].save)
  XCopyArea(This->display, This->ball[num].save, window,
	    This->gc_save,
	    0, 0, 
	    This->ball[num].width, This->ball[num].height,
	    This->ball[num].last_x, This->ball[num].last_y);
 else
  This->ball[num].save = XCreatePixmap(This->display, window,
				       This->ball[num].width,
				       This->ball[num].height,
				       This->depth);
}

/* ----------------------------------------------------------------- ** 
** draw ball                                                         ** 
** ----------------------------------------------------------------- */
static void draw_ball(This, window, x, y, num)
c_Box_language	*This;
Window		window;
int		x;
int		y;
int		num;
{
 x = x-This->ball[num].width/2;
 y = y-This->ball[num].height/2;
 XCopyArea(This->display, window, This->ball[num].save,
	   This->gc_save,
	   x, y,
	   This->ball[num].width, This->ball[num].height,
	   0, 0);
 XCopyArea(This->display, This->ball[num].mask, window,
	   This->gc_mask,
	   0, 0, 
	   This->ball[num].width, This->ball[num].height,
	   x, y);
 XCopyArea(This->display, This->ball[num].image, window,
	   This->gc_image,
	   0, 0, 
	   This->ball[num].width, This->ball[num].height,
	   x, y);
 This->ball[num].last_x = x;
 This->ball[num].last_y = y; 
}

/* ----------------------------------------------------------------- ** 
** create pixmap                                                     ** 
** ----------------------------------------------------------------- */
static void create_pixmap(This, display)
c_Box_language	*This;
Display		*display;
{
 Window		window;
 XImage		*ximage;
 XImage		*xmask;
 GC		gc;
 long		bg,fg;
 int		i;

 window = DefaultRootWindow(display);
 /*--- force mono (last=FALSE) ---*/
 XPMload(display, led5_xpm, &ximage, &xmask, FALSE);
 This->depth = ximage->depth;
 
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

 i = 0;
 while (i < 4)
 {
  This->ball[i].image = XCreatePixmap(display, window,
				      ximage->width,
				      ximage->height,
				      ximage->depth); 
  gc = XCreateGC(display, This->ball[i].image, 0, NULL);
  This->ball[i].width = ximage->width;
  This->ball[i].height = ximage->height;
  XPutImage(display, This->ball[i].image, gc, ximage, 0, 0, 0, 0, 
	    ximage->width, ximage->height);
  XFreeGC(display, gc);

  if (xmask)
  {
   This->ball[i].mask = XCreatePixmap
    (display, window, xmask->width, xmask->height,
     DefaultDepth(display, DefaultScreen(display)));
   
   gc = XCreateGC(display, This->ball[i].mask, 0, 0);
   XSetForeground(display, gc, fg);
   XSetBackground(display, gc, bg);
   XPutImage(display, This->ball[i].mask, gc, xmask, 0, 0, 0, 0,
	     xmask->width, xmask->height);
   XFreeGC(display, gc);
  }
  
  i++;
 }

 if (xmask)
 {
  Xc_free(xmask->data);
  XFree((char *)xmask);
 }
 Xc_free(ximage->data);
 XFree((char *)ximage);
}


static void anim_ball(This)
c_Box_language *This;
{
 float angle;
 click_zone *sel_zone;
 int sizex, sizey, sx, sy;
#if !defined(___mips) && !defined(___sun4)
 static struct tms last, now;
 static clock_t current, prev;
#endif
 
 if (!This->Dialog->mapped_flag || This->lock)
 {
  This->id = XtAppAddTimeOut(This->app, BL_ANIM_DELAY / 4, anim_ball, This);
  return;
 }

 if (TEST_WAIT_MODE) UNSET_WAIT_MODE;

 This->lock = TRUE;

#if !defined(___mips) && !defined(___sun4)
 prev = times(&last);
#else
 This->id = XtAppAddTimeOut(This->app, BL_ANIM_DELAY, anim_ball, This);
#endif
 XFlush(This->display);

 angle = This->angle;
 sel_zone = This->sel_zone;
 sizex = (sel_zone->width / 2);
 sizey = (sel_zone->height / 2);
 sx = This->x_start + sizex + sel_zone->x;
 sy = This->y_start + sizey + sel_zone->y;

 restore_ball(This, This->wdraw, 3);
 restore_ball(This, This->wdraw, 2);
 restore_ball(This, This->wdraw, 1);
 restore_ball(This, This->wdraw, 0);

 draw_ball(This, This->wdraw,
	   (int)(sizex * cos(2 * angle)) + sx,
	   (int)(sizey * sin(2 * angle)) + sy, 0);
 draw_ball(This, This->wdraw,
	   (int)(sizex * cos(2 * angle + 3.14)) + sx,
	   (int)(sizey * sin(2 * angle + 3.14)) + sy, 1);
 draw_ball(This, This->wdraw,
	   (int)(sizex * cos(angle + 1.57)) + sx,
	   (int)(sizey * sin(2 * (angle + 1.57))) + sy, 2);
 draw_ball(This, This->wdraw,
	   (int)(sizex * cos(-(angle + 4.71))) + sx,
	   (int)(sizey * sin(-2 * (angle + 4.71))) + sy, 3);
 
 XFlush(This->display);

 This->angle += 3.1415926 / 50;

#if !defined(___mips) && !defined(___sun4)
#define ELAPSED_TIME  1000 * (current - prev) / CLK_TCK
 current = times(&now);
 This->id = XtAppAddTimeOut(This->app, MAX(1, BL_ANIM_DELAY - ELAPSED_TIME),
			    anim_ball, This); 
#endif
 This->lock = FALSE;
}













