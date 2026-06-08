/*
** XaRegle.c for XAllWrite in XaRegle/
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
** Started on  Tue Jul 18 12:14:09 1995 Emmanuel Paris
** Last update Sat Aug 14 19:08:09 1999 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#define _PRIVATE_XaRegle_
/*--- include ---*/
#include "XaRegle.h"
#include <X11/keysym.h>
#include "Colormap.h"
#include "Keyboard.h"
#include "ImageManager.h"
#include "ContextMenu.h"
#include "MainInterface.h"

/*--- external ---*/
extern c_Colormap *GlobColormap;
extern c_Keyboard *GlobKeyboard;
extern c_ImageManager *GlobImgMng;
extern c_MainInterface *MainInterface1;

/*--- prototype ---*/
static void *cons_XaRegle();
static void dest_XaRegle();
static void *copy_XaRegle();
void Init_XaRegle();
void Reset_XaRegle();
static void resize_XaRegle ___PROTO((c_XaRegle *This, int Size));
static boolean eventlook_XaRegle ___PROTO((c_XaRegle *This, XEvent *Event));
static void configure_XaRegle ___PROTO((c_XaRegle *This, c_Page *Page, 
					c_TextRuler *Ruler));
static void Init_XaRegle_By_Page ___PROTO((c_XaRegle *This, c_Page *Page));
static void Init_XaRegle_By_Ruler ___PROTO((c_XaRegle *This, 
					    c_TextRuler *Ruler));
static void draw_XaRegle ___PROTO((c_XaRegle *This, boolean recalcul));

static void draw_tab();
static void draw_margin_frame();
static void draw_normal_frame();
static void draw_ruler_margin();

static void get_printed_string_size();
static void print_string_XaRegle();
#if 0
static float convert_in_unit();
#endif

static void calcul_graduation();
static int draw_graduation();
static int get_function_and_change_cursor();
static void set_x_within_range ___PROTO((c_XaRegle *This, int *x, 
					 int direction));
static coord_t grid_position ___PROTO((c_XaRegle *This, int *pos, 
				       boolean type));
static void draw_h_mark ___PROTO((c_XaRegle *This, int y));
static void draw_v_mark ___PROTO((c_XaRegle *This, int x, int type));

/*--- struct method fill ---*/
sf_XaRegle fc_XaRegle =
{
 cons_XaRegle,
 dest_XaRegle,
 copy_XaRegle,
 resize_XaRegle,
 draw_XaRegle,
 eventlook_XaRegle,
 configure_XaRegle,
 Init_XaRegle_By_Page,
 Init_XaRegle_By_Ruler,
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_XaRegle(display, window, PosX, PosY, Size, Orient)
Display	*display;
Window	window;
int	PosX;
int	PosY;
int	Size;
char	Orient;
{
 int i;
 c_XaRegle 	*This;
 XGCValues	gcv;
 XSetWindowAttributes	windattrib;

 Xc_HISTORY(("Building XaRegle"));
 if ((This = (c_XaRegle *)Xc_malloc("XaRegle", sizeof(c_XaRegle))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_XaRegle));
 This->f = &fc_XaRegle;
 This->display = display;
 This->Orient = Orient;
 This->Unit = CENTIMETER;
 This->step_in_unit = 0.25;
 This->digit_step_in_unit = 2.0;
 This->Size = Size;
 This->intervs = NULL;
 This->currentTab = 0;

 This->function = XaR_NO_FUNCTION;
 This->source = This->prems = FALSE;
 This->xm = This->ym = This->tabPtr = 0;

 switch(Orient)
 {
 case XaRegle_HORIZONTAL:
  This->window = 
   XCreateSimpleWindow(display, window, 
		       PosX, PosY,
		       Size, XaRegle_RSIZE - 1,
		       0, 
		       BlackPixel(display, DefaultScreen(display)),
		       XaRegle_Color);
  
  This->under_bot_left = XCreatePixmap
   (display, This->window, Xa_Width_Bottom_Left, Xa_Height_Bottom_Left, 
    DefaultDepth(display, DefaultScreen(display)));
  This->under_bot_right = XCreatePixmap
   (display, This->window, Xa_Width_Bottom_Right, Xa_Height_Bottom_Right, 
    DefaultDepth(display, DefaultScreen(display)));
  This->under_top_left = XCreatePixmap
   (display, This->window, Xa_Width_Top_Left, Xa_Height_Top_Left, 
    DefaultDepth(display, DefaultScreen(display)));

  for(i = 0; i < XcTR_MAX_TABULATOR; i++)
   This->under_tabs[i] = XCreatePixmap
    (display, This->window, XaRegle_Font[TAB_START + XcTAB_CPERIOD].width, 
     XaRegle_Font[TAB_START + XcTAB_CPERIOD].height, 
     DefaultDepth(display, DefaultScreen(display)));
   
  break;
 case XaRegle_VERTICAL:
  This->window = 
   XCreateSimpleWindow(display, window, 
		       PosX,  PosY,
		       XaRegle_RSIZE, Size, 
		       0, 
		       BlackPixel(display,DefaultScreen(display)),
		       XaRegle_Color);
  break;
 }

 This->cursor = (c_Cursor *)NEW(c_Cursor)(display, This->window);
 F(This->cursor).set_cursor(This->cursor, C_NORMAL);

 XSelectInput(display, This->window, PointerMotionMask | ButtonPressMask |
	      ButtonReleaseMask | ExposureMask | EnterWindowMask | 
	      KeyPressMask | KeyReleaseMask);
 windattrib.bit_gravity = StaticGravity;
 XChangeWindowAttributes(display,window,CWBitGravity,&windattrib);

 gcv.function = GXcopy;
 gcv.line_width = 0;
 gcv.line_style = LineSolid;
 gcv.cap_style = CapButt;
 gcv.join_style = JoinMiter;
 gcv.background = WhitePixel(display, DefaultScreen(display));
 gcv.foreground = BlackPixel(display, DefaultScreen(display));
 gcv.graphics_exposures = False;
 This->gc = XCreateGC(display, This->window,
		      GCFunction | GCForeground | GCBackground |
		      GCLineWidth | GCLineStyle | GCCapStyle |
		      GCJoinStyle | GCGraphicsExposures,
		      &gcv);

 if (BlackPixel(display,DefaultScreen(display)))
  gcv.function = GXxor;
 else
  gcv.function = GXinvert;

 This->gc_repere = XCreateGC(display, This->window,
			     GCFunction | GCForeground | GCBackground |
			     GCLineWidth | GCLineStyle | GCCapStyle |
			     GCJoinStyle,
			     &gcv);

 gcv.function = GXcopy;
 gcv.foreground = XaRegle_Shadow;
 This->gc_Shadow = XCreateGC(display, This->window,
			     GCFunction | GCForeground | GCBackground |
			     GCLineWidth | GCLineStyle | GCCapStyle |
			     GCJoinStyle,
			     &gcv);
 
 gcv.foreground = XaRegle_Light;
 This->gc_Light = XCreateGC(display, This->window,
			    GCFunction | GCForeground | GCBackground |
			    GCLineWidth | GCLineStyle | GCCapStyle |
			    GCJoinStyle,
			    &gcv);
 
 XMapWindow(display, This->window);
 Xc_TRACE(("Object XaRegle build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_XaRegle(This)
c_XaRegle *This;
{
 if (This->intervs)
  Xc_free(This->intervs);

 XFreePixmap(This->display, This->under_bot_left);
 XFreePixmap(This->display, This->under_bot_right);
 XFreePixmap(This->display, This->under_top_left);

 XFreeGC(This->display, This->gc);
 XFreeGC(This->display, This->gc_repere);
 XFreeGC(This->display, This->gc_Shadow);
 XFreeGC(This->display, This->gc_Light);

 DELETE(c_Cursor)(This->cursor);
 XDestroyWindow(This->display, This->window);
 Xc_free(This);
 Xc_TRACE(("Object XaRegle destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy_XaRegle(This)
c_XaRegle *This;
{
 c_XaRegle *ObjTmp;

 if ((ObjTmp = (c_XaRegle *)Xc_malloc("CXaRegle", sizeof(c_XaRegle))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_XaRegle));
 Xc_TRACE(("Object XaRegle copied"));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** INIT XaRegle                                                      ** 
** ----------------------------------------------------------------- */
void Init_XaRegle(display)
Display	*display;
{
 int		i,count;
 int		x,y;
 int		xtmp,ytmp;
 color_cells_t	realcolor;

 F(GlobColormap).allocate_named_color(GlobColormap, XaRegle_COLOR, &realcolor);
 XaRegle_Color = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, XaRegle_SHADOW,
				      &realcolor);
 XaRegle_Shadow = realcolor.pixel;

 F(GlobColormap).allocate_named_color(GlobColormap, XaRegle_LIGHT,
				      &realcolor);
 XaRegle_Light = realcolor.pixel;

 F(GlobColormap).allocate_named_color(GlobColormap, XaRegle_SEPARATOR,
				      &realcolor);
 XaRegle_Separator = realcolor.pixel;
  
 for(i = 0; i < NB_CHAR; i++)
 {
  count = 0;
  for(y = 0; y < XaRegle_Font[i].height; y++)
   for(x = 0; x < XaRegle_Font[i].width; x++)
    if (XaRegle_Font[i].data[y][x] == '*')
     count++;
  XaRegle_CharH[i].nb_point = count;
  XaRegle_CharH[i].xpoint = (XPoint *)Xc_malloc("xpoint",sizeof(XPoint)*count);
  XaRegle_CharV[i].nb_point = count;
  XaRegle_CharV[i].xpoint = (XPoint *)Xc_malloc("xpoint",sizeof(XPoint)*count);
  count = xtmp = ytmp = 0;
  if (GlobImgMng->draw_points_bug)
  {
   for(y = 0; y < XaRegle_Font[i].height; y++)
    for(x = 0; x < XaRegle_Font[i].width; x++)
     if (XaRegle_Font[i].data[y][x] == '*')
     {
      XaRegle_CharH[i].xpoint[count].x = x - xtmp;
      XaRegle_CharH[i].xpoint[count].y = y - ytmp;
      if (x < xtmp)
       XaRegle_CharH[i].xpoint[count].y--;
      count++;
      xtmp = x;
      ytmp = y;
     }
  }
  else
  {
   for(y = 0; y < XaRegle_Font[i].height; y++)
    for(x = 0; x < XaRegle_Font[i].width; x++)
     if (XaRegle_Font[i].data[y][x] == '*')
     {
      XaRegle_CharH[i].xpoint[count].x = x - xtmp;
      XaRegle_CharH[i].xpoint[count++].y = y - ytmp;
      xtmp = x;
      ytmp = y;
     }
  }
  XaRegle_CharH[i].incx = XaRegle_Font[i].width + 1 - xtmp;
  XaRegle_CharH[i].incy = -ytmp;
  count = xtmp = ytmp = 0;
  for(x = 0; x < XaRegle_Font[i].height; x++)
   for(y = 0; y < XaRegle_Font[i].width; y++)
    if (XaRegle_Font[i].data[x][y] == '*')
    {
     XaRegle_CharV[i].xpoint[count].x = x - xtmp;
     XaRegle_CharV[i].xpoint[count++].y = ytmp - y;
     xtmp = x;
     ytmp = y;
    }
  XaRegle_CharV[i].incx = -xtmp;
  XaRegle_CharV[i].incy = ytmp - XaRegle_Font[i].width - 1;
 }
 
 {
#ifdef WITH_ZPM
  boolean	zpm;	
#endif
  char		**image;
  int 		color, bit;
  int		y_hot;
  Pixmap	pmask;
  XImage	*xsource;
  XImage	*xmask;
  GC		gc;
  XGCValues	xgcv;

  xgcv.foreground = 0;
  xgcv.background = 1;
  xgcv.graphics_exposures = False;

  image = (char **)bot_left_xpm;
#ifdef WITH_ZPM
  if (**image == 'z')
  {
   image = ZPMtoXPM(image);
   zpm = TRUE;
  }
  else
   zpm = FALSE;
#endif 
  
  XPMGetAttrib(image, &Xa_Width_Bottom_Left, &Xa_Height_Bottom_Left, &color,
	       &bit, &Xa_Hot_Bottom_Left, &y_hot);
  Xa_Hot_Bottom_Left--;
  
  XPMload(display, image, &xsource, &xmask, FALSE);
  Xa_Pixmap_Bottom_Left = 
   XCreatePixmap(display, DefaultRootWindow(display),
		 Xa_Width_Bottom_Left, Xa_Height_Bottom_Left,
		 DefaultDepth(display, DefaultScreen(display)));
  
  gc = XCreateGC(display, Xa_Pixmap_Bottom_Left, 0, NULL);
  XPutImage(display, Xa_Pixmap_Bottom_Left, gc, xsource, 0, 0, 
	    0, 0, Xa_Width_Bottom_Left, Xa_Height_Bottom_Left);
  Xc_free(xsource->data);
  XFree((char*)xsource);
  XFreeGC(display, gc);

  pmask = XCreatePixmap(display, 
			DefaultRootWindow(display),
			Xa_Width_Bottom_Left, Xa_Height_Bottom_Left, 1);

  gc = XCreateGC(display, pmask, GCForeground | GCBackground, &xgcv);
  XPutImage(display, pmask, gc, xmask, 0, 0,
	    0, 0, Xa_Width_Bottom_Left, Xa_Height_Bottom_Left);
  Xc_free(xmask->data);
  XFree((char *)xmask);
  XFreeGC(display, gc);

  xgcv.clip_mask = pmask;
  Xa_GC_Bottom_Left = XCreateGC(display, DefaultRootWindow(display),
				GCClipMask | GCGraphicsExposures, &xgcv);
  XFreePixmap(display, pmask);
#ifdef WITH_ZPM
  if (zpm)
  {
   Xc_free(*image);
   Xc_free(image);
  }
#endif

  image = (char **)bot_right_xpm;
#ifdef WITH_ZPM
  if (**image == 'z')
  {
   image = ZPMtoXPM(image);
   zpm = TRUE;
  }
  else
   zpm = FALSE;
#endif 
  
  XPMGetAttrib(image, &Xa_Width_Bottom_Right, &Xa_Height_Bottom_Right, &color,
	       &bit, &Xa_Hot_Bottom_Right, &y_hot);
  
  
  XPMload(display, image, &xsource, &xmask, FALSE);
  Xa_Pixmap_Bottom_Right = 
   XCreatePixmap(display, DefaultRootWindow(display),
		 Xa_Width_Bottom_Right, Xa_Height_Bottom_Right,
		 DefaultDepth(display, DefaultScreen(display)));
  
  gc = XCreateGC(display, Xa_Pixmap_Bottom_Right, 0, NULL);
  XPutImage(display, Xa_Pixmap_Bottom_Right, gc, xsource, 0, 0, 
	    0, 0, Xa_Width_Bottom_Right, Xa_Height_Bottom_Right);
  Xc_free(xsource->data);
  XFree((char*)xsource);
  XFreeGC(display, gc);

  pmask = XCreatePixmap(display, 
			DefaultRootWindow(display),
			Xa_Width_Bottom_Right, Xa_Height_Bottom_Right, 1);
  gc = XCreateGC(display, pmask, GCForeground | GCBackground, &xgcv);
  XPutImage(display, pmask, gc, xmask, 0, 0,
	    0, 0, Xa_Width_Bottom_Right, Xa_Height_Bottom_Right);
  Xc_free(xmask->data);
  XFree((char *)xmask);
  XFreeGC(display, gc);

  xgcv.clip_mask = pmask;
  Xa_GC_Bottom_Right = XCreateGC(display, DefaultRootWindow(display),
				 GCClipMask | GCGraphicsExposures, &xgcv);
  XFreePixmap(display, pmask);
#ifdef WITH_ZPM
  if (zpm)
  {
   Xc_free(*image);
   Xc_free(image);
  }
#endif

  image = (char **)top_left_xpm;
#ifdef WITH_ZPM
  if (**image == 'z')
  {
   image = ZPMtoXPM(image);
   zpm = TRUE;
  }
  else
   zpm = FALSE;
#endif 
  
  XPMGetAttrib(image, &Xa_Width_Top_Left, &Xa_Height_Top_Left, &color,
	       &bit, &Xa_Hot_Top_Left, &y_hot);
  Xa_Hot_Top_Left--;

  XPMload(display, image, &xsource, &xmask, FALSE);
  Xa_Pixmap_Top_Left = 
   XCreatePixmap(display, DefaultRootWindow(display),
		 Xa_Width_Top_Left, Xa_Height_Top_Left,
		 DefaultDepth(display, DefaultScreen(display)));
  
  gc = XCreateGC(display, Xa_Pixmap_Top_Left, 0, NULL);
  XPutImage(display, Xa_Pixmap_Top_Left, gc, xsource, 0, 0, 
	    0, 0, Xa_Width_Top_Left, Xa_Height_Top_Left);
  Xc_free(xsource->data);
  XFree((char*)xsource);
  XFreeGC(display, gc);

  pmask = XCreatePixmap(display, 
			DefaultRootWindow(display),
			Xa_Width_Top_Left, Xa_Height_Top_Left, 1);
  gc = XCreateGC(display, pmask, GCForeground | GCBackground, &xgcv);
  XPutImage(display, pmask, gc, xmask, 0, 0,
	    0, 0, Xa_Width_Top_Left, Xa_Height_Top_Left);
  Xc_free(xmask->data);
  XFree((char *)xmask);
  XFreeGC(display, gc);

  xgcv.clip_mask = pmask;
  Xa_GC_Top_Left = XCreateGC(display, DefaultRootWindow(display),
			     GCClipMask | GCGraphicsExposures, &xgcv);
  XFreePixmap(display, pmask);
#ifdef WITH_ZPM
  if (zpm)
  {
   Xc_free(*image);
   Xc_free(image);
  }
#endif
 }
 
}

/* ----------------------------------------------------------------- ** 
** reset regle                                                       ** 
** ----------------------------------------------------------------- */
void Reset_XaRegle(display)
Display *display;
{
 int	i;

 for(i = 0; i < NB_CHAR; i++)
 {
  Xc_free(XaRegle_CharH[i].xpoint);
  Xc_free(XaRegle_CharV[i].xpoint);
 }
 
 XFreePixmap(display, Xa_Pixmap_Bottom_Left);
 XFreePixmap(display, Xa_Pixmap_Bottom_Right);
 XFreePixmap(display, Xa_Pixmap_Top_Left);
 XFreeGC(display, Xa_GC_Bottom_Left);
 XFreeGC(display, Xa_GC_Bottom_Right);
 XFreeGC(display, Xa_GC_Top_Left);
}

/* ----------------------------------------------------------------- ** 
** configure_XaRegle: call init and draw                             ** 
** ----------------------------------------------------------------- */
static void configure_XaRegle(This, Page, Ruler)
c_XaRegle *This;
c_Page *Page;
c_TextRuler *Ruler;
{
 boolean recalcul = FALSE;

 if (Page != This->Page || !Ruler)
  recalcul = TRUE;
 
 if (Page)
  Init_XaRegle_By_Page(This, Page);
 
 if (This->Orient == XaRegle_HORIZONTAL)
 {
  if (Ruler) This->Ruler = NULL;
  Init_XaRegle_By_Ruler(This, Ruler);
 }

 draw_XaRegle(This, recalcul);
}

/* ----------------------------------------------------------------- ** 
** INIT XaRegle BY PAGE                                              ** 
** ----------------------------------------------------------------- */
static void Init_XaRegle_By_Page(This, Page)
c_XaRegle	*This;
c_Page		*Page;
{
 int	i;
 int	last_pos;

 Xc_TRACE(("init by page"));
 Xc_TRACE(("Orient: %d", This->Orient));

 if (Page)
  This->Page = Page;
 else
  Page = This->Page;

 switch(This->Orient)
 {
 case XaRegle_HORIZONTAL:
  calcul_graduation(This, Page->sScale);
  This->nb_intervs = 0;
  if (This->intervs)
   Xc_free(This->intervs);

  /*--- compute interval numbers ---*/
  if (Page->left_margin) 
   This->nb_intervs++;
  This->nb_intervs++;
  if (Page->right_margin)
   This->nb_intervs++;

  /*--- allocate intervals ---*/
  This->intervs = (Xa_interv_t *)Xc_malloc("interv",
					   This->nb_intervs*
					   sizeof(Xa_interv_t));

  This->Start = Page->PosX + REGLE_SIZE;
  This->End = Page->PosX  + REGLE_SIZE + Page->Width;

  /*--- fill intervals ---*/
  i = 0;
  last_pos = 0;
  if (Page->left_margin) 
  {
   This->intervs[i].min = last_pos;
   last_pos += Page->left_margin / This->Scale;
   This->intervs[i].max = last_pos;
   This->intervs[i].type = MARGIN_INTERV;
   This->MarginStart = This->Start + last_pos;
   i++;
  }
  else
   This->MarginStart = This->Start;
  
  This->intervs[i].min = last_pos + 1;
  last_pos += Page->Width - 
   (Page->left_margin + Page->right_margin) / This->Scale;
  This->intervs[i].max = last_pos;
  This->intervs[i].type = NORMAL_INTERV;
  i++;

  if (Page->right_margin) 
  {
   This->MarginEnd = This->Start + last_pos + 1;
   This->intervs[i].min = last_pos + 1;
   last_pos += Page->right_margin / This->Scale;
   This->intervs[i].max = last_pos;
   This->intervs[i].type = MARGIN_INTERV;
   i++;
  }
  else
   This->MarginEnd = This->End;
  
  break;
 case XaRegle_VERTICAL:
  calcul_graduation(This, Page->sScale);
  This->nb_intervs = 0;
  if (This->intervs)
   Xc_free(This->intervs);

  /*--- compute interval numbers ---*/
  if (Page->top_margin) 
   This->nb_intervs++;
  This->nb_intervs++;
  if (Page->bottom_margin)
   This->nb_intervs++;

  /*--- allocate intervals ---*/
  This->intervs = (Xa_interv_t *)Xc_malloc
   ("interv", This->nb_intervs * sizeof(Xa_interv_t));
  
  This->Start = Page->PosY + 1;
  This->End = Page->PosY + Page->Height;

  /*--- fill intervals ---*/
  i = 0;
  last_pos = 0;
  if (Page->top_margin) 
  {
   This->intervs[i].min = last_pos;
   last_pos += Page->top_margin / This->Scale;
   This->intervs[i].max = last_pos;
   This->intervs[i].type = MARGIN_INTERV;
   This->MarginStart = This->Start + last_pos;
   i++;
  }
  else
   This->MarginStart = This->Start;
  
  This->intervs[i].min = last_pos + 1;
  last_pos += Page->Height - 
   (Page->top_margin + Page->bottom_margin) / This->Scale;
  This->intervs[i].max = last_pos;
  This->intervs[i].type = NORMAL_INTERV;
  i++;

  if (Page->bottom_margin) 
  {
   This->MarginEnd = This->Start + last_pos + 1;
   This->intervs[i].min = last_pos + 1;
   last_pos += Page->bottom_margin / This->Scale;
   This->intervs[i].max = last_pos;
   This->intervs[i].type = MARGIN_INTERV;
   i++;
  }
  else
   This->MarginEnd = This->End;

  break;    
 }
}

/* ----------------------------------------------------------------- ** 
** INIT XaRegle BY RULER                                             ** 
** ----------------------------------------------------------------- */
static void Init_XaRegle_By_Ruler(This, Ruler)
c_XaRegle	*This;
c_TextRuler	*Ruler;
{
 if (This->Orient != XaRegle_HORIZONTAL) return;
 
 if (This->Ruler == Ruler) return;

 if (Ruler)
  This->Ruler = Ruler;
 else
  if (!This->Ruler) return;
 
 This->LeftMargin = This->MarginStart + 
  This->Ruler->left_margin / This->Page->sScale;
 This->RightMargin = This->MarginEnd - 
  This->Ruler->right_margin / This->Page->sScale;
 This->IndentMargin = This->LeftMargin + 
  This->Ruler->paragraph_indent / This->Page->sScale;

 draw_tab(This, TRUE);
 draw_ruler_margin(This, TRUE);
}

/* ----------------------------------------------------------------- ** 
** RESIZE XaRegle                                                    ** 
** ----------------------------------------------------------------- */
static void resize_XaRegle(This, Size) 
c_XaRegle	*This;
int	Size;
{
 if (This->Orient == XaRegle_HORIZONTAL)
 {
  XResizeWindow(This->display, This->window, Size, XaRegle_RSIZE);
 }
 else
 {
  XResizeWindow(This->display, This->window, XaRegle_RSIZE, Size);
 } 
 This->Size = Size;
}

/* ----------------------------------------------------------------- ** 
** EVENTLOOK XaRegle                                                 ** 
** ----------------------------------------------------------------- */
static boolean eventlook_XaRegle(This, Event)
c_XaRegle	*This;
XEvent	*Event;
{
 XRectangle	rect;
 int		xm,ym;
 c_Page		*page;
 boolean	ret_code = FALSE;

 if (Event->xany.window == This->window)
 {
  ret_code = TRUE;
  
  switch(Event->type)
  {
  case Expose:
  case GraphicsExpose:
   rect.x = Event->xexpose.x;
   rect.y = Event->xexpose.y;
   rect.width = Event->xexpose.width;
   rect.height = Event->xexpose.height;
   /* we use some others gcs. So we must clip all gc or none..*/
   /*XSetClipRectangles(This->display, This->gc, 0, 0, &rect, 1, Unsorted);*/
   Xc_TRACE(("event on:%p %d", This, Event->type));
   F(This).Draw(This, FALSE);
   /*XSetClipMask(This->display, This->gc, None);*/
   break;
  case EnterNotify:
   {
    Window noneW;
    int    noneI;
    unsigned int mod_button;
    
    XQueryPointer(This->display, This->window, &noneW, &noneW, &noneI, &noneI,
		  &xm, &ym, &mod_button);
    This->shift = ((mod_button & ShiftMask)!=0);
   }
   xm = Event->xcrossing.x;
   ym = Event->xcrossing.y;
   if (!This->source)
    This->function = get_function_and_change_cursor(This, xm, ym, TRUE);
   break;
  case KeyPress:
   {
    int		nb_car;
    char	tamp_cla[20];
    int		tailtamp = 20;
    KeySym	symtouche;
    XComposeStatus	seqtouche;

    nb_car = XLookupString((XKeyEvent *)Event, tamp_cla, tailtamp,
			   &symtouche, &seqtouche);
    if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
     This->shift = TRUE;
   }
   break;
  case KeyRelease:
   {
    int		nb_car;
    char	tamp_cla[20];
    int		tailtamp = 20;
    KeySym	symtouche;
    XComposeStatus	seqtouche;

    nb_car = XLookupString((XKeyEvent *)Event, tamp_cla, tailtamp,
			   &symtouche, &seqtouche);
    if (symtouche == XK_Shift_R || symtouche == XK_Shift_L)
     This->shift = FALSE;
   }
   break;
   /* ----------------------------------------------------------------- ** 
   ** MotionNotify                                                      ** 
   ** ----------------------------------------------------------------- */
  case MotionNotify:
   xm = Event->xmotion.x;
   ym = Event->xmotion.y;
   page = This->Page;

   if (This->source)
   {
    switch(This->function)
    {
    case XaR_MOVE_PAGE_TOP_MARGIN:
     if (!This->shift) grid_position(This, &ym, FALSE);
     if ((ym - page->PosY) < 0)
      ym = page->PosY;
     
     if ((ym - page->PosY) * page->sScale >
	 (page->sHeight - page->bottom_margin - SCALE_FROM_CENTIMETERS(1.5)))
      ym = (page->sHeight - page->bottom_margin - SCALE_FROM_CENTIMETERS(1.5))/
       page->sScale + page->PosY;
     
     draw_h_mark(This, ym);
     break;
    case XaR_MOVE_PAGE_BOTTOM_MARGIN: 
     if (!This->shift) grid_position(This, &ym, FALSE);
     if (ym - page->PosY - XaRegle_RSIZE > page->Height)
      ym = page->PosY + XaRegle_RSIZE + page->Height;
     
     if ((ym - page->PosY) * page->sScale <
	 (page->top_margin + SCALE_FROM_CENTIMETERS(1.5)))
      ym = (page->top_margin + SCALE_FROM_CENTIMETERS(1.5)) / 
       page->sScale + page->PosY;
       
     draw_h_mark(This, ym);
     break;
    case XaR_MOVE_PAGE_LEFT_MARGIN:
     if (!This->shift) grid_position(This, &xm, FALSE);
     set_x_within_range(This, &xm, XaR_PAGE_MARGIN);     
     draw_v_mark(This, xm, XaR_PAGE_MARGIN);
     break;
    case XaR_MOVE_PAGE_RIGHT_MARGIN:
     if (!This->shift) grid_position(This, &xm, FALSE);
     set_x_within_range(This, &xm, XaR_RIGHT_MARGIN);
     draw_v_mark(This, xm, XaR_PAGE_MARGIN);
     break;
    case XaR_MOVE_RULER_LEFT_MARGIN:
     if (!This->shift) grid_position(This, &xm, TRUE);
     set_x_within_range(This, &xm, XaR_LEFT_MARGIN);
     draw_v_mark(This, xm, XaR_LEFT_MARGIN);
     break;
    case XaR_MOVE_RULER_LEFT2_MARGIN:
     if (!This->shift) grid_position(This, &xm, TRUE);
     set_x_within_range(This, &xm, XaR_LEFT2_MARGIN);
     draw_v_mark(This, xm, XaR_LEFT2_MARGIN);
     break;
    case XaR_MOVE_RULER_RIGHT_MARGIN:
     if (!This->shift) grid_position(This, &xm, TRUE);
     set_x_within_range(This, &xm, XaR_RIGHT_MARGIN);
     draw_v_mark(This, xm, XaR_RIGHT_MARGIN);
     break;
    case XaR_MOVE_RULER_INDENT_MARGIN:
     if (!This->shift) grid_position(This, &xm, TRUE);
     set_x_within_range(This, &xm, XaR_LEFT2_MARGIN);
     draw_v_mark(This, xm, XaR_INDENT_MARGIN);
     break;
    case XaR_MOVE_RULER_TAB:
    case XaR_DROP_RULER_TAB:
     if (!This->shift) grid_position(This, &xm, TRUE);
     draw_v_mark(This, xm, XaR_TAB_MARK);
     break;
    }
   }
   else
    This->function = get_function_and_change_cursor(This, xm, ym, TRUE);
   break;
   /* ----------------------------------------------------------------- ** 
   ** Button Release                                                    ** 
   ** ----------------------------------------------------------------- */
  case ButtonRelease:
   xm = Event->xbutton.x;
   ym = Event->xbutton.y;
   page = This->Page;
   
   if (!This->source || Event->xbutton.button != 1) break;

   switch(This->function)
   {
   case XaR_MOVE_PAGE_TOP_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_h_mark(This, This->ym);
    F(This->Page).SetPageMargin(This->Page, (This->ym - This->Start) * 
				page->sScale, XP_MARGIN_TOP);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_PAGE_BOTTOM_MARGIN: 
    This->source = FALSE;
    This->prems = TRUE;
    draw_h_mark(This, This->ym);
    F(This->Page).SetPageMargin(This->Page, page->sHeight - 
				(This->ym - This->Start) * page->sScale,
				XP_MARGIN_BOTTOM);
    This->MarginEnd = This->ym;
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_PAGE_LEFT_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_v_mark(This, This->xm, XaR_PAGE_MARGIN);
    F(This->Page).SetPageMargin(This->Page, (This->xm - This->Start) * 
				page->sScale, XP_MARGIN_LEFT);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_PAGE_RIGHT_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_v_mark(This, This->xm, XaR_PAGE_MARGIN);
    F(This->Page).SetPageMargin(This->Page, page->sWidth - 
				(This->xm - This->Start) *
				page->sScale, XP_MARGIN_RIGHT);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_RULER_LEFT_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_v_mark(This, This->xm, XaR_LEFT_MARGIN);
    F(This->Page).SetRulerMargin(This->Page, (This->xm - This->Start) * 
				 page->sScale - page->left_margin,
				 XP_MARGIN_LEFT);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_RULER_LEFT2_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_v_mark(This, This->xm, XaR_LEFT2_MARGIN);
    F(This->Page).SetRulerMargin(This->Page, (This->xm - This->Start) *
				 page->sScale - page->left_margin,
				 XP_MARGIN_LEFT2);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_RULER_RIGHT_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_v_mark(This, This->xm, XaR_RIGHT_MARGIN);
    F(This->Page).SetRulerMargin(This->Page, page->sWidth - 
				 (This->xm - This->Start) *
				 page->sScale - page->right_margin,
				 XP_MARGIN_RIGHT);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_RULER_INDENT_MARGIN:
    This->source = FALSE;
    This->prems = TRUE;
    draw_v_mark(This, This->xm, XaR_INDENT_MARGIN);
    F(This->Page).SetRulerMargin(This->Page, (This->xm - This->LeftMargin) *
				 page->sScale, XP_MARGIN_INDENT);
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_MOVE_RULER_TAB:
    This->source = FALSE;
    This->prems = TRUE;
    if (xm < This->Start || xm < XaRegle_RSIZE || 
	xm > This->End || xm > This->Size)
    {
     F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO,
			       (tab_just_t)0, ' ', XP_DEL_TAB);
     This->function = get_function_and_change_cursor(This, xm, ym, TRUE);
    }
    else
    {
     draw_v_mark(This, This->xm, XaR_TAB_MARK);
     F(This->Page).SetRulerTab(This->Page, This->tabPtr, 
			       (This->xm - This->MarginStart) * page->sScale, 
			       (tab_just_t)0, ' ', XP_MOVE_TAB);
    }
    F(This).Configure(This, This->Page, NULL);
    break;
   case XaR_DROP_RULER_TAB:
    This->source = FALSE;
    This->prems = TRUE;
    if (xm < This->Start || xm < XaRegle_RSIZE || 
	xm > This->End || xm > This->Size)
    {
     This->nbTab--;
     break;
    }
    draw_v_mark(This, This->xm, XaR_TAB_MARK);
    F(This->Page).SetRulerTab(This->Page, 0, 
			      (This->xm - This->MarginStart) * page->sScale, 
			      This->currentTab, ' ', XP_ADD_TAB);
    F(This).Configure(This, This->Page, NULL);
   }
   
   break;
   /* ----------------------------------------------------------------- ** 
   ** ButtonPress                                                       ** 
   ** ----------------------------------------------------------------- */
  case ButtonPress:
   xm = Event->xbutton.x;
   ym = Event->xbutton.y;
   
   if (This->source)
   {
    if (Event->xbutton.button == 3)
    {
     This->source = FALSE; This->prems = TRUE;
     switch(This->function)
     {
     case XaR_MOVE_PAGE_TOP_MARGIN:
     case XaR_MOVE_PAGE_BOTTOM_MARGIN: 
      This->source = This->prems = TRUE;
      draw_h_mark(This, This->ym);
      break;
     case XaR_MOVE_PAGE_LEFT_MARGIN:
     case XaR_MOVE_PAGE_RIGHT_MARGIN:
      draw_v_mark(This, This->xm, XaR_PAGE_MARGIN);
      break;
     case XaR_MOVE_RULER_LEFT_MARGIN:
      draw_v_mark(This, This->xm, XaR_LEFT_MARGIN);
      break;
     case XaR_MOVE_RULER_LEFT2_MARGIN:
      draw_v_mark(This, This->xm, XaR_LEFT2_MARGIN);
      break;
     case XaR_MOVE_RULER_RIGHT_MARGIN:
      draw_v_mark(This, This->xm, XaR_RIGHT_MARGIN);
      break;
     case XaR_MOVE_RULER_INDENT_MARGIN:
      draw_v_mark(This, This->xm, XaR_INDENT_MARGIN);
      break;
     case XaR_MOVE_RULER_TAB:
     case XaR_DROP_RULER_TAB:
      draw_v_mark(This, This->xm, XaR_TAB_MARK);
      break;
     }
     F(This).Draw(This, FALSE);
    }
    break;
   }
   
   if (Event->xbutton.button != 1 && This->function != XaR_CHANGE_TAB_TYPE)
   {
    if (Event->xbutton.button == 3 && This->function == XaR_MOVE_RULER_TAB)
    {
     c_ContextMenu *context;
     int           ret_button;
     
     context = (c_ContextMenu *)
      NEW(c_ContextMenu)(This->display, CM_XAREGLE, &ret_button, 
			 FALSE, Event);
     if (context)
     {
      F(context).waitForUnmap(context);
      DELETE(c_ContextMenu)(context);
      
      switch(ret_button)
      {
      case CM_Xar_TAB_LEFT:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 XcTAB_LEFT, ' ',  XP_CHANGE_TAB_TYPE);
       F(This).Configure(This, This->Page, NULL);
       break;
      case CM_Xar_TAB_RIGHT:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 XcTAB_RIGHT, ' ',  XP_CHANGE_TAB_TYPE);
       F(This).Configure(This, This->Page, NULL);
       break;
      case CM_Xar_TAB_CENTER:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 XcTAB_CENTER, ' ',  XP_CHANGE_TAB_TYPE);
       F(This).Configure(This, This->Page, NULL);
       break;
      case CM_Xar_TAB_CPERIOD:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 XcTAB_CPERIOD, ' ',  XP_CHANGE_TAB_TYPE);
       F(This).Configure(This, This->Page, NULL);
       break;
      case CM_Xar_FILLER_NONE:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 (tab_just_t)0, ' ',  XP_CHANGE_TAB_FILLER);
       F(This).Configure(This, This->Page, NULL);
       break;
      case CM_Xar_FILLER_POINT:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 (tab_just_t)0, '.',  XP_CHANGE_TAB_FILLER);
       break;
      case CM_Xar_FILLER_DOT:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 (tab_just_t)0, (char)183,  
				 XP_CHANGE_TAB_FILLER);
       break;
      case CM_Xar_FILLER_HYPHEN:
       F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				 (tab_just_t)0, (char)173,
				 XP_CHANGE_TAB_FILLER);
       break;
      case CM_Xar_FILLER_USERDEF:
       {
	char filler;
	filler = F(GlobKeyboard).GetOneCharBoxInsert
	 (GlobKeyboard, MainInterface1->w_Main, 
	  This->Page->BaseStd->text_base->style);
	if (filler != '\0')
	 F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				   (tab_just_t)0, filler,
				   XP_CHANGE_TAB_FILLER);
       }
       break;
      case CM_Xar_TAB_DELETE:
        F(This->Page).SetRulerTab(This->Page, This->tabPtr, COORD_ZERO, 
				  (tab_just_t)0, ' ', XP_DEL_TAB);      
       break;
      default:
       break;
      }
     }
    }
    break;
   }
   
   switch(This->function)
   {
   case XaR_MOVE_PAGE_TOP_MARGIN:
   case XaR_MOVE_PAGE_BOTTOM_MARGIN: 
    This->source = This->prems = TRUE;
    draw_h_mark(This, ym);
    break;
   case XaR_MOVE_PAGE_LEFT_MARGIN:
   case XaR_MOVE_PAGE_RIGHT_MARGIN:
    This->source = This->prems = TRUE;
    draw_v_mark(This, xm, XaR_PAGE_MARGIN);
    break;
   case XaR_CHANGE_TAB_TYPE:	
    if (Event->xbutton.button == 1)
    {
     This->currentTab++;
     if (This->currentTab >= NB_TAB)
      This->currentTab = 0;
    }
    else if (Event->xbutton.button == 3)
    {
     if (This->currentTab == 0)
      This->currentTab = NB_TAB-1;
     else
      This->currentTab--;
    }
    draw_tab(This, FALSE);
    draw_ruler_margin(This, FALSE);
    break;
   case XaR_MOVE_RULER_LEFT_MARGIN:
    This->source = This->prems = TRUE;
    draw_v_mark(This, xm, XaR_LEFT_MARGIN);
    break;
   case XaR_MOVE_RULER_LEFT2_MARGIN:
    This->source = This->prems = TRUE;
    draw_v_mark(This, xm, XaR_LEFT2_MARGIN);
    break;
   case XaR_MOVE_RULER_RIGHT_MARGIN:
    This->source = This->prems = TRUE;
    draw_v_mark(This, xm, XaR_RIGHT_MARGIN);
    break;
   case XaR_MOVE_RULER_INDENT_MARGIN:
    This->source = This->prems = TRUE;
    draw_v_mark(This, xm, XaR_INDENT_MARGIN);
    break;
   case XaR_MOVE_RULER_TAB:
    This->source = This->prems = TRUE;
    This->xm = This->tabPos[This->tabPtr];
    draw_v_mark(This, xm, XaR_TAB_MARK);
    break;
   case XaR_DROP_RULER_TAB:
    if (This->nbTab >= XcTR_MAX_TABULATOR)
     break;

    This->source = TRUE; This->prems = FALSE;
    This->xm = -1;
    This->tabPos[This->nbTab] = xm;
    This->tabType[This->nbTab] = This->currentTab;
    This->tabPtr = This->nbTab++;
    switch(This->currentTab)
    {
    case XcTAB_LEFT:
     F(This->cursor).set_cursor(This->cursor, C_REGLE_TABL);
     break;
    case XcTAB_RIGHT:
     F(This->cursor).set_cursor(This->cursor, C_REGLE_TABR);
     break;
    case XcTAB_CENTER:
     F(This->cursor).set_cursor(This->cursor, C_REGLE_TABC);
     break;
    case XcTAB_CPERIOD:
     F(This->cursor).set_cursor(This->cursor, C_REGLE_TABCP);
     break;
    }
    draw_v_mark(This, xm, XaR_TAB_MARK);
    break;
   }
   break;
  }
 }
 return ret_code;
}

/* ----------------------------------------------------------------- ** 
** DRAW XaRegle                                                      ** 
** ----------------------------------------------------------------- */
static void draw_XaRegle(This, recalcul)
c_XaRegle	*This;
boolean		recalcul;
{
 int	i;
 int	num;

 Xc_HISTORY(("drawing XaRegle"));
 if (recalcul)
  calcul_graduation(This, This->Page->sScale);
 Xc_TRACE(("Orient: %d this step:%f", This->Orient, This->step_in_pixel));
 if (This->step_in_pixel <= 0)
  return;

 /*--- draw borders ---*/
 if (This->Orient == XaRegle_HORIZONTAL)
 {
  XClearArea(This->display, This->window, XaRegle_RSIZE, 0, 
	     This->Size - XaRegle_RSIZE, XaRegle_RSIZE, False);

  /*--- draw tab selector ---*/
  XDrawLine(This->display, This->window, This->gc_Light,
	    0, 0,
	    This->Size, 0);
  XDrawLine(This->display, This->window, This->gc_Light,
	    0, 1,
	    0, XaRegle_RSIZE);
  XDrawLine(This->display, This->window, This->gc_Shadow,
	    XaRegle_RSIZE-2, XaRegle_RSIZE-2,
	    This->Size, XaRegle_RSIZE-2);
  XDrawLine(This->display, This->window, This->gc,
	    XaRegle_RSIZE-1, XaRegle_RSIZE-1,
	    This->Size, XaRegle_RSIZE-1);
 }
 else
 {
  XClearWindow(This->display, This->window);

  XDrawLine(This->display, This->window, This->gc_Light,
	    0, 0,
	    0, This->Size);
  XDrawLine(This->display, This->window, This->gc_Shadow,
	    XaRegle_RSIZE-2, 0,
	    XaRegle_RSIZE-2, This->Size);
  XDrawLine(This->display, This->window, This->gc,
	    XaRegle_RSIZE-1, 0,
	    XaRegle_RSIZE-1, This->Size);
 }

 /*--- draw frame ---*/
 /* (for each pages?) think only for one ...*/
 /* for each intervall in pages */
 i = 0;
 num = 0;
 while (i < This->nb_intervs)
 {
  switch (This->intervs[i].type)
  {
  case MARGIN_INTERV:
   draw_margin_frame(This, &(This->intervs[i]));
   draw_graduation(This, This->intervs[i].min + This->Start,
		   This->intervs[i].max + This->Start, num);
   break;
  case NORMAL_INTERV:
   draw_normal_frame(This, &(This->intervs[i]));
   num += draw_graduation(This, This->intervs[i].min + This->Start,
			  This->intervs[i].max + This->Start, num);
   break;
  }
  i++;
 }
 if (This->Orient == XaRegle_HORIZONTAL)
 {
  draw_tab(This, FALSE);
  draw_ruler_margin(This, FALSE);
 }
}

/* ----------------------------------------------------------------- ** 
** draw tabulation selector                                          ** 
** ----------------------------------------------------------------- */
static void draw_tab(This, clear)
c_XaRegle	*This;
boolean		clear;
{
 c_Page	     *page;
 c_TextRuler *ruler;
 coord_t pos;
 tab_just_t  type;
 char   filler;
 int	i, x, nseg, nb_point;
 int	c, xorig, yorig;
 XSegment seg[5];
 
 XClearArea(This->display, This->window, 1, 1, 
	    XaRegle_RSIZE - 1, XaRegle_RSIZE - 3, FALSE);

 XDrawRectangle(This->display, This->window, This->gc_Shadow,
		4, 3, 
		XaRegle_RSIZE - 11, XaRegle_RSIZE - 11);

 seg[0].x1 = 5; seg[0].x2 = XaRegle_RSIZE - 8;
 seg[1].x1 = seg[1].x2 = 5;
 seg[2].x1 = 4; seg[2].x2 = XaRegle_RSIZE - 7;
 seg[3].x1 = seg[3].x2 = XaRegle_RSIZE - 6;
 seg[4].x1 = seg[4].x2 = 0;

 seg[0].y1 = seg[0].y2 = 4;
 seg[1].y1 = 5; seg[1].y2 = XaRegle_RSIZE - 9;
 seg[2].y1 = seg[2].y2 = XaRegle_RSIZE - 7;
 seg[3].y1 = 3; seg[3].y2 = XaRegle_RSIZE - 7;
 seg[4].y1 = 3; seg[4].y2 = XaRegle_RSIZE - 7;
 XDrawSegments(This->display, This->window, This->gc_Light, seg, 5);

 memcpy(points, XaRegle_CharH[This->currentTab + TAB_START].xpoint,
	XaRegle_CharH[This->currentTab + TAB_START].nb_point * sizeof(XPoint));

 points[0].x += 8 + ((12 - XaRegle_Font[This->currentTab+TAB_START].width)/2);
 points[0].y += 10;

 XDrawPoints(This->display, This->window, This->gc, points, 
	     XaRegle_CharH[This->currentTab + TAB_START].nb_point,
	     CoordModePrevious);

 if (clear)
 {
  for(i = 0; i < This->nbTab; i++)
  {
   c = This->tabType[i] + TAB_START;
   XCopyArea(This->display, This->under_tabs[i], This->window, This->gc, 
	     0, 0, XaRegle_Font[c].width, XaRegle_Font[c].height,
	     This->tabPos[i] - XaRegle_Font[c].offset_x, XaRegle_RSIZE - 17);
  }
 }

 page = This->Page;
 ruler = This->Ruler;
 
 if (!page || !ruler) return;

 nseg = This->nbTab = 0;

 pos = F(ruler).initGetTab(ruler);
 x = pos / page->sScale + This->MarginStart;
 if (x > This->Size) return;
 
 if (x < XaRegle_RSIZE)
  pos = - (This->MarginStart - XaRegle_SIZE) * page->sScale;
 else
  pos -= COORD_ONE;
 
 while(1)
 {
  if (F(ruler).getNextTab(ruler, &pos, &type, &filler))
  {				/* user defined tab */
   x = pos / page->sScale + This->MarginStart;
   if (x > This->Size) break;
   This->tabPos[This->nbTab] = x;
   This->tabType[This->nbTab] = type;
   This->nbTab++;
  }
  else
  {				/* default tab */
   x = pos / page->sScale + This->MarginStart;
   if (x >= This->MarginEnd) break;
   /*if (x <= This->LeftMargin) continue;*/
   segments[nseg].x1 = segments[nseg].x2 = x;
   segments[nseg].y1 = XaRegle_RSIZE - 6; 
   segments[nseg].y2 = XaRegle_RSIZE - 3;
   nseg++;
  }
 }

 XClearArea(This->display, This->window, This->MarginStart, XaRegle_RSIZE - 6,
	    This->MarginEnd - This->MarginStart, 4, False);

 if (nseg)
  XDrawSegments(This->display, This->window, This->gc_Shadow, segments, nseg);

 for(i = 0; i < This->nbTab; i++)
 {
  c = This->tabType[i] + TAB_START;
  XCopyArea(This->display, This->window, This->under_tabs[i], This->gc, 
	    This->tabPos[i] - XaRegle_Font[c].offset_x, XaRegle_RSIZE - 17,
	    XaRegle_Font[c].width, XaRegle_Font[c].height, 0, 0);
 }

 nb_point = xorig = yorig = 0;
 for(i = 0; i < This->nbTab; i++)
 {
  c = This->tabType[i] + TAB_START;
  memcpy(points + nb_point, XaRegle_CharH[c].xpoint,
	 XaRegle_CharH[c].nb_point * sizeof(XPoint));

  xorig = This->tabPos[i] - XaRegle_Font[c].offset_x - xorig;
  yorig = XaRegle_RSIZE - 17 - yorig;
  points[nb_point].x += xorig;
  points[nb_point].y += yorig;

  xorig = This->tabPos[i] - XaRegle_Font[c].offset_x + 
   XaRegle_Font[c].width - 1;
  yorig = XaRegle_RSIZE - 17 + XaRegle_Font[c].height - 1;

  nb_point += XaRegle_CharH[c].nb_point;
 }
 
 XDrawPoints(This->display, This->window, This->gc, points, nb_point,
	     CoordModePrevious);
}

/* ----------------------------------------------------------------- ** 
** draw margin frame                                                 ** 
** ----------------------------------------------------------------- */
static void draw_margin_frame(This, interv)
c_XaRegle	*This;
Xa_interv_t	*interv;
{
 XSegment seg[4];
 int	nbseg = 0;
 int	p1, p2;

 if (This->Orient == XaRegle_HORIZONTAL)
 {
  p1 = This->Start + interv->min;
  p2 = This->Start + interv->max;

  if (p1 > This->Size || p2 < XaRegle_RSIZE)
   return;

  XDrawRectangle(This->display, This->window, This->gc_Shadow,
		 p1, 3, interv->max - interv->min, XaRegle_RSIZE - (8 + 3));
  
  if (p1 + 1 >= XaRegle_RSIZE && p1 + 1 < This->Size)
  {
   seg[nbseg].x1 = seg[nbseg].x2 = p1 + 1;
   seg[nbseg].y1 = 4; seg[nbseg].y2 = XaRegle_RSIZE - 9;
   nbseg++;
  }
  
  seg[nbseg].x1 = p1 + 1; seg[nbseg].x2 = p2 - 1;
  seg[nbseg].y1 = seg[nbseg].y2 = 4;
  nbseg++;

  seg[nbseg].x1 = p1; seg[nbseg].x2 = p2;
  seg[nbseg].y1 = seg[nbseg].y2 = XaRegle_RSIZE - 7;
  nbseg++;

  if (p2 + 1 >= XaRegle_RSIZE && p2 + 1 < This->Size)
  {
   seg[nbseg].x1 = seg[nbseg].x2 = p2 + 1;
   seg[nbseg].y1 = 3; seg[nbseg].y2 = XaRegle_RSIZE - 7;
   nbseg++;
  }

  XDrawSegments(This->display, This->window, This->gc_Light, seg, nbseg);
 }
 else
 {
  p1 = This->Start + interv->min;
  p2 = This->Start + interv->max;
  
  if (p1 > This->Size || p2 < 0)
   return;
 
  XDrawRectangle(This->display, This->window, This->gc_Shadow,
		 4, p1, XaRegle_RSIZE - (6 + 5), interv->max - interv->min);

  if (p1 + 1 >= 0 && p1 + 1 < This->Size)
  {
   seg[nbseg].x1 = 5; seg[nbseg].x2 = XaRegle_RSIZE - 8;
   seg[nbseg].y1 = seg[nbseg].y2 = p1 + 1;
   nbseg++;
  }
  
  seg[nbseg].x1 = seg[nbseg].x2 = 5;
  seg[nbseg].y1 = p1 + 1; seg[nbseg].y2 = p2 - 1;
  nbseg++;

  seg[nbseg].x1 = seg[nbseg].x2 = XaRegle_RSIZE - 6;
  seg[nbseg].y1 = p1; seg[nbseg].y2 = p2;
  nbseg++;

  if (p2 + 1 >= 0 && p2 + 1 < This->Size)
  {
   seg[nbseg].x1 = 4; seg[nbseg].x2 = XaRegle_RSIZE - 6;
   seg[nbseg].y1 = seg[nbseg].y2 = p2 + 1;
   nbseg++;
  }

  XDrawSegments(This->display, This->window, This->gc_Light, seg, nbseg);
 }
}

/* ----------------------------------------------------------------- ** 
** draw normal frame                                                 ** 
** ----------------------------------------------------------------- */
static void draw_normal_frame(This, interv)
c_XaRegle	*This;
Xa_interv_t	*interv;
{
 if (This->Orient == XaRegle_HORIZONTAL)
 {
  XFillRectangle(This->display, This->window, This->gc_Light,
		 This->Start+interv->min+1, 5,
		 interv->max - interv->min, XaRegle_RSIZE - (8+5));
  XDrawLine(This->display, This->window, This->gc,
	    This->Start+interv->min, 3+1,
	    This->Start+interv->min, XaRegle_RSIZE - 8);
  XDrawLine(This->display, This->window, This->gc,
	    This->Start+interv->min+1, 3+1,
	    This->Start+interv->max, 3+1);
  XDrawLine(This->display, This->window, This->gc_Shadow,
	    This->Start+interv->min, 3,
	    This->Start+interv->max, 3);
  XDrawLine(This->display, This->window, This->gc_Shadow,
	    This->Start+interv->min, XaRegle_RSIZE - 8,
	    This->Start+interv->max, XaRegle_RSIZE - 8);
  XDrawLine(This->display, This->window, This->gc_Light,
	    This->Start+interv->min, XaRegle_RSIZE - 7,
	    This->Start+interv->max, XaRegle_RSIZE - 7);
 }
 else
 {
  XFillRectangle(This->display, This->window, This->gc_Light,
		 6, This->Start+interv->min+1,
		 XaRegle_RSIZE - (6 + 6), interv->max - interv->min);
  XDrawLine(This->display, This->window, This->gc,
	    4 + 1, This->Start+interv->min,
	    XaRegle_RSIZE - 7, This->Start+interv->min);
  XDrawLine(This->display, This->window, This->gc,
	    4 + 1, This->Start+interv->min + 1,
	    4 + 1, This->Start+interv->max);
  XDrawLine(This->display, This->window, This->gc_Shadow,
	    4, This->Start+interv->min,
	    4, This->Start+interv->max);
  XDrawLine(This->display, This->window, This->gc_Shadow,
	    XaRegle_RSIZE - 7, This->Start+interv->min,
	    XaRegle_RSIZE - 7, This->Start+interv->max);
  XDrawLine(This->display, This->window, This->gc_Light,
	    XaRegle_RSIZE - 6, This->Start+interv->min,
	    XaRegle_RSIZE - 6, This->Start+interv->max);
 }
}

/* ----------------------------------------------------------------- ** 
** draw_ruler_margin: draw ruler margin marks                        ** 
** ----------------------------------------------------------------- */
static void draw_ruler_margin(This, clear)
c_XaRegle *This;
boolean	clear;
{
 int yb, yt;

 yb = XaRegle_RSIZE - 14;
 yt = 3;
 
 if (clear)
 {
  if (This->LeftMargin != This->bot_left_pos)
  {
   XCopyArea(This->display, This->under_bot_left, This->window,
	     This->gc, 0, 0, Xa_Width_Bottom_Left, Xa_Height_Bottom_Left,
	     This->bot_left_pos - Xa_Hot_Bottom_Left, yb);
  }
  if (This->RightMargin != This->bot_right_pos)
   XCopyArea(This->display, This->under_bot_right, This->window,
	     This->gc, 0, 0, Xa_Width_Bottom_Right, Xa_Height_Bottom_Right,
	     This->bot_right_pos - Xa_Hot_Bottom_Right, yb);
   
  
  if (This->IndentMargin != This->top_left_pos)
   XCopyArea(This->display, This->under_top_left, This->window,
	     This->gc, 0, 0, Xa_Width_Top_Left, Xa_Height_Top_Left, 
	     This->top_left_pos - Xa_Hot_Top_Left, yt);
  
 }

 if (!clear || (This->LeftMargin != This->bot_left_pos))
 {
  This->bot_left_pos = This->LeftMargin;
  XCopyArea(This->display, This->window, This->under_bot_left,
	    This->gc, This->bot_left_pos - Xa_Hot_Bottom_Left, yb,
	    Xa_Width_Bottom_Left, Xa_Height_Bottom_Left, 0, 0);
 }
 
 XSetClipOrigin(This->display, Xa_GC_Bottom_Left, 
		This->LeftMargin - Xa_Hot_Bottom_Left, yb);
 XCopyArea(This->display, Xa_Pixmap_Bottom_Left, This->window,
	   Xa_GC_Bottom_Left, 0, 0, Xa_Width_Bottom_Left,
	   Xa_Height_Bottom_Left, This->LeftMargin - Xa_Hot_Bottom_Left, yb);
  
 if (!clear || (This->RightMargin != This->bot_right_pos))
 {
  This->bot_right_pos = This->RightMargin;
  XCopyArea(This->display, This->window, This->under_bot_right,
	    This->gc, This->bot_right_pos - Xa_Hot_Bottom_Right, yb,
	    Xa_Width_Bottom_Right, Xa_Height_Bottom_Right, 0, 0);
 }
 
 XSetClipOrigin(This->display, Xa_GC_Bottom_Right, 
		This->RightMargin - Xa_Hot_Bottom_Right, yb);
 XCopyArea(This->display, Xa_Pixmap_Bottom_Right, This->window,
	   Xa_GC_Bottom_Right, 0, 0, Xa_Width_Bottom_Right,
	   Xa_Height_Bottom_Right, 
	   This->RightMargin - Xa_Hot_Bottom_Right, yb);

 if (!clear || (This->IndentMargin != This->top_left_pos))
 {
  This->top_left_pos = This->IndentMargin;
  XCopyArea(This->display, This->window, This->under_top_left,
	    This->gc, This->top_left_pos - Xa_Hot_Top_Left, yt,
	    Xa_Width_Top_Left, Xa_Height_Top_Left, 0, 0);
 }
 
 XSetClipOrigin(This->display, Xa_GC_Top_Left, 
		This->IndentMargin - Xa_Hot_Top_Left, yt);
 XCopyArea(This->display, Xa_Pixmap_Top_Left, This->window,
	   Xa_GC_Top_Left, 0, 0, Xa_Width_Top_Left,
	   Xa_Height_Top_Left, This->IndentMargin - Xa_Hot_Top_Left, yt); 
}

/* ----------------------------------------------------------------- ** 
** get printed string size                                           ** 
** ----------------------------------------------------------------- */
static void get_printed_string_size(pString, Orient, width, height)
char	*pString;
int	Orient;
int	*width;
int	*height;
{
 int	c;
 int	i;
 int	w,h;
  
 w = h = 0;
 for(i = 0; i < (int)strlen(pString); i++)
 {
  c = (pString[i] == '.' ? 10 : (pString[i] == '-' ? 11 : pString[i] - '0'));
  if (Orient == XaRegle_HORIZONTAL)
  {
   w += XaRegle_Font[c].width + 1; 
   h = MAX(h, XaRegle_Font[c].height);
  }
  else
  {
   w = MAX(w, XaRegle_Font[c].height);
   h += XaRegle_Font[c].width + 1;
  }
 }
 *width = w;
 *height = h;
}

/* ----------------------------------------------------------------- ** 
** print string                                                      ** 
** ----------------------------------------------------------------- */
static void print_string_XaRegle(This, pString, xorig, yorig)
c_XaRegle	*This;
char	*pString;
int	xorig;
int	yorig;
{
 int		i,c;
 int		nb_point;
 static		XPoint	xpoint[200];
 
 nb_point = 0;
 for(i = 0; i < (int)strlen(pString); i++)
 {
  c = (pString[i] == '.' ? 10 : (pString[i] == '-' ? 11 : pString[i] - '0'));
  if (This->Orient == XaRegle_HORIZONTAL)
  {
   memcpy(xpoint + nb_point, XaRegle_CharH[c].xpoint,
	  XaRegle_CharH[c].nb_point * sizeof(XPoint));
   xpoint[nb_point].x += xorig;
   xpoint[nb_point].y += yorig + XaRegle_Font[c].offset_y;
   xorig = XaRegle_CharH[c].incx;
   yorig = XaRegle_CharH[c].incy - XaRegle_Font[c].offset_y;
   nb_point += XaRegle_CharH[c].nb_point;
  }
  else
  {
   memcpy(xpoint + nb_point, XaRegle_CharV[c].xpoint,
	  XaRegle_CharV[c].nb_point * sizeof(XPoint));
   xpoint[nb_point].x += xorig + XaRegle_Font[c].offset_y;
   xpoint[nb_point].y += yorig;
   xorig = XaRegle_CharV[c].incx - XaRegle_Font[c].offset_y;
   yorig = XaRegle_CharV[c].incy;
   nb_point += XaRegle_CharV[c].nb_point;
  }
 }
 XDrawPoints(This->display, This->window, This->gc, xpoint, nb_point,
	     CoordModePrevious);
}

/* ----------------------------------------------------------------- ** 
** CALCUL GRADUATION FUNCTONS(2)                                     ** 
** ----------------------------------------------------------------- */
#if 0
static float convert_in_unit(length,unit)
coord_t	length;
int     unit;
{
 switch(unit)
 {
 case POINT:
  return SCALE_TO_POINTS(length);
 case INCH:
  return SCALE_TO_INCHES(length);
 case MILLIMETER:
  return SCALE_TO_MILLIMETERS(length);
 case CENTIMETER:
  return SCALE_TO_CENTIMETERS(length);
 }
 return 0;
}
#endif

static coord_t convert_from_unit(length, unit)
float	length;
int     unit;
{
 switch(unit)
 {
 case POINT:
  return SCALE_FROM_POINTS(length);
 case INCH:
  return SCALE_FROM_INCHES(length);
 case MILLIMETER:
  return SCALE_FROM_MILLIMETERS(length);
 case CENTIMETER:
  return SCALE_FROM_CENTIMETERS(length);
 }
 return 0;
}

/* ----------------------------------------------------------------- ** 
** calcul graduation                                                 ** 
** ----------------------------------------------------------------- */
static void calcul_graduation(This, scale)
c_XaRegle	*This;
coord_t		scale;
{
 Xc_TRACE(("calcul graduation Scale:%ld to scale:%ld", This->Scale, scale));
 if (This->Scale == scale)
  return;

 Xc_TRACE(("step in unit: %f", This->step_in_unit));
 Xc_TRACE(("converted step in unit: %ld", 
	   convert_from_unit(This->step_in_unit, CENTIMETER)));
 This->step_in_pixel = (float)convert_from_unit(This->step_in_unit, 
						CENTIMETER)/scale;
 This->Scale=scale;
}

/* ----------------------------------------------------------------- ** 
** DRAW GRADUATION                                                   ** 
** ----------------------------------------------------------------- */
static int draw_graduation(This, start, end, start_num)
c_XaRegle	*This;
int		start;
int		end;
int		start_num;
{
 float	i;
 int	nseg;
 int	skip;
 int	four_way;
 char	str_num[4];
 int	width;
 int	height;

 skip = 0;
 Xc_TRACE(("step in pixel:%f", This->step_in_pixel));
 if (This->step_in_pixel <= 6.8)/* interrupt small mark */
  skip ++;
 if (This->step_in_pixel <= 3.4)/* interrupt bigmark */
  skip ++;
 if (This->step_in_pixel <= 2.8)/* text/2 */
  skip ++;
 if (This->step_in_pixel <= 1)	/* again text/2 */
  skip ++;

 nseg = 0;
 four_way = 0;
 Xc_TRACE(("draw grad"));
 Xc_TRACE(("Orient: %p / %d", This, This->Orient));
 if (This->Orient == XaRegle_HORIZONTAL)
 {
  for(i = start; i < end; i += This->step_in_pixel)
  {
   switch (four_way%4)
   {
   case 0:
    if (four_way)
    {
     if (((skip < 3) || ((skip >= 3) && (four_way%(8*(skip-2)) == 0))) &&
	 (i > XaRegle_SIZE && i < This->Size))
     {
      sprintf(str_num, "%d", start_num);
      get_printed_string_size(str_num, XaRegle_HORIZONTAL, &width, &height);
      print_string_XaRegle(This, str_num,
			   (int)i-(width/2)+1,
			   (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
			   +XaRegle_UPBORDER-3);
     }
    }
    start_num++;
    break;			/* may put inside first to draw bar ... */
   case 2:
    if (skip < 2 && i > XaRegle_SIZE && i < This->Size)
    {
     segments[nseg].x1 = segments[nseg].x2 = (int)i;
     segments[nseg].y1 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_BIGMARK/2;
     segments[nseg].y2 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_BIGMARK/2+XaRegle_BIGMARK-1;
     nseg++;
    }
    break;
    
   case 1:
   case 3:
    if (skip < 1 && i > XaRegle_SIZE && i < This->Size)
    {
     segments[nseg].x1 = segments[nseg].x2 = (int)i;
     segments[nseg].y1 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_MARK/2;
     segments[nseg].y2 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_MARK/2+XaRegle_MARK;
     nseg++;
    }
   }
   four_way++;
  }
 }
 else
 {
  for(i = start; i < end; i += This->step_in_pixel)
  {
   switch (four_way % 4)
   {
   case 0:
    if (four_way)
    {
     if (((skip < 3) || ((skip >= 3) && (four_way%(8*(skip-2)) == 0))) &&
	 (i > 0 && i < This->Size))
     {
      sprintf(str_num, "%d", start_num);
      get_printed_string_size(str_num, XaRegle_VERTICAL, &width, &height);
      print_string_XaRegle(This, str_num,
			   (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
			   +XaRegle_UPBORDER-3,
			   (int)i+(height/2)-1);
     }
    }    
    start_num++;
    break;			/* may put inside first to draw bar ... */
   case 2:
    if (skip < 2)		/* && i > 0 && i < This->Size)*/
    {
     segments[nseg].x1 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_BIGMARK/2;
     segments[nseg].x2 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_BIGMARK/2+XaRegle_BIGMARK-1;
     segments[nseg].y1 = segments[nseg].y2 = (int)i;
     nseg++; 
    }
    break;
    
   case 1:
   case 3:
    if (skip < 1)		/*  && i > 0 && i < This->Size) */
    {
     segments[nseg].x1 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_MARK/2;
     segments[nseg].x2 = (XaRegle_SIZE-XaRegle_UPBORDER-XaRegle_DOWNBORDER)/2
      +XaRegle_UPBORDER-XaRegle_MARK/2+XaRegle_MARK;
     segments[nseg].y1 = segments[nseg].y2 = (int)i;
     nseg++;
    }
   }
   four_way++;
  }
 }
 if (nseg)
  XDrawSegments(This->display, This->window, This->gc, segments, nseg);
 return start_num;
}

static int get_function_and_change_cursor(This, xm, ym, change)
c_XaRegle *This;
int xm;
int ym;
boolean change;
{
 int		function = -1;

 if (This->Orient == XaRegle_HORIZONTAL)
 {
  if (xm < XaRegle_SIZE)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_C);
   function = XaR_CHANGE_TAB_TYPE;
  } 
  else if (xm > This->LeftMargin - 10 && xm < This->LeftMargin + 10 &&
	   ym > XaRegle_RSIZE - 14)
  {
   if (ym > XaRegle_RSIZE - 8)
   {
    if (change)
     F(This->cursor).set_cursor(This->cursor, C_REGLE_SQUARE);
    function = XaR_MOVE_RULER_LEFT_MARGIN;
   }
   else
   {
    if (change)
     F(This->cursor).set_cursor(This->cursor, C_REGLE_AUP);
    function = XaR_MOVE_RULER_LEFT2_MARGIN;
   }
  }
  else if (xm > This->RightMargin - 10 && xm < This->RightMargin + 10 &&
	   ym > XaRegle_RSIZE - 14 && ym < XaRegle_RSIZE - 14 + 
	   Xa_Height_Bottom_Right)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_AUP);
   function = XaR_MOVE_RULER_RIGHT_MARGIN;   
  }
  else if (xm > This->IndentMargin - 10 && xm < This->IndentMargin + 10 &&
	   ym > 3 && ym < 3 + Xa_Height_Top_Left)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_ADOWN);
   function = XaR_MOVE_RULER_INDENT_MARGIN;   
  }
  else if (ym > XaRegle_RSIZE - 19 && ym < XaRegle_RSIZE - 8)
  {
   int i, best, delta;

   delta = This->Size;
   best = -1;
   for(i = 0; i < This->nbTab; i++)
   {
    if (xm > This->tabPos[i] - 7 && xm < This->tabPos[i] + 7)
    {
     if (delta > ABS(xm - This->tabPos[i]))
     {
      delta = ABS(xm - This->tabPos[i]);
      best = i;
     }
    }
   }
   
   if (best >= 0)
   {
    if (change)
    {
     switch(This->tabType[best])
     {
     case XcTAB_LEFT:
      F(This->cursor).set_cursor(This->cursor, C_REGLE_TABL);
      break;
     case XcTAB_RIGHT:
      F(This->cursor).set_cursor(This->cursor, C_REGLE_TABR);
      break;
     case XcTAB_CENTER:
      F(This->cursor).set_cursor(This->cursor, C_REGLE_TABC);
      break;
     case XcTAB_CPERIOD:
      F(This->cursor).set_cursor(This->cursor, C_REGLE_TABCP);
      break;
     }
    }
    function = XaR_MOVE_RULER_TAB;
    This->tabPtr = best;
   }
  } 
  if (function != -1)
   return function;
  
  if (xm > This->MarginStart - 10 && xm < This->MarginStart + 10)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_V);
   function = XaR_MOVE_PAGE_LEFT_MARGIN;
  }
  else if (xm > This->MarginEnd - 10 && xm < This->MarginEnd + 10)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_V);
   function = XaR_MOVE_PAGE_RIGHT_MARGIN;
  }
  else if (xm > This->MarginStart && xm < This->MarginEnd &&
	   ym > XaRegle_RSIZE - 19 && ym < XaRegle_RSIZE - 8)
  {
   function = XaR_DROP_RULER_TAB;
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_NORMAL);
  }

 }
 else
 {				/* VERTICAL SCALE */
  if (ym > This->MarginStart - 10 && ym < This->MarginStart + 10)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_H);
   function = XaR_MOVE_PAGE_TOP_MARGIN;
  }
  else if (ym > This->MarginEnd - 10 && ym < This->MarginEnd + 10)
  {
   if (change)
    F(This->cursor).set_cursor(This->cursor, C_REGLE_H);
   function = XaR_MOVE_PAGE_BOTTOM_MARGIN;
  }
 }

 if (function == -1)
 {
  function = XaR_NO_FUNCTION;
  if (change)
   F(This->cursor).set_cursor(This->cursor, C_NORMAL);
 }
 return function;
}

/* ----------------------------------------------------------------- ** 
** set_x_within_range                                                ** 
** ----------------------------------------------------------------- */
static void set_x_within_range(This, x, direction)
c_XaRegle *This;
int *x;
boolean direction;
{
 c_Page *page = This->Page;
 int xmin, xmax;
 
 switch(direction)
 {
 case XaR_PAGE_MARGIN:
  xmin = This->Start - MIN(MIN(This->LeftMargin, This->IndentMargin),
			   This->MarginStart) + This->MarginStart;
  xmax = MIN(This->MarginEnd, This->RightMargin) - 
   SCALE_FROM_CENTIMETERS(1.5) / page->sScale - 
    (MAX(MAX(This->LeftMargin, This->IndentMargin),
	 This->MarginStart) - This->MarginStart);
  break;

 case XaR_LEFT_MARGIN:
  xmin = This->Start + MAX(0, This->LeftMargin - This->IndentMargin);
  xmax = MIN(This->MarginEnd, This->RightMargin) - 
   SCALE_FROM_CENTIMETERS(1.5) / page->sScale + 
    MIN(0, This->LeftMargin - This->IndentMargin);
  break;
 case XaR_LEFT2_MARGIN:
  xmin = This->Start;
  xmax = MIN(This->MarginEnd, This->RightMargin) - 
   SCALE_FROM_CENTIMETERS(1.5) / page->sScale;
  break;
 case XaR_RIGHT_MARGIN:
 default:
  xmin = MAX(MAX(This->MarginStart, This->LeftMargin), This->IndentMargin) + 
   SCALE_FROM_CENTIMETERS(1.5) / page->sScale;
  xmax = This->End;
  break;
 }
 
 if (*x < xmin) *x = xmin;
 if (*x > xmax) *x = xmax;
}

/* ----------------------------------------------------------------- ** 
** grid_position: apply a grid on x or y position                    ** 
** ----------------------------------------------------------------- */
static coord_t grid_position(This, pos, type)
c_XaRegle *This;
int *pos;
boolean type;
{
 coord_t origin, spos, step;
 
 if (type == FALSE || This->Orient == XaRegle_VERTICAL)
  origin = COORD_ZERO;
 else
 {
  if (*pos < This->Start + This->MarginStart)
   origin = COORD_ZERO;
  else if (*pos < This->Start + This->MarginEnd)
   origin = This->Page->left_margin;
  else
   origin = This->Page->right_margin;
 }

 step = convert_from_unit(This->step_in_unit, CENTIMETER);
 
 spos = (*pos - This->Start) * This->Page->sScale - origin;
 spos += (coord_t)(step / 2);
 spos = ((spos / step) * step) + origin;
 
 *pos = (spos / This->Page->sScale) + This->Start;
 return spos;
}

/* ----------------------------------------------------------------- ** 
** draw_h_mark: display an horizontal mark                           ** 
** ----------------------------------------------------------------- */
static void draw_h_mark(This, y)
c_XaRegle *This;
int y;
{
 c_Page *page = This->Page;

 if (!This->prems)
 {
  XDrawLine(This->display, This->window, This->gc_repere, 6, This->ym, 
	    XaRegle_RSIZE - 8, This->ym);
  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    0, This->ym - page->PosY, page->Width, This->ym - page->PosY);
 }
 else
  This->prems = FALSE;

 This->ym = y;
 XDrawLine(This->display, This->window, This->gc_repere, 6, This->ym, 
	   XaRegle_RSIZE - 8, This->ym);
 XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	   0, This->ym - page->PosY, page->Width, This->ym - page->PosY);
}

/* ----------------------------------------------------------------- ** 
** draw_v_mark: display a vertical mark                              ** 
** ----------------------------------------------------------------- */
static void draw_v_mark(This, x, type)
c_XaRegle *This;
int x;
int type;
{
 c_Page *page = This->Page;
 int	c, yb, yt;

 switch(type)
 {
 case XaR_PAGE_MARGIN:
  if (!This->prems)
  {
   XDrawLine(This->display, This->window, This->gc_repere, This->xm, 6, 
	     This->xm, XaRegle_RSIZE - 8);
   XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	     This->xm - page->PosX - XaRegle_RSIZE, 0,
	     This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  }
  else
   This->prems = FALSE;
  
  This->xm = x;
  XDrawLine(This->display, This->window, This->gc_repere, This->xm, 6, 
	    This->xm, XaRegle_RSIZE - 8);
  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    This->xm - page->PosX - XaRegle_RSIZE, 0, 
	    This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  break;
 case XaR_LEFT_MARGIN:
  if (!This->prems)
  {
   XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	     This->xm - page->PosX - XaRegle_RSIZE, 0,
	     This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  }
  else
   This->prems = FALSE;

  This->xm = x;
  yb = XaRegle_RSIZE - 14;
  yt = 3;
  
  XCopyArea(This->display, This->under_bot_left, This->window,
	    This->gc, 0, 0, Xa_Width_Bottom_Left, Xa_Height_Bottom_Left,
	    This->bot_left_pos - Xa_Hot_Bottom_Left, yb);

  XCopyArea(This->display, This->under_top_left, This->window,
	    This->gc, 0, 0, Xa_Width_Top_Left, Xa_Height_Top_Left, 
	    This->top_left_pos - Xa_Hot_Top_Left, yt);

  This->top_left_pos += x - This->bot_left_pos;
  This->bot_left_pos = x;

  XCopyArea(This->display, This->window, This->under_bot_left,
	    This->gc, This->bot_left_pos - Xa_Hot_Bottom_Left, yb,
	    Xa_Width_Bottom_Left, Xa_Height_Bottom_Left, 0, 0);

  XCopyArea(This->display, This->window, This->under_top_left,
	    This->gc, This->top_left_pos - Xa_Hot_Top_Left, yt,
	    Xa_Width_Top_Left, Xa_Height_Top_Left, 0, 0);
  
  XSetClipOrigin(This->display, Xa_GC_Bottom_Left, 
		 x - Xa_Hot_Bottom_Left, yb);
  XCopyArea(This->display, Xa_Pixmap_Bottom_Left, This->window,
	    Xa_GC_Bottom_Left, 0, 0, Xa_Width_Bottom_Left,
	    Xa_Height_Bottom_Left, x - Xa_Hot_Bottom_Left, yb);

  XSetClipOrigin(This->display, Xa_GC_Top_Left, 
		 This->top_left_pos - Xa_Hot_Top_Left, yt);
  XCopyArea(This->display, Xa_Pixmap_Top_Left, This->window,
	    Xa_GC_Top_Left, 0, 0, Xa_Width_Top_Left,
	    Xa_Height_Top_Left, This->top_left_pos - Xa_Hot_Top_Left, yt); 

  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    This->xm - page->PosX - XaRegle_RSIZE, 0, 
	    This->xm - page->PosX - XaRegle_RSIZE, page->Height);  
  break;
 case XaR_LEFT2_MARGIN:
  if (!This->prems)
  {
   XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	     This->xm - page->PosX - XaRegle_RSIZE, 0,
	     This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  }
  else
   This->prems = FALSE;
  
  This->xm = x;
  yb = XaRegle_RSIZE - 14;
  
  XCopyArea(This->display, This->under_bot_left, This->window,
	    This->gc, 0, 0, Xa_Width_Bottom_Left, Xa_Height_Bottom_Left,
	    This->bot_left_pos - Xa_Hot_Bottom_Left, yb);
  This->bot_left_pos = x;
  XCopyArea(This->display, This->window, This->under_bot_left,
	    This->gc, This->bot_left_pos - Xa_Hot_Bottom_Left, yb,
	    Xa_Width_Bottom_Left, Xa_Height_Bottom_Left, 0, 0);
  
  XSetClipOrigin(This->display, Xa_GC_Bottom_Left, 
		 x - Xa_Hot_Bottom_Left, yb);
  XCopyArea(This->display, Xa_Pixmap_Bottom_Left, This->window,
	    Xa_GC_Bottom_Left, 0, 0, Xa_Width_Bottom_Left,
	    Xa_Height_Bottom_Left, x - Xa_Hot_Bottom_Left, yb);
  
  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    This->xm - page->PosX - XaRegle_RSIZE, 0, 
	    This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  
  break;
 case XaR_RIGHT_MARGIN:
  if (!This->prems)
  {
   XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	     This->xm - page->PosX - XaRegle_RSIZE, 0,
	     This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  }
  else
   This->prems = FALSE;

  This->xm = x;
  yb = XaRegle_RSIZE - 14;
  
  XCopyArea(This->display, This->under_bot_right, This->window,
	    This->gc, 0, 0, Xa_Width_Bottom_Right, Xa_Height_Bottom_Right,
	    This->bot_right_pos - Xa_Hot_Bottom_Right, yb);

  This->bot_right_pos = x;
  XCopyArea(This->display, This->window, This->under_bot_right,
	    This->gc, This->bot_right_pos - Xa_Hot_Bottom_Right, yb,
	    Xa_Width_Bottom_Right, Xa_Height_Bottom_Right, 0, 0);

  XSetClipOrigin(This->display, Xa_GC_Bottom_Right, 
		 x - Xa_Hot_Bottom_Right, yb);
  XCopyArea(This->display, Xa_Pixmap_Bottom_Right, This->window,
	    Xa_GC_Bottom_Right, 0, 0, Xa_Width_Bottom_Right,
	    Xa_Height_Bottom_Right, x - Xa_Hot_Bottom_Right, yb);
    
  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    This->xm - page->PosX - XaRegle_RSIZE, 0, 
	    This->xm - page->PosX - XaRegle_RSIZE, page->Height);  
  break;
 case XaR_INDENT_MARGIN:
  if (!This->prems)
  {
   XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	     This->xm - page->PosX - XaRegle_RSIZE, 0,
	     This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  }
  else
   This->prems = FALSE;
  
  This->xm = x;
  yt = 3;
  
  XCopyArea(This->display, This->under_top_left, This->window,
	    This->gc, 0, 0, Xa_Width_Top_Left, Xa_Height_Top_Left, 
	    This->top_left_pos - Xa_Hot_Top_Left, yt);
  
  This->top_left_pos = x;
  XCopyArea(This->display, This->window, This->under_top_left,
	    This->gc, This->top_left_pos - Xa_Hot_Top_Left, yt,
	    Xa_Width_Top_Left, Xa_Height_Top_Left, 0, 0);
  
  XSetClipOrigin(This->display, Xa_GC_Top_Left, 
		 x - Xa_Hot_Top_Left, yt);
  XCopyArea(This->display, Xa_Pixmap_Top_Left, This->window,
	    Xa_GC_Top_Left, 0, 0, Xa_Width_Top_Left,
	    Xa_Height_Top_Left, x - Xa_Hot_Top_Left, yt); 
  
  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    This->xm - page->PosX - XaRegle_RSIZE, 0, 
	    This->xm - page->PosX - XaRegle_RSIZE, page->Height);  
  break;
 case XaR_TAB_MARK:
  c = This->tabType[This->tabPtr] + TAB_START;
  if (!This->prems)
  {
   XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	     This->xm - page->PosX - XaRegle_RSIZE, 0,
	     This->xm - page->PosX - XaRegle_RSIZE, page->Height);
  }
  else
   This->prems = FALSE;
  
  if (This->xm >= 0)
   XCopyArea(This->display,  This->under_tabs[This->tabPtr], This->window,
	     This->gc, 0, 0, XaRegle_Font[c].width, XaRegle_Font[c].height,
	     This->xm - XaRegle_Font[c].offset_x, XaRegle_RSIZE - 17);
  
  if (x < This->Start || x < XaRegle_RSIZE || x > This->End || x > This->Size)
  {
   This->prems = TRUE;
   break;
  }

  This->xm = x;
  
  XCopyArea(This->display, This->window, This->under_tabs[This->tabPtr],
	    This->gc, This->xm - XaRegle_Font[c].offset_x, 
	    XaRegle_RSIZE - 17, XaRegle_Font[c].width, XaRegle_Font[c].height,
	    0, 0);
  
  memcpy(points, XaRegle_CharH[c].xpoint, 
	 XaRegle_CharH[c].nb_point * sizeof(XPoint));
  points[0].x += x - XaRegle_Font[c].offset_x;
  points[0].y += XaRegle_RSIZE - 17;
  XDrawPoints(This->display, This->window, This->gc, points, 
	      XaRegle_CharH[c].nb_point, CoordModePrevious);

  XDrawLine(This->display, page->X_info.window, page->X_info.gc_contour,
	    This->xm - page->PosX - XaRegle_RSIZE, 0, 
	    This->xm - page->PosX - XaRegle_RSIZE, page->Height);  
  break;
 }
}
