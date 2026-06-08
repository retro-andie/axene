/*
** Redraw.h for XAllWrite in Redraw/
** Redraw object: redraw text along pages
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
** Started on  Sun May  4 17:35:50 1997 Emmanuel Paris
** Last update Mon Jul 13 17:04:48 1998 Emmanuel Paris
*/


#ifndef _Redraw_h_
#define _Redraw_h_

/*--- Forward declarations ---*/

typedef struct sc_Redraw c_Redraw;

#include "xcalibur.h"
#include "PageArea.h"
#include "Calibration.h"
#include "Text.h"
#include "polygone.h"


#define REDRAW_CACHE_SIZE 100
#define SELECT_CACHE_SIZE 200

#ifdef ___GNUC_COMPOSE_MACRO
#define PUSH_IN_RECT_BASE(x, y, width, height) \
if (This->rect_index == This->rect_base_size) \
{ \
 This->rect_base_size += REDRAW_CACHE_SIZE; \
   This->rect_base = (XRectangle *)Xc_realloc \
   ("rect*", This->rect_base, sizeof(XRectangle) * This->rect_base_size); \
} \
This->rect_base[This->rect_index].##x = (x); \
This->rect_base[This->rect_index].##y = (y); \
This->rect_base[This->rect_index].##width = (width); \
This->rect_base[This->rect_index].##height = (height); \
This->rect_index++
#define PUSH_IN_SELECT_BASE(x, y, width, height) \
if (selection->rect_index == selection->rect_base_size) \
{ \
 selection->rect_base_size += SELECT_CACHE_SIZE; \
 selection->rect_base = (XRectangle *)Xc_realloc \
  ("rect*", selection->rect_base, sizeof(XRectangle) * \
   selection->rect_base_size); \
} \
selection->rect_base[selection->rect_index].##x = (x); \
selection->rect_base[selection->rect_index].##y = (y); \
selection->rect_base[selection->rect_index].##width = (width); \
selection->rect_base[selection->rect_index].##height = (height); \
selection->rect_index++
#else
#define PUSH_IN_RECT_BASE(x, y, width, height) \
if (This->rect_index == This->rect_base_size) \
{ \
 This->rect_base_size += REDRAW_CACHE_SIZE; \
   This->rect_base = (XRectangle *)Xc_realloc \
   ("rect*", This->rect_base, sizeof(XRectangle) * This->rect_base_size); \
} \
This->rect_base[This->rect_index]./**/x = (x); \
This->rect_base[This->rect_index]./**/y = (y); \
This->rect_base[This->rect_index]./**/width = (width); \
This->rect_base[This->rect_index]./**/height = (height); \
This->rect_index++
#define PUSH_IN_SELECT_BASE(x, y, width, height) \
if (selection->rect_index == selection->rect_base_size) \
{ \
 selection->rect_base_size += SELECT_CACHE_SIZE; \
 selection->rect_base = (XRectangle *)Xc_realloc \
   ("rect*", selection->rect_base, sizeof(XRectangle) * \
    selection->rect_base_size); \
} \
selection->rect_base[selection->rect_index]./**/x = (x); \
selection->rect_base[selection->rect_index]./**/y = (y); \
selection->rect_base[selection->rect_index]./**/width = (width); \
selection->rect_base[selection->rect_index]./**/height = (height); \
selection->rect_index++
#endif

typedef struct _select_by_page_t select_by_page_t;
struct _select_by_page_t
{
 select_by_page_t	*next;
 c_Page			*page;
 
 XRectangle		*rect_base;
 int			rect_base_size;
 int			rect_index;
};

typedef struct _redraw_line_cache_t redraw_cache_t;
struct _redraw_line_cache_t
{
 redraw_cache_t *next;
 text_line_t	*tl;
 text_spot_t	*ts;
 int		pos;
 boolean	func_type;  /* 1: draw_from_text_spot; 0: draw_text_line */
};

typedef struct
{
 F_STD;
 void (*Init) ___PROTO((c_Redraw *This));
 void (*Reset) ___PROTO((c_Redraw *This));
 void (*set_clipping) ___PROTO((c_Redraw *This, Pixmap clip_mask, 
				int clip_x, int clip_y, 
				Polygone *clip_over, Polygone *clip_under));
 void (*redraw_rectangle) ___PROTO((c_Redraw *This, c_Page *page,
				    int x, int y, int width, int height,
				    boolean selection));
 void (*redraw_polygone) ___PROTO((c_Redraw *This, c_Page *page,
				   Polygone *polygone, boolean selection));
 void (*display_selection) ___PROTO((c_Redraw *This, c_Page *page));
 void (*undisplay_selection) ___PROTO((c_Redraw *This, c_Page *page));
 void (*redraw_text_line_diff) ___PROTO((c_Redraw *This, c_Page *page,
					 text_line_t *new_line,
					 text_line_t *old_line));
 void (*start_draw_cache) ___PROTO((c_Redraw *This));
 void (*flush_redraw_cache) ___PROTO((c_Redraw *This, c_Page *page));
 void (*start_selection_diff) ___PROTO((c_Redraw *This));
 void (*draw_selection_diff) ___PROTO((c_Redraw *This, c_Page *page));
 coord_t (*get_x_by_item) ___PROTO((text_line_t *tl, text_spot_t *ts,
				    xa_item_t *item, int pos));
 boolean (*get_item_by_x) ___PROTO((text_line_t *tl, coord_t x, 
				    xa_item_t **item, int *pos));
 boolean (*is_within_textline) ___PROTO((text_line_t *tl, coord_t x, 
					 coord_t tolerance));
 boolean (*is_last_pos_in_textline) ___PROTO((text_line_t *tl, 
					      xa_item_t *item, int pos));
} sf_Redraw;

/* define the Redraw Class */
struct sc_Redraw
{
 sf_Redraw        *f;

 c_PageArea	*PageArea;
 BaseStd_t	*BaseStd;

 Display	*display;
 Window		window;

 GC		gc_clear;
 GC		gc_select;
 GC		gc_draw;
 GC		gc_backg;
 GC		gc_paint;
 GC		gc_mask;

 c_Color	*BlackColor;
 c_TextStyle	*LabelStyle;	/* style for displaying page break ... */
 
 Pixmap		clip_mask;
 int		clip_x;
 int		clip_y;
 c_Page		*clipped_page;

 boolean	current_clip_valid;
 XRectangle	current_clip_rect;
 Pixmap		current_clip_mask;
 int		current_clip_x;
 int		current_clip_y;

 Polygone	*clip_over;
 Polygone	*clip_under;

 c_TextStyle	*style;
 c_RasterFont	*rfont;
 c_CharMetrics	*metrics;

 boolean	spec_chars;

 boolean	redraw_cache;
 XRectangle	*rect_base;
 int		rect_base_size;
 int		rect_index;
 ItemHandle     *draw_handle;
 redraw_cache_t	*draw_base;	 
 redraw_cache_t	*draw_ptr;	 

 ItemHandle	*select_handle;
 select_by_page_t *selection;
 select_by_page_t *old_selection;
};

extern sf_Redraw fc_Redraw;

#endif /* _Redraw_h_ */

