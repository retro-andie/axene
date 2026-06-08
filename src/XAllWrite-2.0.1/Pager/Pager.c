/*
** Pager.c for XAllWrite in Pager/
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
** Last update Wed Nov 11 18:15:31 1998 Emmanuel Paris
*/

#define _PRIVATE_PAGER_
#include "Pager.h"
#include "Xpm.h"
#include "Resource.h"
#include "Colormap.h"

extern c_Resource *GlobResources;
extern c_Colormap *GlobColormap;

static void *cons_Pager();
static void dest_Pager();
static void *copy_Pager();
void Init_Pager();
static void resize_Pager();
static void hilight_selected_icon();
static void draw_Pager();
static void draw_Pager_icons();
static boolean scroll_Pager_left();
static boolean scroll_Pager_right();
static void draw_page_number();
static void eventlook_Pager();
static void configure_Pager();
static int calcul_nb_page_visible();
static int calcul_selected_page_pos();
static int get_icon_number();
static c_Page *get_page_by_xy();
static void select_new_page();
static c_Page *get_first_page();
static c_Page *get_previous_page();
static c_Page *get_next_page();
static c_Page *get_last_page();
static void get_last_page_with_first();
static void get_first_page_with_last();
static boolean repeat_time_function();

sf_Pager fc_Pager =
{
 cons_Pager,
 dest_Pager,
 copy_Pager,
 resize_Pager,
 draw_Pager,
 eventlook_Pager,
 configure_Pager
};

static void *cons_Pager(display, window, PosX, PosY, Size, document)
Display	*display;
Window	window;
int	PosX;
int	PosY;
int	Size;
c_Document	*document;
{
 c_Pager	*This;
 XGCValues	gcv;
  
 /*Xc_TRACE(("Create PAger"));*/
 if ((This = (c_Pager *)Xc_malloc("Pager", sizeof (c_Pager))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Pager;
 This->display = display;
 This->Size = Size;
  
 This->Page_Begin_pos = 1;
 This->Page_Previous_pos = This->Page_Begin_pos + Page_Begin_width + 2;
 This->Page_Number_pos = This->Page_Previous_pos + Page_Previous_width + 2;
 This->Page_Next_pos = This->Page_Number_pos + PAGE_NUMBER_SIZE;
 This->Page_End_pos = This->Page_Next_pos + Page_Next_width + 2;
 This->Pager_pos = This->Page_End_pos + Page_End_width + 2;
 /*--- fill pager info ---*/
 This->document	= document;
 This->pagearea = document->PageArea;

 This->nb_page = This->pagearea->NbrPage;
 This->list_page = This->pagearea->PageList;
 if (This->pagearea->SelectedPage &&
     This->pagearea->SelectedPage->page_type == 
     Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT &&
     This->pagearea->SelectedPage->PrevPage &&
     This->pagearea->SelectedPage->PrevPage->page_type == Xc_PAGE_DOUBLE)
  This->selected_page = This->pagearea->SelectedPage->PrevPage;
 else
  This->selected_page = This->pagearea->SelectedPage;
 This->page_number_display =  This->selected_page;
 This->first_page_pos = 0;
 This->first_page_visible = This->pagearea->PageList;
 get_last_page_with_first(This);
 This->selected_page_pos = calcul_selected_page_pos(This);

 /*--- Window creation ---*/
 This->window = 
  XCreateSimpleWindow(display, window, PosX,  PosY,
		      Size, PAGER_SIZE, 0, 
		      BlackPixel(display, DefaultScreen(display)),
		      Pager_Norm_Color);
 XSelectInput(display, This->window, PointerMotionMask | ButtonPressMask |
	      ButtonReleaseMask | ExposureMask | LeaveWindowMask);
 gcv.function = GXcopy;
 gcv.background = Pager_Norm_Color;
 gcv.foreground = BlackPixel(display, DefaultScreen(display));
 This->gc = XCreateGC(display, This->window,
		      GCFunction | GCForeground | GCBackground,
		      &gcv);
 This->font_struct = 
  F(GlobResources).getXFontStruct(GlobResources, XcR_pagerFont);
 XSetFont(display, This->gc,
	  F(GlobResources).getFont(GlobResources, XcR_pagerFont));
  
 XMapWindow(display, This->window);
 Xc_TRACE(("Object Pager build"));
 return This;
}

static void dest_Pager(This)
c_Pager *This;
{ 
 XFreeGC(This->display, This->gc);
 Xc_free(This);
 Xc_TRACE(("Object Pager destroyed"));
}

static void *copy_Pager(This)
c_Pager *This;
{
 c_Pager *ObjTmp;

 if ((ObjTmp = (c_Pager *)Xc_malloc("CPager", sizeof (c_Pager))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_Pager));
 Xc_TRACE(("Object Pager copied"));
 return ObjTmp;
}

void Init_Pager(display)
Display	*display;
{
 color_cells_t	realcolor;
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
 F(GlobColormap).allocate_named_color(GlobColormap, PAGER_NORM, &realcolor);
 Pager_Norm_Color = realcolor.pixel;
 F(GlobColormap).allocate_named_color(GlobColormap, PAGER_SHAD, &realcolor);
 Pager_Shad_Color = realcolor.pixel;
  
 if (DefaultDepth(display, DefaultScreen(display))==1)
  Pager_High_Color = BlackPixel(display, DefaultScreen(display));
 else
 {
  F(GlobColormap).allocate_named_color(GlobColormap, PAGER_HIGH,
				       &realcolor);
  Pager_High_Color = realcolor.pixel;
 }
  
 ptr = F(GlobResources).getString(GlobResources, XcR_defaultPageName);
 name = Xc_malloc("name", strlen(ptr) + 10);
 sprintf(name, "%s: 888-888", ptr);
 font_struct = F(GlobResources).getXFontStruct(GlobResources, XcR_pagerFont);
 page_number_size = XTextWidth(font_struct, name, strlen(name)) + 4;
 Xc_free(name);
  
 for(i = 0; i<NB_PIXMAP_PAGER; i++)
 {
  image=(char **)Tab_Pixmap_Pager[i].Image;
  XPMGetAttrib(image, &width, &height, &color,
	       &bit, &x_hot, &y_hot);
  *(Tab_Pixmap_Pager[i].width)=width;
  *(Tab_Pixmap_Pager[i].height)=height;
  XPMload(display, image, &xsource, &xmask, FALSE);
  psource = XCreatePixmap(display, DefaultRootWindow(display),
			  width, height,
			  DefaultDepth(display, DefaultScreen(display)));
  gc = XCreateGC(display, psource, 0,0);
  XPutImage(display, psource, gc, xsource, 0,0, 0,0, width, height);
  Xc_free(xsource->data);
  XFree((char*)xsource);
  if (xmask && xmask->data)
   Xc_free(xmask->data);
  if (xmask)
   XFree((char *)xmask);
  XFreeGC(display, gc);
  *(Tab_Pixmap_Pager[i].Pixmap)=psource;
 }
}

static void resize_Pager(This, PosX, PosY, Size) 
c_Pager	*This;
int	PosX;
int	PosY;
int	Size;
{
 c_Page *list_page;
 int	   width;

 This->Size = Size;

 list_page =  This->first_page_visible;
 width = This->Pager_pos + 1;

 while(list_page && width < This->Size)
 {
  width += Pager_Icons[get_icon_number(list_page)].width
   +2 * ICON_SEPARATOR;
  if (IS_DOUBLE_PAGE(list_page))
   list_page = list_page->NextPage;
  list_page = list_page->NextPage;   
 }

 if (width >= This->Size)
  get_last_page_with_first(This);
 else
 {
  This->nb_page_visible = calcul_nb_page_visible(This, FALSE);
  if (This->nb_page_visible < This->nb_page)
  {
   This->last_page_visible = get_last_page(This);
   This->last_page_pos = Pager_Icons
    [get_icon_number(This->last_page_visible)].width+2*ICON_SEPARATOR-1;
   get_first_page_with_last(This);
  }
  else
  {
   This->first_page_visible = get_first_page(This);
   This->first_page_pos = 0;
   get_last_page_with_first(This);
  }
  This->nb_page_visible = calcul_nb_page_visible(This, FALSE);
 }
 XMoveResizeWindow(This->display, This->window,
		   PosX, PosY,
		   Size, PAGER_SIZE);
}


static void draw_Pager(This)
c_Pager *This;
{
 XSegment	xseg[7];
  
 draw_Pager_icons(This, FALSE);
 hilight_selected_icon(This);
 draw_page_number(This, NULL);

 XCopyArea(This->display, Page_Begin, This->window, This->gc,
	   0, 0, Page_Begin_width, Page_Begin_height,
	   This->Page_Begin_pos, 1);
 XCopyArea(This->display, Page_Previous, This->window, This->gc,
	   0, 0, Page_Previous_width, Page_Previous_height,
	   This->Page_Previous_pos, 1);
 XCopyArea(This->display, Page_Next, This->window, This->gc,
	   0, 0, Page_Next_width, Page_Next_height,
	   This->Page_Next_pos, 1);
 XCopyArea(This->display, Page_End, This->window, This->gc,
	   0, 0, Page_End_width, Page_End_height,
	   This->Page_End_pos, 1);

 xseg[0].y1 = xseg[1].y1 = xseg[2].y1 = xseg[3].y1=
  xseg[4].y1 = xseg[5].y1 = xseg[6].y1 = 0;
 xseg[0].y2 = xseg[1].y2 = xseg[2].y2 = xseg[3].y2=
  xseg[4].y2 = xseg[5].y2 = xseg[6].y2 = PAGER_SIZE;
  
 XSetForeground(This->display, This->gc, Pager_High_Color);
 xseg[0].x1 = xseg[0].x2 = This->Page_Begin_pos - 1;
 xseg[1].x1 = xseg[1].x2 = This->Page_Previous_pos - 1;
 xseg[2].x1 = xseg[2].x2 = This->Page_Number_pos - 1;
 xseg[3].x1 = xseg[3].x2 = This->Page_Next_pos - 1;
 xseg[4].x1 = xseg[4].x2 = This->Page_End_pos - 1;
 xseg[5].x1 = xseg[5].x2 = This->Pager_pos - 1 ;  
 xseg[6].x1 = 0; xseg[6].x2 = This->Size; 
 xseg[6].y1 = xseg[6].y2 = 0;
 XDrawSegments(This->display, This->window, This->gc, xseg, 7);

 XSetForeground(This->display, This->gc, Pager_Shad_Color);
 xseg[0].x1 = xseg[0].x2 = This->Page_Previous_pos - 2;
 xseg[1].x1 = xseg[1].x2 = This->Page_Number_pos - 2;
 xseg[2].x1 = xseg[2].x2 = This->Page_Next_pos - 2 ;
 xseg[3].x1 = xseg[3].x2 = This->Page_End_pos - 2;   
 xseg[4].x1 = xseg[4].x2 = This->Pager_pos - 2;
 xseg[5].x1 = xseg[5].x2 = This->Size - 1;
 xseg[6].x1 = 0; xseg[6].x2 = This->Size;
 xseg[6].y1 = xseg[6].y2 = PAGER_SIZE-1;
 XDrawSegments(This->display, This->window, This->gc, xseg, 7);
  
 XSetForeground(This->display, This->gc,
		BlackPixel(This->display, DefaultScreen(This->display)));
}

static void hilight_selected_icon(This)
c_Pager	*This;
{
 int	width;
 int	pos;

 This->selected_page_pos = calcul_selected_page_pos(This);
  
 if (This->selected_page_pos != -1)
 {
  width = Pager_Icons[ get_icon_number(This->selected_page)].width;
  if ( This->selected_page_pos <= This->Pager_pos)
  {
   pos =  This->Pager_pos - This->selected_page_pos;
	  
   if (pos > ICON_SEPARATOR)
   {
    if (pos > width + ICON_SEPARATOR)
     return;
    width -= pos - ICON_SEPARATOR;
    pos = This->Pager_pos;
   }
   else
    pos = This->Pager_pos + MAX(0, ICON_SEPARATOR-pos);
  }
  else
   if ( This->selected_page_pos + width > This->Size-1)
   {
    width = This->Size-1 - This->selected_page_pos - ICON_SEPARATOR;
    if (width <=0)
     return;
    pos =  This->selected_page_pos + ICON_SEPARATOR;
   }
   else
    pos =  This->selected_page_pos + ICON_SEPARATOR;
      
  if (GlobColormap->Cblack)
   XSetFunction(This->display, This->gc, GXxor);
  else
   XSetFunction(This->display, This->gc, GXinvert);
      
  XFillRectangle(This->display, This->window, This->gc, 
		 pos, 1, width, PAGER_SIZE - 2);
  XSetFunction(This->display, This->gc, GXcopy);
 } 
}

static void draw_Pager_icons(This, erase)
c_Pager	*This;
boolean erase;
{
 int		pos_orig;
 int		pos_cumul;
 int		page_count;
 int		icon_num;
 c_Page		*list_page;

 if (erase)
 {
  XSetForeground(This->display, This->gc, Pager_Norm_Color);
  XFillRectangle(This->display, This->window, This->gc, 
		 This->Pager_pos, 1,
		 This->Size-1-This->Pager_pos, PAGER_SIZE - 2);
  XSetForeground(This->display, This->gc,
		 BlackPixel(This->display, DefaultScreen(This->display)));
 }
 list_page = This->first_page_visible;
 page_count = This->nb_page_visible;
 if (!page_count) return;
 pos_orig = This->first_page_pos;

 icon_num = get_icon_number(list_page);
 if (pos_orig < Pager_Icons[icon_num].width+ICON_SEPARATOR)
 {
  XCopyArea(This->display, Pager_Icons[icon_num].Icon,
	    This->window, This->gc,
	    MAX( 0 , pos_orig-ICON_SEPARATOR), 0,
	    Pager_Icons[icon_num].width-MAX(0, pos_orig-ICON_SEPARATOR),
	    Pager_Icons[icon_num].height,
	    This->Pager_pos + MAX(0 , ICON_SEPARATOR-pos_orig), 1);
 }      
 if (IS_DOUBLE_PAGE(list_page))
 {
  page_count--;
  list_page = list_page->NextPage;
 }

 list_page = list_page->NextPage;
  
 pos_cumul = This->Pager_pos + 
  Pager_Icons[icon_num].width+2*ICON_SEPARATOR - pos_orig; 
  
 if (This->nb_page_visible <= This->nb_page)
  page_count--;

 if (page_count)
 {
  do
  {
   if (IS_DOUBLE_PAGE(list_page))
   {
    if (page_count <= 2) break;
    icon_num = get_icon_number(list_page);
    page_count--;
    list_page = list_page->NextPage;
   }
   else
    icon_num = get_icon_number(list_page);

   XCopyArea(This->display, Pager_Icons[icon_num].Icon,
	     This->window, This->gc, 0, 0,
	     Pager_Icons[icon_num].width,
	     Pager_Icons[icon_num].height,
	     pos_cumul + ICON_SEPARATOR, 1);
   pos_cumul += Pager_Icons[icon_num].width+2*ICON_SEPARATOR; 

   page_count--;
   list_page = list_page->NextPage;
  }
  while(page_count > 1);
  
  if (This->nb_page_visible <= This->nb_page && list_page)
  {
   icon_num = get_icon_number(list_page);
   /*Xc_TRACE(("icon width: %d  pos_cimul: %d  Size: %d",*/
   /*Pager_Icons[icon_num].width,This->Size,pos_cumul));*/
	  
   XCopyArea(This->display, Pager_Icons[icon_num].Icon,
	     This->window, This->gc, 0, 0,
	     MIN(Pager_Icons[icon_num].width, This->Size-3-pos_cumul),
	     Pager_Icons[icon_num].height,
	     pos_cumul + ICON_SEPARATOR, 1);
  }
 }
}

static boolean scroll_Pager_left(This)
c_Pager	*This;
{
 c_Page		*list_page;
 int		icon_num;
 int		scroll_pos;
  
 if (This->nb_page<=This->nb_page_visible || !This->last_page_visible)
  return TRUE;
  
 if (This->last_page_pos <
     Pager_Icons[get_icon_number(This->last_page_visible)].width+
     2*ICON_SEPARATOR-1)
  This->last_page_pos++;
 else
 {
  list_page = get_next_page(This, This->last_page_visible);
  if (!list_page)
   return TRUE;
  This->last_page_visible = list_page;
  This->last_page_pos = 0;
 }

 if (This->first_page_pos <
     Pager_Icons[get_icon_number(This->first_page_visible)].width+
     2*ICON_SEPARATOR-1)
  This->first_page_pos++;
 else
 {
  list_page = get_next_page(This, This->first_page_visible);
  if (!list_page)
   return TRUE;
  This->first_page_visible = list_page;
  This->first_page_pos = 0;
  draw_page_number(This, This->first_page_visible);
 }
 This->nb_page_visible = calcul_nb_page_visible(This, TRUE);

 XCopyArea(This->display, This->window, This->window, This->gc,
	   This->Pager_pos+1, 1, This->Size-This->Pager_pos-2, 
	   PAGER_SIZE-2, This->Pager_pos, 1); 

 icon_num = get_icon_number(This->last_page_visible);
 if (This->last_page_pos >= ICON_SEPARATOR && 
     This->last_page_pos < Pager_Icons[icon_num].width+ICON_SEPARATOR) 
 {
  scroll_pos = This->Size-2;

  XCopyArea(This->display, Pager_Icons[icon_num].Icon,
	    This->window, This->gc, 
	    This->last_page_pos-ICON_SEPARATOR, 0, 1,
	    Pager_Icons[icon_num].height,
	    scroll_pos, 1);

  if (This->selected_page == This->last_page_visible)
  {
   This->selected_page_pos = calcul_selected_page_pos(This);
   if (scroll_pos >= This->selected_page_pos + ICON_SEPARATOR && 
       scroll_pos< This->selected_page_pos + 
       Pager_Icons[icon_num].width + ICON_SEPARATOR)
   {
    if (GlobColormap->Cblack)
     XSetFunction(This->display, This->gc, GXxor);
    else
     XSetFunction(This->display, This->gc, GXinvert);
	      
    XDrawLine(This->display, This->window, This->gc, 
	      scroll_pos, 1, scroll_pos, PAGER_SIZE - 2);
    XSetFunction(This->display, This->gc, GXcopy);
   } 
  }
 }
 else
 {
  XSetForeground(This->display, This->gc, Pager_Norm_Color);
  XDrawLine(This->display, This->window, This->gc, This->Size-2,
	    1, This->Size-2, PAGER_SIZE-2);
  XSetForeground(This->display, This->gc,
		 BlackPixel(This->display, DefaultScreen(This->display)));
 }
 return FALSE;
}

static boolean scroll_Pager_right(This)
c_Pager	*This;
{
 c_Page		*list_page;
 int		icon_num;
 int		scroll_pos;

 if (This->nb_page<This->nb_page_visible)
  return TRUE;

 if (This->first_page_pos)
  This->first_page_pos--;
 else
 {
  list_page = get_previous_page(This, This->first_page_visible);
  if (!list_page) 
   return TRUE;
  This->first_page_visible = list_page;
  This->first_page_pos = Pager_Icons
   [get_icon_number(list_page)].width+2*ICON_SEPARATOR-1;
  draw_page_number(This, This->first_page_visible);
 }
  
 if (This->last_page_pos) 
  This->last_page_pos--;
 else
 {
  list_page = get_previous_page(This, This->last_page_visible);
  if (!list_page)
   return TRUE;
  This->last_page_visible = list_page;
  This->last_page_pos = Pager_Icons
   [get_icon_number(list_page)].width+2*ICON_SEPARATOR-1;
 }
 This->nb_page_visible = calcul_nb_page_visible(This, TRUE);
  
 XCopyArea(This->display, This->window, This->window, This->gc,
	   This->Pager_pos, 1, This->Size-This->Pager_pos-2, 
	   PAGER_SIZE-2, This->Pager_pos+1, 1); 
  
 icon_num = get_icon_number(This->first_page_visible);
 if (This->first_page_pos >= ICON_SEPARATOR && 
     This->first_page_pos < Pager_Icons[icon_num].width+ICON_SEPARATOR) 
 {
  scroll_pos = This->Pager_pos;
  XCopyArea(This->display, Pager_Icons[icon_num].Icon,
	    This->window, This->gc, 
	    This->first_page_pos-ICON_SEPARATOR , 0, 1,
	    Pager_Icons[icon_num].height,
	    scroll_pos, 1);
  if (This->selected_page == This->first_page_visible)
  {
   This->selected_page_pos = calcul_selected_page_pos(This);
   if (scroll_pos >= This->selected_page_pos + ICON_SEPARATOR && 
       scroll_pos < This->selected_page_pos + 
       Pager_Icons[icon_num].width+ICON_SEPARATOR)
   {
    if (GlobColormap->Cblack)
     XSetFunction(This->display, This->gc, GXxor);
    else
     XSetFunction(This->display, This->gc, GXinvert);
	      
    XDrawLine(This->display, This->window, This->gc, 
	      scroll_pos, 1, scroll_pos, PAGER_SIZE - 2);
    XSetFunction(This->display, This->gc, GXcopy);
   }
  }
 }
 else
 {
  XSetForeground(This->display, This->gc, Pager_Norm_Color);
  XDrawLine(This->display, This->window, This->gc, This->Pager_pos,
	    1, This->Pager_pos, PAGER_SIZE-2);
  XSetForeground(This->display, This->gc,
		 BlackPixel(This->display, DefaultScreen(This->display)));
 }
 return FALSE;
}

static void eventlook_Pager(This, Event)
c_Pager		*This;
XEvent		*Event;
{
 int		xm, ym;
 XRectangle     rect;
 c_Page		*list_page;
 c_Page		*svg_list_page;

 if (Event->xany.window == This->window)
 {
  switch(Event->type)
  {
  case  Expose:
  case GraphicsExpose:
   rect.x = Event->xexpose.x;
   rect.y = Event->xexpose.y;
   rect.width = Event->xexpose.width;
   rect.height = Event->xexpose.height;
   XSetClipRectangles(This->display, This->gc, 0,0, &rect, 1,Unsorted);
   XSetGraphicsExposures(This->display, This->gc, False);
   F(This).Draw(This);
   XSetGraphicsExposures(This->display, This->gc, True);
   XSetClipMask(This->display, This->gc, None);
   break;
  case LeaveNotify:
   if (This->selected_page != This->page_number_display)
   {
    draw_page_number(This, This->selected_page);
    This->page_number_display = This->selected_page;
   }
   break;
  case MotionNotify:
   xm = Event->xmotion.x;
   ym = Event->xmotion.y;
   list_page = get_page_by_xy(This, xm, ym);
   if (list_page)
   {
    if (list_page != This->page_number_display)
    {
     draw_page_number(This, list_page);
     This->page_number_display = list_page;
    }
   }
   else
    if (This->selected_page != This->page_number_display)
    {
     draw_page_number(This, This->selected_page);
     This->page_number_display = This->selected_page;
    }
   break;
  case ButtonPress:
   xm = Event->xbutton.x;
   ym = Event->xbutton.y;
   if (xm >= This->Page_Begin_pos &&
       xm <= This->Page_Begin_pos + Page_Begin_width)
   {
    Xc_TRACE(("button Page begin"));
    list_page = get_first_page(This);
    This->first_page_visible = get_first_page(This);
    This->first_page_pos = 0;
    get_last_page_with_first(This);
    draw_Pager_icons(This, TRUE);
    hilight_selected_icon(This);
    draw_page_number(This, NULL);
    if (list_page != This->selected_page)
    {
     This->selected_page_pos = calcul_selected_page_pos(This);
     select_new_page(This, list_page);
    }
   }
   if (xm >= This->Page_Previous_pos &&
       xm <= This->Page_Previous_pos + Page_Previous_width)
   {
    Xc_TRACE(("button Page previous"));
    if (repeat_time_function(This, scroll_Pager_right))
    {
     list_page = get_previous_page(This, NULL);
     if (list_page && list_page != This->selected_page)
     {
      svg_list_page = This->selected_page;
      This->selected_page = list_page;
      if (calcul_selected_page_pos(This)==-1)
      {
       This->selected_page = svg_list_page;
       This->first_page_visible = list_page;
       This->first_page_pos = 0;
       get_last_page_with_first(This);
       draw_Pager_icons(This, TRUE);
       hilight_selected_icon(This);
      }
      This->selected_page = svg_list_page;
      select_new_page(This, list_page);
     }
    }
    draw_page_number(This, This->selected_page);
   }
   if (xm > This->Page_Previous_pos + Page_Previous_width &&
       xm < This->Page_Next_pos)
   {
    Xc_TRACE(("label Page Number"));
    list_page = This->selected_page;
    if (F(This->pagearea).GotoPage(This->pagearea, &list_page))
    {
     if (This->nb_page_visible != This->nb_page)
     {
      This->first_page_visible = list_page;
      This->first_page_pos = 0;
      get_last_page_with_first(This);
      if (!This->last_page_visible)
      {
       This->last_page_visible = get_last_page(This);
       This->last_page_pos = 0;
       get_first_page_with_last(This);
      }
      draw_Pager_icons(This,TRUE);
      hilight_selected_icon(This);
      draw_page_number(This, NULL);
     }
     select_new_page(This, list_page);
    }   
   }
   if (xm >= This->Page_Next_pos &&
       xm <= This->Page_Next_pos + Page_Next_width)
   {
    Xc_TRACE(("button Page Next"));
    if (repeat_time_function(This, scroll_Pager_left))
    {
     list_page = get_next_page(This, NULL);
     if (list_page && list_page != This->selected_page)
     {
      svg_list_page = This->selected_page;
      This->selected_page = list_page;
      if (calcul_selected_page_pos(This)==-1)
      {
       This->selected_page = svg_list_page;
       This->last_page_visible = list_page;
       This->last_page_pos = Pager_Icons
	[get_icon_number(This->last_page_visible)].width+2*ICON_SEPARATOR-1;
       get_first_page_with_last(This);
       if (!This->first_page_visible)
       {
	This->first_page_visible = list_page;
	This->first_page_pos = 0;
	get_last_page_with_first(This);
       } 
       draw_Pager_icons(This, TRUE);
       hilight_selected_icon(This);
      }
      This->selected_page = svg_list_page;
      select_new_page(This, list_page);
     }
    }
    draw_page_number(This, This->selected_page);
   }
   if (xm >= This->Page_End_pos &&
       xm <= This->Page_End_pos + Page_End_width)
   {
    Xc_TRACE(("button Page End"));
    list_page = get_last_page(This);
    This->last_page_visible = get_last_page(This);
    This->last_page_pos = Pager_Icons
     [get_icon_number(This->last_page_visible)].width+2*ICON_SEPARATOR-1;
    get_first_page_with_last(This);
    if (!This->first_page_visible)
    {
     This->first_page_visible = get_first_page(This);
     This->first_page_pos = 0;
     get_last_page_with_first(This);
    }
    draw_Pager_icons(This, TRUE);
    hilight_selected_icon(This);
    draw_page_number(This, NULL);
    if (list_page != This->selected_page)
     select_new_page(This, list_page);
   }
   if ((list_page = get_page_by_xy(This, xm, ym)) != NULL)
   {
    if (list_page != This->selected_page)
    {
     This->selected_page_pos = calcul_selected_page_pos(This);
     select_new_page(This, list_page);
    } 
   }
   break;
  }
 }
}

static void configure_Pager(This)
c_Pager	*This;
{
 This->nb_page	= This->pagearea->NbrPage;
 This->list_page = This->pagearea->PageList;
 This->selected_page = This->pagearea->SelectedPage;
 This->page_number_display =  This->pagearea->SelectedPage;
 This->first_page_pos = 0;
 This->first_page_visible = This->pagearea->PageList;
 get_last_page_with_first(This);
 
 if (This->last_page_visible && 
     This->selected_page->page_number >= This->last_page_visible->page_number)
 {
  This->last_page_visible = This->selected_page;
  This->last_page_pos = Pager_Icons[get_icon_number
				    (This->last_page_visible)].width+
				     2*ICON_SEPARATOR;
  get_first_page_with_last(This);
 }

 This->selected_page_pos = calcul_selected_page_pos(This);

 XClearWindow(This->display, This->window);
 draw_Pager(This);
}

static void draw_page_number(This, list_page)
c_Pager		*This;
c_Page		*list_page;
{
 char		page_number[21];
 int		txt_width;
  
 if (!list_page)
  list_page = This->selected_page;
 else
 {
  XSetForeground(This->display, This->gc, Pager_Norm_Color);
		     
  XFillRectangle(This->display, This->window, This->gc,
		 This->Page_Number_pos, 1,
		 This->Page_Next_pos-This->Page_Number_pos-3,
		 PAGER_SIZE-2);
  XSetForeground(This->display, This->gc, 
		 BlackPixel(This->display, DefaultScreen(This->display)));
 }
 if (list_page != This->selected_page)
  XSetForeground(This->display, This->gc, 
		 WhitePixel(This->display, DefaultScreen(This->display)));
  
 if ((list_page->page_type & Xc_PAGE_DOUBLE)  != 0)
 {
  if (list_page->page_type == Xc_PAGE_DOUBLE &&
      list_page->NextPage && list_page->NextPage->page_type == 
      Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT)
   sprintf(page_number, "%s: %d-%d",
	   F(GlobResources).getString(GlobResources, XcR_defaultPageName),
	   list_page->page_number, list_page->page_number+1);
  else
  {
   if (list_page->page_type == Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT &&
       list_page->PrevPage && list_page->PrevPage->page_type == 
       Xc_PAGE_DOUBLE)
    sprintf(page_number, "%s: %d-%d",
	    F(GlobResources).getString(GlobResources, XcR_defaultPageName),
	    list_page->page_number-1, list_page->page_number);
   else
    sprintf(page_number, "%s: %d",
	    F(GlobResources).getString(GlobResources, XcR_defaultPageName),
	    list_page->page_number);
  }
 }
 else
  sprintf(page_number, "%s: %d",
	  F(GlobResources).getString(GlobResources, XcR_defaultPageName),
	  list_page->page_number);
 txt_width = XTextWidth(This->font_struct, page_number, strlen(page_number));
 XDrawString(This->display, This->window, This->gc,
	     This->Page_Number_pos + (PAGE_NUMBER_SIZE - txt_width) / 2,
	     PAGER_SIZE-3, page_number, strlen(page_number));  
 if (list_page != This->selected_page)
  XSetForeground(This->display, This->gc,
		 BlackPixel(This->display, DefaultScreen(This->display)));
}

static int calcul_nb_page_visible(This, first)
c_Pager *This;
boolean	first;
{
 int      pager_width;
 int      nb_page;
 c_Page	  *list_page;

 pager_width = This->Pager_pos + 1;
 nb_page = 0;
 if (first)
 {
  list_page = This->first_page_visible;
  pager_width = This->Pager_pos + 1 -This->first_page_pos;
 }
 else
 {
  list_page = This->list_page;
 }
 while(list_page && pager_width < This->Size - 1)
 {
  pager_width += Pager_Icons[get_icon_number(list_page)].width
   + 2*ICON_SEPARATOR;
  if (IS_DOUBLE_PAGE(list_page))
  {
   nb_page ++;
   list_page = list_page->NextPage;
  }
  nb_page ++;
  list_page = list_page->NextPage;
 }
 /*if (list_page) nb_page--;*/
 return nb_page;
}

static int calcul_selected_page_pos(This)
c_Pager *This;
{
 int	pager_width;
 int	nb_page;
 c_Page	*list_page;

 pager_width = This->Pager_pos - This->first_page_pos;
 list_page = This->first_page_visible;

 nb_page = 0;
 while(list_page && nb_page < This->nb_page_visible)
 {
  if (list_page == This->selected_page)
   return pager_width;

  if (IS_DOUBLE_PAGE(list_page))
  {
   if (list_page->NextPage == This->selected_page)
    return pager_width;

   pager_width += Pager_Icons[get_icon_number(list_page)].width
    + 2*ICON_SEPARATOR;
   nb_page ++;
   list_page = list_page->NextPage;
  }
  else
  {
   pager_width += Pager_Icons[get_icon_number(list_page)].width
    + 2*ICON_SEPARATOR;
  }
  nb_page ++;
  list_page = list_page->NextPage;
 }
 return -1;
}

static int get_icon_number(list_page)
c_Page *list_page;
{
 switch(list_page->page_orient)
 {
 case Xc_PORTRAIT:
  if (list_page->page_type == Xc_PAGE_LEFT) 
   return PAGER_PORTRAIT_LEFT;
  if (list_page->page_type == Xc_PAGE_RIGHT)
   return PAGER_PORTRAIT_RIGHT;
  
  if (list_page->page_type == Xc_PAGE_DOUBLE)
  {
   if (list_page->NextPage &&  list_page->NextPage->page_type == 
       (Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT))
    return PAGER_PORTRAIT_DOUBLE;
   else
    return PAGER_PORTRAIT_LEFT;    
  }
  if (list_page->PrevPage &&  list_page->PrevPage->page_type == 
      Xc_PAGE_DOUBLE)
   return PAGER_PORTRAIT_DOUBLE;
  return PAGER_PORTRAIT_RIGHT;

 case Xc_LANDSCAPE:
  if (list_page->page_type == Xc_PAGE_LEFT) 
   return PAGER_LANDSCAPE_LEFT;
  if (list_page->page_type == Xc_PAGE_RIGHT)
   return PAGER_LANDSCAPE_RIGHT;
  
  if (list_page->page_type == Xc_PAGE_DOUBLE)
  {
   if (list_page->NextPage &&  list_page->NextPage->page_type == 
       (Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT))
    return PAGER_LANDSCAPE_DOUBLE;
   else
    return PAGER_LANDSCAPE_LEFT;    
  }
  if (list_page->PrevPage &&  list_page->PrevPage->page_type == 
      Xc_PAGE_DOUBLE)
   return PAGER_LANDSCAPE_DOUBLE;
  
  return PAGER_LANDSCAPE_RIGHT;
 }
 return 0;
}

static c_Page *get_page_by_xy(This, x, y)
c_Pager	*This;
int	x;
int	y;
{
 int		nb_page;
 int		pos_cumul;
 c_Page		*list_page;
  
 if (y < 0 || y > PAGER_SIZE || x < This->Pager_pos || x > This->Size-2)
  return NULL;
  
 nb_page = 0;
 pos_cumul = This->Pager_pos - This->first_page_pos;
 list_page = This->first_page_visible;
 while(list_page && nb_page < This->nb_page_visible)
 {
  if (x >= pos_cumul &&
      x <= pos_cumul + Pager_Icons[get_icon_number(list_page)].width
      + 2*ICON_SEPARATOR)
   return list_page;
  pos_cumul +=  Pager_Icons[get_icon_number(list_page)].width 
   + 2*ICON_SEPARATOR;
  if (IS_DOUBLE_PAGE(list_page))
  {
   nb_page++;
   list_page = list_page->NextPage;
  }
  nb_page++;
  list_page = list_page->NextPage;
 }
 return NULL;
}

static void select_new_page(This, list_page)
c_Pager	*This;
c_Page	*list_page;
{
 hilight_selected_icon(This);

 /*Xc_TRACE(("deselctionne page %d",This->selected_page->page_number));*/
 /*F(This->selected_page->Page).deselect_Page(This->selected_page->Page);*/
  
 if (list_page->page_type == Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT &&
     list_page->PrevPage && list_page->PrevPage->page_type == 
     Xc_PAGE_DOUBLE)
  This->selected_page = list_page->PrevPage;
 else
  This->selected_page = list_page;
 This->selected_page_pos = calcul_selected_page_pos(This);  

 F(This->pagearea).SelectPage(This->pagearea, This->selected_page);
  
 draw_page_number(This, This->selected_page);

 hilight_selected_icon(This);
}   

static c_Page *get_first_page(This)
c_Pager	*This;
{
 return This->list_page; 
}

static c_Page *get_previous_page(This, list_page_def)
c_Pager	*This;
c_Page	*list_page_def;
{
 if (!list_page_def)
  list_page_def = This->selected_page;

 list_page_def = list_page_def->PrevPage;
 if (list_page_def && list_page_def->page_type == 
     Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT &&
     list_page_def->PrevPage &&
     list_page_def->PrevPage->page_type == Xc_PAGE_DOUBLE)
  list_page_def = list_page_def->PrevPage;
 
 return list_page_def;
}

static c_Page *get_next_page(This, list_page_def)
c_Pager	*This;
c_Page	*list_page_def;
{
 if (!list_page_def)
  list_page_def = This->selected_page;

 if (list_page_def->page_type == Xc_PAGE_DOUBLE &&
     list_page_def->NextPage && list_page_def->NextPage->page_type == 
     Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT)
  list_page_def = list_page_def->NextPage;

 if (list_page_def)
  list_page_def = list_page_def->NextPage; 
 return list_page_def;
}

static c_Page *get_last_page(This)
c_Pager	*This;
{
 c_Page	*list_page;
  
 list_page = This->list_page;
 while(list_page->NextPage)
  list_page = list_page->NextPage;
 if (list_page->page_type == Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT &&
     list_page->PrevPage &&
     list_page->PrevPage->page_type == Xc_PAGE_DOUBLE)
  list_page = list_page->PrevPage;
 return list_page; 
}

static void get_last_page_with_first(This)
c_Pager	*This;
{
 c_Page	*list_page;
 int	last_pos;
 int	nb_page;

 list_page = This->first_page_visible;
 last_pos = This->Pager_pos + 1 - This->first_page_pos;
 nb_page = 0;
 while(list_page && last_pos < This->Size - 1)
 {
  last_pos += Pager_Icons[get_icon_number(list_page)].width +
   2*ICON_SEPARATOR;
  if (IS_DOUBLE_PAGE(list_page))
  {
   nb_page++;
   list_page = list_page->NextPage;
  }

  nb_page++;
  list_page = list_page->NextPage;
 }
 This->last_page_visible = list_page;
 This->last_page_pos = This->Size - 2 - last_pos;
 This->nb_page_visible = nb_page;
}

static void get_first_page_with_last(This)
c_Pager	*This;
{
 c_Page	*list_page;
 int	first_pos;
 int	nb_page;

 list_page = This->last_page_visible;
 first_pos = This->Size - 1 - This->last_page_pos;
 nb_page = 1;
 while(list_page && first_pos > This->Pager_pos)
 {
  list_page = list_page->PrevPage;
  if (list_page)
  {
   if (list_page->page_type == Xc_PAGE_DOUBLE + Xc_PAGE_RIGHT &&
       list_page->PrevPage && list_page->PrevPage->page_type == 
       Xc_PAGE_DOUBLE)
   {
    nb_page++;
    list_page = list_page->PrevPage;   
   }
   first_pos -= Pager_Icons[get_icon_number(list_page)].width+
    2*ICON_SEPARATOR;
  }
  nb_page++;
 }
 This->first_page_visible = list_page;
 This->first_page_pos = This->Pager_pos - first_pos;
 This->nb_page_visible = nb_page;
}

static boolean repeat_time_function(This, function)
c_Pager	*This;
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
 wtime2 = WTIME2;
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

