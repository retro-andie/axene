/*
** Redraw.c for XAllWrite in Redraw/
** Redraw object: redraw text along pages.
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
** Started on  Sun May  4 17:39:48 1997 Emmanuel Paris
** Last update Sun Nov 22 18:29:23 1998 Emmanuel Paris
*/

#define NTRACE

#include "Redraw.h"
#include "VectorFont.h"
#include "RasterFont.h"
#include "CharMetrics.h"
#include "FontManager.h"
#include "Colormap.h"
#include "Resource.h"
#include "Undo.h"

static void *cons_Redraw();
static void dest_Redraw();
static void *copy_Redraw();
static void init_Redraw();
static void reset_Redraw();

static void set_clipping ___PROTO((c_Redraw *This, Pixmap clip_mask, 
				   int clip_x, int clip_y, 
				   Polygone *clip_over, Polygone *clip_under));
static void redraw_rectangle ___PROTO((c_Redraw *This, c_Page *page,
				       int x, int y, int width, int height,
				       boolean selection));
static void redraw_polygone ___PROTO((c_Redraw *This, c_Page *page,
				      Polygone *polygone, boolean selection));
static void display_selection ___PROTO((c_Redraw *This, c_Page *page));
static void undisplay_selection ___PROTO((c_Redraw *This, c_Page *page));
static void redraw_text_line_diff ___PROTO((c_Redraw *This, c_Page *page,
					    text_line_t *new_line,
					    text_line_t *old_line));
static void start_draw_cache ___PROTO((c_Redraw *This));
static void flush_redraw_cache ___PROTO((c_Redraw *This, c_Page *page));
static void start_selection_diff ___PROTO((c_Redraw *This));
static void draw_selection_diff ___PROTO((c_Redraw *This, c_Page *page));

static void clear_from_text_spot ___PROTO((c_Redraw *This, c_Page *page,
					   text_line_t *tl, text_spot_t *ts,
					   int pos));
static void clear_text_line ___PROTO((c_Redraw *This, c_Page *page,
				      text_line_t *tl, text_spot_t *ts));
static void draw_from_text_spot ___PROTO((c_Redraw *This, c_Page *page,
					  text_line_t *tl, text_spot_t *ts,
					  int pos));
static void draw_text_spot ___PROTO((c_Redraw *This, c_Page *page,
				     text_line_t *tl, text_spot_t *ts));
static void draw_text_line ___PROTO((c_Redraw *This, c_Page *page,
				     text_line_t *tl, text_spot_t *ts));
static void update_font ___PROTO((c_Redraw *This, c_TextStyle *tstyle,
				  scale_t factor));
static void calcul_selection_height ___PROTO((text_line_t *tl,
					      xa_item_t *it1, xa_item_t *it2, 
					      coord_t *y, coord_t *height));
static coord_t get_x_by_item ___PROTO((text_line_t *tl, text_spot_t *ts,
				       xa_item_t *item, int pos));
static boolean get_item_by_x ___PROTO((text_line_t *tl, coord_t x, 
				       xa_item_t **item, int *pos));
static boolean is_within_textline ___PROTO((text_line_t *tl, coord_t x,
					    coord_t tolerance));
static boolean is_last_pos_in_textline ___PROTO((text_line_t *tl, 
						 xa_item_t *item, int pos));
___INLINE static coord_t get_x_min ___PROTO((text_line_t *tl));
___INLINE static coord_t get_x_max ___PROTO((text_line_t *tl));

extern c_FontManager	*GlobFntMng;
extern c_Colormap	*GlobColormap;
extern c_VectorFont	*GlobFontBase;
extern c_Resource	*GlobResources;

#define CHECK_CLIPPING(page) \
if (This->clipped_page != page) \
{ \
   F(page).CalculClipping(page); \
    This->clipped_page = page; \
}

sf_Redraw fc_Redraw =
{
 cons_Redraw,
 dest_Redraw,
 copy_Redraw,
 init_Redraw,
 reset_Redraw,
 set_clipping,
 redraw_rectangle,
 redraw_polygone,
 display_selection,
 undisplay_selection,
 redraw_text_line_diff,
 start_draw_cache,
 flush_redraw_cache,
 start_selection_diff,
 draw_selection_diff,
 get_x_by_item,
 get_item_by_x,
 is_within_textline,
 is_last_pos_in_textline
};

static void *cons_Redraw(BaseStd, PageArea)
BaseStd_t       *BaseStd;
c_PageArea	*PageArea;
{
 c_Redraw *This;
 c_VectorFont	*vfont;
 Xc_HISTORY(("Constructor Redraw..."));

 if((This = (c_Redraw *)Xc_malloc("Redraw", sizeof (c_Redraw))) == NULL)
  return NULL;
 memset(This, 0, sizeof(c_Redraw));
 This->f= &fc_Redraw;

 This->BaseStd = BaseStd;
 This->PageArea = PageArea;
 
 This->BlackColor = (c_Color *)NEW(c_Color)(&This->BlackColor, NULL);
 vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Helvetica");
 if(!vfont) vfont = GlobFontBase;
 This->LabelStyle = (c_TextStyle *)NEW(c_TextStyle)
  (&This->LabelStyle, NULL, vfont, SCALE_FROM_POINTS(8), This->BlackColor);

 This->style = NULL;
 This->rfont = NULL;
 This->metrics = NULL;
 This->spec_chars = FALSE;

 This->redraw_cache = FALSE;
 
 This->select_handle = Xc_createItem("sh", 10, sizeof(select_by_page_t));
 This->selection = NULL;
 This->old_selection = NULL;

 This->clip_mask = None;
 This->clip_x = This->clip_y = 0;;
 This->clipped_page = NULL;
 This->current_clip_valid = FALSE;
 
 This->clip_over = This->clip_under = NULL;

 Xc_TRACE(("Constructor Redraw done"));
 return This;
}

static void dest_Redraw(This)
c_Redraw *This;
{ 
 select_by_page_t *selection, *sel_ptr;
 Xc_HISTORY(("Destructor Redraw..."));

 if (This->gc_backg)
  XFreeGC(This->display, This->gc_backg);
 if (This->gc_mask)
  XFreeGC(This->display, This->gc_mask);
 if (This->gc_paint)
  XFreeGC(This->display, This->gc_paint);
 if (This->gc_draw)
  XFreeGC(This->display, This->gc_draw);
 if (This->gc_select)
  XFreeGC(This->display, This->gc_select);
 if (This->gc_clear)
  XFreeGC(This->display, This->gc_clear);

 if (This->clip_mask != None)
  XFreePixmap(This->display, This->clip_mask);

 if (This->clip_over)  FreePolygone(This->clip_over,  DEL_POLYGONE);
 if (This->clip_under) FreePolygone(This->clip_under, DEL_POLYGONE);

 selection = This->selection;
 while(selection)
 {
  sel_ptr = selection->next;
  Xc_free(selection->rect_base);
  Xc_delete(This->select_handle, selection); 
  selection = sel_ptr;
 }

 Xc_destroyItem(This->select_handle);

 DELETE(c_TextStyle)(This->LabelStyle);
 while(This->BlackColor->next)
  DELETE(c_Color)(This->BlackColor->next);
 DELETE(c_Color)(This->BlackColor);

 Xc_free(This);
 Xc_TRACE(("Redraw Destroyed"));
}

static void *copy_Redraw(Orig)
c_Redraw *Orig;
{
 Xc_TRACE(("Copy Redraw not done."));
 return NULL;
}

static void init_Redraw(This)
c_Redraw *This;
{
 XGCValues xgc;
 
 This->display = This->PageArea->display;
 This->window = This->PageArea->window;
 
 xgc.foreground = GlobColormap->Cwhite;
 xgc.background = GlobColormap->Cwhite;
 This->gc_clear = XCreateGC(This->display, This->window, 
			    GCForeground | GCBackground, &xgc);
 
 xgc.foreground = xgc.background = GlobColormap->Cblack;
 This->gc_paint = XCreateGC(This->display, This->window, 
			    GCForeground | GCBackground, &xgc);

 This->gc_backg = XCreateGC(This->display, This->window, 
			    GCForeground | GCBackground, &xgc);

 This->gc_mask =  XCreateGC(This->display, This->window, 
			    GCForeground | GCBackground, &xgc);

 This->gc_draw = XCreateGC(This->display, This->window, 
			   GCForeground | GCBackground, &xgc);
 if (GlobColormap->Cblack)
  xgc.function = GXxor;
 else
  xgc.function = GXinvert;
 xgc.plane_mask= -1;
 xgc.background = GlobColormap->Cwhite;
 xgc.foreground = GlobColormap->Cblack;
 This->gc_select = XCreateGC(This->display, This->window, 
			     GCForeground | GCBackground | GCFunction, &xgc);
}

static void reset_Redraw(This)
c_Redraw *This;
{
 XGCValues xgcv;

 XGetGCValues(This->display, This->gc_paint, GCForeground, &xgcv);
 xgcv.foreground ^= (unsigned int)-1;
 XChangeGC(This->display, This->gc_paint, GCForeground, &xgcv);

 XGetGCValues(This->display, This->gc_backg, GCForeground, &xgcv);
 xgcv.foreground ^= (unsigned int)-1;
 XChangeGC(This->display, This->gc_backg, GCForeground, &xgcv);

 This->style = NULL;
 This->rfont = NULL;
 This->metrics = NULL;
}

static void set_clipping(This, clip_mask, clip_x, clip_y, 
			 clip_over, clip_under)
c_Redraw *This;
Pixmap clip_mask;
int clip_x;
int clip_y;
Polygone *clip_over;
Polygone *clip_under;
{ 
 if (clip_mask != None)
  REGISTER_PIXMAP(clip_mask);

 if (This->clip_mask != None)
  if (UNREGISTER_PIXMAP(This->clip_mask))
   XFreePixmap(This->display, This->clip_mask);
 
 if (This->clip_over)  FreePolygone(This->clip_over,  DEL_POLYGONE);
 if (This->clip_under) FreePolygone(This->clip_under, DEL_POLYGONE);

 This->clip_over = clip_over;
 This->clip_under = clip_under;

 if (This->clip_mask != clip_mask)
 {
  This->clip_mask = clip_mask;
  
  XSetClipMask(This->display, This->gc_clear, clip_mask);
  XSetClipMask(This->display, This->gc_backg, clip_mask);
  XSetClipMask(This->display, This->gc_paint, clip_mask);
  XSetClipMask(This->display, This->gc_mask, clip_mask);
  XSetClipMask(This->display, This->gc_select, clip_mask);  
 }
 
 if (This->clip_x != clip_x || This->clip_y != clip_y)
 {
  This->clip_x = clip_x;
  This->clip_y = clip_y;
  
  XSetClipOrigin(This->display, This->gc_clear, clip_x, clip_y);
  XSetClipOrigin(This->display, This->gc_backg, clip_x, clip_y);
  XSetClipOrigin(This->display, This->gc_paint, clip_x, clip_y);
  XSetClipOrigin(This->display, This->gc_mask, clip_x, clip_y);
  XSetClipOrigin(This->display, This->gc_select, clip_x, clip_y);
 }

}

static void redraw_rectangle(This, page, x, y, width, height, selection)
c_Redraw *This;
c_Page	*page;
int x;
int y;
int width;
int height;
boolean selection;
{
 coord_t tmp, sxo, sxd, syo, syd;
 text_line_t	*tl, *tlend;
 text_spot_t	*ts;

 if (!page->mapped) return;
 
 tmp = x * page->sScale - COORD_HALF_ONE;
 sxo = MAX(tmp, page->vx);
 tmp += width * page->sScale;
 sxd = MIN(tmp, page->vx + page->vwidth);
 if (sxo > sxd) return;

 tmp = y * page->sScale - COORD_HALF_ONE;
 syo = MAX(tmp, page->vy);
 tmp += height * page->sScale + COORD_ONE;
 syd = MIN(tmp, page->vy + page->vheight);
 if (syo > syd) return;
 
 Xc_TRACE(("x: %d y: %d width: %d height: %d factor: %d vx: %d vy: %d vwidth: %d vheight: %d sxo: %d syo: %d sxd: %d syd: %d", x, y, width, height, page->sScale, page->vx, page->vy, page->vwidth, page->vheight, sxo, syo, sxd, syd));

 tl = page->text_line_start;
 tlend = page->text_line_end;
 if (!tl || !tlend) return;
 
 CHECK_CLIPPING(page);
 
 This->current_clip_valid = TRUE;
 if (This->clip_mask == None)
 {
  This->current_clip_rect.x = x;
  This->current_clip_rect.y = y;
  This->current_clip_rect.width = width;
  This->current_clip_rect.height = height;

  XSetClipRectangles(This->display, This->gc_backg, 0, 0, 
		     &This->current_clip_rect, 1, YXSorted);
  XSetClipRectangles(This->display, This->gc_paint, 0, 0, 
		     &This->current_clip_rect, 1, YXSorted);
  XSetClipRectangles(This->display, This->gc_mask, 0, 0, 
		      &This->current_clip_rect, 1, YXSorted);
  XSetClipRectangles(This->display, This->gc_select, 0, 0, 
		     &This->current_clip_rect, 1, YXSorted);
 }
 else
 {
  GC gc;

  This->current_clip_mask = XCreatePixmap(This->display, This->window,
					  width + 1, height + 1, 1);
  gc = XCreateGC(This->display, This->current_clip_mask, 0, NULL);
  XSetFunction(This->display, gc, GXcopy);

  XCopyArea(This->display, This->clip_mask, This->current_clip_mask,
	    gc, x - This->clip_x, y - This->clip_y, width + 1, height + 1,
	    0, 0);
  XFreeGC(This->display, gc);
  This->current_clip_x = x;
  This->current_clip_y = y;

  XSetClipMask(This->display, This->gc_backg, This->current_clip_mask);
  XSetClipMask(This->display, This->gc_paint, This->current_clip_mask);
  XSetClipMask(This->display, This->gc_mask, This->current_clip_mask);
  XSetClipMask(This->display, This->gc_select, This->current_clip_mask);  

  XSetClipOrigin(This->display, This->gc_backg, x, y);
  XSetClipOrigin(This->display, This->gc_paint, x, y);
  XSetClipOrigin(This->display, This->gc_mask, x, y);
  XSetClipOrigin(This->display, This->gc_select, x, y);  
 }

 do
 {
  Xc_TRACE(("tl->y: %d tl->height: %d tl->x_min: %d tl->x_max: %d",
	    tl->y, tl->height, tl->x_min, tl->x_max));
  
  if (tl->y <= syd && tl->y + tl->height >= syo &&
      tl->x_min <= sxd && tl->x_max >= sxo)
  {
   Xc_TRACE(("line accepted: %d", tl->line_number));
   ts = tl->text_spot;
   while(ts)
   {
    if (ts->x <= sxd && ts->x + ts->width >= sxo)
    {
     Xc_TRACE(("text spot accepted"));
     draw_text_spot(This, page, tl, ts);
    }
    ts = ts->next;
   }
  }

  tl = tl->next;
 } 
 while(tl != tlend->next); 
 
 if (selection)
  display_selection(This, page);

 if (This->clip_mask == None)
 {
  XSetClipMask(This->display, This->gc_backg, None);
  XSetClipMask(This->display, This->gc_paint, None);
  XSetClipMask(This->display, This->gc_mask, None);
  XSetClipMask(This->display, This->gc_select, None);
 }  
 else
 {
  XFreePixmap(This->display, This->current_clip_mask);
  This->current_clip_mask = None;
  
  XSetClipMask(This->display, This->gc_backg, This->clip_mask);
  XSetClipMask(This->display, This->gc_paint, This->clip_mask);
  XSetClipMask(This->display, This->gc_mask, This->clip_mask);
  XSetClipMask(This->display, This->gc_select, This->clip_mask);  

  XSetClipOrigin(This->display, This->gc_backg, This->clip_x, This->clip_y);
  XSetClipOrigin(This->display, This->gc_paint, This->clip_x, This->clip_y);
  XSetClipOrigin(This->display, This->gc_mask, This->clip_x, This->clip_y);
  XSetClipOrigin(This->display, This->gc_select, This->clip_x, This->clip_y);
 }
 This->current_clip_valid = FALSE;
}

static void redraw_polygone(This, page, polygone, selection)
c_Redraw *This;
c_Page	*page;
Polygone *polygone;
boolean selection;
{
 coord_t tmp, sxo, sxd, syo, syd;
 text_line_t	*tl, *tlend;
 text_spot_t	*ts;
 int x, y, width, height;
 
 if (!page->mapped) return;
 
 x = polygone->Orig_X + polygone->x_min;
 y = polygone->Orig_Y + polygone->y_min;
 width = polygone->x_max - polygone->x_min + 1;
 height = polygone->y_max - polygone->y_min + 1;

 tmp = x * page->sScale - COORD_HALF_ONE;
 sxo = MAX(tmp, page->vx);
 tmp += width * page->sScale;
 sxd = MIN(tmp, page->vx + page->vwidth);
 if (sxo > sxd) return;

 tmp = y * page->sScale - COORD_HALF_ONE;
 syo = MAX(tmp, page->vy);
 tmp += height * page->sScale + COORD_ONE;
 syd = MIN(tmp, page->vy + page->vheight);
 if (syo > syd) return;
 
 tl = page->text_line_start;
 tlend = page->text_line_end;
 if (!tl || !tlend) return;

 CHECK_CLIPPING(page);

 This->current_clip_valid = TRUE;

 This->current_clip_x = x;
 This->current_clip_y = y;
 This->current_clip_mask = CalculRectPolygoneMask
  (polygone, x, y, width, height, This->display, page->X_info.window);
 
 XSetClipMask(This->display, This->gc_backg, This->current_clip_mask);
 XSetClipMask(This->display, This->gc_paint, This->current_clip_mask);
 XSetClipMask(This->display, This->gc_mask, This->current_clip_mask);
 XSetClipMask(This->display, This->gc_select, This->current_clip_mask);  
 
 XSetClipOrigin(This->display, This->gc_backg, x, y);
 XSetClipOrigin(This->display, This->gc_paint, x, y);
 XSetClipOrigin(This->display, This->gc_mask, x, y);
 XSetClipOrigin(This->display, This->gc_select, x, y);  
 
 do
 {
  Xc_TRACE(("tl->y: %d tl->height: %d tl->x_min: %d tl->x_max: %d",
	    tl->y, tl->height, tl->x_min, tl->x_max));
  
  if (tl->y <= syd && tl->y + tl->height >= syo &&
      tl->x_min <= sxd && tl->x_max >= sxo)
  {
   Xc_TRACE(("line accepted: %d", tl->line_number));
   ts = tl->text_spot;
   while(ts)
   {
    if (ts->x <= sxd && ts->x + ts->width >= sxo)
    {
     Xc_TRACE(("text spot accepted"));
     draw_text_spot(This, page, tl, ts);
    }
    ts = ts->next;
   }
  }

  tl = tl->next;
 } 
 while(tl != tlend->next); 

 if (selection)
  display_selection(This, page);

 XFreePixmap(This->display, This->current_clip_mask);
 This->current_clip_mask = None;

 if (This->clip_mask == None)
 {
  XSetClipMask(This->display, This->gc_backg, None);
  XSetClipMask(This->display, This->gc_paint, None);
  XSetClipMask(This->display, This->gc_mask, None);
  XSetClipMask(This->display, This->gc_select, None);
 }  
 else
 {
  XSetClipMask(This->display, This->gc_backg, This->clip_mask);
  XSetClipMask(This->display, This->gc_paint, This->clip_mask);
  XSetClipMask(This->display, This->gc_mask, This->clip_mask);
  XSetClipMask(This->display, This->gc_select, This->clip_mask);  

  XSetClipOrigin(This->display, This->gc_backg, This->clip_x, This->clip_y);
  XSetClipOrigin(This->display, This->gc_paint, This->clip_x, This->clip_y);
  XSetClipOrigin(This->display, This->gc_mask, This->clip_x, This->clip_y);
  XSetClipOrigin(This->display, This->gc_select, This->clip_x, This->clip_y);
 }
 This->current_clip_valid = FALSE;
}

static void display_selection(This, page)
c_Redraw *This;
c_Page	 *page;
{
 c_Calibration  *calib = This->PageArea->Calibration;
 xa_item_t	*sel_start, *sel_end;
 int		sel_start_pos, sel_end_pos;
 text_line_t	*tls_start, *tls_end;
 text_line_t	*tl, *tlend;
 select_by_page_t *selection, *old_ptr, *sel_ptr;
 c_Text		*text;
 int		x, y, width, height;
 coord_t	scale;

 if (page)
 {
  if (!page->mapped)
   return;

  text = This->BaseStd->text_base;
  sel_start = text->select_start;
  sel_start_pos = text->select_start_pos;
  sel_end = text->select_end;
  sel_end_pos = text->select_end_pos;
  tls_start = calib->tl_select_start;
  tls_end = calib->tl_select_end;
  
  if (page->text_line_start->line_number > tls_end->line_number ||
      page->text_line_end->line_number < tls_start->line_number)
   return;			/* no selection on this page */
  
  selection = Xc_new(This->select_handle);
  if (This->selection)
  {
   sel_ptr = This->selection; old_ptr = NULL;
   while(sel_ptr && sel_ptr->page->page_number < page->page_number)
   {
    old_ptr = sel_ptr;
    sel_ptr = sel_ptr->next;
   }
   
   if (sel_ptr && sel_ptr->page == page)
   {
    Xc_free(sel_ptr->rect_base);
    Xc_delete(This->select_handle, selection);
    selection = sel_ptr;
   }
   else
   {
    if (old_ptr)
    {
     old_ptr->next = selection;
     selection->next = sel_ptr;
    }
    else
    {
     This->selection = selection;
     selection->next = sel_ptr;
    }
   }
  }
  else
  {
   This->selection = selection;
   selection->next = NULL;
  }

  selection->page = page;
  selection->rect_index = 0;
  selection->rect_base = (XRectangle *)Xc_malloc("rb", SELECT_CACHE_SIZE * 
						 sizeof(XRectangle));
  selection->rect_base_size = SELECT_CACHE_SIZE;

  scale = page->sScale;
  
  if (tls_start->line_number < page->text_line_start->line_number)
   tl = page->text_line_start;
  else
   tl = tls_start;

  if (tls_end->line_number > page->text_line_end->line_number)
   tlend = page->text_line_end;
  else
   tlend = tls_end;

  if (tl == tls_start)
   if (is_last_pos_in_textline(tl, sel_start, sel_start_pos))
    x = (get_x_max(tl) - COORD_HALF_ONE ) / scale;
   else
    x = (get_x_by_item(tl, NULL, sel_start, sel_start_pos) - COORD_HALF_ONE)
     / scale;
  else
   x = (get_x_min(tl) - COORD_HALF_ONE ) / scale;
  
  if (sel_start == sel_end && sel_start_pos == sel_end_pos)
   width = 1;
  else if (tl == tlend && tlend == tls_end && 
	   !is_last_pos_in_textline(tl, sel_end, sel_end_pos))
   width = get_x_by_item(tl, NULL, sel_end, sel_end_pos) / scale - x + 1;
  else
   width = get_x_max(tl) / scale - x + 1;
  if (width <= 0) width = 1;

  if (tl == tlend)
  {
   coord_t sy, sheight;

   calcul_selection_height(tl, sel_start, sel_end, &sy, &sheight);
   y = (sy - COORD_HALF_ONE) / scale;
   height = (sheight + COORD_ONE) / scale + 1;
   PUSH_IN_SELECT_BASE(x, y, width, height);

   if (!This->old_selection)
   {
    CHECK_CLIPPING(page);
    XFillRectangle(This->display, page->X_info.window, This->gc_select, 
		   x, y, width, height);
   }
   return;
  }
  else
  {
   y = (tl->y - COORD_HALF_ONE) / scale;
   height = (tl->height + COORD_ONE) / scale + 1;
   PUSH_IN_SELECT_BASE(x, y, width, height);
  }

  tl = tl->next;
  while(tl != tlend)
  {
   x = (get_x_min(tl) - COORD_HALF_ONE) / scale;
   width = get_x_max(tl) / scale - x + 1;
   y = (tl->y - COORD_HALF_ONE) / scale;
   height = (tl->height + COORD_ONE) / scale + 1;
   PUSH_IN_SELECT_BASE(x, y, width, height);
   
   tl = tl->next;
  }
  
  x = (get_x_min(tl) - COORD_HALF_ONE) / scale;
  if (tlend == tls_end && 
      !is_last_pos_in_textline(tl, sel_end, sel_end_pos))
   width = get_x_by_item(tl, NULL, sel_end, sel_end_pos) / scale - x + 1;
  else
   width = get_x_max(tl) / scale - x + 1;
  y = (tl->y - COORD_HALF_ONE) / scale;
  height = (tl->height + COORD_ONE) / scale + 1;
  PUSH_IN_SELECT_BASE(x, y, width, height);

  if (!This->old_selection)
  {
   CHECK_CLIPPING(page);
   XFillRectangles(This->display, page->X_info.window, This->gc_select, 
		   selection->rect_base, selection->rect_index);
  }
 }
 else
 {
  int		first_ln;
  int		last_ln;

  selection = This->selection;
  while(selection)
  {
   sel_ptr = selection->next;
   Xc_free(selection->rect_base);
   Xc_delete(This->select_handle, selection); 
   selection = sel_ptr;
  }
  This->selection = NULL;

  page = This->PageArea->SelectedPage;
  text = This->BaseStd->text_base;

  first_ln = calib->tl_select_start->line_number;
  if (text->select_start->type == XA_LINEFEED)
   first_ln++;

  while(page->text_line_start->line_number > first_ln)
   page = page->PrevPage;

  last_ln = calib->tl_select_end->line_number;
  if (text->select_end->type == XA_LINEFEED)
   last_ln++;
  do
  {
   if (page->mapped)
    display_selection(This, page);    
   page = page->NextPage;
  } 
  while(page && page->text_line_start->line_number <= last_ln);
 }

}

static void undisplay_selection(This, page)
c_Redraw *This;
c_Page	 *page;
{
 c_Calibration  *calib = This->PageArea->Calibration;
 select_by_page_t *selection;
 int		first_ln;
 int		last_ln;
 
 if (page)
 {
  if (!page->mapped)
   return;
  
  selection = This->selection;
  while(selection && selection->page != page)
   selection = selection->next;
  
  if (selection)
  {
   CHECK_CLIPPING(page);
   XFillRectangles(This->display, page->X_info.window, This->gc_select, 
		   selection->rect_base, selection->rect_index);
  }
 }
 else
 {
  page = This->PageArea->SelectedPage;
  
  first_ln = calib->tl_select_start->line_number;
  while(page->text_line_start->line_number > first_ln)
   page = page->PrevPage;

  last_ln = calib->tl_select_end->line_number;
  do
  {
   if (page->mapped)
    undisplay_selection(This, page);    
   page = page->NextPage;
  }
  while(page && page->text_line_start->line_number <= last_ln);
 }
 
}

static void redraw_text_line_diff(This, page, new_tl, old_tl)
c_Redraw *This;
c_Page *page;
text_line_t *new_tl;
text_line_t *old_tl;
{
 c_Text *text;
 text_spot_t *new_ts, *old_ts;
 int pos;
 
 CHECK_CLIPPING(page);

 while(new_tl && old_tl)
 {
  if (memcmp((void *)&(new_tl->line_number), (void *)&(old_tl->line_number),
	     offsetof(text_line_t, new_paragraph) + sizeof(boolean)
	     - offsetof(text_line_t, line_number)))
  {
   clear_text_line(This, page, old_tl, NULL);
   draw_text_line(This, page, new_tl, NULL);
   new_tl = new_tl->next;
   old_tl = old_tl->next;
   continue;
  }
  
  text = This->BaseStd->text_base;
  new_ts = new_tl->text_spot;
  old_ts = old_tl->text_spot;
  while(new_ts && old_ts)
  {
   pos = -1;
   if ( new_ts->x != old_ts->x || F(text).checkDiffTextItem
       (text, new_ts->item, old_ts->item, &pos) ||
       new_ts->width != old_ts->width || 
       new_ts->y_offset != old_ts->y_offset)
    break;
   new_ts = new_ts->next;
   old_ts = old_ts->next;
  }
  
  if (!new_ts && !old_ts)
  {
   new_tl = new_tl->next;
   old_tl = old_tl->next;
   continue;
  }
  
  if (old_ts)
  {
   if (pos >= 0)
    clear_from_text_spot(This, page, old_tl, old_ts, pos);
   else
    clear_text_line(This, page, old_tl, old_ts);
  }
  
  if (new_ts)
  {
   if (pos >= 0)
    draw_from_text_spot(This, page, new_tl, new_ts, pos);
   else
    draw_text_line(This, page, new_tl, new_ts);
  }

  new_tl = new_tl->next;
  old_tl = old_tl->next;
 }

 if (!old_tl)
 {
  if (!new_tl) return;
  
  while(new_tl)
  {
   draw_text_line(This, page, new_tl, NULL);
   new_tl = new_tl->next;
  }
  return;
 }

 if (!new_tl)
 {
  while(old_tl)
  {
   clear_text_line(This, page, old_tl, NULL);
   old_tl = old_tl->next;
  }
  return;
 }
}


static void clear_from_text_spot(This, page, tl, ts, pos)
c_Redraw *This;
c_Page *page;
text_line_t *tl;
text_spot_t *ts;
int pos;
{
 coord_t x, y, width, height;
 
 if (!page->mapped) return;
 
 if (ts->x < page->vx + page->vwidth && tl->x_max >= page->vx &&
     tl->y < page->vy + page->vheight && tl->y + tl->height >= page->vy)
 {  
  if (ts->item->type == XA_TEXT && pos > 0)
  {
   x = get_x_by_item(tl, ts, ts->item, pos);
   if (x == -1 || x > page->vx + page->vwidth)
    return;
  }
  else
   x = ts->x;
  
  y = (tl->y - COORD_HALF_ONE) / page->sScale;
  width = (get_x_max(tl) - x + COORD_ONE) / page->sScale + 1;
  height = (tl->height + COORD_ONE) / page->sScale + 1;
  x /= page->sScale;

  if (This->redraw_cache)
  {
   PUSH_IN_RECT_BASE(x, y, width, height);
  }
  else
   XFillRectangle(This->display, page->X_info.window, This->gc_clear, 
		  x, y, width, height);
 }
}

static void clear_text_line(This, page, tl, ts)
c_Redraw *This;
c_Page *page;
text_line_t *tl;
text_spot_t *ts;
{
 coord_t x, y, width, height;
 
 if (!page->mapped) return;

 if (!ts)
 {
  ts = tl->text_spot;
  if (!ts) return;
 }
 
 if (ts->x < page->vx + page->vwidth && tl->x_max >= page->vx &&
     tl->y < page->vy + page->vheight && tl->y + tl->height >= page->vy)
 {  
  x = (ts->x - COORD_HALF_ONE) / page->sScale;
  y = (tl->y - COORD_HALF_ONE) / page->sScale;
  width =  (get_x_max(tl) - ts->x + COORD_ONE) / page->sScale + 1;
  height = (tl->height + COORD_ONE) / page->sScale + 1;
  
  if (This->redraw_cache)
  {
   PUSH_IN_RECT_BASE(x, y, width, height);
  }
  else
   XFillRectangle(This->display, page->X_info.window, This->gc_clear, 
		  x, y, width, height);
 }
}

static void draw_text_spot(This, page, tl, ts)
c_Redraw *This;
c_Page *page;
text_line_t *tl;
text_spot_t *ts;
{
 coord_t x, y, width, height; 
 xa_item_t *item;
 vector_t pos;
 scale_t factor;
 coord_t char_inc, word_inc, sx;

 if (!page->mapped) return;
 
 if (ts->x < page->vx + page->vwidth && ts->x + ts->width >= page->vx &&
     tl->y < page->vy + page->vheight && tl->y + tl->height >= page->vy)
 {  
  factor = page->sFactor * COORD_SCALEVALUE / COORD_DEFAULT;
  
  x = (ts->x - COORD_HALF_ONE) / page->sScale;
  y = (tl->y - COORD_HALF_ONE) / page->sScale,
  width = ts->width / page->sScale + 1;
  height = (tl->height + COORD_ONE) / page->sScale + 1;
 
  char_inc = tl->char_inc * factor;
  word_inc = tl->word_inc * factor;

  item = ts->item;

  switch(item->type)
  {
  case XA_TEXT:
   update_font(This, item->any.style, factor);

   if (!item->any.style->bgcolor->transparency)
    XFillRectangle(This->display, page->X_info.window, This->gc_backg, 
		   x, y, width, height);
      
   pos.dx = sx = ts->x * factor;
   pos.dy = (tl->y + ts->y_offset) * factor;
   
   F(This->rfont).drawString(This->rfont, This->display, page->X_info.window, 
			     This->gc_mask, This->gc_paint, &pos, 
			     char_inc, word_inc, item->text.text,
			     item->text.text_len, This->style);

   if (This->spec_chars)
   {
    int i, j;

    for(i = j = 0; i < item->text.text_len; i++)
    {
     if (item->text.text[i] == ' ')
     {
      /*c_TextStyle *newstyle;*/
      
      sx += (F(This->style->metrics).textWidthSpec
	     (This->style->metrics, item->text.text + j, (i - j), 
	      tl->char_inc, tl->word_inc)) * factor;
      pos.dx = sx;
      
      /*newstyle = F(item->any.style).get*/
      /*(item->any.style,page->BaseStd, XcTS_UNDERLINE_OFF, XcTS_STRIKEOUT_OFF,*/
      /*XcTS_SUBSCRIPT_OFF, XcTS_SUPSCRIPT_OFF, XcTS_SHADOW_OFF, XcTS_FONT,*/
      /*F(GlobFontBase).getFontByFamilyAndAttribs(GlobFontBase, "Helvetica",*/
      /*FALSE, FALSE), XcTS_END);*/
      /*update_font(This, newstyle, factor);*/
      
      F(This->rfont).drawString(This->rfont, This->display, 
				page->X_info.window, 
				This->gc_mask, This->gc_paint, &pos, 
				char_inc, word_inc, "\267", 1,
				This->style);/* newstyle); */
      
      /*DELETE(c_TextStyle)(newstyle);*/
      j = i;
     } 
     else if (item->text.text[i] == '\240')
     {
      sx += (F(This->style->metrics).textWidthSpec
	     (This->style->metrics, item->text.text + j, (i - j), 
	      tl->char_inc, tl->word_inc)) * factor;
      pos.dx = sx;
      
      F(This->rfont).drawString(This->rfont, This->display, 
				page->X_info.window, 
				This->gc_mask, This->gc_paint, &pos, 
				char_inc, word_inc, "\260", 1,
				This->style);/* newstyle); */
      j = i;
     }
    }
   }
   /*XDrawRectangle(This->display, page->X_info.window, page->X_info.gc_contour,*/
   /*x, y, width, height);*/
   break;
  case XA_TAB:

   if (!item->any.style->bgcolor->transparency)
    XFillRectangle(This->display, page->X_info.window, This->gc_backg, 
		   x, y, width, height);

   if (item->tab.filler != ' ' ||
       (item->any.style->attributes & (XcTS_H_UNDERLINE | XcTS_H_STRIKEOUT)))
   {
    coord_t fs, space;
    int nb, i, l;
    char filler[101];

    memset(filler, (int)item->tab.filler, 100);
    
    update_font(This, item->any.style, factor);
    fs = F(item->any.style->metrics).textWidth(item->any.style->metrics,
					       &item->tab.filler, 1);
    nb = (int)(ts->width / fs);
    
    if (nb)
    {
     space = (coord_t)ceil(((ts->width - (fs * nb)) / (double)nb) * factor);
     if (space < COORD_ZERO) space = COORD_ZERO;
    }
    else
    {
     nb = 1;
     space = COORD_ZERO;;
    }
    
    pos.dx = ts->x * factor;
    pos.dy = (tl->y + ts->y_offset) * factor;
    
    i = nb;
    while(i != 0)
    {
     l = MIN(i, 100);

     
     F(This->rfont).drawString(This->rfont, This->display, 
			       page->X_info.window, 
			       This->gc_mask, This->gc_paint, &pos, 
			       space, COORD_ZERO, filler, l, This->style);
     i -= l;
    }
   }
   
   if (This->spec_chars)
   {
    XSegment arrow[4];
    int nx, ny, ns;

    ns = 0;

    update_font(This, item->any.style, factor);
        
    ny = (tl->y + ts->y_offset - This->style->metrics->ascender / 2)
     / page->sScale;

    if (width > 10 && height > 7) ns = 3;
    else if (width > 8 && height > 5) ns = 2;
    else if (width > 6 && height > 3) ns = 1;
    else 
    {
     XDrawLine(This->display, page->X_info.window, This->gc_draw,
	       x, ny, x + width, ny);
     break;
    }

    nx = x + (width - 4) / 2 - ns; 
    
    arrow[0].x1 = nx; arrow[0].x2 = nx + 4 + ns * 2;
    arrow[0].y1 = arrow[0].y2 = ny;
    nx += 4 + ns;

    switch(ns)
    {
    case 3:
     arrow[3].x1 = arrow[3].x2 = nx++;
     arrow[3].y1 = ny - 3; arrow[3].y2 = ny + 3;
    case 2:
     arrow[2].x1 = arrow[2].x2 = nx++;
     arrow[2].y1 = ny - 2; arrow[2].y2 = ny + 2;
    case 1:
     arrow[1].x1 = arrow[1].x2 = nx++;
     arrow[1].y1 = ny - 1; arrow[1].y2 = ny + 1;
    }

    XDrawSegments(This->display, page->X_info.window, This->gc_draw,
		  arrow, ns + 1);
   }
   break;
  case XA_PAGEBREAK:
   if (This->spec_chars)
   {
    coord_t lwidth, mwidth;
    char *pb_label;
    int ny, segwidth, mw;
    
    if (ts->width <= 2) break;

    update_font(This, This->LabelStyle, factor);

    pb_label = F(GlobResources).getString(GlobResources, XcR_pageBreakName);
    lwidth = F(item->any.style->metrics).textWidth(This->LabelStyle->metrics,
						   pb_label, strlen(pb_label));
    mwidth = SCALE_FROM_MILLIMETERS(1.5);
    mw = mwidth / page->sScale;

    ny = (tl->y + ts->y_offset - This->LabelStyle->metrics->ascender / 2)
     / page->sScale;
    
    if (ts->width > 6 * mwidth + lwidth)
    {
     segwidth = ((ts->width - lwidth) / 2) / page->sScale;
     
     XDrawLine(This->display, page->X_info.window, This->gc_draw, x + mw,
	       ny, x + segwidth - mw, ny);
     
     pos.dx = ( ts->x + ((ts->width - lwidth) / 2)) * factor;
     pos.dy = (tl->y + ts->y_offset) * factor;
     
     F(This->rfont).drawString(This->rfont, This->display, 
			       page->X_info.window, 
			       This->gc_mask, This->gc_paint, &pos, 
			       COORD_ZERO, COORD_ZERO, pb_label, 
			       strlen(pb_label),
			       This->LabelStyle);

     XDrawLine(This->display, page->X_info.window, This->gc_draw, 
	       x + width - segwidth + mw, ny, x + width - mw, ny);
    }
    else if (width > mw * 2) 
    {
     XDrawLine(This->display, page->X_info.window, This->gc_draw, x + mw,
	       ny, x + width - mw, ny);
    }
   }
   break;
  case XA_END:
  case XA_LINEFEED:
   if (This->spec_chars)
   {
    update_font(This, item->any.style, factor);
    pos.dx = ts->x * factor + COORD_HALF_ONE;
    pos.dy = (tl->y + ts->y_offset) * factor;

    F(This->rfont).drawString(This->rfont, This->display, page->X_info.window, 
			      This->gc_mask, This->gc_paint, &pos, 
			      char_inc, word_inc, "\266", 1,
			      This->style);
   }
   /*XDrawRectangle(This->display, page->X_info.window, page->X_info.gc_contour,*/
   /*x, y, width, height);*/
   break;
  default:
   break;
  }
  
  if (This->clip_over &&
      IsRectIntersPolygone(This->clip_over, x, y, width, height))
  {
   Polygone *forme;
   
   forme = IntersectRectPolygone(This->clip_over, x, y, width, height);
   F(This->clipped_page->frmg).redraw_polygone(This->clipped_page->frmg,
					       forme, FALSE);
    FreePolygone(forme, DEL_POLYGONE);
  }
  
 }
}

static void draw_from_text_spot(This, page, tl, ts, pos)
c_Redraw *This;
c_Page *page;
text_line_t *tl;
text_spot_t *ts;
int pos;
{
 coord_t x;
 xa_item_t *item;
 vector_t posX;
 scale_t factor;
 coord_t char_inc, word_inc, sx;
 
 if (!page->mapped) return;
 
 if (ts->item->type != XA_TEXT || pos <= 0)
 {
  draw_text_line(This, page, tl, ts);
  return;
 }
 
 if (This->redraw_cache)
 {
  This->draw_ptr->tl = tl;
  This->draw_ptr->ts = ts;
  This->draw_ptr->pos = pos;
  This->draw_ptr->func_type = TRUE;
  This->draw_ptr->next = Xc_new(This->draw_handle);
  This->draw_ptr = This->draw_ptr->next;
  return;
 }
 
 if (tl->y < page->vy + page->vheight && tl->y + tl->height >= page->vy)
 {
  if (ts->x < page->vx + page->vwidth && ts->x + ts->width >= page->vx)
  {
   factor = page->sFactor * COORD_SCALEVALUE / COORD_DEFAULT;
   
   item = ts->item;
   x = get_x_by_item(tl, ts, ts->item, pos);
  
   if (x != -1 && x < page->vx + page->vwidth)
   {
    update_font(This, item->any.style, factor);

    if (!item->any.style->bgcolor->transparency)
    {
     int dx = (x - ts->x - COORD_HALF_ONE) / page->sScale;

     XFillRectangle(This->display, page->X_info.window, This->gc_backg, 
		    (x - COORD_HALF_ONE) / page->sScale,
		    (tl->y - COORD_HALF_ONE) / page->sScale,
		    ts->width / page->sScale - dx + 1,
		    (tl->height + COORD_ONE) / page->sScale + 1);
    }
    
    posX.dx = sx = x * factor;
    posX.dy = (tl->y + ts->y_offset) * factor;
    
    char_inc = tl->char_inc * factor;
    word_inc = tl->word_inc * factor;
    
    F(This->rfont).drawString(This->rfont, This->display, page->X_info.window, 
			      This->gc_mask, This->gc_paint, &posX, 
			      char_inc, word_inc, 
			      item->text.text + pos,
			      item->text.text_len - pos, This->style);

    if (This->spec_chars)
    {
     int i, j;
     
     for(i = j = pos; i < item->text.text_len; i++)
     {
      if (item->text.text[i] == ' ')
      {
       sx += (F(This->style->metrics).textWidthSpec
	      (This->style->metrics, item->text.text + j, (i - j), 
	       tl->char_inc, tl->word_inc)) * factor;
       posX.dx = sx;
     
       F(This->rfont).drawString(This->rfont, This->display, 
				 page->X_info.window, 
				 This->gc_mask, This->gc_paint, &posX, 
				 char_inc, word_inc, "\267", 1,
				 This->style);
       j = i;
      }
     }
    }
   }

   if (This->clip_over)
   {
    coord_t dx, y, width, height;
    Polygone *forme;
    
    dx = (x - ts->x - COORD_HALF_ONE) / page->sScale;
    x = (x - COORD_HALF_ONE) / page->sScale;
    y = (tl->y - COORD_HALF_ONE) / page->sScale;
    width = ts->width / page->sScale - dx + 1;
    height = (tl->height + COORD_ONE) / page->sScale + 1;
    
    if (IsRectIntersPolygone(This->clip_over, x, y, width, height))
    {
     forme = IntersectRectPolygone(This->clip_over, x, y, width, height);
     F(This->clipped_page->frmg).redraw_polygone(This->clipped_page->frmg,
						 forme, TRUE);
     FreePolygone(forme, DEL_POLYGONE);
    }
   }
   
  }
  ts = ts->next;
  if (!ts) return;
  draw_text_line(This, page, tl, ts);
 }
}

static void draw_text_line(This, page, tl, ts)
c_Redraw *This;
c_Page *page;
text_line_t *tl;
text_spot_t *ts;
{
 if (!page->mapped) return;
 
 if (!ts)
 {
  ts = tl->text_spot;
  if (!ts) return;
 }

 if (This->redraw_cache)
 {
  This->draw_ptr->tl = tl;
  This->draw_ptr->ts = ts;
  This->draw_ptr->func_type = FALSE;
  This->draw_ptr->next = Xc_new(This->draw_handle);
  This->draw_ptr = This->draw_ptr->next;
  return;
 }
 
 if (tl->y < page->vy + page->vheight && tl->y + tl->height >= page->vy)
 {
  while(ts)
  {
   draw_text_spot(This, page, tl, ts);
   ts = ts->next;
  }
 }
}

static void update_font(This, tstyle, factor)
c_Redraw *This;
c_TextStyle *tstyle;
scale_t factor;
{
 if (tstyle != This->style)
 {
  This->style = tstyle;
  
  This->metrics = 
   F(tstyle->metrics).transform(tstyle->metrics, factor,
				XcTS_GET_WIDTH(tstyle), ANGLE_ZERO);
  if (This->rfont)
  {
   F(GlobFntMng).delete_font(GlobFntMng, This->rfont);
   This->rfont = NULL;
  }
  This->metrics->raster_font = 
   F(GlobFntMng).new_font(GlobFntMng, tstyle->font, This->metrics);
  if((This->rfont = This->metrics->raster_font) == NULL)
  {
   F(tstyle->metrics).purgeImpure(tstyle->metrics, factor,
				  XcTS_GET_WIDTH(tstyle), ANGLE_ZERO);
   return;
  }
  
  F(This->rfont).setGC(This->display, This->window, 
		       This->gc_mask, This->gc_paint);

  F(tstyle->color).setGC(tstyle->color, This->display, This->gc_paint);
  F(tstyle->bgcolor).setGC(tstyle->bgcolor, This->display, This->gc_backg);
 }
}

static coord_t get_x_by_item(tl, ts, item, pos)
text_line_t *tl;
text_spot_t *ts;
xa_item_t *item;
int pos;
{
 coord_t     x;
 c_TextStyle *style;

 if (!ts)
 {
  ts = tl->text_spot;
  while(ts && ts->item != item)
   ts = ts->next;
  
  if (!ts) 
   return (coord_t)-1;
 }
 
 switch(item->type)
 {
 case XA_TEXT:
  if (!pos)
   x = ts->x;
  else if (pos == item->text.text_len)
   x = ts->x + ts->width;
  else
  {
   style = item->any.style;
   x = ts->x + F(style->metrics).textWidthSpec
    (style->metrics, item->text.text, pos, tl->char_inc, tl->word_inc);
  }
  break;
 case XA_END:
  x = ts->x;
  break;
 default:
  x = ts->x;
  if (pos)
   x += ts->width;
  break;
 }
 return x;
}

/* return TRUE if x is inside a block item */
static boolean get_item_by_x(tl, x, item, pos)
text_line_t *tl;
coord_t x;
xa_item_t **item;
int *pos;
{
 text_spot_t *ts, *ts2;

 ts = tl->text_spot;
 while(ts && ts->x + ts->width <= x)
  ts = ts->next;

 if (!ts || x < ts->x)
 {
  ts2 = tl->text_spot;
  while(ts2 != ts && ts2->next != ts)
   ts2 = ts2->next;

  *item = ts2->item;
  switch((*item)->type)
  {
  case XA_TEXT:
   if (ts)
    *pos = 0;
   else
    *pos = (*item)->text.text_len;
   break;
  case XA_BEGIN:
  case XA_BULLET:
   *pos = 1;
   break;
  case XA_LINEFEED:
  case XA_END:
   *pos = 0;
   break;
  default:
   if (ts)
    *pos = 0;
   else
    *pos = 1;
  }
  
  return FALSE;
 }
 
 *item = ts->item;
 switch((*item)->type)
 case XA_TEXT:
 {
  int i;
  coord_t	width, cwidth;
  c_TextStyle	*style;

  style = (*item)->any.style;
  width = x - ts->x;
  cwidth = COORD_ZERO;

  i = 0;
  while(i < (*item)->text.text_len)
  {
   cwidth = F(style->metrics).textWidthSpec
    (style->metrics, (*item)->text.text + i, 1, tl->char_inc, tl->word_inc);
   width -= cwidth;
   if (width <= 0) break;
   i++;
  }
  
  if (-width < cwidth / 2) i++;
  *pos = i;
  break;
 case XA_BEGIN:
  *pos = 1;
  break;
 case XA_LINEFEED:
 case XA_END:
  *pos = 0;
  break;
 default:
  if (x > ts->x + ts->width / 2)
   *pos = 1;
  else
   *pos = 0;
 }
 return TRUE;
}

static void calcul_selection_height(tl, item1, item2, y, height)
text_line_t *tl;
xa_item_t *item1;
xa_item_t *item2;
coord_t	*y;
coord_t	*height;
{
 text_spot_t	*ts;
 coord_t	max_height, max_desc;
 coord_t	tmp;

 ts = tl->text_spot;
 while(ts && ts->item != item1)
  ts = ts->next;
 
 if (!ts) 
 {
  *y = tl->y;
  *height = tl->height;
  return;
 }

 max_height = item1->any.style->metrics->font_bbox.ury - 
  item1->any.style->metrics->font_bbox.lly;
 max_desc = -item1->any.style->metrics->descender;
 
 while(item1 != item2)
 {
  item1 = item1->any.next;

  tmp = item1->any.style->metrics->font_bbox.ury - 
   item1->any.style->metrics->font_bbox.lly;
  if (tmp > max_height) max_height = tmp;
  
  tmp = -item1->any.style->metrics->descender;
  if (tmp > max_desc) max_desc = tmp;

 }
 
 *height = max_height + COORD_ONE;
 *y = tl->y + ts->y_offset + max_desc - max_height;
 
 if (*height > tl->height) *height = tl->height;
}

static boolean is_within_textline(tl, x, tolerance)
text_line_t *tl;
coord_t x;
coord_t tolerance;
{
 if (x >= get_x_min(tl) - tolerance && x <= get_x_max(tl) + tolerance)
  return TRUE;
 return FALSE;
}

static boolean is_last_pos_in_textline(tl, item, pos)
text_line_t *tl;
xa_item_t *item;
int pos;
{
 text_spot_t *ts = tl->text_spot;

 if ( ( (item->type != XA_TEXT) && (pos != 1) ) ||
     (pos != item->text.text_len) )
  return FALSE;
   
 while(ts && ts->item != item)
  ts = ts->next;
 
 if (!ts || ts->next) return FALSE;
 return TRUE;
}


___INLINE static coord_t get_x_min(tl)
text_line_t *tl;
{
 if (!tl->text_spot) return (coord_t)-1;
 return tl->text_spot->x;
}

___INLINE static coord_t get_x_max(tl)
text_line_t *tl;
{
 text_spot_t *ts = tl->text_spot;
 xa_item_t	*item;
 coord_t	xmax;
 
 if (!ts) return (coord_t)-1;

 while(ts->next)
  ts = ts->next;

 xmax = ts->x + ts->width - tl->char_inc;
 
 item = ts->item;
 if (item->type == XA_TEXT && item->text.text[item->text.text_len-1] == ' ')
  xmax -= tl->word_inc;
 
 return xmax;
}

static void start_draw_cache(This)
c_Redraw *This;
{
 This->redraw_cache = TRUE;
 This->rect_base = (XRectangle *)Xc_malloc
  ("rect*", sizeof(XRectangle) * REDRAW_CACHE_SIZE);
 This->rect_base_size = REDRAW_CACHE_SIZE;
 This->rect_index = 0;
 This->draw_handle = Xc_createItem
  ("rc", REDRAW_CACHE_SIZE, sizeof(redraw_cache_t));
 This->draw_base = This->draw_ptr = Xc_new(This->draw_handle);
 
 if (This->clipped_page && (This->clip_over || This->clip_under))
  F(This->clipped_page->frmg).start_draw_cache(This->clipped_page->frmg);
 
}

static void flush_redraw_cache(This, page)
c_Redraw *This;
c_Page *page;
{
 redraw_cache_t *rc;
 boolean select_type;
 int i;

 if (!This->redraw_cache) return;
 This->redraw_cache = FALSE;	/* to avoid recursion ! */
 
 CHECK_CLIPPING(page);

 if (This->rect_index)
  XFillRectangles(This->display, page->X_info.window, This->gc_clear, 
		  This->rect_base, This->rect_index);
 
 select_type = *page->select_type; *(page->select_type) = FALSE;

 if (This->clip_over)
 {
  XRectangle *xrect = This->rect_base;
  Polygone *forme;
  
  for(i = 0; i < This->rect_index; i++)
  {
   if (IsRectIntersPolygone(This->clip_over, xrect->x, xrect->y, 
			    xrect->width, xrect->height))
   {
    forme = IntersectRectPolygone(This->clip_over, xrect->x, xrect->y, 
				 xrect->width, xrect->height);
    F(page->frmg).redraw_polygone(page->frmg, forme, TRUE);
    FreePolygone(forme, DEL_POLYGONE);
   }
   xrect++;
  }
 }

 if (This->clip_under)
 {
  XRectangle *xrect = This->rect_base;
  Polygone *forme;
  
  for(i = 0; i < This->rect_index; i++)
  {
   if (IsRectIntersPolygone(This->clip_under, xrect->x, xrect->y, 
			    xrect->width, xrect->height))
   {
    forme = IntersectRectPolygone(This->clip_under, xrect->x, xrect->y, 
				  xrect->width, xrect->height);
    F(page->frmg).redraw_polygone(page->frmg, forme, TRUE);
    FreePolygone(forme, DEL_POLYGONE);
   }
   xrect++;
  }
  
  F(page->frmg).flush_draw_cache(page->frmg);
  
  if (This->clip_over)
   F(page->frmg).start_draw_cache(page->frmg);
 }
 
 rc = This->draw_base;
 while(rc != This->draw_ptr)
 {
  if (rc->func_type)
   draw_from_text_spot(This, page, rc->tl, rc->ts, rc->pos);
  else
   draw_text_line(This, page, rc->tl, rc->ts);
  rc = rc->next;
 }
 
 if (This->clip_over)
  F(page->frmg).flush_draw_cache(page->frmg);

 *(page->select_type) = select_type;
 
 Xc_destroyItem(This->draw_handle);
 Xc_free(This->rect_base);
}

static void start_selection_diff(This)
c_Redraw *This;
{
 This->old_selection = This->selection;
 This->selection = NULL;
}

static void draw_selection_diff(This, page)
c_Redraw *This;
c_Page	*page;
{
 select_by_page_t	*select, *osel, *sel1;
 XRectangle		*rect, *rect_ptr;
 int			i, j, y, x, x2, xo, h, w, w2, wo;
 boolean		*found, *found2;
 
 display_selection(This, page);
 
 /*--- first undisplay old page with no corresponding new page ---*/
 osel = NULL;
 select = This->old_selection;
 while(select)
 {
  sel1 = This->selection;
  while(sel1 && sel1->page != select->page)
   sel1 = sel1->next;

  if (!sel1)			/* found one page */
  {
   if (!page || page == select->page)
   {
    if (select->page->mapped)
    {
     CHECK_CLIPPING(select->page);
     XFillRectangles(This->display, select->page->X_info.window, 
		     This->gc_select, select->rect_base, select->rect_index);
    }
    if (osel)
    {
     osel->next = select->next;
     Xc_free(select->rect_base);
     Xc_delete(This->select_handle, select);
     select = osel->next;
    }
    else
    {
     This->old_selection = select->next;
     Xc_free(select->rect_base);
     Xc_delete(This->select_handle, select);
     select = This->old_selection;
    }
    continue;
   }
  }
  osel = select;
  select = select->next;
 }

 /*--- second display new page with no corresponding old page ---*/
 select = This->selection;
 while(select)
 {
  sel1 = This->old_selection;
  while(sel1 && sel1->page != select->page)
   sel1 = sel1->next;

  if (!sel1)			/* found one page */
  {
   if (!page || page == select->page)
   {
    if (select->page->mapped)
    {
     CHECK_CLIPPING(select->page);
     XFillRectangles(This->display, select->page->X_info.window, 
		     This->gc_select, select->rect_base, select->rect_index);
    }
   }
  }
  select = select->next;
 }
 
 /*--- last find corresponding new and old page and draw diff ---*/
 select = This->old_selection;
 while(select)
 {
  sel1 = This->selection;
  while(sel1 && sel1->page != select->page)
   sel1 = sel1->next;

  if (sel1 && sel1->page->mapped)		
  {			
   rect = select->rect_base;
   rect_ptr = sel1->rect_base;

   found = (boolean *)Xc_malloc("fb", sizeof(boolean) * select->rect_index);
   found2 = (boolean *)Xc_malloc("fb2", sizeof(boolean) * sel1->rect_index);
   memset(found, 0, sizeof(boolean) * select->rect_index);
   memset(found2, 0, sizeof(boolean) * sel1->rect_index);

   for(i = 0; i < select->rect_index; i++)
   {
    y = rect[i].y; h = rect[i].height;
    for(j = 0; j < sel1->rect_index; j++)
    {
     if (found2[j]) continue;
     
     if (rect_ptr[j].y == y && rect_ptr[j].height == h)
     {
      x = rect[i].x; w = rect[i].width;
      x2 = rect_ptr[j].x; w2 = rect_ptr[j].width;
      if (x != x2 || w != w2)
      {
       if (!(x + w <= x2 || x2 + w2 <= x))
       {
	xo = MIN(x, x2);
	wo = MAX(x, x2) - xo;
	if (wo > 0)
	{
	 CHECK_CLIPPING(select->page);
	 XFillRectangle(This->display, select->page->X_info.window,
			This->gc_select, xo, y, wo, h);
	}	
	xo = MIN(x + w, x2 + w2);
	wo = MAX(x + w, x2 + w2) - xo;
	if (wo > 0)
	{
	 CHECK_CLIPPING(select->page);
	 XFillRectangle(This->display, select->page->X_info.window,
			This->gc_select, xo, y, wo, h);
	}
	found[i] = found2[j] = TRUE;
       }
       else
       {
	/*CHECK_CLIPPING(select->page);*/
	/*	XFillRectangle(This->display, select->page->X_info.window, */
	/*This->gc_select, x, y, w, h); */
	/*found[i] = TRUE;*/
	/*XFillRectangle(This->display, select->page->X_info.window,*/
	/*This->gc_select, x2, y, w2, h);*/
       }
      }
      else
       found[i] = found2[j] = TRUE;
     }
    }
   }

   for(i = 0; i < select->rect_index; i++)
   {
    if (!found[i])		/* rect not found, clear */
    {
     CHECK_CLIPPING(select->page);
     XFillRectangle(This->display, select->page->X_info.window,
		    This->gc_select, rect[i].x, rect[i].y,
		    rect[i].width, rect[i].height);
    }
   }
   for(i = 0; i < sel1->rect_index; i++)
   {
    if (!found2[i])		/* rect not found, clear */
    {
     CHECK_CLIPPING(select->page);
     XFillRectangle(This->display, select->page->X_info.window,
		    This->gc_select, rect_ptr[i].x, rect_ptr[i].y,
		    rect_ptr[i].width, rect_ptr[i].height);
    }
   }
   Xc_free(found);
   Xc_free(found2);

#if 0
   for(i = 0; i < sel1->rect_index; i++)
   {
    y = rect_ptr[i].y; h = rect_ptr[i].height;
    for(j = 0; j < select->rect_index; j++)
    {
     if (rect[j].y == y && rect[j].height == h)
      break;
    }
    if (j == select->rect_index)/* rect not found, draw */
    {
     CHECK_CLIPPING(select->page);
     XFillRectangle(This->display, select->page->X_info.window,
		    This->gc_select, rect_ptr[i].x, y, rect_ptr[i].width, h);
    }
   }
#endif

  }
  osel = select->next;
  Xc_free(select->rect_base);
  Xc_delete(This->select_handle, select);
  select = osel;
 }
 This->old_selection = NULL;
}




