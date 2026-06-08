/*
** Classor.c for XQuad in Classor/
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
** Started on  Mon Jul 17 20:30:58 1995 Emmanuel Paris
** Last update Fri Sep 29 10:59:37 1995 Emmanuel Paris
*/

#define _PRIVATE_CLASSOR_
#include "Classor.h"
#include "Xpm.h"
#include "ContextMenu.h"
#include "Resource.h"
extern c_Resource *GlobResources;

static void *cons_Classor();
static void dest_Classor();
static void *copy_Classor();
void Init_Classor();
static void resize_Classor();
static void hilight_selected_icon();
static void draw_Classor();
static void draw_Classor_icons();
static boolean scroll_Classor_left();
static boolean scroll_Classor_right();
static void draw_sheet_number();
static void eventlook_Classor();
static void configure_Classor();
static int calcul_nb_sheet_visible();
static int calcul_selected_sheet_pos();
static int get_icon_number();
static listsheet *get_sheet_by_xy();
static void select_new_sheet();
static listsheet *get_first_sheet();
static listsheet *get_previous_sheet();
static listsheet *get_next_sheet();
static listsheet *get_last_sheet();
static void get_last_sheet_with_first();
static void get_first_sheet_with_last();
static boolean repeat_time_function();

sf_Classor fc_Classor =
{
  cons_Classor,
  dest_Classor,
  copy_Classor,
  resize_Classor,
  draw_Classor,
  eventlook_Classor
};

static void *cons_Classor(display, window, PosX, PosY, Size, tableau)
Display	*display;
Window	window;
int	PosX;
int	PosY;
int	Size;
c_Tableau	*tableau;
{
  c_Classor	*ObjTmp;
  XGCValues	gcv;
  
/*Xc_TRACE(("Create Classor"));*/
  if ((ObjTmp = (c_Classor *)Xc_malloc("Classor", sizeof (c_Classor))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Classor;
  ObjTmp->display = display;
  ObjTmp->Size = Size;
  
  ObjTmp->Sheet_Begin_pos = 1;
  ObjTmp->Sheet_Previous_pos = ObjTmp->Sheet_Begin_pos+Sheet_Begin_width + 2;
  ObjTmp->Sheet_Number_pos = ObjTmp->Sheet_Previous_pos+Sheet_Previous_width+2;
  ObjTmp->Sheet_Next_pos = ObjTmp->Sheet_Number_pos+SHEET_NUMBER_SPACE;
  ObjTmp->Sheet_End_pos = ObjTmp->Sheet_Next_pos+Sheet_Next_width+2;
  ObjTmp->Classor_pos = ObjTmp->Sheet_End_pos+Sheet_End_width+2;
  /*--- fill classor info ---*/
  ObjTmp->tableau	= tableau;
  ObjTmp->nb_sheet	= tableau->NbrSheet;
  ObjTmp->list_sheet	= tableau->ListSheet;
  ObjTmp->selected_sheet = tableau->SelectedSheet;
  ObjTmp->sheet_number_display =  tableau->SelectedSheet;
  ObjTmp->first_sheet_pos = 0;
  ObjTmp->first_sheet_visible = tableau->ListSheet;
  get_last_sheet_with_first(ObjTmp);
  ObjTmp->selected_sheet_pos = calcul_selected_sheet_pos(ObjTmp);

  /*--- Window creation ---*/
  ObjTmp->window = 
    XCreateSimpleWindow(display, window, 
			PosX,  PosY,
			Size, CLASSOR_SIZE, 
			0, 
			BlackPixel(display,DefaultScreen(display)),
			Classor_Norm_Color);
  XSelectInput(display, ObjTmp->window, PointerMotionMask | ButtonPressMask |
	       ButtonReleaseMask | ExposureMask | LeaveWindowMask);
  gcv.function = GXcopy;
  gcv.background = Classor_Norm_Color;
  gcv.foreground = BlackPixel(display,DefaultScreen(display));
  ObjTmp->gc = XCreateGC(display, ObjTmp->window,
			 GCFunction | GCForeground | GCBackground,
			 &gcv);
  ObjTmp->font_struct = 
    F(GlobResources).getXFontStruct(GlobResources, XcR_classorFont);
  XSetFont(display, ObjTmp->gc,
	   F(GlobResources).getFont(GlobResources, XcR_classorFont));
  
  XMapWindow(display, ObjTmp->window);
  Xc_TRACE(("Object Classor build"));
  return ObjTmp;
}

static void dest_Classor(This)
c_Classor *This;
{ 
  XFreeGC(This->display,This->gc);
  Xc_free(This);
  Xc_TRACE(("Object Classor destroyed"));
}

static void *copy_Classor(This)
c_Classor *This;
{
  c_Classor *ObjTmp;

  if ((ObjTmp = (c_Classor *)Xc_malloc("CClassor", sizeof (c_Classor))) == NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp, This, sizeof(c_Classor));
  Xc_TRACE(("Object Classor copied"));
  return ObjTmp;
}

void Init_Classor(display)
Display	*display;
{
  XColor	realcolor;
  Colormap 	c_map;
  int		width;
  int		height;
  int 		color;
  int		bit;
  int		x_hot;
  int		y_hot;
  int		i;
  Pixmap        psource;
  XImage	*xsource;
  XImage	*xmask;
  char		**image;
  char		*name, *ptr;
  GC		gc;
  XFontStruct	*font_struct;
  
  /*--- Color Allocation ---*/
  c_map = DefaultColormap(display, DefaultScreen(display));
  XParseColor(display, c_map, CLASSOR_NORM, &realcolor);
  if (!XAllocColor(display, c_map, &realcolor))
    {
      Xc_WARNING(("can't allocate NORM color for Classor"));
      Classor_Norm_Color = WhitePixel(display,DefaultScreen(display));
    }
  else
    Classor_Norm_Color = realcolor.pixel;
  XParseColor(display, c_map, CLASSOR_SHAD, &realcolor);
  if (!XAllocColor(display, c_map, &realcolor))
    {
      Xc_WARNING(("can't allocate SHAD color for Classor"));
      Classor_Shad_Color = BlackPixel(display,DefaultScreen(display));
    }
  else
    Classor_Shad_Color = realcolor.pixel;

  if (DefaultDepth(display, DefaultScreen(display))==1)
      Classor_High_Color = BlackPixel(display,DefaultScreen(display));
  else
    {
      XParseColor(display, c_map, CLASSOR_HIGH, &realcolor);
      if (!XAllocColor(display, c_map, &realcolor))
	{
	  Xc_WARNING(("can't allocate HIGH color for Classor"));
	  Classor_High_Color = BlackPixel(display,DefaultScreen(display));
	}
      else
	Classor_High_Color = realcolor.pixel;
    }
  
  ptr = F(GlobResources).getString(GlobResources, XcR_defaultSheetName);
  name = Xc_malloc("name", strlen(ptr) + 10);
  sprintf(name, "%s: 888-888", ptr);
  font_struct = 
    F(GlobResources).getXFontStruct(GlobResources, XcR_classorFont);
  page_number_size = XTextWidth(font_struct, name, strlen(name)) + 4;
  Xc_free(name);
  
  for(i=0; i<NB_PIXMAP_CLASSOR; i++)
    {
      image=(char **)Tab_Pixmap_Classor[i].Image;
      XPMGetAttrib(image, &width, &height, &color,
		   &bit, &x_hot, &y_hot);
      *(Tab_Pixmap_Classor[i].width)=width;
      *(Tab_Pixmap_Classor[i].height)=height;
      XPMload(display,image,&xsource,&xmask,FALSE);
      psource=XCreatePixmap(display, DefaultRootWindow(display),
			    width,height,
			    DefaultDepth(display, DefaultScreen(display)));
      gc=XCreateGC(display,psource,0,0);
      XPutImage(display,psource,gc,xsource,0,0,0,0,width,height);
      Xc_free(xsource->data);
      XFree((char*)xsource);
      if (xmask && xmask->data)
	Xc_free(xmask->data);
      if (xmask)
	XFree((char *)xmask);
      XFreeGC(display,gc);
      *(Tab_Pixmap_Classor[i].Pixmap)=psource;
    }
}

static void resize_Classor(This, PosX, PosY, Size) 
c_Classor	*This;
int	PosX;
int	PosY;
int	Size;
{
  listsheet *list_sheet;
  int	   width;

  This->Size=Size;

  list_sheet =  This->first_sheet_visible;
  width = Size-This->Classor_pos-1;
  while(list_sheet && width<This->Size)
    {
      width += Classor_Icons[get_icon_number(list_sheet)].width
	+2*ICON_SEPARATOR;
      list_sheet = list_sheet->NextSheet;
    }
  if (width >= This->Size)
    get_last_sheet_with_first(This);
  else
    {
      This->nb_sheet_visible = calcul_nb_sheet_visible(This,FALSE);
      if (This->nb_sheet_visible<This->nb_sheet)
	{
	  This->last_sheet_visible = get_last_sheet(This);
	  This->last_sheet_pos = 0;
	  get_first_sheet_with_last(This);
	}
      else
	{
	  This->first_sheet_visible = get_first_sheet(This);
	  This->first_sheet_pos = 0;
	  get_last_sheet_with_first(This);
	}
    }
  XMoveResizeWindow(This->display, This->window,
		    PosX, PosY,
		    Size, CLASSOR_SIZE);
}


static void draw_Classor(This)
c_Classor *This;
{
  XSegment	xseg[7];
  
  draw_Classor_icons(This,FALSE);
  hilight_selected_icon(This);
  draw_sheet_number(This, NULL);
  
  XCopyArea(This->display, Sheet_Begin, This->window, This->gc,
	    0, 0, Sheet_Begin_width, Sheet_Begin_height,
	    This->Sheet_Begin_pos, 1);
  XCopyArea(This->display, Sheet_Previous, This->window, This->gc,
	    0, 0, Sheet_Previous_width, Sheet_Previous_height,
	    This->Sheet_Previous_pos, 1);
  XCopyArea(This->display, Sheet_Next, This->window, This->gc,
		0, 0, Sheet_Next_width, Sheet_Next_height,
	    This->Sheet_Next_pos, 1);
  XCopyArea(This->display, Sheet_End, This->window, This->gc,
	    0, 0, Sheet_End_width, Sheet_End_height,
		This->Sheet_End_pos, 1);

  xseg[0].y1=xseg[1].y1=xseg[2].y1=xseg[3].y1=
    xseg[4].y1=xseg[5].y1=xseg[6].y1=0;
  xseg[0].y2=xseg[1].y2=xseg[2].y2=xseg[3].y2=
    xseg[4].y2=xseg[5].y2=xseg[6].y2=CLASSOR_SIZE;
  
  XSetForeground(This->display, This->gc, Classor_High_Color);
  xseg[0].x1=xseg[0].x2=This->Sheet_Begin_pos - 1;
  xseg[1].x1=xseg[1].x2=This->Sheet_Previous_pos - 1;
  xseg[2].x1=xseg[2].x2=This->Sheet_Number_pos - 1;
  xseg[3].x1=xseg[3].x2=This->Sheet_Next_pos - 1;
  xseg[4].x1=xseg[4].x2=This->Sheet_End_pos - 1;
  xseg[5].x1=xseg[5].x2=This->Classor_pos - 1 ;  
  xseg[6].x1=0; xseg[6].x2=This->Size; 
  xseg[6].y1=xseg[6].y2=0;
  XDrawSegments(This->display, This->window, This->gc, xseg, 7);

  XSetForeground(This->display, This->gc, Classor_Shad_Color);
  xseg[0].x1=xseg[0].x2=This->Sheet_Previous_pos - 2;
  xseg[1].x1=xseg[1].x2=This->Sheet_Number_pos - 2;
  xseg[2].x1=xseg[2].x2=This->Sheet_Next_pos - 2 ;
  xseg[3].x1=xseg[3].x2=This->Sheet_End_pos - 2;   
  xseg[4].x1=xseg[4].x2=This->Classor_pos - 2;
  xseg[5].x1=xseg[5].x2=This->Size - 1;
  xseg[6].x1=0; xseg[6].x2=This->Size;
  xseg[6].y1=xseg[6].y2=CLASSOR_SIZE-1;
  XDrawSegments(This->display, This->window, This->gc, xseg, 7);
  
  XSetForeground(This->display, This->gc,
		 BlackPixel(This->display,DefaultScreen(This->display)));
}

static void hilight_selected_icon(This)
c_Classor	*This;
{
  int	width;
  int	pos;

  This->selected_sheet_pos = calcul_selected_sheet_pos(This);
  
  if (This->selected_sheet_pos != -1)
    {
      width = Classor_Icons[ get_icon_number(This->selected_sheet)].width;
      if ( This->selected_sheet_pos <= This->Classor_pos)
	{
	  pos =  This->Classor_pos - This->selected_sheet_pos;
	  
	  if (pos > ICON_SEPARATOR)
	    {
	      if (pos > width + ICON_SEPARATOR)
		return;
	      width -= pos - ICON_SEPARATOR;
	      pos = This->Classor_pos;
	    }
	  else
	    pos = This->Classor_pos + MAX(0,ICON_SEPARATOR-pos);
	}
      else
	if ( This->selected_sheet_pos + width > This->Size-1)
	  {
	    width = This->Size-1 - This->selected_sheet_pos - ICON_SEPARATOR;
	    if (width <=0)
	      return;
	    pos =  This->selected_sheet_pos + ICON_SEPARATOR;
	  }
	else
	  pos =  This->selected_sheet_pos + ICON_SEPARATOR;
      
      if (GlobColormap->Cblack)
	XSetFunction(This->display, This->gc, GXxor);
      else
	XSetFunction(This->display, This->gc, GXinvert);
      
      XFillRectangle(This->display, This->window, This->gc, 
		     pos, 1, width, CLASSOR_SIZE - 2);
      XSetFunction(This->display, This->gc, GXcopy);
    } 
}

static void draw_Classor_icons(This,erase)
c_Classor	*This;
boolean erase;
{
  int		pos_orig;
  int		pos_cumul;
  int		sheet_count;
  int		icon_num;
  listsheet	*list_sheet;

  if (erase)
    {
      XSetForeground(This->display, This->gc, Classor_Norm_Color);
      XFillRectangle(This->display, This->window, This->gc, 
		     This->Classor_pos, 1,
		     This->Size-1-This->Classor_pos, CLASSOR_SIZE - 2);
      XSetForeground(This->display, This->gc,
		     BlackPixel(This->display,DefaultScreen(This->display)));
    }
  list_sheet = This->first_sheet_visible;
  sheet_count = This->nb_sheet_visible;
  if (!sheet_count) return;
  pos_orig = This->first_sheet_pos;

  icon_num = get_icon_number(list_sheet);
  if (pos_orig < Classor_Icons[icon_num].width+ICON_SEPARATOR)
    {
      XCopyArea(This->display, Classor_Icons[icon_num].Icon,
		This->window, This->gc,
		MAX( 0 ,pos_orig-ICON_SEPARATOR), 0,
		Classor_Icons[icon_num].width-MAX(0,pos_orig-ICON_SEPARATOR),
		Classor_Icons[icon_num].height,
		This->Classor_pos + MAX(0 ,ICON_SEPARATOR-pos_orig), 1);
    }      
  list_sheet=list_sheet->NextSheet;
  
  pos_cumul = This->Classor_pos + 
    Classor_Icons[icon_num].width+2*ICON_SEPARATOR - pos_orig; 
  
  if (This->nb_sheet_visible<This->nb_sheet)
    sheet_count--;
  if (sheet_count)
    {
      while(sheet_count-1)
	{
	  icon_num = get_icon_number(list_sheet);
	  XCopyArea(This->display, Classor_Icons[icon_num].Icon,
		    This->window, This->gc, 0, 0,
		    Classor_Icons[icon_num].width,
		    Classor_Icons[icon_num].height,
		    pos_cumul + ICON_SEPARATOR, 1);
	  pos_cumul += Classor_Icons[icon_num].width+2*ICON_SEPARATOR; 
	  sheet_count--;
	  list_sheet=list_sheet->NextSheet;
	}
      if (This->nb_sheet_visible<This->nb_sheet)
	{
	  icon_num = get_icon_number(list_sheet);
/*Xc_TRACE(("icon width: %d  pos_cimul: %d  Size: %d",*/
/*Classor_Icons[icon_num].width,This->Size,pos_cumul));*/
	  
	  XCopyArea(This->display, Classor_Icons[icon_num].Icon,
		    This->window, This->gc, 0, 0,
		    MIN(Classor_Icons[icon_num].width,This->Size-3-pos_cumul),
		    Classor_Icons[icon_num].height,
		    pos_cumul + ICON_SEPARATOR, 1);
	}
    }
}

static boolean scroll_Classor_left(This)
c_Classor	*This;
{
  listsheet	*list_sheet;
  int		icon_num;
  int		scroll_pos;
  
  if (This->nb_sheet<=This->nb_sheet_visible || !This->last_sheet_visible)
    return TRUE;
  
  if (This->last_sheet_pos <
      Classor_Icons[get_icon_number(This->last_sheet_visible)].width+
      2*ICON_SEPARATOR-1)
    This->last_sheet_pos++;
  else
    {
      list_sheet = get_next_sheet(This, This->last_sheet_visible);
      if (!list_sheet)
	return TRUE;
      This->last_sheet_visible=list_sheet;
      This->last_sheet_pos=0;
    }

  if (This->first_sheet_pos <
      Classor_Icons[get_icon_number(This->first_sheet_visible)].width+
      2*ICON_SEPARATOR-1)
    This->first_sheet_pos++;
  else
    {
      list_sheet = get_next_sheet(This, This->first_sheet_visible);
      if (!list_sheet)
	return TRUE;
      This->first_sheet_visible=list_sheet;
      This->first_sheet_pos=0;
      draw_sheet_number(This, This->first_sheet_visible);
    }
  This->nb_sheet_visible = calcul_nb_sheet_visible(This,TRUE);

  XCopyArea(This->display, This->window, This->window, This->gc,
	    This->Classor_pos+1, 1, This->Size-This->Classor_pos-2, 
	    CLASSOR_SIZE-2, This->Classor_pos, 1); 

  icon_num = get_icon_number(This->last_sheet_visible);
  if (This->last_sheet_pos >= ICON_SEPARATOR && 
      This->last_sheet_pos < Classor_Icons[icon_num].width+ICON_SEPARATOR) 
    {
      scroll_pos = This->Size-2;

      XCopyArea(This->display, Classor_Icons[icon_num].Icon,
		This->window, This->gc, 
		This->last_sheet_pos-ICON_SEPARATOR, 0, 1,
		Classor_Icons[icon_num].height,
		scroll_pos, 1);

      if (This->selected_sheet == This->last_sheet_visible)
	{
	  This->selected_sheet_pos = calcul_selected_sheet_pos(This);
	  if (scroll_pos>=This->selected_sheet_pos + ICON_SEPARATOR && 
	      scroll_pos< This->selected_sheet_pos + 
	  Classor_Icons[icon_num].width+ICON_SEPARATOR)
	    {
	      if (GlobColormap->Cblack)
		XSetFunction(This->display, This->gc, GXxor);
	      else
		XSetFunction(This->display, This->gc, GXinvert);
	      
	      XDrawLine(This->display, This->window, This->gc, 
			scroll_pos, 1, scroll_pos, CLASSOR_SIZE - 2);
	      XSetFunction(This->display, This->gc, GXcopy);
	    } 
	}
    }
  else
    {
      XSetForeground(This->display, This->gc, Classor_Norm_Color);
      XDrawLine(This->display, This->window, This->gc, This->Size-2,
		1, This->Size-2, CLASSOR_SIZE-2);
      XSetForeground(This->display, This->gc,
		     BlackPixel(This->display,DefaultScreen(This->display)));
    }
  return FALSE;
}

static boolean scroll_Classor_right(This)
c_Classor	*This;
{
  listsheet	*list_sheet;
  int		icon_num;
  int		scroll_pos;

  if (This->nb_sheet<This->nb_sheet_visible)
    return TRUE;

  if (This->first_sheet_pos)
      This->first_sheet_pos--;
  else
    {
      list_sheet= get_previous_sheet(This, This->first_sheet_visible);
      if (!list_sheet) 
	return TRUE;
      This->first_sheet_visible=list_sheet;
      This->first_sheet_pos=Classor_Icons
	[get_icon_number(list_sheet)].width+2*ICON_SEPARATOR-1;
      draw_sheet_number(This, This->first_sheet_visible);
    }
  
  if (This->last_sheet_pos) 
    This->last_sheet_pos--;
  else
    {
      list_sheet = get_previous_sheet(This, This->last_sheet_visible);
      if (!list_sheet)
	return TRUE;
      This->last_sheet_visible=list_sheet;
      This->last_sheet_pos=Classor_Icons
	[get_icon_number(list_sheet)].width+2*ICON_SEPARATOR-1;
    }
  This->nb_sheet_visible = calcul_nb_sheet_visible(This,TRUE);
  
  XCopyArea(This->display, This->window, This->window, This->gc,
	    This->Classor_pos, 1, This->Size-This->Classor_pos-2, 
	    CLASSOR_SIZE-2, This->Classor_pos+1, 1); 
  
  icon_num = get_icon_number(This->first_sheet_visible);
  if (This->first_sheet_pos>=ICON_SEPARATOR && 
      This->first_sheet_pos<Classor_Icons[icon_num].width+ICON_SEPARATOR) 
    {
      scroll_pos = This->Classor_pos;
      XCopyArea(This->display, Classor_Icons[icon_num].Icon,
		This->window, This->gc, 
		This->first_sheet_pos-ICON_SEPARATOR , 0, 1,
		Classor_Icons[icon_num].height,
		scroll_pos, 1);
      if (This->selected_sheet == This->first_sheet_visible)
	{
	  This->selected_sheet_pos = calcul_selected_sheet_pos(This);
	  if (scroll_pos>=This->selected_sheet_pos + ICON_SEPARATOR && 
	      scroll_pos< This->selected_sheet_pos + 
	      Classor_Icons[icon_num].width+ICON_SEPARATOR)
	    {
	      if (GlobColormap->Cblack)
		XSetFunction(This->display, This->gc, GXxor);
	      else
		XSetFunction(This->display, This->gc, GXinvert);
	      
	      XDrawLine(This->display, This->window, This->gc, 
			scroll_pos, 1, scroll_pos, CLASSOR_SIZE - 2);
	      XSetFunction(This->display, This->gc, GXcopy);
	    }
	}
    }
  else
    {
      XSetForeground(This->display, This->gc, Classor_Norm_Color);
      XDrawLine(This->display, This->window, This->gc, This->Classor_pos,
		1, This->Classor_pos, CLASSOR_SIZE-2);
      XSetForeground(This->display, This->gc,
		     BlackPixel(This->display,DefaultScreen(This->display)));
    }
  return FALSE;
}

static void eventlook_Classor(This, display, Event)
c_Classor		*This;
Display		*display;
XEvent		*Event;
{
  int		xm,ym;
  XRectangle    rect;
  listsheet	*list_sheet;
  listsheet	*svg_list_sheet;

  if (Event->xany.window == This->window)
    {
      switch(Event->type)
	{
	case  Expose:
	case GraphicsExpose:
	  rect.x=Event->xexpose.x;
	  rect.y=Event->xexpose.y;
	  rect.width=Event->xexpose.width;
	  rect.height=Event->xexpose.height;
	  XSetClipRectangles(This->display,This->gc,0,0,&rect,1,Unsorted);
	  XSetGraphicsExposures(This->display, This->gc, False);
	  F(This).Draw(This);
	  XSetGraphicsExposures(This->display, This->gc, True);
	  XSetClipMask(This->display,This->gc,None);
	  break;
	case LeaveNotify:
	  if (This->selected_sheet != This->sheet_number_display)
	    {
	      draw_sheet_number(This, This->selected_sheet);
	      This->sheet_number_display = This->selected_sheet;
	    }
	  break;
	case MotionNotify:
	  xm=Event->xmotion.x;
	  ym=Event->xmotion.y;
	  list_sheet = get_sheet_by_xy(This, xm, ym);
	  if (list_sheet)
	    {
	      if (list_sheet != This->sheet_number_display)
		{
		  draw_sheet_number(This, list_sheet);
		  This->sheet_number_display = list_sheet;
		}
	    }
	  else
	    if (This->selected_sheet != This->sheet_number_display)
	      {
		draw_sheet_number(This, This->selected_sheet);
		This->sheet_number_display = This->selected_sheet;
	      }
	  break;
	case ButtonPress:
	  xm = Event->xbutton.x;
	  ym = Event->xbutton.y;
	  if (xm>=This->Sheet_Begin_pos &&
	      xm<=This->Sheet_Begin_pos + Sheet_Begin_width)
	    {
/*Xc_TRACE(("button Sheet begin"));*/
	      list_sheet = get_first_sheet(This);
	      This->first_sheet_visible = get_first_sheet(This);
	      This->first_sheet_pos = 0;
	      get_last_sheet_with_first(This);
	      draw_Classor_icons(This,TRUE);
	      hilight_selected_icon(This);
	      draw_sheet_number(This, NULL);
	      if (list_sheet != This->selected_sheet)
		{
		  This->selected_sheet_pos = calcul_selected_sheet_pos(This);
		  select_new_sheet(This, list_sheet);
		}
	    }
	  if (xm>=This->Sheet_Previous_pos &&
	      xm<=This->Sheet_Previous_pos + Sheet_Previous_width)
	    {
/*Xc_TRACE(("button Sheet previous"));*/
	      if (repeat_time_function(This, scroll_Classor_right))
		{
		  list_sheet = get_previous_sheet(This,NULL);
		  if (list_sheet && list_sheet != This->selected_sheet)
		    {
		      svg_list_sheet = This->selected_sheet;
		      This->selected_sheet = list_sheet;
		      if (calcul_selected_sheet_pos(This)==-1)
			{
			  This->selected_sheet = svg_list_sheet;
			  This->first_sheet_visible = list_sheet;
			  This->first_sheet_pos = 0;
			  get_last_sheet_with_first(This);
			  draw_Classor_icons(This,TRUE);
			  hilight_selected_icon(This);
			}
		      This->selected_sheet = svg_list_sheet;
		      select_new_sheet(This, list_sheet);
		    }
		}
	      draw_sheet_number(This, This->selected_sheet);
	    }
	  if (xm>=This->Sheet_Next_pos &&
	      xm<=This->Sheet_Next_pos + Sheet_Next_width)
	    {
/*Xc_TRACE(("button Sheet Next"));*/
	      if (repeat_time_function(This, scroll_Classor_left))
		{
		  list_sheet = get_next_sheet(This,NULL);
		  if (list_sheet && list_sheet != This->selected_sheet)
		    {
		      svg_list_sheet = This->selected_sheet;
		      This->selected_sheet = list_sheet;
		      if (calcul_selected_sheet_pos(This)==-1)
			{
			  This->selected_sheet = svg_list_sheet;
			  This->last_sheet_visible = list_sheet;
			  This->last_sheet_pos = 0;
			  get_first_sheet_with_last(This);
			  if (!This->first_sheet_visible)
			    {
			      This->first_sheet_visible = list_sheet;
			      This->first_sheet_pos = 0;
			      get_last_sheet_with_first(This);
			    } 
			  draw_Classor_icons(This,TRUE);
			  hilight_selected_icon(This);
			}
		      This->selected_sheet = svg_list_sheet;
		      select_new_sheet(This, list_sheet);
		    }
		}
	      draw_sheet_number(This, This->selected_sheet);
	    }
	  if (xm>=This->Sheet_End_pos &&
	      xm<=This->Sheet_End_pos + Sheet_End_width)
	    {
/*Xc_TRACE(("button Sheet End"));*/
	      list_sheet = get_last_sheet(This);
	      This->last_sheet_visible = get_last_sheet(This);
	      This->last_sheet_pos = 0;
	      get_first_sheet_with_last(This);
	      if (!This->first_sheet_visible)
		{
		  This->first_sheet_visible = get_first_sheet(This);
		  This->first_sheet_pos = 0;
		  get_last_sheet_with_first(This);
		}
	      draw_Classor_icons(This,TRUE);
	      hilight_selected_icon(This);
	      draw_sheet_number(This, NULL);
	      if (list_sheet != This->selected_sheet)
		select_new_sheet(This, list_sheet);
	    }
	  if ((list_sheet=get_sheet_by_xy(This, xm, ym))!=NULL)
	    {
	      c_ContextMenu *context;
	      int	ret_button;
	      boolean	button = FALSE;
	      boolean	before = FALSE;

	      context = (c_ContextMenu *)
		NEW(c_ContextMenu)(display, CM_CLASSOR, &ret_button,
				   (Event->xbutton.button==1), Event);
	      
	      if (context)
		{
		  F(context).waitForUnmap(context);
		  DELETE(c_ContextMenu)(context);
		  Xc_TRACE(("ret button : %d", ret_button));
		  switch(ret_button)
		    {
		    case CM_Pgr_INSERT_BEFORE:
		      before = TRUE;
		    case CM_Pgr_INSERT_AFTER:
		      F(This->tableau).InsertSheet(This->tableau,
						   list_sheet, before);
		      This->selected_sheet_pos=
			calcul_selected_sheet_pos(This);
		      if (before)
			{
			  This->list_sheet = This->tableau->ListSheet;
			  select_new_sheet(This,
					  get_previous_sheet(This,list_sheet));
			}
		      else
			select_new_sheet(This, list_sheet->NextSheet);
		      configure_Classor(This);
		      break;
		    case CM_Pgr_MODIFY:
		      if (F(This->tableau).ModifySheet(This->tableau,
						       &list_sheet))
			{
			  select_new_sheet(This, list_sheet);
			  configure_Classor(This);
			}
		      break;
		    case CM_Pgr_DELETE:
		      if (This->nb_sheet == 1)
			{
			  Xc_WARNING(("last sheet destroy not allowed"));
			  break;
			}
		      
		      if (list_sheet==This->selected_sheet)
			{
			  This->selected_sheet_pos=
			    calcul_selected_sheet_pos(This);
			  if (list_sheet->NextSheet)
			    select_new_sheet(This, list_sheet->NextSheet);
			  else
			    select_new_sheet(This,
					    get_previous_sheet(This,list_sheet));
			}
		      F(This->tableau).DeleteSheet(This->tableau,
						   list_sheet);
		      configure_Classor(This);
		      break;
		    default:
		      button = TRUE;
		      break;
		    }
		}
	      else
		button = TRUE;
	      if (button)
		{
		  if (list_sheet != This->selected_sheet)
		    {
		      This->selected_sheet_pos = calcul_selected_sheet_pos(This);
		      select_new_sheet(This, list_sheet);
		    } 
		}
	    }
	  break;
	}
    }
}

static void configure_Classor(This)
c_Classor	*This;
{

  This->nb_sheet	= This->tableau->NbrSheet;
  This->list_sheet = This->tableau->ListSheet;
  This->selected_sheet = This->tableau->SelectedSheet;
  This->sheet_number_display =  This->tableau->SelectedSheet;
  This->first_sheet_pos = 0;
  This->first_sheet_visible = This->tableau->ListSheet;
  get_last_sheet_with_first(This);
  This->selected_sheet_pos = calcul_selected_sheet_pos(This);

  XClearWindow(This->display, This->window);
  draw_Classor(This);
}

static void draw_sheet_number(This, list_sheet)
c_Classor		*This;
listsheet	*list_sheet;
{
  char		sheet_number[21];
  int		txt_width;
  
  if (!list_sheet)
    list_sheet=This->selected_sheet;
  else
    {
      XSetForeground(This->display, This->gc, Classor_Norm_Color);
		     
      XFillRectangle(This->display, This->window, This->gc,
		     This->Sheet_Number_pos, 1,
		     This->Sheet_Next_pos-This->Sheet_Number_pos-3,
		     CLASSOR_SIZE-2);
      XSetForeground(This->display, This->gc, 
		    BlackPixel(This->display,DefaultScreen(This->display)));
    }
  if (list_sheet != This->selected_sheet)
    XSetForeground(This->display, This->gc, 
		   WhitePixel(This->display,DefaultScreen(This->display)));
  
  if (list_sheet->Sheet->sheet_type == Xc_SHEET_DOUBLE)
    sprintf(sheet_number,"%s: %d-%d",
	    F(GlobResources).getString(GlobResources, XcR_defaultSheetName),
	    list_sheet->Sheet->sheet_number,
	    list_sheet->Sheet->sheet_number+1);
  else
    sprintf(sheet_number,"%s: %d",
	    F(GlobResources).getString(GlobResources, XcR_defaultSheetName),
	    list_sheet->Sheet->sheet_number);
  txt_width = XTextWidth(This->font_struct, sheet_number,strlen(sheet_number));
  XDrawString(This->display, This->window, This->gc,
	      This->Sheet_Number_pos+(SHEET_NUMBER_SPACE-txt_width)/2,
	      CLASSOR_SIZE-3,sheet_number,strlen(sheet_number));  
  if (list_sheet != This->selected_sheet)
    XSetForeground(This->display, This->gc,
		   BlackPixel(This->display,DefaultScreen(This->display)));
}

static int calcul_nb_sheet_visible(This,first)
c_Classor *This;
boolean	first;
{
  int      classor_width;
  int      nb_sheet;
  listsheet *list_sheet;

  classor_width = nb_sheet = 0;
  if (first)
    {
      list_sheet = This->first_sheet_visible;
      classor_width = -This->first_sheet_pos;
    }
  else
    {
      list_sheet = This->list_sheet;
    }
  while(list_sheet && classor_width<MAX(This->Size-This->Classor_pos-2, 0))
    {
      classor_width += Classor_Icons[get_icon_number(list_sheet)].width
        + 2*ICON_SEPARATOR;
      nb_sheet ++;
      list_sheet=list_sheet->NextSheet;
    }
/*if (list_sheet) nb_sheet--;*/
  return nb_sheet;
}

static int calcul_selected_sheet_pos(This)
c_Classor *This;
{
  int	   classor_width;
  int	   nb_sheet;
  listsheet *list_sheet;

  classor_width = This->Classor_pos;
  list_sheet = This->first_sheet_visible;
  if (list_sheet == This->selected_sheet)
    return classor_width - This->first_sheet_pos;
  classor_width += Classor_Icons[get_icon_number(list_sheet)].width
    + 2*ICON_SEPARATOR - This->first_sheet_pos;;
  list_sheet=list_sheet->NextSheet;

  nb_sheet = 1;
  while(list_sheet && nb_sheet<This->nb_sheet_visible)
    {
      if (list_sheet==This->selected_sheet)
	return classor_width;
      classor_width += Classor_Icons[get_icon_number(list_sheet)].width
	+ 2*ICON_SEPARATOR;
      nb_sheet ++;
      list_sheet=list_sheet->NextSheet;
    }
  return -1;
}

static int get_icon_number(list_sheet)
listsheet	*list_sheet;
{
  switch(list_sheet->Sheet->sheet_orient)
    {
    case Xc_PORTRAIT:
      switch(list_sheet->Sheet->sheet_type)
	{
	case Xc_SHEET_LEFT:
	  return CLASSOR_PORTRAIT_LEFT;
	  break;
	case Xc_SHEET_RIGHT:
	  return CLASSOR_PORTRAIT_RIGHT;
	  break;
	case Xc_SHEET_DOUBLE:
	  return CLASSOR_PORTRAIT_DOUBLE;
	  break;
	}
      break;
    case Xc_LANDSCAPE:
      switch(list_sheet->Sheet->sheet_type)
	{
	case Xc_SHEET_LEFT:
	  return CLASSOR_LANDSCAPE_LEFT;
	  break;
	case Xc_SHEET_RIGHT:
	  return CLASSOR_LANDSCAPE_RIGHT;
	  break;
	case Xc_SHEET_DOUBLE:
	  return CLASSOR_LANDSCAPE_DOUBLE;
	  break;
	}
      break;
    }
  return 0;
}

static listsheet *get_sheet_by_xy(This, x, y)
c_Classor	*This;
int	x;
int	y;
{
  int		nb_sheet;
  int		pos_cumul;
  listsheet	*list_sheet;
  
  if (y<0 || y>CLASSOR_SIZE || x<This->Classor_pos || x>This->Size-2)
    return NULL;
  
  nb_sheet = 0;
  pos_cumul = This->Classor_pos-This->first_sheet_pos;
  list_sheet = This->first_sheet_visible;
  while(list_sheet && nb_sheet<This->nb_sheet_visible)
    {
      if (x >= pos_cumul &&
	  x <= pos_cumul + Classor_Icons[get_icon_number(list_sheet)].width
	  + 2*ICON_SEPARATOR)
	return list_sheet;
      pos_cumul +=  Classor_Icons[get_icon_number(list_sheet)].width 
	+ 2*ICON_SEPARATOR;
      nb_sheet++;
      list_sheet = list_sheet->NextSheet;
    }
  return NULL;
}

static void select_new_sheet(This, list_sheet)
c_Classor		*This;
listsheet	*list_sheet;
{
  boolean	regle;
  boolean	scrollbH;
  boolean	scrollbV;

  hilight_selected_icon(This);

/*Xc_TRACE(("deselctionne sheet %d",This->selected_sheet->Sheet->sheet_number));*/
  regle = This->selected_sheet->Sheet->Regle_actif;
  scrollbH = This->selected_sheet->Sheet->ScrollBarH_actif;
  scrollbV = This->selected_sheet->Sheet->ScrollBarV_actif;
  F(This->selected_sheet->Sheet).deselect_Sheet(This->selected_sheet->Sheet);
  
  This->selected_sheet=list_sheet;
  This->selected_sheet_pos = calcul_selected_sheet_pos(This);  
  This->tableau->SelectedSheet = list_sheet;
  This->selected_sheet->Sheet->Regle_actif = regle;
  This->selected_sheet->Sheet->ScrollBarH_actif = scrollbH;
  This->selected_sheet->Sheet->ScrollBarV_actif = scrollbV;

/*Xc_TRACE(("nouvelle sheet selectionnee: %d",list_sheet->Sheet->sheet_number));*/
  F(This->selected_sheet->Sheet).
    select_Sheet(This->selected_sheet->Sheet,
		This->tableau->display, This->tableau->window,
		REGLE_SIZE*regle, REGLE_SIZE*regle,
		This->tableau->SizeX-(REGLE_SIZE*regle+
				       SCROLLB_SIZE*scrollbV)-1,
		This->tableau->SizeY-(REGLE_SIZE*regle+
				       SCROLLB_SIZE*scrollbH
				       +CLASSOR_SIZE)-1,
		This->tableau->ScrollBarV, This->tableau->ScrollBarH,
		This->tableau->RegleV,	This->tableau->RegleH,
		This->tableau->RegleC, This->tableau->Classor);
  
  draw_sheet_number(This, This->selected_sheet);

  hilight_selected_icon(This);
}   

static listsheet	*get_first_sheet(This)
c_Classor	*This;
{
  return This->list_sheet; 
}

static listsheet	*get_previous_sheet(This,list_sheet_def)
c_Classor	*This;
listsheet	*list_sheet_def;
{
  listsheet	*list_sheet;
  
  if (!list_sheet_def)
    list_sheet_def = This->selected_sheet;
  list_sheet=This->list_sheet;
  if (list_sheet==list_sheet_def)
    return NULL;
  while(list_sheet->NextSheet!=list_sheet_def)
    list_sheet=list_sheet->NextSheet;
  return list_sheet; 
}

static listsheet	*get_next_sheet(This,list_sheet_def)
c_Classor	*This;
listsheet	*list_sheet_def;
{
  listsheet	*list_sheet;
  
  if (!list_sheet_def)
    list_sheet_def = This->selected_sheet;
  list_sheet = This->list_sheet;
  while(list_sheet!=list_sheet_def)
    list_sheet=list_sheet->NextSheet;
  return list_sheet->NextSheet; 
}

static listsheet	*get_last_sheet(This)
c_Classor	*This;
{
  listsheet	*list_sheet;
  
  list_sheet=This->list_sheet;
  while(list_sheet->NextSheet)
    list_sheet=list_sheet->NextSheet;
  return list_sheet; 
}

static void get_last_sheet_with_first(This)
c_Classor	*This;
{
  listsheet  *list_sheet;
  int	    last_pos;
  int	    nb_sheet;

  list_sheet = This->first_sheet_visible;
  last_pos = This->first_sheet_pos+This->Classor_pos;
  nb_sheet = 0;
  while(list_sheet &&
	last_pos+Classor_Icons[get_icon_number(list_sheet)].width
	+2*ICON_SEPARATOR<This->Size-1)
    {
      last_pos += Classor_Icons[get_icon_number(list_sheet)].width+
	2*ICON_SEPARATOR;
      nb_sheet++;
      list_sheet = list_sheet->NextSheet;
    }
  This->last_sheet_visible = list_sheet;
  This->last_sheet_pos = This->Size-2-last_pos;
  This->nb_sheet_visible = nb_sheet+ (list_sheet?1:0);
/*  if (list_sheet)
    Xc_TRACE(("lastsheet: %d pos: %d nb_sheet: %d",
	      list_sheet->Sheet->sheet_number,
	      This->last_sheet_pos, This->nb_sheet_visible));
  else
    Xc_TRACE(("lastsheet NULL")); */
}

static void get_first_sheet_with_last(This)
c_Classor	*This;
{
  listsheet	*list_sheet;
  int		first_pos;
  int		nb_sheet;

  list_sheet = This->last_sheet_visible;
  first_pos = This->Size-1-Classor_Icons[get_icon_number(list_sheet)].width-
    2*ICON_SEPARATOR+This->last_sheet_pos;
  nb_sheet = 1;
  while(list_sheet &&
	first_pos>This->Classor_pos)
    {
      list_sheet = get_previous_sheet(This, list_sheet);
      if (list_sheet)
	{
	  first_pos -= Classor_Icons[get_icon_number(list_sheet)].width+
	    2*ICON_SEPARATOR;
	}
      nb_sheet++;
    }
  This->first_sheet_visible = list_sheet;
  This->first_sheet_pos = This->Classor_pos - first_pos;
  This->nb_sheet_visible = nb_sheet;
/*  if (list_sheet)
Xc_TRACE(("firstage: %d pos: %d nb_sheet: %d",
	      list_sheet->Sheet->sheet_number,
	      This->first_sheet_pos, This->nb_sheet_visible));
  else
    Xc_TRACE(("firstsheet NULL")); */
}

static boolean repeat_time_function(This, function)
c_Classor 	*This;
boolean (*function)();
{
  boolean	sortie;
  int		sortie2;
  int		wtime2;
  XEvent	none;

  sortie = FALSE;
  XFlush(This->display);
  sortie2 = 0;
  while (sortie2<LOOPTIME)
    {
      Zusleep(WTIME);
      sortie2++;
      if (XEventsQueued(This->display, QueuedAfterReading))
	{
	  if (XCheckTypedEvent(This->display, ButtonRelease, &none))
	  {
	    sortie = TRUE;
	    break;
	  }
	}
    }
  if (sortie) return TRUE;
  wtime2=WTIME2;
  do
    {
      sortie = function(This);
      if (sortie) return FALSE;
      XSync(This->display, False);
      Zusleep(wtime2);
      if (XEventsQueued(This->display, QueuedAfterReading))
      {
	if (XCheckTypedEvent(This->display, ButtonRelease, &none))
	{
	  sortie = TRUE;
	}
      }
      if (wtime2>MIN_WTIME2)
	wtime2 -= 10;
    } while (!sortie);
  return FALSE;
}

