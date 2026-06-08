/*
** Keyboard_Box.c for Xclamation and XAllWrite in Keyboard/
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
** Started on  Sat Dec 28 16:26:43 1996 Emmanuel Paris
** Last update Sat Jul 31 16:16:48 1999 Emmanuel Paris
*/

#include "Keyboard.h"
#include "Resource.h"
#include "Colormap.h"
#include "FontManager.h" 

extern c_FontManager	*GlobFntMng;
extern c_Colormap	*GlobColormap;
extern c_Resource	*GlobResources;

static void calcul_font_size ___PROTO((c_Keyboard *This));
static void redraw_area ___PROTO((c_Keyboard *This, int x, int y, int width,
				  int height));
static void redraw_cell ___PROTO((c_Keyboard *This, int x, int y, 
				  unsigned long color));
static void cb_action ___PROTO((c_CustomDialog *cb_dialog,
                                c_Keyboard *This, int cb_reason));

static void eventlook_drawing ___PROTO((Widget w, c_Keyboard *This,
					XmDrawingAreaCallbackStruct *cbs));
static void eventlook_drawing2 ___PROTO((Widget w, c_Keyboard *This,
					 XEvent *event));

static void cb_one_char ___PROTO((c_Keyboard *This, char c, 
				  c_TextStyle *style));

void create_Box_Insert(This, w_Parent, style, callback, cb_data)
c_Keyboard *This;
Widget	   w_Parent;
c_TextStyle *style;
void	   (*callback)();
void	   *cb_data;
{
 XmString mstring;
 char	   string[200];
 Arg      args[1];
  
 This->keyb_box.tstyle = style;
 This->keyb_box.callback = callback;
 This->keyb_box.cb_data = cb_data;
 This->insertBox = FALSE;
 This->keyb_box.xs = This->keyb_box.ys = -1;
 This->keyb_box.pressed = FALSE;
 calcul_font_size(This);
  
 This->keyb_box.Dialog = (c_CustomDialog *)NEW(c_CustomDialog)
  (w_Parent, "BoxInsertChar", 0, 0/*, MIN(450, This->keyb_box.width + 100),
   MIN(450, This->keyb_box.height + 100)*/, XcCD_HORIZONTAL | 
   XcCD_HRESIZE | XcCD_VRESIZE);


 XtVaSetValues(WIDGET(This->keyb_box.Dialog),
	       XmNresizePolicy, XmRESIZE_GROW, NULL);

 This->keyb_box.w_This = WIDGET(This->keyb_box.Dialog);
  
 This->keyb_box.Grid = (c_Grid *)NEW(c_Grid)(WIDGET((&This->keyb_box)),
					     MANAGER(This->keyb_box.Dialog),
					     "gBoxInsertChar", FALSE);
  
 This->keyb_box.lBKeybTitle = (c_Label *)NEW(c_Label)
  (WIDGET(This->keyb_box.Grid), MANAGER(This->keyb_box.Dialog),
   "lBoxKeybTitle" );
  
 This->keyb_box.lBKeybFontName = (c_Label *)NEW(c_Label)
  (WIDGET(This->keyb_box.Grid), MANAGER(This->keyb_box.Dialog),
   "lBoxKeybFontName" );
  
 sprintf(string, F(GlobResources).getString(GlobResources, XcR_font_name),
	 style->font->name);
 mstring = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(args[0], XmNlabelString, mstring);
 XtSetValues(WIDGET(This->keyb_box.lBKeybFontName), args, 1);
  
 This->keyb_box.fBKeyb = (c_Frame *)NEW(c_Frame)
  (WIDGET(This->keyb_box.Grid), MANAGER(This->keyb_box.Dialog), NULL, 0);
  
 This->keyb_box.swBKeyb = (c_ScrolledWindow *)NEW(c_ScrolledWindow)
  (WIDGET(This->keyb_box.fBKeyb), MANAGER(This->keyb_box.Dialog), "swBKeyb");
    
 This->keyb_box.drawing = (c_DrawingArea *)NEW(c_DrawingArea)
  (WIDGET(This->keyb_box.swBKeyb), MANAGER(This->keyb_box.Dialog), "dBKeyb",
   This->keyb_box.width, This->keyb_box.height);
  
 XtSetArg(args[0], XmNworkWindow, WIDGET(This->keyb_box.drawing));
 XtSetValues(WIDGET(This->keyb_box.swBKeyb), args, 1);
 This->keyb_box.window = (Window)0;
  
 XtAddCallback(WIDGET(This->keyb_box.drawing), XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, This);
 XtAddCallback(WIDGET(This->keyb_box.drawing), XmNinputCallback,
	       (XtCallbackProc)eventlook_drawing, This);
 XtAddEventHandler(WIDGET(This->keyb_box.drawing), 
		   PointerMotionMask| LeaveWindowMask,
		   False, (XtEventHandler)eventlook_drawing2, This);
 
 {
  Dimension mw, mh, st, w, h;

  XtVaGetValues(WIDGET(This->keyb_box.fBKeyb), 
		XmNmarginWidth, &mw, XmNmarginHeight, &mh,
		XmNshadowThickness, &st, NULL);

  w = This->keyb_box.width + 2 * (mw + st);
  h =  This->keyb_box.height + 2 * (mh + st);
  
  XtVaSetValues(WIDGET(This->keyb_box.fBKeyb), 
		XmNwidth, w, XmNheight, h, NULL);
  
  F(This->keyb_box.Grid).addColumn(This->keyb_box.Grid, XcG_MAX_SIZE,
				   WIDGET(This->keyb_box.lBKeybTitle), 20,
				   WIDGET(This->keyb_box.lBKeybFontName), 20,
				   WIDGET(This->keyb_box.fBKeyb), XcG_MAX_SIZE,
				   NULL);
  
  XtVaSetValues(WIDGET(This->keyb_box.Grid), 
		XmNwidth, w + 5, XmNheight, h + 99, NULL);

 }

 F(This->keyb_box.Dialog).createActionArea(This->keyb_box.Dialog, 
					   XcCD_CLOSE, XcCD_CLOSE, FALSE,
					   (void (*)())cb_action, This);
  
 F(This->keyb_box.Dialog).map(This->keyb_box.Dialog, TRUE);
}

char get_one_char_Box_Insert(This, w_Parent, style)
c_Keyboard *This;
Widget w_Parent;
c_TextStyle *style;
{
 int rc;

 This->keyb_box.got_char = '\0';
 create_Box_Insert(This, w_Parent, style, (void (*)())cb_one_char, This);
 
 rc = F(This->keyb_box.Dialog).waitForUnmap(This->keyb_box.Dialog);
 Xc_TRACE(("Get One Char: `%c'", This->keyb_box.got_char));
 return This->keyb_box.got_char;
}

static void calcul_font_size(This)
c_Keyboard *This;
{
 coord_t xmax, ymax, tmp;
 scale_t scale;
 int	i, width, height;
 unsigned char  c;
  
 scale = SCALE_FROM_COEF( (real)SCALE_FROM_POINTS(16.0) / 
			 (real)This->keyb_box.tstyle->metrics->point_size);
  
 This->keyb_box.metrics = F(This->keyb_box.tstyle->metrics).transform
  (This->keyb_box.tstyle->metrics, scale, SCALE_ONE, ANGLE_ZERO);
  
 xmax = ymax = COORD_ZERO;
 for(i = 32; i < 256; i++)
 {
  c = (unsigned char)i;
  tmp = F(This->keyb_box.metrics).textWidth(This->keyb_box.metrics, &c, 1);
  if (tmp > xmax) xmax = tmp;
 }
 ymax = SCALE_FROM_POINTS(16.0);
 /* This->keyb_box.metrics->ascender - This->keyb_box.metrics->descender; */
  
 width = 16 * SCALE_TO_PIXELS(xmax);
 height = 14 * SCALE_TO_PIXELS(ymax);
  
 Xc_TRACE(("ascender: %d descender: %d", 
	   SCALE_TO_PIXELS(This->keyb_box.metrics->ascender), 
	   SCALE_TO_PIXELS(-This->keyb_box.metrics->descender) ));
 /*  if (width > 800 || height > 600)
     {
     scale = MIN(SCALE_FROM_COEF(width / 800.0), 
     SCALE_FROM_COEF(height / 600.0));
     
     This->keyb_box.metrics = F(This->keyb_box.tstyle->metrics).transform
     (This->keyb_box.tstyle->metrics, SCALE_ONE, SCALE_ONE, ANGLE_ZERO);
     
     xmax = ymax = COORD_ZERO;
     for(i = 32; i < 256; i++)
     {
     c = (unsigned char)i;
     tmp = F(This->keyb_box.metrics).textWidth(This->keyb_box.metrics, &c, 1);
     if (tmp > xmax) xmax = tmp;
     }
     ymax = (This->keyb_box.metrics->ascender - 
     This->keyb_box.metrics->descender);
     
     width = 16 * SCALE_TO_PIXELS(xmax);
     height = 14 * SCALE_TO_PIXELS(ymax);
     } */
 This->keyb_box.width = width + 10 * 16 + 1;
 This->keyb_box.height = height + 10 * 14 + 1;  
 This->keyb_box.cwidth = SCALE_TO_PIXELS(xmax) + 10;
 This->keyb_box.cheight = SCALE_TO_PIXELS(ymax) + 10;
}

static void redraw_area(This, x, y, width, height)
c_Keyboard	*This;
int		x, y, width, height;
{
 Display *display;
 Window  window;
 GC	gc;
 int	i, j, k, l, x1, x2, y1, y2;
 int	xmin, xmax, ymin, ymax;
 static XSegment segment[32];
 vector_t	pos;
 coord_t	to_center, tmp;
 char	c;
  
 display = This->keyb_box.display;
 window = This->keyb_box.window;
 gc = This->keyb_box.gc;
  
 xmin = (x / This->keyb_box.cwidth);
 xmax = ((x + width) / This->keyb_box.cwidth) + 1;
 ymin = (y / This->keyb_box.cheight);
 ymax = ((y + height) / This->keyb_box.cheight) + 1;
  
 x1 = xmin * This->keyb_box.cwidth;
 x2 = xmax * This->keyb_box.cwidth;
 y1 = ymin * This->keyb_box.cheight;
 y2 = ymax * This->keyb_box.cheight;
  
 k = 0; j = x1 + 1;
 for(i = xmin; i <= xmax+1; i++)
 {
  segment[k].x1 = segment[k].x2 = j;
  segment[k].y1 = y1; segment[k].y2 = y2; 
  j += This->keyb_box.cwidth;
  k++;
 }
 j = y1 + 1;
 for(i = ymin; i <= ymax+1; i++)
 {
  segment[k].y1 = segment[k].y2 = j;
  segment[k].x1 = x1; segment[k].x2 = x2; 
  j += This->keyb_box.cheight;
  k++;
 }
  
 XClearArea(display, window, x1, y1, x2-x1+1, y2-y1+1, False);
 XDrawSegments(display, window, gc, segment, k);

 if (xmax == 16) x2 --;
 if (ymax == 14) y2 --;
 k = 0; j = x1;
 for(i = xmin; i <= xmax+1; i++)
 {
  segment[k].x1 = segment[k].x2 = j;
  segment[k].y1 = y1; segment[k].y2 = y2; 
  j += This->keyb_box.cwidth;
  k++;
 }
 j = y1;
 for(i = ymin; i <= ymax+1; i++)
 {
  segment[k].y1 = segment[k].y2 = j;
  segment[k].x1 = x1; segment[k].x2 = x2; 
  j += This->keyb_box.cheight;
  k++;
 }
 XSetForeground(display, gc, This->keyb_box.dark);
 XDrawSegments(display, window, gc, segment, k);
  
 to_center = ((This->keyb_box.cwidth - 12) << COORD_SCALEBITS);
 k = y1 + This->keyb_box.cheight - 3;
 for(j = ymin; j <= ymax; j++)
 {
  l = x1 + 7;
  for(i = xmin; i <= xmax; i++)
  {
   c = i + j * 16 + 32;
   tmp = F(This->keyb_box.metrics).textWidth(This->keyb_box.metrics, &c, 1);
      
   pos.dx = (l << COORD_SCALEBITS) + ((to_center - tmp) >> 1);
   pos.dy = (k << COORD_SCALEBITS) + This->keyb_box.metrics->descender;
      
   F(This->keyb_box.rfont).drawString(This->keyb_box.rfont, display, window,
				      This->keyb_box.gc_mask, 
				      This->keyb_box.gc_font, &pos, 
				      COORD_ZERO, COORD_ZERO,
				      &c, 1, NULL); 
   l += This->keyb_box.cwidth;
  }
  k += This->keyb_box.cheight;
 }  
  
 XSetForeground(display, gc, This->keyb_box.light);
}

static void redraw_cell(This, cx, cy, color)
c_Keyboard	*This;
int		cx, cy;
unsigned long	color;
{
 Display *display;
 Window  window;
 GC	gc;
 int	x, y;
 vector_t	pos;
 coord_t	to_center, tmp;
 char	c;
  
 display = This->keyb_box.display;
 window = This->keyb_box.window;
 gc = This->keyb_box.gc;
  
 x = cx * This->keyb_box.cwidth + 2;
 y = cy * This->keyb_box.cheight + 2;
    
 if (color)
 {
  XSetForeground(display, gc, color);
  XFillRectangle(display, window, gc, x, y, 
		 This->keyb_box.cwidth - 2, This->keyb_box.cheight - 2);
 }
 else
  XClearArea(display, window, x, y, 
	     This->keyb_box.cwidth - 2, This->keyb_box.cheight - 2, False);
 
 to_center = ((This->keyb_box.cwidth - 12) << COORD_SCALEBITS);
 x += 5; y += This->keyb_box.cheight - 5;

 c = cx + cy * 16 + 32;
 tmp = F(This->keyb_box.metrics).textWidth(This->keyb_box.metrics, &c, 1);
 
 pos.dx = (x << COORD_SCALEBITS) + ((to_center - tmp) >> 1);
 pos.dy = (y << COORD_SCALEBITS) + This->keyb_box.metrics->descender;
 
 F(This->keyb_box.rfont).drawString(This->keyb_box.rfont, display, window,
				    This->keyb_box.gc_mask, 
				    This->keyb_box.gc_font, &pos, 
				    COORD_ZERO, COORD_ZERO,
				    &c, 1, NULL); 
  
 XSetForeground(display, gc, This->keyb_box.light);
}
static void cb_action(cb_dialog, This, cb_reason)
c_CustomDialog          *cb_dialog;
c_Keyboard		*This;
int                     cb_reason;
{
 XFreeGC(This->keyb_box.display, This->keyb_box.gc);
 XFreeGC(This->keyb_box.display, This->keyb_box.gc_font);
#ifdef Xc_XCLAMATION 
 if (This->keyb_box.gc_mask != (GC)0)
  XFreeGC(This->keyb_box.display, This->keyb_box.gc_mask);
#else
 XFreeGC(This->keyb_box.display, This->keyb_box.gc_mask);
#endif

 F(cb_dialog).unmap(cb_dialog);
  
 DELETE(c_DrawingArea)(This->keyb_box.drawing);
 DELETE(c_ScrolledWindow)(This->keyb_box.swBKeyb);
 DELETE(c_Frame)(This->keyb_box.fBKeyb);
 DELETE(c_Label)(This->keyb_box.lBKeybFontName);
 DELETE(c_Label)(This->keyb_box.lBKeybTitle);
 DELETE(c_Grid)(This->keyb_box.Grid);
 DELETE(c_CustomDialog)(This->keyb_box.Dialog);
  
 F(GlobFntMng).delete_font(GlobFntMng, This->keyb_box.rfont);
 This->keyb_box.Dialog = NULL;
}

static void eventlook_drawing(w, This, cbs)
Widget				w;
c_Keyboard			*This;
XmDrawingAreaCallbackStruct	*cbs;
{
 eventlook_drawing2(w, This, cbs->event);
}

static void eventlook_drawing2(w, This, event)
Widget				w;
c_Keyboard			*This;
XEvent				*event;
{
 Display	*display;
 Window	window;
 color_cells_t	realcolor;
 int		x, y;
  
  
 if (!This->keyb_box.window)
 {
  XGCValues	val_gc;
    
  display = This->keyb_box.display = 
   XtDisplay(WIDGET(This->keyb_box.drawing));
  window = This->keyb_box.window = XtWindow(WIDGET(This->keyb_box.drawing));
    
  if (DefaultDepth(display, DefaultScreen(display)) == 1)
  {
   This->keyb_box.light = This->keyb_box.normal = This->keyb_box.select =
    WhitePixel(display, DefaultScreen(display));
   This->keyb_box.dark =
    BlackPixel(display, DefaultScreen(display));
  }
  else
  {
   F(GlobColormap).allocate_named_color(GlobColormap, BOX_KEYB_LIGHT,
					&realcolor);
   This->keyb_box.light = realcolor.pixel;
   F(GlobColormap).allocate_named_color(GlobColormap, BOX_KEYB_DARK,
					&realcolor);
   This->keyb_box.dark = realcolor.pixel;
   F(GlobColormap).allocate_named_color(GlobColormap, BOX_KEYB_NORM,
					&realcolor);
   This->keyb_box.normal = realcolor.pixel;
   F(GlobColormap).allocate_named_color(GlobColormap, BOX_KEYB_SELECT,
					&realcolor);
   This->keyb_box.select = realcolor.pixel;
  }
    
  val_gc.function = GXcopy;
  val_gc.foreground = This->keyb_box.light;
  val_gc.line_width = 0;
  val_gc.line_style = LineSolid; 
  val_gc.cap_style = CapNotLast;
  val_gc.join_style = JoinMiter;
  This->keyb_box.gc = XCreateGC(display,window, GCFunction | GCForeground
				| GCLineWidth | GCLineStyle | GCCapStyle
				| GCJoinStyle, &val_gc);
  This->keyb_box.gc_font = XCreateGC(display, window, 0, NULL);
#ifdef Xc_XCLAMATION
  This->keyb_box.gc_mask = (GC)0;
#else
  This->keyb_box.gc_mask = XCreateGC(display, window, 0, NULL);
#endif
  This->keyb_box.rfont = This->keyb_box.metrics->raster_font = 
   F(GlobFntMng).new_font(GlobFntMng, This->keyb_box.tstyle->font,
			  This->keyb_box.metrics);
    
#ifdef Xc_XCLAMATION
  F(This->keyb_box.rfont).setGC(display, window, &(This->keyb_box.gc_mask),
				This->keyb_box.gc_font);
#else
  F(This->keyb_box.rfont).setGC(display, window, This->keyb_box.gc_mask,
				This->keyb_box.gc_font);
#endif
  XSetForeground(display, This->keyb_box.gc_font, 
		 BlackPixel(display, DefaultScreen(display)));
 }
  
 switch(event->type)
 {
 case Expose:
 case GraphicsExpose:
  Xc_TRACE(("Expose"));
  redraw_area(This, event->xexpose.x, event->xexpose.y,
	      event->xexpose.width, event->xexpose.height);
  break;
 case ButtonPress:
  Xc_TRACE(("ButtonPress"));
  if (event->xbutton.button == 1)
  {
   This->keyb_box.pressed = TRUE;
   x = (event->xbutton.x / This->keyb_box.cwidth);
   y = (event->xbutton.y / This->keyb_box.cheight);
   redraw_cell(This, x, y, This->keyb_box.select);
  }
  break;
 case ButtonRelease:
  if (event->xbutton.button == 1)
  {
   This->keyb_box.pressed = FALSE;
   x = (event->xbutton.x / This->keyb_box.cwidth);
   y = (event->xbutton.y / This->keyb_box.cheight);
   This->keyb_box.callback(This->keyb_box.cb_data, (char)(x + y * 16 + 32),
			   This->keyb_box.tstyle);
   if (This->keyb_box.Dialog)
    redraw_cell(This, x, y, This->keyb_box.normal);
  }
  Xc_TRACE(("ButtonRelease"));
  break;
 case MotionNotify:
  x = (event->xbutton.x / This->keyb_box.cwidth);
  y = (event->xbutton.y / This->keyb_box.cheight);

  if (x != This->keyb_box.xs || y != This->keyb_box.ys)
  {
   if (This->keyb_box.xs != -1 && This->keyb_box.ys != -1)
    redraw_cell(This, This->keyb_box.xs, This->keyb_box.ys, 0);		
   
   if (This->keyb_box.pressed)
    redraw_cell(This, x, y, This->keyb_box.select);
   else
    redraw_cell(This, x, y, This->keyb_box.normal);
   
   This->keyb_box.xs = x; This->keyb_box.ys = y;
  }
  break;
 case LeaveNotify:
  if (This->keyb_box.xs != -1 && This->keyb_box.ys != -1)
  {
   redraw_cell(This, This->keyb_box.xs, This->keyb_box.ys, 0);		
   This->keyb_box.xs = This->keyb_box.ys = -1;
  }
  break;
 }
}

/* ----------------------------------------------------------------- ** 
** cb_one_char - callback for get_char_Box_Insert()                  ** 
** ----------------------------------------------------------------- */
static void cb_one_char(This, c, style)
c_Keyboard *This;
char c; 
c_TextStyle *style;
{
 This->keyb_box.got_char = c;
 cb_action(This->keyb_box.Dialog, This, 0);
}













