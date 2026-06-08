/*
** PageArea.c for XAllWrite in PageArea/
** Methods for the PageArea class
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
** Started on  Fri Mar  7 18:17:47 1997 Emmanuel Paris
** Last update Wed Nov 11 19:10:38 1998 Emmanuel Paris
*/

/*#define NTRACE*/

#include "PageArea.h"
#include "PageAreaP.h"
#include "Colormap.h"
#include "MainInterface.h"
#include "Box_gotopage.h"
#include "Zoom_PageArea.h"
#include "Dispatch_To_Page.h"
#include "FontManager.h"

extern c_MainInterface *MainInterface1;
extern c_Colormap *GlobColormap;
extern c_FontManager *GlobFntMng;
extern Widget wMainWindow;
extern void XcHandleEvent ___PROTO((XEvent *event));
extern void XcFlushEvents ___PROTO((Widget w, void (*func) 
				    ___PROTO((XEvent *))));

/*--- Prototypes ---*/
static void *cons_PageArea();
static void dest_PageArea();
static void *copy_PageArea();
static void init_PageArea ___PROTO((c_PageArea *This, Display *display,
				    Window window, c_ScrollBarX *scrollbV, 
				    c_ScrollBarX *scrollbH, int PosX, int PosY,
				    int Width, int Height, c_Pager *Pager,
				    c_XaRegle *RegleV, c_XaRegle *RegleH));

static void configure_PageArea ___PROTO((c_PageArea *This));

static void resize_PageArea ___PROTO((c_PageArea *This, int PosX, int PosY,
				      int Width, int Height));

static status_t eventlook_PageArea ___NPROTO((c_PageArea *This, XEvent *Event,
					      listmanaged *listegwm,
					      boolean DocSelected));

static boolean set_function_PageArea ___PROTO((c_PageArea *This,
					       int function));

static void select_page_PageArea ___PROTO((c_PageArea *This,
					   c_Page *page_list));

static boolean is_in_subwindow_PageArea ___PROTO((c_PageArea *This, 
						  Window window));
static void unset_cursor_PageArea ___PROTO((c_PageArea *This));
static void reset_cursor_PageArea ___PROTO((c_PageArea *This));

static void addPage_PageArea ___PROTO((c_PageArea *This));

static void delPage_PageArea ___PROTO((c_PageArea *This, c_Page *from_page));
static void redrawAllPages_PageArea ___PROTO((c_PageArea *This));
static void changeOrigin_PageArea_Relative();
static void changeOrigin_PageArea ___PROTO((c_PageArea *This, int x, int y));
void changeScaling_PageArea ___PROTO((c_PageArea *This, scale_t scale,
				      int x, int y, c_Page *pagel));
void calculAreaSize_PageArea ___PROTO((c_PageArea *This));
c_Page *GetActivePage_PageArea ___PROTO((c_PageArea *This));

static void make_area_visible_PageArea ___PROTO((c_PageArea *This, 
						 c_Page *page, int x, int y, 
						 int width, int height));
static c_AutoScroll *create_autoscroll ___PROTO((c_PageArea *This, 
						 c_Page *page,
						 void (*function)(), 
						 void *data));
static void destroy_autoscroll ___PROTO((c_PageArea *This, 
					 c_AutoScroll *autoscroll));
static c_Page *get_page_by_xy ___PROTO((c_PageArea *This, int x, int y));
static void recalibrate_all ___PROTO((c_PageArea *This));
static void update_PageArea ___PROTO((c_PageArea *This));
static boolean goto_page_PageArea ___PROTO((c_PageArea *This, c_Page **Lpage));

sf_PageArea fc_PageArea =
{
 cons_PageArea, 
 dest_PageArea,
 copy_PageArea,
 init_PageArea,
 resize_PageArea,
 eventlook_PageArea,
 set_function_PageArea,
 select_page_PageArea,
 is_in_subwindow_PageArea,
 unset_cursor_PageArea,
 reset_cursor_PageArea,
 addPage_PageArea,
 delPage_PageArea,
 configure_PageArea,
 make_area_visible_PageArea,
 create_autoscroll,
 destroy_autoscroll,
 get_page_by_xy,
 recalibrate_all,
 update_PageArea,
 goto_page_PageArea,
 readPageArea,
 writePageArea,
 ps_printPageArea
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_PageArea(BaseStd, pagearea_cb)
BaseStd_t    *BaseStd;
pagearea_cb_t *pagearea_cb;
{
 c_PageArea 	*This;
 
 Xc_HISTORY(("constructor"));
 
 if((This = Xc_malloc("PageArea", sizeof(c_PageArea))) == NULL) return NULL;
 memset(This, 0, sizeof(c_PageArea));
 
 This->f = &fc_PageArea;
 
 This->BaseStd = BaseStd;
 
 memcpy(&(This->callbacks), pagearea_cb, sizeof(pagearea_cb_t));
 This->sFactor = SCALE_FROM_PERCENT(100);
 This->view_page = Xa_VIEW_NORMAL_PAGE;
 This->select_type = TRUE;

 This->Calibration = (c_Calibration *)NEW(c_Calibration)(BaseStd, This);
 This->Redraw = (c_Redraw *)NEW(c_Redraw)(BaseStd, This);

 This->PageList = NULL;
 This->PageList =(c_Page *)NEW(c_Page)
  (&This->PageList, BaseStd, This->Calibration, This->Redraw, 
   &This->select_type);
 This->SelectedPage = This->ActivePage = This->PageList;
 This->NbrPage = 1;
 This->FirstPageViewable = This->LastPageViewable = This->PageList;

 This->recto_verso = This->double_page = FALSE;

 This->function = NULL;
 This->function_data = NULL;
 This->ForcedWidth = 0;
 This->NbGutter = 0;

 This->AreaOrig_X = PA_HMARGIN - PA_VMARGIN;
 This->AreaOrig_Y = 0;

 /*--- test: creation de page fictive ---*/
 /* This->recto_verso = TRUE;
    addPage_PageArea(This);
    addPage_PageArea(This);
    addPage_PageArea(This);
    addPage_PageArea(This);
    This->recto_verso = FALSE;
    This->double_page = TRUE;
    addPage_PageArea(This);
    addPage_PageArea(This);
    addPage_PageArea(This);
    addPage_PageArea(This);
    {
    int i;
    
    for(i = 0; i < 50; i++)
    addPage_PageArea(This);
    }
    This->SelectedPage = This->PageList->NextPage->NextPage->NextPage->NextPage;
    */

 return This;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_PageArea(This)
c_PageArea *This;
{
 Xc_HISTORY(("destructor"));
 
 if (This->function_data)
  ABORT(This);
 
 while(This->PageList != NULL)
  DELETE(c_Page)(This->PageList);

 DELETE(c_Redraw)(This->Redraw);
 DELETE(c_Calibration)(This->Calibration);
 if (This->cursor)
  DELETE(c_Cursor)(This->cursor);
 
 if (This->tracing_window != (Window)0)
  XDestroyWindow(This->display, This->tracing_window);
 if (This->window != (Window)0)
  XDestroyWindow(This->display, This->window);
 Xc_free(This);
}


/* ----------------------------------------------------------------- ** 
** Copier                                                            ** 
** ----------------------------------------------------------------- */
static void *copy_PageArea(This)
c_PageArea *This;
{
 c_PageArea *ObjTmp;
 
 Xc_HISTORY(("copy"));
 
 if((ObjTmp = Xc_malloc("CPageArea", sizeof(c_PageArea))) == NULL)
  return NULL;
 
 memcpy(ObjTmp, This, sizeof(c_PageArea));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Init_PageArea                                                      ** 
** ----------------------------------------------------------------- */
static void init_PageArea(This, display, window, scrollbH, scrollbV, 
			  PosX, PosY, Width, Height, Pager, RegleV, RegleH)
c_PageArea	*This;
Display		*display;
Window		window;
c_ScrollBarX	*scrollbV;
c_ScrollBarX	*scrollbH;
int PosX, PosY, Width, Height;
c_Pager		*Pager;
c_XaRegle	*RegleV;
c_XaRegle	*RegleH;
{
 color_cells_t	realcolor;
 long		doc_color;
 long		border_color;
 XSetWindowAttributes attributes;
 
 This->display = display;
 This->doc_window = window;
 
 This->sScale = (coord_t)(((double)COORD_DEFAULT *
			   (double)SCALE_FROM_PERCENT(100) )/
			  (double)This->sFactor);

 F(GlobColormap).allocate_named_color(GlobColormap, PA_COLOR, &realcolor);
 doc_color = realcolor.pixel; 
 F(GlobColormap).allocate_named_color(GlobColormap, PA_SHADOW, &realcolor);
 border_color = realcolor.pixel; 
 
 This->ScrollBarV = scrollbV;
 This->ScrollBarH = scrollbH;
 This->Pager = Pager;
 This->RegleV = RegleV;
 This->RegleH = RegleH;
 
 This->PosX = PosX; This->PosY = PosY; 
 This->Width = Width; This->Height = Height;
 
 This->window = 
  XCreateSimpleWindow(This->display, window,
		      This->PosX, This->PosY,
		      This->Width - 2, This->Height - 2,
		      1, border_color, doc_color);

 XSelectInput(This->display, This->window,  KeyPressMask | KeyReleaseMask |
	      KeymapStateMask | PointerMotionMask | ButtonPressMask | 
	      ButtonReleaseMask | ExposureMask | 
	      LeaveWindowMask | EnterWindowMask);
 XMapWindow(display, This->window);
 
 attributes.background_pixmap = None;
 This->tracing_window = XCreateWindow(display, This->window, 0, 0, 
				      This->Width, This->Height,
				      0, 0, InputOnly, CopyFromParent, 
				      0, &attributes);
 XSelectInput(This->display, This->tracing_window,  KeyPressMask | 
	      KeyReleaseMask | PointerMotionMask | ButtonPressMask | 
	      ButtonReleaseMask | ExposureMask | LeaveWindowMask | 
	      EnterWindowMask);
 This->cursor = (c_Cursor *)NEW(c_Cursor)(This->display, This->tracing_window);

 {
  int			count = 0;
  boolean		left = FALSE;
  c_Page		*pagelist;
  
  pagelist = This->PageList;
  while(pagelist)
  {
   count++;
   Xc_TRACE(("Init page %d\n", pagelist->page_number));
   F(pagelist).Init(pagelist, display, This->window, This->sFactor);
   if (!pagelist->NextPage)
   {
    if ((pagelist->page_type & Xc_PAGE_DOUBLE) == Xc_PAGE_DOUBLE)
     This->double_page = TRUE;
    
    if (left || (pagelist->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT)
     This->recto_verso = TRUE;
   }
   if ((pagelist->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT)
    left = TRUE; else left = FALSE;
   pagelist = pagelist->NextPage;
  }
  This->NbrPage = count;
 }
 
 F(This->ScrollBarV).Set_Callback(This->ScrollBarV, changeOrigin_PageArea,
				  This);
 F(This->ScrollBarH).Set_Callback(This->ScrollBarH, changeOrigin_PageArea,
				  This);
 
 F(This->Redraw).Init(This->Redraw);
 configure_PageArea(This);
 F(This->Calibration).Init(This->Calibration, NULL);
 
 F(This).SetFunction(This, SELECT_MOVE_EDIT_TEXT);
}

static void configure_PageArea(This)
c_PageArea *This;
{
 calculAreaSize_PageArea(This);

 This->callbacks.mapScrollBar(This->callbacks.cbData, This->NeedScrollBarH,
			      This->NeedScrollBarV);

 F(This->ScrollBarV).Change_Rsize
  (This->ScrollBarV, This->display, This->AreaWidth, This->AreaHeight);
 F(This->ScrollBarH).Change_Rsize
  (This->ScrollBarH, This->display, This->AreaWidth, This->AreaHeight);

 
 changeOrigin_PageArea(This, This->AreaOrig_X, This->AreaOrig_Y);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->AreaOrig_Y);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->AreaOrig_X);

 F(This->Pager).Configure(This->Pager);

 F(This->RegleV).Configure(This->RegleV, This->SelectedPage, NULL);
 F(This->RegleH).Configure(This->RegleH, This->SelectedPage, 
			   This->BaseStd->text_base->ruler);
}

static void resize_PageArea(This,  PosX, PosY, Width, Height)
c_PageArea	*This;
int PosX, PosY, Width, Height;
{
 if (This->PosX == PosX && This->PosY == PosY &&
     This->Width == Width && This->Height == Height)
  return;

 This->PosX = PosX; This->PosY = PosY; 
 This->Width = Width; This->Height = Height;

 XMoveResizeWindow(This->display, This->window, PosX, PosY, 
		   Width - 2, Height - 2);
 XResizeWindow(This->display, This->tracing_window, Width - 2, Height - 2);

 calculAreaSize_PageArea(This);
 This->callbacks.mapScrollBar(This->callbacks.cbData, This->NeedScrollBarH,
			      This->NeedScrollBarV);

 F(This->ScrollBarV).Change_Rsize
  (This->ScrollBarV, This->display, This->AreaWidth, This->AreaHeight);
 F(This->ScrollBarH).Change_Rsize
  (This->ScrollBarH, This->display, This->AreaWidth, This->AreaHeight);

 changeOrigin_PageArea(This, This->AreaOrig_X, This->AreaOrig_Y);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->AreaOrig_Y);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->AreaOrig_X);
}

static status_t eventlook_PageArea(This, event)
c_PageArea	*This;
XEvent		*event;
{ 
 status_t	status = FUNCTION_NOT_FINISHED;
 c_Page		*page, *lastpage = NULL;
 int		num_function;

 if (!F(This->ActivePage).IsActive(This->ActivePage, event->xany.window))
 {
  if (!F(This->SelectedPage).IsActive(This->SelectedPage, event->xany.window))
  {
   page = This->FirstPageViewable;
   lastpage = This->LastPageViewable->NextPage;
   
   while(page != lastpage)
   {
    if (F(page).IsActive(page, event->xany.window))
     break;
    page = page->NextPage;
   }
  }
  else
   page = This->SelectedPage;

  if (page != lastpage && page != This->ActivePage)
  {
   if (This->function == &fc_Dispatch_To_Page)
   {
    if (This->ActivePage->in_use) return status;

    num_function = This->ActivePage->function_type;
    if (This->function_data)
     ABORT(This);

    This->ActivePage = page;
    F(page).SetFunction(page, num_function);
    INIT(This);
   }
   else
    This->ActivePage = page;
  }
 }

 switch(event->type)
 {
 case KeyRelease:
  if (This->function)
   status = EVENTLOOK(This, *event);
  break;
 case KeyPress:
  if (This->function)
   status = EVENTLOOK(This, *event);
  break;
 case Expose:
 case GraphicsExpose:
  F(This->ActivePage).Eventlook(This->ActivePage, event);
  break;
 default:
  if (This->function)
   status = EVENTLOOK(This, *event);
  break;
 }
 
 return status;
}

static boolean set_function_PageArea(This, num_function)
c_PageArea	*This;
int		num_function;
{
 boolean       ret_code = FALSE;
 
 if (This->function_data)
  ABORT(This);
 
 switch(num_function)
 {
 case ZOOM_PLUS_PAGE:
  if (This->sFactor < SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT))
  {
   This->function = &fc_Zoom_Plus_PageArea;
   This->function_type = num_function;
  }
  break;
 case ZOOM_MINUS_PAGE:
  if (This->sFactor > SCALE_FROM_PERCENT(ZOOM_MINUS_LIMIT))
  {
   This->function = &fc_Zoom_Minus_PageArea;
   This->function_type = num_function;
  }
  break;
 case ZOOM_IN_ZONE:
  if (This->sFactor < SCALE_FROM_PERCENT(ZOOM_PLUS_LIMIT))
  {
   This->function = &fc_Zoom_PageArea_In_Zone;
   This->function_type = num_function;
  }
  break;
 case ZOOM_FIT_IN_WIDTH:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_fit_in_width(This, GetActivePage_PageArea(This));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_FIT_IN_SCREEN:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_fit_in_screen(This, GetActivePage_PageArea(This));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_AT_REAL_SIZE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_real_size(This, GetActivePage_PageArea(This));
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_25_PAGE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_percent(This, GetActivePage_PageArea(This), 25);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_50_PAGE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_percent(This, GetActivePage_PageArea(This), 50);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_75_PAGE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_percent(This, GetActivePage_PageArea(This), 75);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_100_PAGE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_percent(This, GetActivePage_PageArea(This), 100);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_200_PAGE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_percent(This, GetActivePage_PageArea(This), 200);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case ZOOM_400_PAGE:
  SET_WAIT_MODE_STATIC;
  zoom_pagearea_at_percent(This, GetActivePage_PageArea(This), 400);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case REDRAW_ALL:
  SET_WAIT_MODE_STATIC;
  redrawAllPages_PageArea(This);
  F(This->RegleH).Draw(This->RegleH, TRUE);
  F(This->RegleV).Draw(This->RegleV, TRUE);
  F(This->ScrollBarH).Draw(This->ScrollBarH);
  F(This->ScrollBarV).Draw(This->ScrollBarV);
  F(This->Pager).Draw(This->Pager);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 case DISPLAY_SPEC_CHARS:
  SET_WAIT_MODE_STATIC;
  This->Redraw->spec_chars ^= TRUE;
  redrawAllPages_PageArea(This);
  UNSET_WAIT_MODE;
  ret_code = TRUE;
  break;
 default:
  if (num_function > PAGE_FUNCTION_BEGIN && num_function < FRMG_FUNCTION_END)
  {
   if (!(ret_code = F(This->ActivePage).SetFunction
	 (This->ActivePage, num_function)))
   {
    This->function_type = This->ActivePage->function_type;
    This->function = &fc_Dispatch_To_Page;
   }
  }
 }
 
 Xc_TRACE(("init function %d", This->function_type));
 INIT(This);
 
 return ret_code;
}

static void select_page_PageArea(This, page_list)
c_PageArea	 *This;
c_Page		 *page_list;
{
 int	offset;
 
 if (page_list == This->SelectedPage) return;
 
 This->SelectedPage = page_list;
 
 offset = PA_HMARGIN - PA_VMARGIN;

 This->AreaOrig_X += page_list->PosX - offset;
 This->AreaOrig_Y += page_list->PosY - offset;

 changeOrigin_PageArea(This, This->AreaOrig_X, This->AreaOrig_Y);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->AreaOrig_Y);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->AreaOrig_X);
}

static boolean is_in_subwindow_PageArea(This, window)
c_PageArea *This;
Window window;
{
 c_Page *page, *last_page;

 if (window == This->tracing_window ||
     window == This->ActivePage->X_info.window ||
     window == This->SelectedPage->X_info.window)
  return TRUE;
 
 if (This->FirstPageViewable) 
  page = This->FirstPageViewable;
 else
  page = This->PageList;

 if (This->LastPageViewable)
  last_page = This->LastPageViewable->NextPage;
 else
  last_page = NULL;

 while(page && page != last_page)
 {
  if (page->X_info.window == window)
   return TRUE;
  page = page->NextPage;
 }
 return FALSE;
}

static void unset_cursor_PageArea(This)
c_PageArea *This;
{ 
 c_Page *page, *last_page;
 
 if (This->FirstPageViewable) 
  page = This->FirstPageViewable;
 else
  page = This->PageList;
 
 if (This->LastPageViewable)
  last_page = This->LastPageViewable->NextPage;
 else
  last_page = NULL;
 
 while(page && page != last_page)
 {
  if (page->mapped)
   F(page->cursor).unset_cursor(page->cursor);
  page = page->NextPage;
 }
}

static void reset_cursor_PageArea(This)
c_PageArea *This;
{
 c_Page *page, *last_page;
 
 if (This->FirstPageViewable) 
  page = This->FirstPageViewable;
 else
  page = This->PageList;
 
 if (This->LastPageViewable)
  last_page = This->LastPageViewable->NextPage;
 else
  last_page = NULL;
 
 while(page && page != last_page)
 {
  if (page->mapped)
   F(page->cursor).reset_cursor(page->cursor);
  page = page->NextPage;
 }
}

static void addPage_PageArea(This)
c_PageArea	*This;
{
 c_Page	*pagel;

 pagel = This->PageList;
 while(pagel->NextPage)
  pagel = pagel->NextPage;

 COPY(c_Page)(pagel, This->recto_verso, This->double_page);
 This->NbrPage++;
}

static void delPage_PageArea(This, from_page)
c_PageArea	*This;
c_Page		*from_page;
{
 c_Page		*pagel, *next_page, *prev_page;
 boolean	active_found = FALSE;
 int		num_function = 0;
 
 if (from_page)
  pagel = from_page;
 else
 {
  pagel = This->PageList;
  while(pagel->NextPage)
   pagel = pagel->NextPage;
 }
 
 prev_page = pagel->PrevPage;

 Xc_ASSERT((pagel != This->PageList));
 while(pagel)
 {
  if (pagel == This->ActivePage)
  {
   active_found = TRUE;
   num_function = pagel->function_type;
  }
  next_page = pagel->NextPage;

  if (active_found && This->function_data)
  {
   ABORT(This);
  }

  DELETE(c_Page)(pagel);
  pagel = next_page;
  This->NbrPage--;
 }
 
 if (active_found)
 {
  if (This->function == &fc_Dispatch_To_Page)
  {
   This->ActivePage = prev_page;
   F(prev_page).SetFunction(prev_page, num_function);
   INIT(This);
  }
  else
   This->ActivePage = prev_page;
 }
}

static void redrawAllPages_PageArea(This)
c_PageArea *This;
{
 c_Page *pagel;

 pagel = This->PageList;
 while(pagel)
 {
  F(pagel).RedrawAll(pagel);
  pagel = pagel->NextPage;
 }
 
}

static void make_area_visible_PageArea(This, page, x, y, width, height)
c_PageArea *This;
c_Page *page;
int x;
int y;
int width;
int height;
{
 boolean scrX = FALSE, scrY = FALSE;
 int	newX, newY;

 x += page->PosX;
 y += page->PosY;

 if (x < 0 || x + width > This->Width)
  scrX = TRUE;
 
 if (y < 0 || y + height > This->Height)
  scrY = TRUE;
 
 if (!scrX && !scrY) return;
 
 newX = newY = 0;
 
 if (scrX)
 {
  if (x < 0)
   newX = x - (This->Width >> 2);
  else
   newX = x + width - This->Width + (This->Width >> 2);
 }
 newX += This->AreaOrig_X;

 if (scrY)
 {
  if (y < 0)
   newY = y - (This->Height >> 2);
  else
   newY = y + height - This->Height + (This->Height >> 2);
 }
 newY += This->AreaOrig_Y;
 
 changeOrigin_PageArea(This, newX, newY);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->AreaOrig_Y);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->AreaOrig_X);
}

/* ----------------------------------------------------------------- ** 
** get_page_by_xy: get the page under the pointer.                   ** 
** ----------------------------------------------------------------- */
static c_Page *get_page_by_xy(This, x, y)
c_PageArea *This;
int x;
int y;
{
 c_Page *page, *lastpage;
 int yo;
 
 page = This->FirstPageViewable;
 lastpage = This->LastPageViewable->NextPage;
 
 while(page != lastpage)
 {
  if (x >= page->PosX && x <= page->PosX + page->Width &&
      y >= page->PosY && y <= page->PosY + page->Height)
   return page;
  page = page->NextPage;
 }

 page = This->FirstPageViewable;
 while(page != lastpage)
 {
  if (y < page->PosY + page->Height)
   break;
  page = page->NextPage;
 }

 if (!page)
 {
  if (lastpage)
   page = lastpage->PrevPage;
  else
   page = This->LastPageViewable;

  yo = page->PosY;
  while(page->PrevPage && page->PrevPage->PosY == yo)
   page = page->PrevPage;
 }

 yo = page->PosY;
 while(page != lastpage && page->PosY == yo)
 {
  if (x < page->PosX + page->Width)
   break;
  page = page->NextPage;
 }

 if (page == lastpage) return This->LastPageViewable;
 if (page->PosY != yo) return page->PrevPage;

 return page;
}

/* ------------------------------------------------------------------ ** 
** recalibrate_all: launch a recalibration and redraw of all the text ** 
** ------------------------------------------------------------------ */
static void recalibrate_all(This)
c_PageArea *This;
{
 c_Page *page;

 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 F(This->Redraw).Reset(This->Redraw);
 F(This->Calibration).Init(This->Calibration, NULL);

 page = This->FirstPageViewable;
 while(page && page != This->LastPageViewable->NextPage)
 {
  if (page->mapped) F(page).RedrawAll(page);
  page = page->NextPage;
 }
}

/* ------------------------------------------------------------------ ** 
** update_PageArea : update PageArea, called on undo/redo             **
** ------------------------------------------------------------------ */
static void update_PageArea(This)
c_PageArea *This;
{
 c_Page *page;
 Display *display = This->display;
 Window root, parent, *sons;
 int i, nbson;
  
/*
 DELETE(c_Redraw)(This->Redraw);
 This->Redraw = (c_Redraw *)NEW(c_Redraw)(This->BaseStd, This);
 F(This->Redraw).Init(This->Redraw);

 page = This->PageList;
 while(page)
 {
  page->Redraw = This->Redraw;
  page = page->NextPage;
 }
 */
 F(This->Redraw).Reset(This->Redraw);

 XQueryTree(display, This->window, &root, &parent, &sons, &nbson);

 for(i = 0; i < nbson ; i++)
 {
  if (sons[i] == This->tracing_window) continue;
  page = This->PageList;
  while(page)
  {
   if (sons[i] == page->shadowWin || sons[i] == page->X_info.window) break;
   page = page->NextPage;
  }
  if (!page)			/* kill unused window */
   XDestroyWindow(display, sons[i]);
 }
  
 page = This->PageList;
 while(page)
 {
  i = 0;
  while(i < nbson)
  {
   if (sons[i] == page->shadowWin) break;
   i++;
  }

  if (i == nbson)
   F(page).Init(page, display, This->window, This->sFactor);

  page = page->NextPage;
 }
  
 XFree(sons);

 /* configure_PageArea(This);
    set_function_PageArea(This, REDRAW_ALL); */

 page = This->PageList;
 while(page)
 {
  if (!page->mapped)
  {
   XUnmapWindow(display, page->shadowWin);
   XUnmapWindow(display, page->X_info.window);
  }

  if ((page->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT)
   XMoveResizeWindow(display, page->shadowWin, page->PosX - 5, page->PosY + 5,
		     page->Width, page->Height);
  else
   XMoveResizeWindow(display, page->shadowWin, page->PosX + 5, page->PosY + 5,
		     page->Width, page->Height);
  XMoveResizeWindow(display, page->X_info.window, page->PosX, page->PosY,
		    page->Width, page->Height);
  
  if (page->mapped)
  {
   XMapWindow(display, page->shadowWin);
   XMapWindow(display, page->X_info.window);
  }

  page = page->NextPage;
 }

 configure_PageArea(This);
 set_function_PageArea(This, REDRAW_ALL);
}

/* ----------------------------------------------------------------- ** 
** goto_page_PageArea - launch Box_gotopage - called by pager        ** 
** ----------------------------------------------------------------- */
static boolean goto_page_PageArea(This, Lpage)
c_PageArea *This;
c_Page **Lpage;
{
 c_Box_gotopage *box;
 int page, nb_page;
 c_Page *lpage;
 Xc_TRACE(("goto_Page"));
 
 if (*Lpage)
  page = (*Lpage)->page_number;
 else
  page = 1;
 
 nb_page = This->NbrPage;

 box = (c_Box_gotopage *)NEW(c_Box_gotopage)(MainInterface1->w_Main, 
					     "BoxGotoPage", nb_page, &page);
 
 switch(F(box->Dialog).waitForUnmap(box->Dialog))
 {
 case  XcCD_CANCEL:
 case XcCD_DESTROY:
  DELETE(c_Box_gotopage)(box);
  return FALSE;
 }
 DELETE(c_Box_gotopage)(box);
 
 if (page > nb_page || page == 0) return FALSE;
 
 lpage = This->PageList;
 while(lpage)
 {
  if (lpage->page_number == (unsigned int)page)
   break;

  lpage = lpage->NextPage;
 }
 
 if (lpage && lpage != *Lpage)
 {
  *Lpage = lpage;
  return TRUE;
 }
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** create_autoscroll: create an autoscroll for the specified         ** 
** page.                                                             ** 
** ----------------------------------------------------------------- */
static c_AutoScroll *create_autoscroll(This, cpage, function, data)
c_PageArea *This;
c_Page *cpage;
void (*function)();
void *data;
{
 int	x, y;
 Window rwin;
 
 F(This->ScrollBarV).Set_Callback(This->ScrollBarV, 
				  changeOrigin_PageArea_Relative,
				  This);
 F(This->ScrollBarH).Set_Callback(This->ScrollBarH, 
				  changeOrigin_PageArea_Relative,
				  This);
 
 XTranslateCoordinates(This->display, This->window, 
		       DefaultRootWindow(This->display), 0, 0, &x, &y, &rwin);

 XFlush(This->display);
 Xc_TRACE(("win coord: (%d, %d) (%d, %d)", x, y, 
	   x+This->Width, y+This->Height));
 
 return (c_AutoScroll *)NEW(c_AutoScroll)
  (cpage->X_info.display, x, y, x + This->Width - 3, y + This->Height - 3,
   This->ScrollBarH, This->ScrollBarV, function, data);
}

/* ----------------------------------------------------------------- ** 
** destroy_autoscroll: destroy a previously created autoscroll.      ** 
** ----------------------------------------------------------------- */
static void destroy_autoscroll(This, autoscroll)
c_PageArea *This;
c_AutoScroll *autoscroll;
{
 F(This->ScrollBarV).Set_Callback(This->ScrollBarV, changeOrigin_PageArea,
				  This);
 F(This->ScrollBarH).Set_Callback(This->ScrollBarH, changeOrigin_PageArea,
				  This);
 DELETE(c_AutoScroll)(autoscroll);
}
/* ----------------------------------------------------------------- ** 
** just used with autoscroll (scroll in relativ mode + redraw).      ** 
** ----------------------------------------------------------------- */
static void changeOrigin_PageArea_Relative(This, x, y)
c_PageArea	*This;
int		x, y;
{
 c_Page *page;
 struct 
 {
  int x1, x2, y1, y2;
 } rect1, rect2;
 int xr, yr, widthr, heightr;
 boolean srect = FALSE;

 Xc_TRACE(("change Origin Relative: +x: %d +y: %d", x, y));

 if (This->AreaOrig_X + x + This->Width > This->AreaWidth)
  x = This->AreaWidth - This->AreaOrig_X - This->Width;
 if (This->AreaOrig_Y + y + This->Height > This->AreaHeight)
  y = This->AreaHeight - This->AreaOrig_Y - This->Height;
 if (This->AreaOrig_X + x < 0) x = -This->AreaOrig_X;
 if (This->AreaOrig_Y + y < 0) y = -This->AreaOrig_Y;

 if (x == 0 && y == 0) return;

 if (x == 0)
 {
  if (y < 0)
  {
   rect1.y1 = 0;
   rect1.y2 = -y;
  }
  else
  {
   rect1.y2 = This->Height - 2;
   rect1.y1 = rect1.y2 - y - 2;
  }
  rect1.x1 = 0;
  rect1.x2 = This->Width;
 }
 else 
 {
  if (x < 0)
  {
   rect1.x1 = 0;
   rect1.x2 = -x;
  }
  else
  {
   rect1.x2 = This->Width - 2;
   rect1.x1 = rect1.x2 - x - 2;
  }
   
  if (y == 0)
  {
   rect1.y1 = 0;
   rect1.y2 = This->Height;
  }
  else
  {
   rect2.x1 = 0;
   rect2.x2 = This->Width;
   srect = TRUE;

   if (y < 0)
   {
    rect2.y1 = 0;
    rect1.y1 = rect2.y2 = -y;
    rect1.y2 = This->Height - 2;
   }
   else
   {
    rect1.y1 = 0;
    rect2.y2 = This->Height - 2;
    rect1.y2 = rect2.y1 = rect2.y2 - y - 2;
   }
  }
 }
 
 changeOrigin_PageArea(This, This->AreaOrig_X + x, This->AreaOrig_Y + y);
 
 page = This->FirstPageViewable;
 while(page != This->LastPageViewable->NextPage)
 {
  
  if (!(page->PosX > rect1.x2 || page->PosX + page->Width < rect1.x1 ||
	page->PosY > rect1.y2 || page->PosY + page->Height < rect1.y1))
  {
   xr = MAX(0, rect1.x1 - page->PosX);
   widthr = MIN(page->Width, rect1.x2 - page->PosX) - xr + 1;
   yr = MAX(0, rect1.y1 - page->PosY);
   heightr = MIN(page->Height, rect1.y2 - page->PosY) - yr + 1;
   
   F(page).RedrawRectangle(page, xr, yr, widthr, heightr);
  }
  
  if (srect && 
      !(page->PosX > rect2.x2 || page->PosX + page->Width < rect2.x1 ||
	page->PosY > rect2.y2 || page->PosY + page->Height < rect2.y1))
  {
   xr = MAX(0, rect2.x1 - page->PosX);
   widthr = MIN(page->Width, rect2.x2 - page->PosX) - xr + 1;
   yr = MAX(0, rect2.y1 - page->PosY);
   heightr = MIN(page->Height, rect2.y2 - page->PosY) - yr + 1;

   F(page).RedrawRectangle(page, xr, yr, widthr, heightr);   
  }
  page = page->NextPage;
 }
}

static void changeOrigin_PageArea(This, x, y)
c_PageArea	*This;
int		x, y;
{
 c_Page		*page;
 int		gutter, sx, xx, yy;
 int		xo, yo, xd, yd, w;
 int		maxwidth, maxy, rwidth;
 int		maxheight, tmpwidth;

 Xc_TRACE(("change Origin Absolute: x: %d y: %d", x, y));

 page = This->PageList;
 gutter = PA_GUTTER;

 if (x + This->Width > This->AreaWidth)
  x = This->AreaWidth - This->Width;
 if (y + This->Height > This->AreaHeight)
  y = This->AreaHeight - This->Height;
 if (x < 0) x = 0;
 if (y < 0) y = 0;
 
 xo = This->AreaOrig_X = x; xd = xo + This->Width;
 yo = This->AreaOrig_Y = y; yd = yo + This->Height;

 This->FirstPageViewable = This->LastPageViewable = NULL;
 
 sx = PA_HMARGIN;
 y = PA_VMARGIN;

 rwidth = This->AreaWidth - 2 * sx;

 switch(This->view_page)
 {
 case Xa_VIEW_SINGLE_PAGE:
  while(page)
  {
   w = page->Width;
   x = sx + ((rwidth - w) >> 1);
   xx = x + w;
   yy = y + page->Height;
   
   if (x > xd || xx < xo || y > yd || yy < yo)
   {
    F(page).MapWindow(page, FALSE);
    F(page).MoveWindow(page, x - xo, y - yo);
   }
   else
   {
    F(page).MoveWindow(page, x - xo, y - yo);
    F(page).MapWindow(page, TRUE);
    F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			      MIN(xx, xd) - xo, MIN(yy, yd) - yo);
    
    if (!This->FirstPageViewable) 
     This->FirstPageViewable = page;
    This->LastPageViewable = page;
   }
   y = yy + gutter;
   page = page->NextPage;
  }
  break;
 case Xa_VIEW_NORMAL_PAGE:
  while(page)
  {
   if (((page->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT) &&
       (page->NextPage) && 
       ((page->NextPage->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_RIGHT))
   {
    w = page->Width;
    if ((page->page_type & Xc_PAGE_DOUBLE) == 0)
     w += gutter;
    maxwidth = w + page->NextPage->Width;
    
    x = sx + ((rwidth - maxwidth) >> 1);
    xx = x + page->Width;
    yy = maxy = y + page->Height;

    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }

    page = page->NextPage;
    x += w;
    xx = x + page->Width;
    yy = y + page->Height;
    
    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }
    if (yy < maxy) yy = maxy;
   }
   else
   {
    w = page->Width;
    x = sx + ((rwidth - w) >> 1);
    xx = x + w;
    yy = y + page->Height;
    
    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     Xc_TRACE(("Move window to: %d %d", x - xo, y - yo));
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }
   }
   y = yy + gutter;
   page = page->NextPage;
  }  
  break;
 case Xa_VIEW_TWO_PAGES:
  while(page)
  {
   if (((page->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_RIGHT) &&
       (((page->NextPage) && 
	 ((page->NextPage->page_type & ~Xc_PAGE_DOUBLE) != Xc_PAGE_RIGHT)) ||
	(!page->NextPage)))
   {
    w = page->Width;
    x = sx + ((rwidth - w) >> 1);
    xx = x + w;
    yy = y + page->Height;
    
    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }
   }
   else
   {
    w = page->Width;
    if ((page->page_type & Xc_PAGE_DOUBLE) == 0)
     w += gutter;
    maxwidth = w + page->NextPage->Width;
    
    x = sx + ((rwidth - maxwidth) >> 1);
    xx = x + page->Width;
    yy = maxy = y + page->Height;

    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }

    page = page->NextPage;
    x += w;
    xx = x + page->Width;
    yy = y + page->Height;
    
    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }
    if (yy < maxy) yy = maxy;    
   }
   y = yy + gutter;
   page = page->NextPage;
  }  
  break;
 case Xa_VIEW_MULTIPLE_PAGES:
  if (This->ForcedWidth != 0)
   rwidth = This->ForcedWidth - 2 * sx;
  else
   rwidth = This->Width - 2 * sx;
  while(page)
  {
   maxwidth = page->Width;
   maxheight = page->Height;
   
   x = sx;
   xx = x + maxwidth;
   yy = y + page->Height;
   
   if (x > xd || xx < xo || y > yd || yy < yo)
   {
    F(page).MapWindow(page, FALSE);
    F(page).MoveWindow(page, x - xo, y - yo);
   }
   else
   {
    F(page).MoveWindow(page, x - xo, y - yo);
    F(page).MapWindow(page, TRUE);
    F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			      MIN(xx, xd) - xo, MIN(yy, yd) - yo);
    if (!This->FirstPageViewable) 
     This->FirstPageViewable = page;
    This->LastPageViewable = page;
   }
   
   if (((page->page_type & Xc_PAGE_DOUBLE) != 0) && 
       (page->NextPage) && 
       ((page->NextPage->page_type & Xc_PAGE_DOUBLE) != 0))
   {
    page = page->NextPage;      
    
    maxwidth += page->Width;
    if (maxheight < page->Height) maxheight = page->Height;

    x = xx;
    xx += page->Width;
    yy = y + page->Height;
    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }
   }
   
   page = page->NextPage;      

   tmpwidth = maxwidth;
   while(page)
   {
    w = tmpwidth + gutter;
    tmpwidth += gutter + page->Width;
    
    if (((page->page_type & Xc_PAGE_DOUBLE) != 0) && 
	(page->NextPage) && 
	((page->NextPage->page_type & Xc_PAGE_DOUBLE) != 0))
    {
     tmpwidth += page->NextPage->Width;
    }
    
    if (tmpwidth > rwidth)
     break;

    x = w + sx;
    xx = x + page->Width;
    yy = y + page->Height;
    if (x > xd || xx < xo || y > yd || yy < yo)
    {
     F(page).MapWindow(page, FALSE);
     F(page).MoveWindow(page, x - xo, y - yo);
    }
    else
    {
     F(page).MoveWindow(page, x - xo, y - yo);
     F(page).MapWindow(page, TRUE);
     F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
			       MIN(xx, xd) - xo, MIN(yy, yd) - yo);
     if (!This->FirstPageViewable) 
      This->FirstPageViewable = page;
     This->LastPageViewable = page;
    }
   
    if (maxheight < page->Height) maxheight = page->Height;

    if (((page->page_type & Xc_PAGE_DOUBLE) != 0) && 
	(page->NextPage) && 
	((page->NextPage->page_type & Xc_PAGE_DOUBLE) != 0))
    {
     page = page->NextPage;      
     
     x = xx;
     xx = x + page->Width;
     yy = y + page->Height;
     if (x > xd || xx < xo || y > yd || yy < yo)
     {
      F(page).MapWindow(page, FALSE);
      F(page).MoveWindow(page, x - xo, y - yo);
     }
     else
     {
      F(page).MoveWindow(page, x - xo, y - yo);
      F(page).MapWindow(page, TRUE);
      F(page).UpdateVisiblePart(page, MAX(x, xo) - xo, MAX(y, yo) - yo, 
				MIN(xx, xd) - xo, MIN(yy, yd) - yo);
      if (!This->FirstPageViewable) 
       This->FirstPageViewable = page;
      This->LastPageViewable = page;
     }
     if (maxheight < page->Height) maxheight = page->Height;
    }

    page = page->NextPage;      
   }
   
   y += maxheight + gutter;
  }
  break;
 }

 F(This->RegleV).Configure(This->RegleV, This->SelectedPage, NULL);
 F(This->RegleH).Configure(This->RegleH, This->SelectedPage, NULL);
}

void changeScaling_PageArea(This, scale, x, y, page_active)
c_PageArea	*This;
scale_t		scale;
int		x;
int		y;
c_Page	*page_active;
{
 c_Page	*pagel;
 scale_t factor_ratio;

 if (This->sFactor == scale)
 {
  if (page_active)
   changeOrigin_PageArea(This, This->AreaOrig_X + page_active->PosX + x,
			 This->AreaOrig_Y + page_active->PosY + y);
  else
   changeOrigin_PageArea(This, x, y);

  F(This->ScrollBarV).AMove(This->ScrollBarV, This->AreaOrig_Y);
  F(This->ScrollBarH).AMove(This->ScrollBarH, This->AreaOrig_X);
  
  return;
 }
 
 F(This->Redraw).Reset(This->Redraw);
 factor_ratio = scale / This->sFactor;
 This->sFactor = scale;
 This->sScale = (coord_t)(((double)COORD_DEFAULT *
			   (double)SCALE_FROM_PERCENT(100) )/
			  (double)scale);
 
 pagel = This->PageList;
 while(pagel)
 {
  F(pagel).ChangeScaling(pagel, scale);
  pagel = pagel->NextPage;
 }

 This->GuestPage = page_active;

 if (page_active)
 {
  This->ForcedWidth = 0;
  calculAreaSize_PageArea(This);
  x += This->GuestX;
  y += This->GuestY;
 }
 else
 {
  int aw, ah, mg;

  mg = 2 * PA_HMARGIN + This->NbGutter * PA_GUTTER;
  aw = This->AreaWidth; ah = This->AreaHeight;
  if (This->ForcedWidth != 0)
   This->ForcedWidth = (This->ForcedWidth - mg) * factor_ratio + mg;
  else
   This->ForcedWidth = (This->Width - mg) * factor_ratio + mg;
  calculAreaSize_PageArea(This);

  x = (x - PA_HMARGIN) * This->AreaWidth/ aw + PA_HMARGIN - (This->Width>>1);
  y = (y - PA_VMARGIN) * This->AreaHeight/ ah + PA_VMARGIN - (This->Height>>1);
 }

 F(This->ScrollBarV).Change_Rsize
  (This->ScrollBarV, This->display, This->AreaWidth, This->AreaHeight);
 F(This->ScrollBarH).Change_Rsize
  (This->ScrollBarH, This->display, This->AreaWidth, This->AreaHeight);
 
 This->callbacks.mapScrollBar(This->callbacks.cbData, This->NeedScrollBarH,
			      This->NeedScrollBarV);

 F(This->RegleV).Init_XaRegle_By_Page(This->RegleV, This->SelectedPage);
 F(This->RegleH).Init_XaRegle_By_Page(This->RegleH, This->SelectedPage);
 
 Xc_TRACE(("AreaOrigin: %d %d AreaSize: %d %d", x, y, 
	   This->AreaWidth, This->AreaHeight));

 changeOrigin_PageArea(This, x, y);
 F(This->ScrollBarV).AMove(This->ScrollBarV, This->AreaOrig_Y);
 F(This->ScrollBarH).AMove(This->ScrollBarH, This->AreaOrig_X);

 Xc_TRACE(("Scroll Pos: %d %d", This->AreaOrig_X, This->AreaOrig_Y));
}

void calculAreaSize_PageArea(This)
c_PageArea	*This;
{
 int	gutter, width, height, mxnbg, nbg;
 int	maxwidth, maxheight, pawidth, tmpwidth;
 c_Page		*page, *guest;
 
 Xc_TRACE(("calculAreaSize"));

 page = This->PageList;
 guest = This->GuestPage;

 width = height = 0;
 gutter = PA_GUTTER;
 nbg = 0;			/* number of gutter */

 switch(This->view_page)
 {
 case Xa_VIEW_SINGLE_PAGE:
  while(page)
  {
   if (page == guest)
   {
    This->GuestX = 0; This->GuestY = height;
   }

   height += page->Height + gutter;
   if (width < page->Width)
    width = page->Width;
   page = page->NextPage;
  }
  break;
 case Xa_VIEW_NORMAL_PAGE:
  while(page)
  {
   if (page == guest)
   {
    This->GuestX = 0; This->GuestY = height;
   }
   if (((page->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_LEFT) &&
       (page->NextPage) && 
       ((page->NextPage->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_RIGHT))
   {
    maxwidth = page->Width;
    if ((page->page_type & Xc_PAGE_DOUBLE) == 0)
     maxwidth += gutter;
    maxheight = page->Height;
    page = page->NextPage;
    if (page == guest)
    {
     This->GuestX = maxwidth; This->GuestY = height;
    }
    maxwidth += page->Width;
    if (width < maxwidth)
     width = maxwidth;
    if (maxheight < page->Height)
     maxheight = page->Height;
    height += maxheight + gutter;
   }
   else
   {
    height += page->Height + gutter;
    if (width < page->Width)
     width = page->Width;
   }
   page = page->NextPage;
  }  
  break;
 case Xa_VIEW_TWO_PAGES:
  while(page)
  {
   if (page == guest)
   {
    This->GuestX = 0; This->GuestY = height;
   }
   if (((page->page_type & ~Xc_PAGE_DOUBLE) == Xc_PAGE_RIGHT) &&
       (((page->NextPage) && 
	 ((page->NextPage->page_type & ~Xc_PAGE_DOUBLE)
	  != Xc_PAGE_RIGHT)) ||
	(!page->NextPage)))
   {
    height += page->Height + gutter;
    if (width < page->Width)
     width = page->Width;
   }
   else
   {
    maxwidth = page->Width;
    if ((page->page_type & Xc_PAGE_DOUBLE) != 0)
     maxwidth += gutter;
    page = page->NextPage;
    if (page == guest)
    {
     This->GuestX = maxwidth; This->GuestY = height;
    }
    maxwidth += page->Width;
    if (width < maxwidth)
     width = maxwidth;
    maxheight = page->Height;
    if (maxheight < page->Height)
     maxheight = page->Height;
    height += maxheight + gutter;
   }
   page = page->NextPage;   
  }
  nbg = 1;
  break;
 case Xa_VIEW_MULTIPLE_PAGES:
  if (This->ForcedWidth != 0)
   pawidth = This->ForcedWidth - 2 * PA_HMARGIN;
  else
   pawidth = This->Width - 2 * PA_HMARGIN;

  while(page)
  {
   if (page == guest)
   {
    This->GuestX = 0; This->GuestY = height;
   }
   maxwidth = page->Width;
   maxheight = page->Height;
   
   if (((page->page_type & Xc_PAGE_DOUBLE) != 0) && 
       (page->NextPage) && 
       ((page->NextPage->page_type & Xc_PAGE_DOUBLE) != 0))
   {
    page = page->NextPage;      
    if (page == guest)
    {
     This->GuestX = maxwidth; This->GuestY = height;
    }    
    maxwidth += page->Width;
    if (maxheight < page->Height) maxheight = page->Height;
   }
   
   page = page->NextPage;      

   mxnbg = 0;
   tmpwidth = maxwidth;
   while(page)
   {
    tmpwidth += gutter + page->Width;

    if (((page->page_type & Xc_PAGE_DOUBLE) != 0) && 
	(page->NextPage) && 
	((page->NextPage->page_type & Xc_PAGE_DOUBLE) != 0))
    {
     tmpwidth += page->NextPage->Width;
    }
    
    if (tmpwidth > pawidth)
     break;

    if (page == guest)
    {
     This->GuestX = maxwidth + gutter; This->GuestY = height;
    }
    maxwidth = tmpwidth; mxnbg++;

    if (maxheight < page->Height) maxheight = page->Height;

    if (((page->page_type & Xc_PAGE_DOUBLE) != 0) && 
	(page->NextPage) && 
	((page->NextPage->page_type & Xc_PAGE_DOUBLE) != 0))
    {
     page = page->NextPage;      
     if (page == guest)
     {
      This->GuestX = maxwidth - page->Width; This->GuestY = height;
     }
     if (maxheight < page->Height) maxheight = page->Height;
    }

    page = page->NextPage;      
   }
   
   if (width < maxwidth) width = maxwidth;
   if (nbg < mxnbg) nbg = mxnbg;

   height += maxheight + gutter;
  }
  break;
 }
 height = height - gutter + 2 * PA_VMARGIN;
 width += 2 * PA_HMARGIN;

 This->GuestX += PA_HMARGIN;
 This->GuestY += PA_VMARGIN;

 if (height < This->Height) 
 {
  This->GuestY += (This->Height - height) >> 1;
  /*height = This->Height;*/
  This->NeedScrollBarV = FALSE;
 }
 else
  This->NeedScrollBarV = TRUE;
 
 if (width < This->Width)
 {
  This->GuestX += (This->Width - width) >> 1;
  /*width = This->Width;*/
  This->NeedScrollBarH = FALSE;
 }
 else
  This->NeedScrollBarH = TRUE;
 
 This->AreaWidth = width;
 This->AreaHeight = height;
 This->NbGutter = nbg;

 Xc_TRACE(("AreaSize: Width: %d Height: %d", 
	   This->AreaWidth, This->AreaHeight));
}

c_Page *GetActivePage_PageArea(This)
c_PageArea *This;
{
 c_Page *page, *retpgl = NULL;
 int x, y, x2, y2;
 long maxarea = 0, area;
 
 if (This->SelectedPage->mapped)
  return This->SelectedPage;
 
 page = This->PageList;
 while(page)
 {
  if (page->mapped)
  {
   x = MAX(page->PosX, 0);
   y = MAX(page->PosY, 0);
   
   x2 = MIN(page->PosX + page->Width, This->Width);
   y2 = MIN(page->PosY + page->Height, This->Height);
   
   area = (x2 - x) * (y2 - y);
   if (area > maxarea)
   {
    maxarea = area;
    retpgl = page;
   }
  }
  page = page->NextPage;
 }
 return retpgl;
}
