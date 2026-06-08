/*
** Regle.c for Xclamation in Regle/
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
** Started on  Tue Jul 18 12:14:09 1995 Emmanuel Paris
** Last update Fri Oct  2 15:35:17 1998 Robin Castanier
*/

#define NTRACE
#define NHISTORY

#define _PRIVATE_REGLE_
#include "Regle.h"
#include "Colormap.h"
#include "ImageManager.h"

extern c_Colormap *GlobColormap;
extern c_ImageManager *GlobImgMng;

void *cons_Regle();
void dest_Regle();
void *copy_Regle();
void Init_Regle();
void Reset_Regle();
void resize_Regle();
boolean eventlook_Regle();
void Init_Regle_By_Page();
void draw_Regle();
void draw_repere_Regle();
void clear_repere_Regle();

static void rehash_graduation_interval();
static void get_printed_string_size();
static void print_string_Regle();
static float convert_in_unit();
static void calcul_graduation();
static void draw_graduation();
static void draw_graduation_interval_positive();
static void draw_graduation_interval_negative();

sf_Regle fc_Regle =
{
 cons_Regle,
 dest_Regle,
 copy_Regle,
 resize_Regle,
 eventlook_Regle,
 Init_Regle_By_Page,
 draw_Regle,
 draw_repere_Regle,
 clear_repere_Regle,
};

void *cons_Regle(display, window, PosX, PosY, Size, Orient)
Display	*display;
Window	window;
int	PosX;
int	PosY;
int	Size;
char	Orient;
{
 c_Regle 	*ObjTmp;
 XGCValues	gcv;
 XSetWindowAttributes	windattrib;

 Xc_HISTORY(("Building Regle"));
 if ((ObjTmp = (c_Regle *)Xc_malloc("Regle", sizeof(c_Regle))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Regle;
 ObjTmp->display = display;
 ObjTmp->Orient = Orient;
 ObjTmp->Unit = MILLIMETER;
 ObjTmp->nb_intervs = 0;
 ObjTmp->rep1_actif = FALSE;
 ObjTmp->rep1_drawn = FALSE;
 ObjTmp->rep2_drawn = FALSE;
 ObjTmp->source = FALSE;
 switch(Orient)
 {
 case REGLE_HORIZONTAL:
  ObjTmp->window = 
   XCreateSimpleWindow(display, window, 
		       PosX-1,  PosY,
		       Size, REGLE_RSIZE, 
		       0, 
		       BlackPixel(display,DefaultScreen(display)),
		       Regle_Color);
  ObjTmp->cursor=(c_Cursor *)NEW(c_Cursor)(display,ObjTmp->window);
  F(ObjTmp->cursor).set_cursor(ObjTmp->cursor,C_REGLE_H);
  ObjTmp->Size=Size;

  break;
 case REGLE_VERTICAL:
  ObjTmp->window = 
   XCreateSimpleWindow(display, window, 
		       PosX,  PosY-1,
		       REGLE_RSIZE, Size, 
		       0, 
		       BlackPixel(display,DefaultScreen(display)),
		       Regle_Color);
  ObjTmp->cursor=(c_Cursor *)NEW(c_Cursor)(display,ObjTmp->window);
  F(ObjTmp->cursor).set_cursor(ObjTmp->cursor,C_REGLE_V);
  ObjTmp->Size=Size;

  break;
 case REGLE_CENTER:
  ObjTmp->window = 
   XCreateSimpleWindow(display, window, 
		       PosX,  PosY,
		       Size, Size, 
		       0, 
		       BlackPixel(display,DefaultScreen(display)),
		       Regle_Color);
  ObjTmp->cursor=(c_Cursor *)NEW(c_Cursor)(display,ObjTmp->window);
  F(ObjTmp->cursor).set_cursor(ObjTmp->cursor,C_REGLE_C);
  ObjTmp->Size=Size;
  break;
 }
 XSelectInput(display, ObjTmp->window, PointerMotionMask | ButtonPressMask |
	      ButtonReleaseMask | ExposureMask);
 windattrib.bit_gravity=StaticGravity;
 XChangeWindowAttributes(display,window,CWBitGravity,&windattrib);
 gcv.function = GXcopy;
 gcv.line_width = 0;
 gcv.line_style = LineSolid;
 gcv.cap_style = CapButt;
 gcv.join_style = JoinMiter;
 gcv.background = WhitePixel(display,DefaultScreen(display));
 gcv.foreground = BlackPixel(display,DefaultScreen(display));
 ObjTmp->gc=XCreateGC(display, ObjTmp->window,
		      GCFunction | GCForeground | GCBackground |
		      GCLineWidth | GCLineStyle | GCCapStyle |
		      GCJoinStyle,
		      &gcv);
 if (BlackPixel(display,DefaultScreen(display)))
  gcv.function = GXxor;
 else
  gcv.function = GXinvert;
 ObjTmp->gc_repere=XCreateGC(display, ObjTmp->window,
			     GCFunction | GCForeground | GCBackground |
			     GCLineWidth | GCLineStyle | GCCapStyle |
			     GCJoinStyle,
			     &gcv);
 XMapWindow(display, ObjTmp->window);
 if (ObjTmp->Orient == REGLE_CENTER)
  draw_Regle(ObjTmp, FALSE);
 Xc_TRACE(("Object Regle build"));
 return ObjTmp;
}

void dest_Regle(This)
c_Regle *This;
{
 XFreeGC(This->display,This->gc);
 if (This->nb_intervs)
  Xc_free(This->intervs);
 DELETE(c_Cursor)(This->cursor);
 XDestroyWindow(This->display, This->window);
 Xc_free(This);
 Xc_TRACE(("Object Regle destroyed"));
}

void *copy_Regle(This)
c_Regle *This;
{
 c_Regle *ObjTmp;

 if ((ObjTmp = (c_Regle *)Xc_malloc("CRegle", sizeof(c_Regle))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_Regle));
 Xc_TRACE(("Object Regle copied"));
 return ObjTmp;
}
/* ----------------------------------------------------------------- ** 
** INIT REGLE                                                        ** 
** ----------------------------------------------------------------- */

void Init_Regle(display)
Display	*display;
{
 int		i,count;
 int		x,y;
 int		xtmp,ytmp;
 color_cells_t	realcolor;

 F(GlobColormap).allocate_named_color(GlobColormap, REGLE_COLOR, &realcolor);
 Regle_Color = realcolor.pixel;
  
 F(GlobColormap).allocate_named_color(GlobColormap, REGLE_SEPARATOR,
				      &realcolor);
 Regle_Separator = realcolor.pixel;
  
 if (DefaultDepth(display, DefaultScreen(display)) == 1)
 {
  for(i=0;i<NB_SLICE-1;i++)
  {
   Regle_Slice[i].real_color= 
    WhitePixel(display,DefaultScreen(display));
  }
  Regle_Slice[i].real_color= 
   BlackPixel(display,DefaultScreen(display));
 }
 else
 {
  for(i=0;i<NB_SLICE;i++)
  {
   F(GlobColormap).allocate_named_color
    (GlobColormap, Regle_Slice[i].color, &realcolor);
   Regle_Slice[i].real_color = realcolor.pixel;
  }
 }
 for(i=0; i<NB_CHAR; i++)
 {
  count=0;
  for(y=0; y<Regle_Font[i].height; y++)
   for(x=0; x<Regle_Font[i].width; x++)
    if (Regle_Font[i].data[y][x]=='*')
     count++;
  Regle_CharH[i].nb_point=count;
  Regle_CharH[i].xpoint=(XPoint *)Xc_malloc("xpoint",sizeof(XPoint)*count);
  Regle_CharV[i].nb_point=count;
  Regle_CharV[i].xpoint=(XPoint *)Xc_malloc("xpoint",sizeof(XPoint)*count);
  count=xtmp=ytmp=0;
  if (GlobImgMng->draw_points_bug)
  {
   for(y=0; y<Regle_Font[i].height; y++)
    for(x=0; x<Regle_Font[i].width; x++)
     if (Regle_Font[i].data[y][x]=='*')
     {
      Regle_CharH[i].xpoint[count].x=x-xtmp;
      Regle_CharH[i].xpoint[count].y=y-ytmp;
      if (x < xtmp)
       Regle_CharH[i].xpoint[count].y--;
      count++;
      xtmp=x;
      ytmp=y;
     }
  }
  else
  {
   for(y=0; y<Regle_Font[i].height; y++)
    for(x=0; x<Regle_Font[i].width; x++)
     if (Regle_Font[i].data[y][x]=='*')
     {
      Regle_CharH[i].xpoint[count].x=x-xtmp;
      Regle_CharH[i].xpoint[count++].y=y-ytmp;
      xtmp=x;
      ytmp=y;
     }
  }
  Regle_CharH[i].incx=Regle_Font[i].width+1-xtmp;
  Regle_CharH[i].incy = -ytmp;
  count=xtmp=ytmp=0;
  for(x=0; x<Regle_Font[i].height; x++)
   for(y=0; y<Regle_Font[i].width; y++)
    if (Regle_Font[i].data[x][y]=='*')
    {
     Regle_CharV[i].xpoint[count].x=x-xtmp;
     Regle_CharV[i].xpoint[count++].y=ytmp-y;
     xtmp=x;
     ytmp=y;
    }
  Regle_CharV[i].incx = -xtmp;
  Regle_CharV[i].incy = ytmp -Regle_Font[i].width-1;
 }
}

void Reset_Regle()
{
 int	i;
  
 for(i=0; i<NB_CHAR; i++)
 {
  Xc_free(Regle_CharH[i].xpoint);
  Xc_free(Regle_CharV[i].xpoint);
 }
}

/* ----------------------------------------------------------------- ** 
** INIT REGLE BY PAGE                                                ** 
** ----------------------------------------------------------------- */

void Init_Regle_By_Page(This,Page)
c_Regle	*This;
c_Page	*Page;
{
 This->Page = Page;
 switch(This->Orient)
 {
 case REGLE_HORIZONTAL:
  calcul_graduation(This,Page->sScale);
  break;
 case REGLE_VERTICAL:
  calcul_graduation(This,Page->sScale);
  break;    
 }
 rehash_graduation_interval(This);
}



/* ----------------------------------------------------------------- ** 
** RESIZE REGLE                                                      ** 
** ----------------------------------------------------------------- */

void resize_Regle(This, PosX, PosY, Size) 
c_Regle	*This;
int	PosX;
int	PosY;
int	Size;
{
 /*  if (This->Orient == REGLE_HORIZONTAL)
     {
     XMoveResizeWindow(This->display, This->window,
     PosX-1, PosY,
     Size, REGLE_RSIZE);
     }
     else
     {
     XMoveResizeWindow(This->display, This->window,
     PosX, PosY-1,
     REGLE_RSIZE, Size);
     } */
 if (This->Orient == REGLE_HORIZONTAL)
 {
  XResizeWindow(This->display, This->window, Size, REGLE_RSIZE);
 }
 else
 {
  XResizeWindow(This->display, This->window, REGLE_RSIZE, Size);
 } 
  
 This->Size=Size;
}

/* ----------------------------------------------------------------- ** 
** EVENTLOOK REGLE                                                   ** 
** ----------------------------------------------------------------- */

boolean eventlook_Regle(This, Event)
c_Regle	*This;
XEvent	*Event;
{
 XRectangle    rect;
 int		i,xm,ym;
 c_Page	*cpage;
 coord_t	orgH,orgV;
 boolean	ret_code = FALSE;

 cpage=This->Page;
 if (Event->xany.window==This->window)
 {
  switch(Event->type)
  {
  case Expose:
  case GraphicsExpose:
   if (This->rep1_drawn || This->rep2_drawn)
   {
    switch(This->Orient)
    {
    case REGLE_HORIZONTAL:
     rect.x=Event->xexpose.x;
     rect.y=0;
     rect.width=Event->xexpose.width;
     rect.height=REGLE_SIZE;
     XSetClipRectangles(This->display,This->gc,
			0,0,&rect,1,Unsorted);
     F(This).Draw(This,FALSE);
     if (This->rep1_drawn && This->repere1>=rect.x &&
	 This->repere1<rect.x+rect.width)
     {
      This->rep1_drawn = FALSE;
      draw_repere_Regle(This, REGLE_REPERE_1, This->repere1);
     }
     if (This->rep2_drawn && This->repere2>=rect.x &&
	 This->repere2<rect.x+rect.width)
     {
      This->rep2_drawn = FALSE;
      draw_repere_Regle(This, REGLE_REPERE_2, This->repere2);
     }
     XSetClipMask(This->display,This->gc,None);
     break;
    case REGLE_VERTICAL:
     rect.x=0;
     rect.y=Event->xexpose.y;
     rect.width=REGLE_SIZE;
     rect.height=Event->xexpose.height;
     XSetClipRectangles(This->display,This->gc,
			0,0,&rect,1,Unsorted);
     F(This).Draw(This,FALSE);
     if (This->rep1_drawn && This->repere1>=rect.y &&
	 This->repere1<rect.y+rect.height)
     {
      This->rep1_drawn = FALSE;
      draw_repere_Regle(This, REGLE_REPERE_1, This->repere1);
     }
     if (This->rep2_drawn && This->repere2>=rect.y &&
	 This->repere2<rect.y+rect.height)
     {
      This->rep2_drawn = FALSE;
      draw_repere_Regle(This, REGLE_REPERE_2, This->repere2);
     }
     XSetClipMask(This->display,This->gc,None);
     break;
    }
   }
   else
   {
    rect.x=Event->xexpose.x;
    rect.y=Event->xexpose.y;
    rect.width=Event->xexpose.width;
    rect.height=Event->xexpose.height;
    XSetClipRectangles(This->display,This->gc,0,0,&rect,1,Unsorted);
    F(This).Draw(This,FALSE);
    XSetClipMask(This->display,This->gc,None);
   }
   break;
  case MotionNotify:
   xm=Event->xmotion.x;
   ym=Event->xmotion.y;
   if (This->source)
   {
    if (This->Orient==REGLE_HORIZONTAL ||
	This->Orient==REGLE_CENTER )
    {
     ym -= This->Page->PosY;
     if (!This->premsh)
     {
      clear_repere_Regle(This->Page->RegleV, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window, 
		This->Page->X_info.gc_contour, 0, This->yo,
		This->Page->RegleH->Size, This->yo);
     }
     if (ym>=0 && ym<This->Page->Height)
     {
      draw_repere_Regle(This->Page->RegleV, 
			REGLE_REPERE_1, ym);
      XDrawLine(This->display, This->Page->X_info.window, 
		This->Page->X_info.gc_contour, 0, 
		ym, This->Page->RegleH->Size, ym);
      This->yo=ym;
      This->premsh=FALSE;
     }
     else
      This->premsh=TRUE;
    }
    if (This->Orient==REGLE_VERTICAL ||
	This->Orient==REGLE_CENTER )
    {
     xm -= This->Page->PosX;
     if (!This->premsv)
     {
      clear_repere_Regle(This->Page->RegleH, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window,
		This->Page->X_info.gc_contour, This->xo, 0,
		This->xo, This->Page->RegleV->Size);
     }
     if (xm>=0 && ym<This->Page->Width)
     {
      draw_repere_Regle(This->Page->RegleH, 
			REGLE_REPERE_1, xm);
      XDrawLine(This->display, This->Page->X_info.window, 
		This->Page->X_info.gc_contour,xm , 0,
		xm, This->Page->RegleV->Size);
      This->xo=xm;
      This->premsv=FALSE;
     }
     else
      This->premsv=TRUE;
    }
   }
   break;
  case ButtonRelease:
   xm=Event->xmotion.x;
   ym=Event->xmotion.y;
   if (This->source)
   {
    xm -= This->Page->PosX;
    ym -= This->Page->PosY;
    switch(This->Orient)
    {
    case REGLE_HORIZONTAL:
     xm += REGLE_SIZE;
     if (!This->premsh)
     {
      clear_repere_Regle(This->Page->RegleV, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window, 
		This->Page->X_info.gc_contour, 0, This->yo,
		This->Page->RegleH->Size, This->yo);
     }
     if (ym>=0 && ym<This->Page->Height)
     {
      F(cpage).add_page_align_mark
       (cpage, ALIGN_MARK_HORIZONTAL,
	xm*cpage->sScale+cpage->sOrig_X, 
	ym*cpage->sScale+cpage->sOrig_Y);
      ret_code = TRUE;
     }
     This->source=FALSE;
     break;
    case REGLE_VERTICAL:
     ym += REGLE_SIZE;
     if (!This->premsv)
     {
      clear_repere_Regle(This->Page->RegleH, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window,
		This->Page->X_info.gc_contour, This->xo, 0,
		This->xo, This->Page->RegleV->Size);
     }
     if (xm>=0 && xm<This->Page->Width)
     {
      F(cpage).add_page_align_mark
       (cpage, ALIGN_MARK_VERTICAL,
	xm*cpage->sScale+cpage->sOrig_X, 
	ym*cpage->sScale+cpage->sOrig_Y);
      ret_code = TRUE;
     }
     This->source=FALSE;
     break;
    case REGLE_CENTER:
     if (!This->premsh)
     {
      clear_repere_Regle(This->Page->RegleV, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window, 
		This->Page->X_info.gc_contour, 0, This->yo,
		This->Page->RegleH->Size, This->yo);
     }
     if (!This->premsv)
     {
      clear_repere_Regle(This->Page->RegleH, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window,
		This->Page->X_info.gc_contour, This->xo, 0,
		This->xo, This->Page->RegleV->Size);
     }
     orgH=(xm+This->Page->Orig_X)*This->Page->sScale;
     i=0; 
     while(i<This->Page->RegleH->nb_intervs)
     {
      if (orgH>This->Page->RegleH->intervs[i].max)
       i++;
      else
      {
       This->Page->RegleH->intervs[i].origine=orgH;
       draw_Regle(This->Page->RegleH,FALSE);
       break;
      }
     }
     orgV=(ym+This->Page->Orig_Y)*This->Page->sScale;
     i=0;
     while(i<This->Page->RegleV->nb_intervs)
     {
      if (orgV>This->Page->RegleV->intervs[i].max)
       i++;
      else
      {
       This->Page->RegleV->intervs[i].origine=orgV;
       draw_Regle(This->Page->RegleV,FALSE);
       break;
      }
     }
     break;
    }
    This->source=FALSE;
   }
   break;
  case ButtonPress:
   xm=Event->xbutton.x;
   ym=Event->xbutton.y;
   switch(Event->xbutton.button)
   {
   case 1:
    This->premsv=This->premsh=This->source=TRUE;
    break;
   case 2:
    if (!This->source && This->Orient==REGLE_CENTER)
    {
     rehash_graduation_interval(This->Page->RegleH);
     rehash_graduation_interval(This->Page->RegleV);
     draw_Regle(This->Page->RegleH,FALSE);
     draw_Regle(This->Page->RegleV,FALSE);
    }
   case 3:
    if (This->source && (!This->premsv || !This->premsh))
    {
     if (This->Orient==REGLE_HORIZONTAL || 
	 ( This->Orient==REGLE_CENTER && !This->premsh))
     {
      clear_repere_Regle(This->Page->RegleV, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window, 
		This->Page->X_info.gc_contour, 0, This->yo,
		This->Page->RegleH->Size, This->yo);
     }
     if (This->Orient==REGLE_VERTICAL || 
	 (This->Orient==REGLE_CENTER && !This->premsv))
     {
      clear_repere_Regle(This->Page->RegleH, REGLE_REPERE_1);
      XDrawLine(This->display, This->Page->X_info.window,
		This->Page->X_info.gc_contour, This->xo, 0,
		This->xo, This->Page->RegleV->Size);
     }
    }
    This->source=FALSE;
    break;
   }
   break;
  }
 }
 return ret_code;
}

/* ----------------------------------------------------------------- ** 
** CALCUL GRADUATION INTERVAL                                        ** 
** ----------------------------------------------------------------- */

static void rehash_graduation_interval(This)
c_Regle	*This;
{
 int	i;

 if (This->nb_intervs)
  Xc_free(This->intervs);
 switch(This->Orient)
 {
 case REGLE_HORIZONTAL:
  This->nb_intervs = This->Page->Page_Contour.nb_page_horizontal + 2;
  This->intervs = (interv_t *)Xc_malloc("intervH",sizeof(interv_t)*
					This->nb_intervs);
  This->intervs[0].min=0;
  This->intervs[0].max=This->intervs[0].origine=
   This->Page->Page_Contour.sOrig_X;
  for(i=1; i<=This->Page->Page_Contour.nb_page_horizontal; i++)
  {
   This->intervs[i].min=This->intervs[i].origine=
    This->intervs[0].max+(i-1)*This->Page->Page_Contour.sWidth;
   This->intervs[i].max=
    This->intervs[0].max+i*This->Page->Page_Contour.sWidth;
  }
  This->intervs[i].min=This->intervs[i].origine=This->intervs[i-1].max;
  This->intervs[i].max=This->Page->sDeskArea_Width;
  break;
 case REGLE_VERTICAL:
  This->nb_intervs = This->Page->Page_Contour.nb_page_vertical + 2;
  This->intervs = (interv_t *)Xc_malloc("intervV",sizeof(interv_t)*
					This->nb_intervs);
  This->intervs[0].min=0;
  This->intervs[0].max=This->intervs[0].origine=
   This->Page->Page_Contour.sOrig_Y;
  for(i=1; i<=This->Page->Page_Contour.nb_page_vertical; i++)
  {
   This->intervs[i].min=This->intervs[i].origine=
    This->intervs[0].max+(i-1)*This->Page->Page_Contour.sHeight;
   This->intervs[i].max=
    This->intervs[0].max+i*This->Page->Page_Contour.sHeight;
  }
  This->intervs[i].min=This->intervs[i].origine=This->intervs[i-1].max;
  This->intervs[i].max=This->Page->sDeskArea_Height;
  break;
 case REGLE_CENTER:
  This->nb_intervs=0;
  This->intervs=(interv_t *)NULL;
  break;
 }
}

/* ----------------------------------------------------------------- ** 
** DRAW REGLE                                                        ** 
** ----------------------------------------------------------------- */

void draw_Regle(This,recalcul)
c_Regle *This;
boolean	recalcul;
{
 int		i,j,k;
 XSegment	xsegment[REGLE_RSIZE];

 Xc_HISTORY(("drawing regle"));
 if (recalcul && This->Orient!=REGLE_CENTER)
  calcul_graduation(This,This->Page->sScale);
 if (!This->nb_intervs && This->Orient!=REGLE_CENTER)
  return;
 k=0;
 for(i=0; i<NB_SLICE; i++)
 {
  XSetForeground(This->display, This->gc, Regle_Slice[i].real_color);
  switch(This->Orient)
  {
  case REGLE_HORIZONTAL:
   for(j=0; j<Regle_Slice[i].nb_pixel; j++)
   {
    xsegment[j].y1=xsegment[j].y2=k++;
    xsegment[j].x1=0;
    xsegment[j].x2=This->Size;
   }
   XDrawSegments(This->display, This->window, This->gc, xsegment, j);
   XSetForeground(This->display, This->gc, Regle_Slice[0].real_color);      
   XDrawLine(This->display, This->window, This->gc, 0, 0, 0, 8);
   break;
  case REGLE_VERTICAL:
   for(j=0; j<Regle_Slice[i].nb_pixel; j++)
   {
    xsegment[j].x1=xsegment[j].x2=k++;
    xsegment[j].y1=0;
    xsegment[j].y2=This->Size;
   }      
   XDrawSegments(This->display, This->window, This->gc, xsegment, j);
   XSetForeground(This->display, This->gc, Regle_Slice[0].real_color);      
   XDrawLine(This->display, This->window, This->gc, 0, 0, 8, 0);
   break;
  case REGLE_CENTER:
   for(j=0; j<Regle_Slice[i].nb_pixel; j++)
   {
    xsegment[j<<1].x1=xsegment[j<<1].x2=k;
    xsegment[j<<1].y1=k;
    xsegment[j<<1].y2=This->Size-1;
    xsegment[(j<<1)+1].x1=k+1,
    xsegment[(j<<1)+1].x2=This->Size-1;
    xsegment[(j<<1)+1].y1=xsegment[(j<<1)+1].y2=k++;
   }      
   XDrawSegments(This->display, This->window, This->gc, xsegment, j<<1);
   XSetForeground(This->display, This->gc, Regle_Separator);
   XDrawLine(This->display, This->window, This->gc, 9, 9, 16, 16);
   break;
  }
 }
 XSetForeground(This->display, This->gc, 
		BlackPixel(This->display,DefaultScreen(This->display)));
 switch(This->Orient)
 {
 case REGLE_HORIZONTAL:
  draw_graduation(This,0,This->Size);
  break;
 case REGLE_VERTICAL:
  draw_graduation(This,0,This->Size);
  break;  
 case REGLE_CENTER:
  xsegment[0].x1=0;
  xsegment[0].x2=8;
  xsegment[0].y1=xsegment[0].y2=REGLE_SIZE-1;
  xsegment[1].x1=xsegment[1].x2=REGLE_SIZE-1;
  xsegment[1].y1=0;
  xsegment[1].y2=8;
  XDrawSegments(This->display, This->window, This->gc, xsegment, 2);
  break;
 }
}
/* ----------------------------------------------------------------- ** 
** DRAW REPERE                                                       ** 
** ----------------------------------------------------------------- */

void draw_repere_Regle(This, repere, position)
c_Regle	*This;
int	repere;
int	position;
{
 if (This->rep1_actif && repere==REGLE_REPERE_2)
  return;
 if (repere == REGLE_REPERE_1 && This->rep1_drawn)
  clear_repere_Regle(This, repere);
 if (repere == REGLE_REPERE_2 && This->rep2_drawn)
  clear_repere_Regle(This, repere);

 if (This->Orient==REGLE_HORIZONTAL)
 {
  position += This->Page->PosX-REGLE_SIZE;
  XDrawLine(This->display, This->window, This->gc_repere, 
	    position, 2, position, REGLE_SIZE-1);
 }
 else
 {
  position += This->Page->PosY-REGLE_SIZE;
  XDrawLine(This->display, This->window, This->gc_repere, 
	    2, position, REGLE_SIZE-1, position);
 }
 if (repere == REGLE_REPERE_1)
 {
  This->repere1 = position;
  This->rep1_drawn = TRUE;
 }
 else
 {
  This->repere2 = position;
  This->rep2_drawn = TRUE;
 }
}

/* ----------------------------------------------------------------- ** 
** CLEAR REPERE                                                      ** 
** ----------------------------------------------------------------- */

void clear_repere_Regle(This, repere)
c_Regle	*This;
int	repere;
{
 int	position;
  
 if (repere == REGLE_REPERE_1)
 {
  if (!This->rep1_drawn)
   return;
  position=This->repere1;
 }
 else
 {
  position=This->repere2;
  if (!This->rep2_drawn || This->rep1_actif)
   return;
 }
  
 if (This->Orient==REGLE_HORIZONTAL)
  XDrawLine(This->display, This->window, This->gc_repere, 
	    position, 2, position, REGLE_SIZE-1);
 else
  XDrawLine(This->display, This->window, This->gc_repere, 
	    2, position, REGLE_SIZE-1, position);
 if (repere == REGLE_REPERE_1)
  This->rep1_drawn = FALSE;
 else
  This->rep2_drawn = FALSE;
}

/* ----------------------------------------------------------------- ** 
** PRINT GRADUATION FUNCTIONS(2)                                     ** 
** ----------------------------------------------------------------- */

static void get_printed_string_size(pString,Orient,width,height)
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
  c = (pString[i]=='.'?10:(pString[i]=='-'?11:pString[i]-'0'));
  if (Orient==REGLE_HORIZONTAL)
  {
   w += Regle_Font[c].width+1; 
   h = MAX(h, Regle_Font[c].height);
  }
  else
  {
   w = MAX(w, Regle_Font[c].height);
   h += Regle_Font[c].width+1;
  }
 }
 *width = w;
 *height = h;
}

static void print_string_Regle(This, pString, xorig, yorig)
c_Regle	*This;
char	*pString;
int	xorig;
int	yorig;
{
 int		i,c;
 int		nb_point;
  
 nb_point = 0;
 for(i = 0; i < (int)strlen(pString); i++)
 {
  c = (pString[i]=='.' ? 10 : (pString[i] == '-' ? 11 : pString[i] - '0'));
  if (This->Orient==REGLE_HORIZONTAL)
  {
   memcpy(points + nb_point,Regle_CharH[c].xpoint,
	  Regle_CharH[c].nb_point * sizeof(XPoint));
   points[nb_point].x += xorig;
   points[nb_point].y += yorig + Regle_Font[c].offset_y;
   xorig= Regle_CharH[c].incx;
   yorig= Regle_CharH[c].incy-Regle_Font[c].offset_y;
   nb_point += Regle_CharH[c].nb_point;
  }
  else
  {
   memcpy(points + nb_point,Regle_CharV[c].xpoint,
	  Regle_CharV[c].nb_point*sizeof(XPoint));
   points[nb_point].x += xorig + Regle_Font[c].offset_y;
   points[nb_point].y += yorig;
   xorig= Regle_CharV[c].incx-Regle_Font[c].offset_y;
   yorig= Regle_CharV[c].incy;
   nb_point += Regle_CharV[c].nb_point;
  }
 }
 XDrawPoints(This->display, This->window, This->gc ,points, nb_point,
	     CoordModePrevious);
}

/* ----------------------------------------------------------------- ** 
** CALCUL GRADUATION FUNCTONS(2)                                     ** 
** ----------------------------------------------------------------- */

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

static void calcul_graduation(This,scale)
c_Regle	*This;
coord_t	scale;
{
 float	length_in_unit;
 real	division;
 int	grad;
  
 if (This->Scale==scale)
  return;
  
 This->Scale=scale;
 length_in_unit=convert_in_unit(scale*This->Size,This->Unit);
 grad=0;
 while((grad<NB_GRADUATE_FACTOR) &&
       (This->Size*graduate_factor[grad].factor/
	length_in_unit)<REGLE_MIN_GRAD)
  grad++;
 This->Grad=grad;
 division=This->Size*graduate_factor[grad].factor/length_in_unit;
 This->Division=division;
 if (graduate_factor[grad].subdivision)
 {
  division /= graduate_factor[grad].subdivision;
  if (division>2)
   This->SubDivision= division;
  else
   This->SubDivision=This->SubSubDivision=This->SubSubSubDivision=0;
 }
 else
  This->SubDivision=0;
 if (graduate_factor[grad].subsubdivision)
 {
  division /= graduate_factor[grad].subsubdivision;
  if (division>2)
   This->SubSubDivision= division;
  else
   This->SubSubDivision=This->SubSubSubDivision=0;
 }
 else
  This->SubSubDivision=0;
 if (graduate_factor[grad].subsubsubdivision)
 {
  division /= graduate_factor[grad].subsubsubdivision;
  if (division>2)
   This->SubSubSubDivision= division;
  else
   This->SubSubSubDivision=0;
 }
 else
  This->SubSubSubDivision=0;
}

/* ----------------------------------------------------------------- ** 
** DRAW GRADUATION                                                   ** 
** ----------------------------------------------------------------- */

static void draw_graduation(This,offset,size)
c_Regle	*This;
int	offset;
int	size;
{
 int	i, grad, xseg;
 real	pos,tmp,length,value;
 coord_t  start,end;
 coord_t  starti,endi;
 coord_t  orig_regle;
  
 if (This->Orient == REGLE_HORIZONTAL)
 {
  orig_regle=This->Page->PosX-REGLE_SIZE-This->Page->Orig_X;
 }
 else
 {
  orig_regle=This->Page->PosY-REGLE_SIZE-This->Page->Orig_Y;
 }
 start = (offset-orig_regle)*This->Scale;
 end = (offset+size-orig_regle)*This->Scale;
  
 grad=This->Grad;
 xseg=0;
 for (i=0 ; i< This->nb_intervs; i++)
 {
  if ((end >  This->intervs[i].min) && (start<This->intervs[i].max))
  {
   starti=MAX(This->intervs[i].min,start);
   endi=MIN(This->intervs[i].max,end);
   if (starti==This->intervs[i].min)
   {
    graduation[xseg].offset=starti/This->Scale+orig_regle;
    graduation[xseg++].length=REGLE_SIZE;
    graduation[xseg].offset=starti/This->Scale+orig_regle+1;
    graduation[xseg++].length=REGLE_SIZE;
   }
   if (endi==This->intervs[i].max)
   {
    graduation[xseg].offset=endi/This->Scale+orig_regle;
    graduation[xseg++].length=REGLE_SIZE;
    graduation[xseg].offset=endi/This->Scale+orig_regle+1;
    graduation[xseg++].length=REGLE_SIZE;
   }
   tmp=(real)(starti/This->Scale+orig_regle);
   pos=(real)(This->intervs[i].origine/This->Scale+orig_regle);
   length=(real)((endi-This->intervs[i].origine)/This->Scale);
   value=0.0;
   if (length>0)
   {
    while(pos<tmp-This->Division)
    {
     pos+=This->Division;
     length-=This->Division;
     value+=graduate_factor[grad].factor;
    }
    draw_graduation_interval_positive(This,pos,length,value,grad,&xseg);
   }
   tmp=(real)(endi/This->Scale+orig_regle);
   length=(starti-This->intervs[i].origine)/This->Scale;
   value=0.0;
   if (length<0)
   {
    while(pos>tmp+This->Division)
    {
     pos-=This->Division;
     length+=This->Division;
     value-=graduate_factor[grad].factor;
    }	
    draw_graduation_interval_negative(This,pos,length,value,grad,&xseg);
   }
  }
 }
  
 /*  grad=This->Grad;
     draw_graduation_interval(This,pos,size,-20.0,grad,&xseg); */
  
 if (This->Orient==REGLE_HORIZONTAL)
 {
  for(i=0; i<xseg; i++)
  {
   segments[i].x1=segments[i].x2=graduation[i].offset+offset;
   segments[i].y1=REGLE_SIZE-graduation[i].length-1;
   segments[i].y2=REGLE_SIZE-1;
  }
 }
 else
 {
  for(i=0; i<xseg; i++)
  {
   segments[i].x1=REGLE_SIZE-graduation[i].length-1;
   segments[i].x2=REGLE_SIZE-1;
   segments[i].y1=segments[i].y2=graduation[i].offset+offset;
  }
 }
 XDrawSegments(This->display, This->window, This->gc, segments, xseg);
}

static void draw_graduation_interval_positive(This,offset,length,value,
					      grad,xseg)
c_Regle	*This;
real 	offset;
real	length;
real	value;
int	grad;
int	*xseg;
{
 int	si,ssi,sssi;
 real	pos,spos,sspos,ssspos;
 char	svalue[10];
 int	width,height;
  
 pos = 0.0;
 while(pos<length)
 {
  spos=pos;
  for (si=0; si<graduate_factor[grad].subdivision; si++)
  {
   sspos=pos;
   for(ssi=0; ssi<graduate_factor[grad].subsubdivision; ssi++)
   {
    ssspos=pos;
    for(sssi=1; sssi<graduate_factor[grad].subsubsubdivision; sssi++)
    {
     pos += This->SubSubSubDivision;
     if (pos > length)  return;
     if (pos+offset>0)
     {
      graduation[*xseg].offset=(int)(pos+offset);
      graduation[(*xseg)++].length=GRAD_SUBSUBSUBDIVISION;
     }
    }
    if (ssi<graduate_factor[grad].subsubdivision-1)
    {
     pos = ssspos + This->SubSubDivision;
     if (pos > length) return;
     if (pos+offset>0)
     {
      graduation[*xseg].offset=(int)(pos+offset);
      graduation[(*xseg)++].length=GRAD_SUBSUBDIVISION;
     }
    }
   }
   if (si<graduate_factor[grad].subdivision-1)
   {
    pos = sspos + This->SubDivision;
    if (pos > length)	return;
    if (pos+offset>0)
    {
     graduation[*xseg].offset=(int)(pos+offset);
     graduation[(*xseg)++].length=GRAD_SUBDIVISION;
    }
   }
  }
  pos = spos + This->Division;
  if (pos > length)	return;
  if (pos+offset>0)
  {
   graduation[*xseg].offset=(int)(pos+offset);
   graduation[(*xseg)++].length=GRAD_DIVISION;
  }
  value += graduate_factor[grad].factor;
  if (value-(int)value)
   sprintf(svalue,"%.1f",value);
  else
   sprintf(svalue,"%d",(int)value);
  get_printed_string_size(svalue,This->Orient,&width,&height);
  if (This->Orient==REGLE_HORIZONTAL)
   print_string_Regle(This,svalue,(int)(pos+offset-width-2),2);
  else
   print_string_Regle(This,svalue,2,(int)(pos+offset-3));
 } 
}

static void draw_graduation_interval_negative(This,offset,length,value,
					      grad,xseg)
c_Regle	*This;
real 	offset;
real	length;
real	value;
int	grad;
int	*xseg;
{
 int	si,ssi,sssi;
 real	pos,spos,sspos,ssspos;
 char	svalue[10];
 int	width,height;
  
 pos = 0.0;
 while(pos>length)
 {
  if (pos < length)	return;
  graduation[*xseg].offset=(int)(pos+offset);
  graduation[(*xseg)++].length=GRAD_DIVISION;
  if (value-(int)value)
   sprintf(svalue,"%.1f",value);
  else
   sprintf(svalue,"%d",(int)value);
  get_printed_string_size(svalue,This->Orient,&width,&height);
      
  if (This->Orient==REGLE_HORIZONTAL)
  {
   if (pos-width-2>length)
    print_string_Regle(This,svalue,(int)(pos+offset-width-2),2);
  }
  else
  {
   if (pos-height-3>length)
    print_string_Regle(This,svalue,2,(int)(pos+offset-3));
  }
  spos=pos;
  for (si=0; si<graduate_factor[grad].subdivision; si++)
  {
   sspos=pos;
   for(ssi=0; ssi<graduate_factor[grad].subsubdivision; ssi++)
   {
    ssspos=pos;
    for(sssi=1; sssi<graduate_factor[grad].subsubsubdivision; sssi++)
    {
     pos -= This->SubSubSubDivision;
     if (pos < length)  return;
     graduation[*xseg].offset=(int)(pos+offset);
     graduation[(*xseg)++].length=GRAD_SUBSUBSUBDIVISION;
    }
    if (ssi<graduate_factor[grad].subsubdivision-1)
    {
     pos = ssspos - This->SubSubDivision;
     if (pos < length) return;
     graduation[*xseg].offset=(int)(pos+offset);
     graduation[(*xseg)++].length=GRAD_SUBSUBDIVISION;
    }
   }
   if (si<graduate_factor[grad].subdivision-1)
   {
    pos = sspos - This->SubDivision;
    if (pos < length)	return;
    graduation[*xseg].offset=(int)(pos+offset);
    graduation[(*xseg)++].length=GRAD_SUBDIVISION;
   }
  }
  pos = spos - This->Division;
  value -= graduate_factor[grad].factor;
 } 
}
