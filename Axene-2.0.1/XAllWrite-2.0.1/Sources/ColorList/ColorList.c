/*
** ColorList.c for Xclamation, XQuad and XAllWrite in ColorList/
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
** Started on  Fri Feb 21 13:59:21 1997 Emmanuel Paris
** Last update Mon Aug  2 00:01:44 1999 Emmanuel Paris
*/

#include "ColorList.h"
#include "ColorListP.h"
#include <Xm/ArrowB.h>
#include <Xm/DrawingA.h>
#include "RegisterHelp.h"
#include "Resource.h"
#include "Colormap.h"
#include "Xpm.h"

#if defined(Xc_XQUAD)
#define AUTOMATIC_COLOR
#endif

#ifdef AUTOMATIC_COLOR
extern c_Color		*GlobColorAuto;
#endif
extern c_Colormap	*GlobColormap;
extern c_Resource	*GlobResources;

extern void XtResizeWidget ___PROTO((Widget w, Dimension width,
				     Dimension height, Dimension bdrwidth));

static void *constructor();
static void destructor();
static void *copy();
static void setColorBase();
static void setColor();
static c_Color *getColor();
static void setCallback();

static void cb_icon();
static void popup_colorlist();
static void popdown_colorlist();
static void eventlook_icon();
static void eventlook_colorlist();

static int count_color();
static c_Color *get_color();
static int get_index();

sf_ColorList fc_ColorList =
{
 constructor,
 destructor,
 copy,
 setColorBase,
 setColor,
 getColor,
 setCallback
};

/*----------------------------------------------------------------------**
**		Constructor for object ColorList ...			**
**----------------------------------------------------------------------*/

static void *constructor(w_Parent, Manage_W, Title, type)
Widget 		w_Parent;
c_ManageWidget 	*Manage_W;
char		*Title;
int type;
{
 c_ColorList	*This;
 Arg		argsw[4];
 int		width;
 int		height;

 Xc_HISTORY(("Constructor ..."));
 Xc_ASSERT(type < NB_XcCL_TYPE);

 if (!(This = (c_ColorList *) Xc_malloc("ColorList", sizeof (c_ColorList))))
  Xc_FATAL(("Can't allocate object ... exit"));
  
 This->f= &fc_ColorList;
  
 This->type = type;

 if (cl_icon[type].type == NAME_TYPE)
 {
  width = cl_icon[type].width;
  height = cl_icon[type].height;
 }
 else
 {
  int           color;
  int           bit;
  int           x_hot;
  int           y_hot;
  char		 **image;
#ifdef WITH_ZPM
  boolean	zpm = FALSE;
#endif
  
  image = cl_icon[type].icon_data;
#ifdef WITH_ZPM
  if (**image == 'z')
  {
   image = ZPMtoXPM(image);
   zpm = TRUE;
  }
#endif
  
  XPMGetAttrib(image, &width, &height, &color, &bit, &x_hot, &y_hot);
  if (cl_icon[type].x < 0) cl_icon[type].x = x_hot;
  if (cl_icon[type].y < 0) cl_icon[type].y = y_hot;
  
#ifdef WITH_ZPM
  if (zpm)
  {
   Xc_free(*image);
   Xc_free(image);
  }
#endif
 }
 
 This->rowcolumn = (c_RowColumn *)NEW(c_RowColumn)
  (w_Parent, Manage_W, Title, width + 19, height + 2, XmHORIZONTAL);
 
 This->w_This = WIDGET(This->rowcolumn);
 Xc_REGISTER_HELP(This->w_This);

 XtSetArg(argsw[0], XmNspacing, 0);
 XtSetArg(argsw[1], XmNresizeWidth, True);
 XtSetValues(WIDGET(This->rowcolumn), argsw, 2);
  
 if (cl_icon[type].type == NAME_TYPE)
 {
  XmString	xms;
   
  xms = XmStringCreate("", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);
  XtSetArg(argsw[1], XmNrecomputeSize, False);
  XtSetArg(argsw[2], XmNwidth, cl_icon[type].width - 2);
   
  if (cl_icon[type].widget_type == XcPush)
  {
   This->pushb = (c_PushButton *)NEW(c_PushButton)
    (WIDGET(This->rowcolumn), Manage_W, "clPushb");
   XtSetValues(WIDGET(This->pushb), argsw, 3);
   XtAddCallback(WIDGET(This->pushb), XmNactivateCallback,
		 (XtCallbackProc)cb_icon, This);
  }
  else
  {
   Arg argsw2[2];

   This->lframe = (c_Frame *)NEW(c_Frame)
    (WIDGET(This->rowcolumn), Manage_W, NULL);
   XtSetArg(argsw2[0], XmNshadowType, XmSHADOW_OUT);
   XtSetArg(argsw2[1], XmNshadowThickness, 1);
   XtSetValues(WIDGET(This->lframe), argsw2, 2);
     
   This->label = (c_Label *)NEW(c_Label)
    (WIDGET(This->lframe), Manage_W, "clLabel");    
   XtSetValues(WIDGET(This->label), argsw, 3);
  }
  XmStringFree(xms);
 }
 else
 {
  if (cl_icon[type].widget_type == XcLabel)
  {
   Arg argsw2[2];
      
   This->lframe = (c_Frame *)NEW(c_Frame)
    (WIDGET(This->rowcolumn), Manage_W, NULL);
   XtSetArg(argsw2[0], XmNshadowType, XmSHADOW_OUT);
   XtSetArg(argsw2[1], XmNshadowThickness, 1);
     XtSetValues(WIDGET(This->lframe), argsw2, 2);
      
   This->icon = (c_Icon *)NEW(c_Icon)(WIDGET(This->lframe), Manage_W,
				      "clIcon", XtDisplay(w_Parent),
				      cl_icon[type].icon_data,
				      NULL, cl_icon[type].widget_type);
  }
  else
   This->icon = (c_Icon *)NEW(c_Icon)(WIDGET(This->rowcolumn), Manage_W,
				      "clIcon", XtDisplay(w_Parent),
				      cl_icon[type].icon_data,
				      NULL, cl_icon[type].widget_type);
    
  if (cl_icon[type].widget_type == XcPush)
   XtAddCallback(WIDGET(This->icon), XmNactivateCallback,
		 (XtCallbackProc)cb_icon, This);
  XtAddEventHandler(WIDGET(This->icon), SubstructureNotifyMask, 
		    False, eventlook_icon, (void *)This);
 }
  
 XtSetArg(argsw[0], XmNarrowDirection, XmARROW_DOWN);
  
 This->arrow = XmCreateArrowButton(This->w_This, "clArrow", argsw, 1);  
 XtOverrideTranslations(This->arrow, 
			XtParseTranslationTable("<LeaveWindow>:  Enter()"));

 if (Manage_W)
  F(Manage_W).Add_child(Manage_W, This->arrow, WIDGET(This->rowcolumn));
 
  
 XtAddCallback(This->arrow, XmNarmCallback,
	       (XtCallbackProc)popup_colorlist, This);
  
 {
  Display *display = XtDisplay(w_Parent);
  color_cells_t	realcolor;
  XGCValues	val_gc;
    
  if (DefaultDepth(display, DefaultScreen(display)) == 1)
  {
   This->light = WhitePixel(display, DefaultScreen(display));
   This->dark = This->normal = BlackPixel(display, DefaultScreen(display));
  }
  else
  {
   F(GlobColormap).allocate_named_color(GlobColormap, CL_POPUP_LIGHT,
					&realcolor);
   This->light = realcolor.pixel;
   F(GlobColormap).allocate_named_color(GlobColormap, CL_POPUP_DARK,
					&realcolor);
   This->dark = realcolor.pixel;
   F(GlobColormap).allocate_named_color(GlobColormap, CL_POPUP_NORM,
					&realcolor);
   This->normal = realcolor.pixel;
   This->font = F(GlobResources).getXFontStruct(GlobResources,
						XcR_colorlistFont);
      
   val_gc.function = GXcopy;
   val_gc.foreground = This->light;
   val_gc.font = This->font->fid;
   val_gc.line_width = 0;
   val_gc.line_style = LineSolid; 
   val_gc.cap_style = CapButt;
   val_gc.join_style = JoinMiter;
   val_gc.fill_style = FillSolid;
   This->gc = XCreateGC(display, DefaultRootWindow(display), 
			GCFunction | GCForeground | GCFont
			| GCLineWidth | GCLineStyle | GCCapStyle 
			| GCJoinStyle | GCFillStyle, &val_gc);
  }

  if (cl_icon[type].type == MASK_TYPE && 
      cl_icon[type].mask_pixmap == (Pixmap)0)
  {
   cl_icon[type].mask_pixmap = XCreateBitmapFromData
    (display, DefaultRootWindow(display), cl_icon[type].mask_data,
     cl_icon[type].width, cl_icon[type].height);
  }
 }
  
 This->cb_func = NULL; This->cb_data = NULL;
 This->color_base = This->color_set = NULL;
 This->keepTransparent = FALSE;
 
 if (!Manage_W)
 {
  if (cl_icon[This->type].type == NAME_TYPE)
  {
   if (cl_icon[This->type].widget_type == XcPush)
    XtManageChild(WIDGET(This->pushb));
   else
    XtManageChild(WIDGET(This->label));    
  }
  else
   XtManageChild(WIDGET(This->icon));
  
  if (cl_icon[This->type].widget_type == XcLabel)
   XtManageChild(WIDGET(This->lframe));

  XtManageChild(This->arrow);
 }

 if (cl_icon[This->type].widget_type == XcLabel)
 {
  XtResizeWidget(WIDGET(This->lframe), (Dimension)(width), 
		 (Dimension)(height +2), (Dimension)0);
 }

 Xc_HISTORY(("Constructor ok"));
 return This;
}

/*----------------------------------------------------------------------**
**		Destructor for object ColorList ...			**
**----------------------------------------------------------------------*/

static void destructor(This)
c_ColorList *This;
{
 Xc_HISTORY(("Destructor ..."));
  
 if (cl_icon[This->type].mask_pixmap != (Pixmap)0)
 {
  XFreePixmap(XtDisplay(This->w_This), cl_icon[This->type].mask_pixmap);
  cl_icon[This->type].mask_pixmap = (Pixmap)0;
 }
 XFreeGC(XtDisplay(This->w_This), This->gc);
 XtDestroyWidget(This->arrow);
 if (cl_icon[This->type].type == NAME_TYPE)
 {
  if (cl_icon[This->type].widget_type == XcPush)
   DELETE(c_PushButton)(This->pushb);
  else
   DELETE(c_Label)(This->label);    
 }
 else
  DELETE(c_Icon)(This->icon);
  
 if (cl_icon[This->type].widget_type == XcLabel)
  DELETE(c_Frame)(This->lframe);
 DELETE(c_RowColumn)(This->rowcolumn);
  
 Xc_free(This);

 Xc_HISTORY(("Destructor ok"));
}

/*----------------------------------------------------------------------**
**		Copy method for object ColorList ...			**
**----------------------------------------------------------------------*/

static void *copy(ObjectToCopy)
c_ColorList *ObjectToCopy;
{
 Xc_HISTORY(("Copy ...")); 

 Xc_HISTORY(("Copy failed"));
 return NULL;
}

/*----------------------------------------------------------------------**
**		Set Color Base ...					**
**----------------------------------------------------------------------*/

static void setColorBase(This, color_base, specific)
c_ColorList *This;
c_Color *color_base;
int specific;
{
 c_Color *color;
  
 color = color_base;
 if (color)
  while(color && color->previous) color = color->previous;
  
 This->color_base = color;
 This->keepTransparent = ((specific & CL_KEEP_TRANSPARENCY) != 0);
 This->addAutomatic =  ((specific & CL_ADD_AUTOMATIC) != 0);
}

/*----------------------------------------------------------------------**
**		Set & Get Color...  					**
**----------------------------------------------------------------------*/

static void setColor(This, color)
c_ColorList *This;
c_Color *color;
{
 Display *display;
 Window window;
 
 This->color_set = color;

 if (cl_icon[This->type].type == NAME_TYPE)
 {
  Arg	argsw[4];
  XmString	xms;

  if (color)
   xms = XmStringCreate(color->name, XmSTRING_DEFAULT_CHARSET);
  else
   xms = XmStringCreate("", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);

  if (color && color->transparency == 0)
  {
   ycbcr_t	*ycbcr;
   real		y;
   
   XtSetArg(argsw[1], XmNbackground, color->cell.pixel);
   
   ycbcr = F(color).getYCbCr(color);
   y = XcC_DESCALE_COLOR(double, ycbcr->y);
   if (y > 0.5) 
    XtSetArg(argsw[2], XmNforeground, GlobColormap->Cblack);
   else
    XtSetArg(argsw[2], XmNforeground, GlobColormap->Cwhite);
  }
  else
  {
   XtSetArg(argsw[1], XmNbackground, GlobColormap->Cwhite);
   XtSetArg(argsw[2], XmNforeground, GlobColormap->Cblack);
  }
  if (cl_icon[This->type].widget_type == XcPush)
   XtSetValues(WIDGET(This->pushb), argsw, 3);
  else
   XtSetValues(WIDGET(This->label), argsw, 3);
  XmStringFree(xms);
 }
 else
 {
  display = XtDisplay(WIDGET(This->icon));
  window = XtWindow(WIDGET(This->icon));
  if (XtIsRealized(WIDGET(This->icon)) == False) return;
    
  if (cl_icon[This->type].type == RECT_TYPE)
  {
   if (color && color->transparency == 0)
   {
    XSetForeground(display, This->gc, color->cell.pixel);
   }
   else
   {
    unsigned long pixel;
       
    XtVaGetValues(WIDGET(This->icon), XmNbackground, &pixel, NULL);
    XSetForeground(display, This->gc, pixel);
   }
     
   XFillRectangle(display, This->icon->PixImg, This->gc, 
		  cl_icon[This->type].x, cl_icon[This->type].y,
		  cl_icon[This->type].width, cl_icon[This->type].height);
   if (This->icon->PixImg2)
    XFillRectangle(display, This->icon->PixImg2, This->gc, 
		   cl_icon[This->type].x, cl_icon[This->type].y,
		   cl_icon[This->type].width, cl_icon[This->type].height);
  }
  else
  {
   XGCValues gc_values;
   
   /*--- Set mask ---*/
   gc_values.function = GXand;
   gc_values.background = -1;
   gc_values.foreground = 0;
   XChangeGC(display, This->gc,
	     GCFunction | GCForeground | GCBackground, &gc_values);
     
   XCopyPlane(display, cl_icon[This->type].mask_pixmap, This->icon->PixImg,
	      This->gc, 0, 0, cl_icon[This->type].width,
	      cl_icon[This->type].height, 0, 0, (unsigned long)1);
   if (This->icon->PixImg2)
    XCopyPlane(display, cl_icon[This->type].mask_pixmap, This->icon->PixImg2,
	       This->gc, 0, 0, cl_icon[This->type].width,
	       cl_icon[This->type].height, 0, 0, (unsigned long)1);
      
   gc_values.function = GXor;
   gc_values.background = 0;
   if (color && color->transparency == 0)
    gc_values.foreground = color->cell.pixel;
   else
    XtVaGetValues(WIDGET(This->icon), XmNbackground, 
		  &(gc_values.foreground), NULL);
   XChangeGC(display, This->gc,
	     GCFunction | GCForeground | GCBackground, &gc_values);
      
   XCopyPlane(display, cl_icon[This->type].mask_pixmap, This->icon->PixImg,
	      This->gc, 0, 0, cl_icon[This->type].width,
	      cl_icon[This->type].height, 0, 0, (unsigned long)1);
   if (This->icon->PixImg2)
    XCopyPlane(display, cl_icon[This->type].mask_pixmap, This->icon->PixImg2,
	       This->gc, 0, 0, cl_icon[This->type].width,
	       cl_icon[This->type].height, 0, 0, (unsigned long)1);
      
   XSetFunction(display, This->gc, GXcopy);
  }
  XClearArea(display, XtWindow(WIDGET(This->icon)), 0, 0, 0, 0, True);
 }
}

static c_Color *getColor(This)
c_ColorList *This;
{
 return This->color_set;
}


/*----------------------------------------------------------------------**
**		SetCallback...  					**
**----------------------------------------------------------------------*/

static void setCallback(This, cb_func, cb_data)
c_ColorList *This;
void (*cb_func)();
void *cb_data;
{
 This->cb_func = cb_func;
 This->cb_data = cb_data;
}

/* ----------------------------------------------------------------- ** 
** callbacks for icon                                                ** 
** ----------------------------------------------------------------- */

static void cb_icon(w, This, cbs)
Widget		w;
c_ColorList	*This;
XmAnyCallbackStruct *cbs;
{
 if (This->cb_func && This->color_set)
  This->cb_func(This, This->cb_data, This->color_set);
}

/* ----------------------------------------------------------------- ** 
** callbacks for arrow arm, disarm & eventlook                       ** 
** ----------------------------------------------------------------- */

static void popup_colorlist(w, This, cbs)
Widget		w;
c_ColorList	*This;
XmAnyCallbackStruct *cbs;
{
 Arg		argsw[4];
 int		width, height;
 int		i;
    
 if (!This->color_base) return;
  
 This->nb_color = i = count_color(This);
 This->nb_col = (int)ceil(sqrt((double)(i * 4) / 3.0));
 This->nb_row = (int)ceil((double)This->nb_color / (double)This->nb_col);
  
 This->color_selected = NULL;
 width = This->nb_col * (CL_CELL_WIDTH + 2) + 4;
 height = This->nb_row * (CL_CELL_HEIGHT + 2) + 8 + 
  This->font->max_bounds.ascent + This->font->max_bounds.descent;
 if (This->addAutomatic)
  height += This->font->max_bounds.ascent + This->font->max_bounds.descent;
  
 XtSetArg(argsw[0], XmNwidth, width);
 XtSetArg(argsw[1], XmNheight, height);

 This->popup = XmCreatePopupMenu(w, "clPopup", argsw, 2);
  
 XtAddCallback(This->popup, XmNunmapCallback, 
	       (XtCallbackProc)popdown_colorlist, This);
  
 XtSetArg(argsw[0], XmNrecomputeSize, False);
 XtSetArg(argsw[1], XmNwidth, width);
 XtSetArg(argsw[2], XmNheight, height);
 XtSetArg(argsw[3], XmNlabelType, XmPIXMAP);
  
 This->drawing = XmCreateLabel(This->popup, "clDrawing", argsw, 4);
 XtManageChild(This->drawing);
  
 {
  Display *display = XtDisplay(WIDGET(This->rowcolumn));
  Window window = XtWindow(This->arrow), root, child;
  unsigned none, rwidth, rheight;
  int x, y;
  
  XGetGeometry(display, window, &root, &none, &none,
	       &rwidth, &rheight, &none, &none);
  XTranslateCoordinates(display, window, root, 
			rwidth - width - 2, rheight, &x, &y, &child);

  if (x < 0) x = 0;
  cbs->event->xbutton.x_root = x;
  cbs->event->xbutton.y_root = y;
  
  XmMenuPosition(This->popup, (XButtonPressedEvent *)cbs->event);

#if 0
  Dimension w, h;
   
  if (cl_icon[This->type].type == NAME_TYPE)
  {
   if (cl_icon[This->type].widget_type == XcPush)
    XtVaGetValues(WIDGET(This->pushb), XmNwidth, &w, XmNheight, &h, NULL);
   else
    XtVaGetValues(WIDGET(This->label), XmNwidth, &w, XmNheight, &h, NULL);
  }
  else
   XtVaGetValues(WIDGET(This->icon), XmNwidth, &w, XmNheight, &h, NULL);
   
  cbs->event->xbutton.x_root -= (int)w + cbs->event->xbutton.x;
  cbs->event->xbutton.y_root += (int)h - cbs->event->xbutton.y;

  XmMenuPosition(This->popup, (XButtonPressedEvent *)cbs->event);
#endif
 }
 XtAddEventHandler(This->drawing, PointerMotionMask | ExposureMask,
		   False, eventlook_colorlist, (void *)This);
 
 XtManageChild(This->popup);
}

static void popdown_colorlist(w, This, cbs)
Widget		w;
c_ColorList	*This;
XmAnyCallbackStruct *cbs;
{
 if (!This->color_base) return;
  
 XtDestroyWidget(This->drawing);
 XtDestroyWidget(This->popup);
  

 XtCallActionProc(This->arrow, "Disarm", cbs->event, NULL, 0);

 if (This->color_selected)
 {
  setColor(This, This->color_selected);
    
  if (This->cb_func)
   This->cb_func(This, This->cb_data, This->color_selected);
 }
}

static void eventlook_icon(w, This, event)
Widget	w;
c_ColorList *This;
XEvent	*event;
{
 if (event->type == MapNotify)
  setColor(This, This->color_set);
}

static void eventlook_colorlist(w, This, event)
Widget	w;
c_ColorList *This;
XEvent	*event;
{
 Display *display;
 Window window;
 int x, y, i, j, width, height;
 int fheight, twidth;
 XPoint points[3];
 c_Color	*color, *selcolor;;
  
 display = event->xany.display;
 window = event->xany.window;
 width = This->nb_col * (CL_CELL_WIDTH + 2) + 4;
 fheight = This->font->max_bounds.ascent + This->font->max_bounds.descent;
 height = This->nb_row * (CL_CELL_HEIGHT + 2) + 7;
 if (This->addAutomatic) height += fheight;
  
 switch(event->type)
 {
 case Expose:
 case GraphicsExpose:
 case NoExpose:
  x = 3; y = 3; j = 0;
  XSetForeground(display, This->gc, This->dark);
  for(i = 0; i < This->nb_color; i++)
  {
   points[0].x = points[1].x = x; points[2].x = x + CL_CELL_WIDTH;
   points[0].y = y + CL_CELL_HEIGHT; points[1].y = points[2].y = y;
   XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
      
   j++;
   if (j >= This->nb_col)
   {
    j = 0; x = 3; y += CL_CELL_HEIGHT + 2;
   }
   else
   {
    x += CL_CELL_WIDTH + 2;
   }
  }
  if (j)
   y += CL_CELL_HEIGHT + 2;
  if (This->addAutomatic)
  {
   points[0].x = points[1].x = 3; points[2].x = width - 3;
   points[0].y = y + fheight; points[1].y = points[2].y = y;
   XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
   y += fheight;
  }
    
  y += 2;
  XDrawLine(display, window, This->gc, 0, y, width, y);
    
  x = 3; y = 3; j = 0;
  XSetForeground(display, This->gc, This->light);
  for(i = 0; i < This->nb_color; i++)
  {
   points[0].x = points[1].x = x + CL_CELL_WIDTH; points[2].x = x;
   points[0].y = y; points[1].y = points[2].y = y + CL_CELL_HEIGHT;
   XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
      
   j++;
   if (j >= This->nb_col)
   {
    j = 0; x = 3; y += CL_CELL_HEIGHT + 2;
   }
   else
   {
    x += CL_CELL_WIDTH + 2;
   }
  }
  if (j)
   y += CL_CELL_HEIGHT + 2;

  if (This->addAutomatic)
  {
   points[0].x = points[1].x = width - 3; points[2].x = 3;
   points[0].y = y; points[1].y = points[2].y = y + fheight;
   XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
   y += fheight;
  }

  y += 3;
  XDrawLine(display, window, This->gc, 0, y, width, y);
    
  x = 4; y = 4; j = 0;    
  for(i = 0; i < This->nb_color; i++)
  {
   color = get_color(This, i);
   if (color->transparency == 0)
   {
    XSetForeground(display, This->gc, color->cell.pixel);
    XFillRectangle(display, window, This->gc, x, y, 
		   CL_CELL_WIDTH - 1, CL_CELL_HEIGHT - 1);
   }
      
   j++;
   if (j >= This->nb_col)
   {
    j = 0; x = 4; y += CL_CELL_HEIGHT + 2;
   }
   else
   {
    x += CL_CELL_WIDTH + 2;
   }
  }      

  if (This->addAutomatic)
  {
   XSetForeground(display, This->gc, 
		  WhitePixel(display, DefaultScreen(display)));
   XFillRectangle(display, window, This->gc, 4, height - fheight - 3,
		  width - 7, fheight - 1);
     
#ifdef AUTOMATIC_COLOR
   XSetForeground(display, This->gc, 
		  BlackPixel(display, DefaultScreen(display)));
   twidth = XTextWidth(This->font, GlobColorAuto->name,
		       strlen(GlobColorAuto->name));
   XDrawString(display, window, This->gc, (width - twidth) / 2,
	       height - fheight - 4 + This->font->max_bounds.ascent,
	       GlobColorAuto->name, strlen(GlobColorAuto->name));
#endif
  }

  if (This->color_set)
  {      
   i = get_index(This, This->color_set);

   if (i >= 0)
   {
    x = 3 + (i % This->nb_col) * (CL_CELL_WIDTH + 2);
    y = 3 + (i / This->nb_col) * (CL_CELL_HEIGHT + 2);
       
    XSetForeground(display, This->gc, This->light);
    points[0].x = points[1].x = x; points[2].x = x + CL_CELL_WIDTH;
    points[0].y = y + CL_CELL_HEIGHT; points[1].y = points[2].y = y;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
       
    XSetForeground(display, This->gc, This->dark);
    points[0].x = points[1].x = x + CL_CELL_WIDTH; points[2].x = x;
    points[0].y = y; points[1].y = points[2].y = y + CL_CELL_HEIGHT;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
   }
   else
   {
    x = 3; y = height - fheight - 4;
       
    XSetForeground(display, This->gc, This->light);
    points[0].x = points[1].x = x; points[2].x = width - 3;
    points[0].y = y + fheight; points[1].y = points[2].y = y;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
       
    XSetForeground(display, This->gc, This->dark);
    points[0].x = points[1].x = width - 3; points[2].x = x;
    points[0].y = y; points[1].y = points[2].y = y + fheight;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
   }
   XSetForeground(display, This->gc, 
		  BlackPixel(display, DefaultScreen(display)));
      
   twidth = XTextWidth(This->font, This->color_set->name,
		       strlen(This->color_set->name));
   XDrawString(display, window, This->gc, (width - twidth) / 2,
	       height + This->font->max_bounds.ascent,
	       This->color_set->name, strlen(This->color_set->name));
   This->color_selected = This->color_set; 
  }
         
  break;
 case MotionNotify:
    
  i = 0;
  selcolor = NULL;
  do
  {
   x = event->xmotion.x;
   y = event->xmotion.y;
   if (x < 3 || y < 3) break;
      
   if (This->addAutomatic && 
       y >= height - fheight - 4 && y <= height - 4)
   {
    i = -1;
    selcolor = get_color(This, -1);
    break;
   }

   x -= 3; y -= 3;
   i = x % (CL_CELL_WIDTH + 2);
   if (i > CL_CELL_WIDTH) break;      
      
   i = y % (CL_CELL_HEIGHT + 2);
   if (i > CL_CELL_HEIGHT) break;      
      
   i = x / (CL_CELL_WIDTH + 2);
   j = y / (CL_CELL_HEIGHT + 2);
   if (i >= This->nb_col || j >= This->nb_row) break;      
      
   i += j * This->nb_col;
   if (i >= This->nb_color) break;      
      
   selcolor = get_color(This, i);

  } while(0);
    
  if (selcolor == This->color_selected)
   break;
    
  if (This->color_selected)
  {
   j = get_index(This, This->color_selected);
     
   if (This->color_selected)
   {
    if (j >= 0)
    {
     x = 3 + (j % This->nb_col) * (CL_CELL_WIDTH + 2);
     y = 3 + (j / This->nb_col) * (CL_CELL_HEIGHT + 2);
       
     XSetForeground(display, This->gc, This->dark);
     points[0].x = points[1].x = x; points[2].x = x + CL_CELL_WIDTH;
     points[0].y = y + CL_CELL_HEIGHT; points[1].y = points[2].y = y;
     XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
       
     XSetForeground(display, This->gc, This->light);
     points[0].x = points[1].x = x + CL_CELL_WIDTH; points[2].x = x;
     points[0].y = y; points[1].y = points[2].y = y + CL_CELL_HEIGHT;
     XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
    }
    else
    {
     x = 3; y = height - fheight - 4;
       
     XSetForeground(display, This->gc, This->dark);
     points[0].x = points[1].x = x; points[2].x = width - 3;
     points[0].y = y + fheight; points[1].y = points[2].y = y;
     XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
       
     XSetForeground(display, This->gc, This->light);
     points[0].x = points[1].x = width - 3; points[2].x = x;
     points[0].y = y; points[1].y = points[2].y = y + fheight;
     XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
    }
   }
     
   XClearArea(display, window, 0, height, width,
	      This->font->max_bounds.ascent + 
	      This->font->max_bounds.descent + 2, False);
  }    
    
  This->color_selected = selcolor;
  if (selcolor)
  {
   if (i >= 0)
   {
    x = 3 + (i % This->nb_col) * (CL_CELL_WIDTH + 2);
    y = 3 + (i / This->nb_col) * (CL_CELL_HEIGHT + 2);
      
    XSetForeground(display, This->gc, This->light);
    points[0].x = points[1].x = x; points[2].x = x + CL_CELL_WIDTH;
    points[0].y = y + CL_CELL_HEIGHT; points[1].y = points[2].y = y;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
      
    XSetForeground(display, This->gc, This->dark);
    points[0].x = points[1].x = x + CL_CELL_WIDTH; points[2].x = x;
    points[0].y = y; points[1].y = points[2].y = y + CL_CELL_HEIGHT;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
   }
   else
   {
    x = 3; y = height - fheight - 4;
       
    XSetForeground(display, This->gc, This->light);
    points[0].x = points[1].x = x; points[2].x = width - 3;
    points[0].y = y + fheight; points[1].y = points[2].y = y;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
       
    XSetForeground(display, This->gc, This->dark);
    points[0].x = points[1].x = width - 3; points[2].x = x;
    points[0].y = y; points[1].y = points[2].y = y + fheight;
    XDrawLines(display, window, This->gc, points, 3, CoordModeOrigin);
   }
   XSetForeground(display, This->gc, 
		  BlackPixel(display, DefaultScreen(display)));
     
   twidth = XTextWidth(This->font, selcolor->name, strlen(selcolor->name));
   XDrawString(display, window, This->gc, (width - twidth) / 2,
	       height + This->font->max_bounds.ascent,
	       selcolor->name, strlen(selcolor->name));
  }
    
  break;
 }
}

/* ----------------------------------------------------------------- ** 
** color base functions                                              ** 
** ----------------------------------------------------------------- */

static int count_color(This)
c_ColorList *This;
{
 int i = 0;
 c_Color *c;
  
 c = This->color_base;
 while(c)
 {
  if (!c->hide_flag)
  {
   if (This->keepTransparent || (c->transparency == 0))
    i++;
  }
  c = c->next;
 }
 return i;
}

static c_Color *get_color(This, index)
c_ColorList *This;
int index;
{
 int i = 0;
 c_Color *c;
  
#ifdef AUTOMATIC_COLOR
 if (index == -1)
  return GlobColorAuto;
#endif


 if (This->keepTransparent && index == This->nb_color - 1)
 {
  c = This->color_base;
  while(c && (c->hide_flag || c->transparency == 0)) 
   c = c->next;
  if (c) return c;
 }

 c = This->color_base;
  
 do
 {
  while(c && c->hide_flag)
   c = c->next;
  if (!c) break;
  if (c->transparency == 0)
  {
   if (i == index) break;
   i++;
  }
  c = c->next;
 } 
 while(c);
 
 return c;
}

static int get_index(This, color)
c_ColorList *This;
c_Color *color;
{
 int i = 0;
 c_Color *c;
  
#ifdef AUTOMATIC_COLOR
 if (This->addAutomatic && color == GlobColorAuto)
  return -1;
#endif
 if (color->transparency != 0)
  return This->nb_color - 1;

 c = This->color_base;
  
 while(c)
 {
  if (!c->hide_flag)
  {
   if (c->transparency == 0)
    break;
  }
  c = c->next;
 }
  
 while(c && c != color)
 {
  if (!c->hide_flag)
  {
   if (c->transparency == 0)
    i++;
  }
  c = c->next;
 }
 return i;  
}











