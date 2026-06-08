/*
** Box_fond.c for XQuad in Box_fond/
** Object and method for Box_fond Class
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
** Started on  Mon Dec  5 16:33:17 1994 Emmanuel Paris
** Last update Sat Aug 14 12:56:51 1999 Emmanuel Paris
*/

#define _CELLSTYLE_PATTERN_
#include "Box_fond.h"
#include "Colormap.h"

extern c_Colormap *GlobColormap;

static void *cons_Box_fond();
static void dest_Box_fond();
static void *copy_Box_fond();

static void change_fore_color();
static void change_back_color();
static void refresh_drawing();
static void focus_in_pattern();
static void focus_out_pattern();
static void change_pattern();
static void eventlook_drawing();
static void eventlook_drawing_motion();

sf_Box_fond fc_Box_fond =
{
  cons_Box_fond,
  dest_Box_fond,
  copy_Box_fond
};

/* --------- Constructor of the object ... -------------- */

static void *cons_Box_fond(w_Parent, pszTitle, BaseStd, inside)
Widget		w_Parent;
char		*pszTitle;
BaseStd_t 	*BaseStd;
cell_inside_t	*inside;
{
  c_Box_fond	*ObjTmp;
  Display	*display;
  color_cells_t	realcolor;
  Arg		args[10];
  int		nargs;
  c_Color	*color_base;
  
  color_base = BaseStd->color_base;
  
  if ((ObjTmp = (c_Box_fond *)Xc_malloc("Box_pattern",
					  sizeof(c_Box_fond))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Box_fond;
  ObjTmp->BaseStd = BaseStd;
  ObjTmp->inside = inside;
  
  display = XtDisplay(w_Parent);
  if (DefaultDepth(display, DefaultScreen(display)) == 1)
    {
      ObjTmp->light = WhitePixel(display,DefaultScreen(display));
      ObjTmp->dark =
	ObjTmp->normal = BlackPixel(display,DefaultScreen(display));
    }
  else
    {
      F(GlobColormap).allocate_named_color(GlobColormap, BOX_FOND_LIGHT,
					   &realcolor);
      ObjTmp->light = realcolor.pixel;
      F(GlobColormap).allocate_named_color(GlobColormap, BOX_FOND_DARK,
					   &realcolor);
      ObjTmp->dark = realcolor.pixel;
      F(GlobColormap).allocate_named_color(GlobColormap, BOX_FOND_NORM,
					   &realcolor);
      ObjTmp->normal = realcolor.pixel;
    }
  ObjTmp->source = FALSE;
  
  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle,
							 365, 365,
							 XcCD_VERTICAL);
  
  ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

  ObjTmp->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Dialog),
					   MANAGER(ObjTmp->Dialog),
					   "lBoxPatternTitle");

  ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Dialog),
					   MANAGER(ObjTmp->Dialog), 
					   "fBPColors");
  
  ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
					MANAGER(ObjTmp->Dialog),
					"gBPColors", TRUE);
  ObjTmp->LaColorFor = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					       MANAGER(ObjTmp->Dialog),
					       "lBPForeColor");
  ObjTmp->ClColorFor =(c_ColorList *)
   NEW(c_ColorList)(WIDGET(ObjTmp->Grid1), MANAGER(ObjTmp->Dialog),
		    "clBPForeColor", XcCL_NORMAL);
  
  F(ObjTmp->ClColorFor).setCallback(ObjTmp->ClColorFor,
				    change_fore_color, ObjTmp);
  
  F(ObjTmp->ClColorFor).setColorBase(ObjTmp->ClColorFor,
				     color_base, CL_KEEP_TRANSPARENCY);
  
  ObjTmp->LaColorBac = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					       MANAGER(ObjTmp->Dialog),
					       "lBPBackColor");
  ObjTmp->ClColorBac = (c_ColorList *)
   NEW(c_ColorList)(WIDGET(ObjTmp->Grid1), MANAGER(ObjTmp->Dialog),
		    "clBPBackColor", XcCL_NORMAL);
  
  F(ObjTmp->ClColorBac).setCallback(ObjTmp->ClColorBac,
				    change_back_color, ObjTmp);
  
  F(ObjTmp->ClColorBac).setColorBase(ObjTmp->ClColorBac,
				     color_base, CL_KEEP_TRANSPARENCY);


  ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Dialog),
					   MANAGER(ObjTmp->Dialog), 
					   "fBPPatterns", 0);
  F(ObjTmp->ClColorFor).setColor(ObjTmp->ClColorFor,
				 ObjTmp->inside->foreground);
  F(ObjTmp->ClColorBac).setColor(ObjTmp->ClColorBac,
				 ObjTmp->inside->background);
  
  nargs = 0;
  XtSetArg(args[nargs], XmNshadowThickness, 0);
  nargs++;
  ObjTmp->drawing = XmCreateDrawingArea(WIDGET(ObjTmp->Frame2),
					"daBPdrawing", args, nargs);
  XtAddCallback(ObjTmp->drawing, XmNexposeCallback,
		(XtCallbackProc)eventlook_drawing, ObjTmp);
  XtAddCallback(ObjTmp->drawing, XmNinputCallback,
		(XtCallbackProc)eventlook_drawing, ObjTmp);
  
  
  XtAddEventHandler(ObjTmp->drawing, PointerMotionMask, False,
		    eventlook_drawing_motion, (void *)ObjTmp);  
  /*  gc = XCreateGC(XtDisplay(ObjTmp->drawing),
RootWindowOfScreen(XtScreen(ObjTmp->drawing)), 0, NULL);
XtVaSetValues(ObjTmp->drawing, XmNuserData, gc, NULL);
*/

  F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				       ObjTmp->drawing,
				       WIDGET(ObjTmp->Frame2));
  
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 30,
			     WIDGET(ObjTmp->LaColorFor), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->ClColorFor), 122,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 30,
			     WIDGET(ObjTmp->LaColorBac), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->ClColorBac), 122,
			     NULL);
  F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);

  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog,
				     XcCD_OK | XcCD_CANCEL, XcCD_OK,
				     TRUE, NULL, NULL);
  
  F(ObjTmp->Dialog).blockChild((ObjTmp->Dialog), WIDGET(ObjTmp->Ltitre), 30);
  
  F(ObjTmp->Dialog).blockChild((ObjTmp->Dialog), WIDGET(ObjTmp->Frame2), 158);
  
  F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
  refresh_drawing(ObjTmp);
  Xc_TRACE(("Object Box_pattern build"));
  return ObjTmp;
}

/* ----------------- Destructor of the object ---------------- */

static void dest_Box_fond(This)
c_Box_fond *This;
{
  
  XtDestroyWidget(This->drawing);
  DELETE(c_Frame)(This->Frame2);
  
  DELETE(c_ColorList)(This->ClColorFor);
  DELETE(c_ColorList)(This->ClColorBac);
  DELETE(c_Label)(This->LaColorFor);
  DELETE(c_Label)(This->LaColorBac);

  DELETE(c_Grid)(This->Grid1);
  DELETE(c_Frame)(This->Frame1);
  DELETE(c_Label)(This->Ltitre);
  DELETE(c_CustomDialog)(This->Dialog);
  Xc_free(This);
  Xc_TRACE(("Object Box_pattern destroyed"));
}

/* ------------ copy method for the object .... --------------- */

static void *copy_Box_fond(This)
c_Box_fond *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

/* ----------------------------------------------------------------- ** 
** Change_Sel - Callback for reason Selection had change             ** 
** ----------------------------------------------------------------- */

static void change_fore_color(this, box, color)
c_ColorList	*this;
c_Box_fond	*box;
c_Color		*color;
{
  if (box->inside->foreground != color)
    {
      box->inside->foreground = color;
      refresh_drawing(box);
    }
}

static void change_back_color(this, box, color)
c_ColorList	*this;
c_Box_fond	*box;
c_Color		*color;
{
  if (box->inside->background != color)
    {
      box->inside->background = color;
      refresh_drawing(box);
    }
}

static void refresh_drawing(This)
c_Box_fond	*This;
{
  Display	*display;
  Window	window;
  GC		gc;
  XGCValues	val_gc;
  static XSegment	segment[14];
  int		x, y, i;
  
  display = XtDisplay(This->drawing);
  window = XtWindow(This->drawing);
  val_gc.function = GXcopy;
  val_gc.foreground = This->light;
  val_gc.line_width = 0;
  val_gc.line_style = LineSolid; 
  val_gc.cap_style = CapNotLast;
  val_gc.join_style = JoinMiter;
  gc = XCreateGC(display,window, GCFunction | GCForeground
		 | GCLineWidth | GCLineStyle | GCCapStyle
		 | GCJoinStyle, &val_gc);
  i = 0;
  for (x = 0; x < 306; x += 34)
    {
      segment[i].x1 = segment[i].x2 = x;
      segment[i].y1 = 0;
      segment[i++].y2 = 136;
    }
  for (y = 0; y < 102; y += 34)
    {
      segment[i].y1 = segment[i].y2 = y;
      segment[i].x1 = 0;
      segment[i++].x2 = 340;
    }
  XDrawSegments(display, window, gc, segment, 14);
  XSetForeground(display, gc, This->dark);
  i = 0;
  for (x = 33; x <= 339; x += 34)
    {
      segment[i].x1 = segment[i].x2 = x;
      segment[i].y1 = 0;
      segment[i++].y2 = 136;
    }
  for (y = 33; y <= 135; y += 34)
    {
      segment[i].y1 = segment[i].y2 = y;
      segment[i].x1 = 0;
      segment[i++].x2 = 340;
    }
  XDrawSegments(display, window, gc, segment, 14);
  
  
  i = CELL_PATTERN_DATA_FIRST;
  for (y = 1; y <= 103; y += 34)
    {
      for (x = 1; x <= 307; x += 34)
	{
	  if (x != 1 || y != 1)
	    {
	      XSetStipple(display, gc, 
			  Cell_Pattern[i++].definition.data.pixmap);
	      XFillRectangle(display, window, gc, x, y, 32, 32);
	    }
	  else
	    {
	      val_gc.foreground = This->normal;
	      val_gc.fill_style = FillSolid;
	      XChangeGC(display, gc, 
			GCForeground|GCFillStyle, &val_gc);
	      XFillRectangle(display, window, gc, x, y, 32, 32);
	      val_gc.foreground = This->inside->foreground->cell.pixel;
	      val_gc.background = This->inside->background->cell.pixel;
	      val_gc.fill_style = FillOpaqueStippled;
	      XChangeGC(display, gc, 
			GCForeground | GCBackground | GCFillStyle, &val_gc);
	    }
	}
    }
  XFreeGC(display, gc);
  focus_in_pattern(This);
}

static void focus_in_pattern(This)
c_Box_fond	*This;
{
  Display	*display;
  Window	window;
  GC		gc;
  XGCValues	val_gc;
  int	pattern;
  int	x;
  int	y;
  static XRectangle	rect[4];
  static XSegment	segment[2];
  
  display = XtDisplay(This->drawing);
  window = XtWindow(This->drawing);
  val_gc.foreground = This->normal;
  val_gc.line_width = 0;
  val_gc.fill_style = FillSolid;
  gc = XCreateGC(display, window, 
		 GCForeground | GCLineWidth | GCFillStyle, &val_gc);
  
  if (This->inside->pattern == PATTERN_NONE)
    pattern = 0;
  else
    pattern = This->inside->pattern - CELL_PATTERN_DATA_FIRST + 1;
  
  x = (pattern % 10) * 34 + 1;
  y = (pattern / 10) * 34 + 1;
  
  rect[0].x = rect[1].x = rect[2].x = x;
  rect[3].x = x + 28;
  rect[0].y = y;
  rect[1].y = y + 28;
  rect[2].y = rect[3].y = y + 4;
  rect[0].width = rect[1].width = 32;
  rect[0].height = rect[1].height = 4;
  rect[2].width = rect[3].width = 4;
  rect[2].height = rect[3].height = 24;
  XFillRectangles(display, window, gc, rect, 4);
  
  XSetForeground(display, gc, This->dark);
  segment[0].x1 = segment[0].x2 = segment[1].x1 = x + 4;
  segment[1].x2 = x + 27;
  segment[0].y2 = segment[1].y1 = segment[1].y2 = y + 4;
  segment[0].y1 = y + 27;
  XDrawSegments(display, window, gc, segment, 2);
  
  XSetForeground(display, gc, This->light);
  segment[0].x1 = segment[0].x2 = segment[1].x1 = x + 27;
  segment[1].x2 = x + 5;
  segment[0].y2 = segment[1].y1 = segment[1].y2 = y + 27;
  segment[0].y1 = y + 5;
  XDrawSegments(display, window, gc, segment, 2);
  
  XFreeGC(display, gc);
}

static void focus_out_pattern(This)
c_Box_fond	*This;
{
  Display	*display;
  Window	window;
  GC		gc;
  XGCValues	val_gc;
  int	pattern;
  int	x;
  int	y;
  
  display = XtDisplay(This->drawing);
  window = XtWindow(This->drawing);
  
  if (This->inside->pattern == PATTERN_NONE)
    {
      val_gc.foreground = This->normal;
      val_gc.fill_style = FillSolid;
      gc = XCreateGC(display, window, 
		     GCForeground|GCFillStyle, &val_gc);
      XFillRectangle(display, window, gc, 1, 1, 32, 32);
    }
  else
    {
      pattern = This->inside->pattern - CELL_PATTERN_DATA_FIRST + 1;
      x = (pattern % 10) * 34 + 1;
      y = (pattern / 10) * 34 + 1;
      val_gc.foreground = This->inside->foreground->cell.pixel;
      val_gc.background = This->inside->background->cell.pixel;
      val_gc.fill_style = FillOpaqueStippled;
      val_gc.stipple = 
	Cell_Pattern[This->inside->pattern].definition.data.pixmap;
      gc = XCreateGC(display, window, 
		     GCForeground | GCBackground | GCFillStyle | GCStipple,
		     &val_gc);
      XFillRectangle(display, window, gc, x, y, 32, 32);
    }
  
  XFreeGC(display, gc);
}

static void change_pattern(This, x, y)
c_Box_fond	*This;
int		x;
int		y;
{
  int	pattern;
  
  if (x<0 || x>=340 || y<0 || y>=136)
    return;
  pattern = (x/34) + (y/34) * 10;
  if (pattern)
    pattern += CELL_PATTERN_DATA_FIRST -1;
  else
    pattern = PATTERN_NONE;
    
  if (pattern != This->inside->pattern)
    {
      focus_out_pattern(This);
      This->inside->pattern = pattern;
      focus_in_pattern(This);
    }
}

static void eventlook_drawing(w, This, cbs)
Widget				w;
c_Box_fond			*This;
XmDrawingAreaCallbackStruct	*cbs;
{
  XEvent	*event;
  
  event = cbs->event;
  switch(event->type)
    {
    case Expose:
    case GraphicsExpose:
      refresh_drawing(This);
      break;
    case ButtonPress:
      This->source = TRUE;
      change_pattern(This, event->xbutton.x, event->xbutton.y);
      break;
    case ButtonRelease:
      This->source = FALSE;
      change_pattern(This, event->xbutton.x, event->xbutton.y);
      break;
    }
}

static void eventlook_drawing_motion(w, This, event)
Widget		w;
c_Box_fond	*This;
XEvent		*event;
{
  if (This->source)
    {
      change_pattern(This, event->xmotion.x, event->xmotion.y);
    }
}



