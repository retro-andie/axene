/*
** Display_CellStyle.c for XQuad in CellStyle/
** define Display_CellStyle Object
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
** Started on  Tue Mar 28 13:43:16 1995 Emmanuel Paris
** Last update Mon Sep 15 19:12:29 1997 Emmanuel Paris
*/

#define _CELLSTYLE_PATTERN_
#define _CELLSTYLE_DASHES_
#include "CellStyle.h"
#include "VectorFont.h"
#include "RasterFont.h"
#include "CharMetrics.h"
#include "FontManager.h"
#include "Moteur.h"

#define MOTEUR (c_Moteur *)This->moteur

extern c_VectorFont	*GlobFontBase;
extern c_FontManager	*GlobFntMng;

typedef struct DS_line_s
{
 char		*string;
 int		length;
 coord_t	char_inc;
 coord_t	word_inc;
 coord_t	offset;
  
 struct DS_line_s	*Next;
} DS_line_t;
  
static boolean diff_inside ___PROTO((CellStyle_t *cell_style1,
				     CellStyle_t *cell_style2));
static boolean diff_border ___PROTO((CellBorderStyle_t *cell_border1,
				     CellBorderStyle_t *cell_border2));
void CSdrawLine ___PROTO((Display *display, Window window, 
			  GC gc, CellBorderStyle_t *cborder,
			  cell_info_t *cellinfo, int x1, int y1,
			  int x2, int y2));
static void CSdrawLines ___PROTO((Display *display, Window window, 
				  GC gc, CellBorderStyle_t *cborder,
				  cell_info_t *cellinfo, XPoint *point,
				  int nb_point));
static void CSdrawRectangle ___PROTO((Display *display, Window window, 
				      GC gc, CellBorderStyle_t *cborder,
				      cell_info_t *cellinfo, int x, int y,
				      int width, int height));
static void change_gc_border();

#if 0
static boolean diff_text ___PROTO((CellStyle_t *cell_style1,
				   CellStyle_t *cell_style2));
#endif

static int cut_text_for_multiline ___PROTO((DS_line_t	*DS_line,
					    coord_t	sWidth,
					    c_CharMetrics *metrics,
					    boolean	angle90));
static coord_t supress_space_and_get_width ___PROTO((DS_line_t *DS_line,
						     c_CharMetrics *metrics,
						     boolean angle90, 
						     boolean first_line,
						     boolean last_line));
static void justify_DS_line ___PROTO((DS_line_t *DS_line, coord_t sWidth,
				      coord_t txt_width));
static void calcul_string_clipping ___PROTO((c_CharMetrics *metrics,
					     boolean angle90, char *orig,
					     coord_t offset, coord_t width,
					     char **dest, int	*length,
					     coord_t *incrx));
static void update_metrics ___PROTO((c_CellStyle *This, CellStyle_t *cstyle,
				     c_TextStyle *tstyle, angle_t angle,
				     scale_t factor));

/* ----------------------------------------------------------------- ** 
** diff_border - Compare border parameters                           ** 
** ----------------------------------------------------------------- */
static boolean diff_border(cell_border1, cell_border2)
CellBorderStyle_t	*cell_border1;
CellBorderStyle_t	*cell_border2;
{
 if (memcmp(cell_border1, cell_border2, sizeof(CellBorderStyle_t)))
  return TRUE;
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** change_gc_border - Update base border GC                          ** 
** ----------------------------------------------------------------- */
static void change_gc_border(This, cstyle, border)
c_CellStyle	*This; 
CellStyle_t	*cstyle;
int		border;
{
 CellBorderStyle_t	bstyle_current;
 CellBorderStyle_t	bstyle;
 GC			gc = (GC)0;
 int		masq_bin;
 XGCValues	gc_values;
  
 switch(border)
 {
 case BORDER_LEFT:
  bstyle_current = This->cell_style_current.left;
  bstyle = cstyle->left;
  gc = This->gc_left;
  break;
 case BORDER_RIGHT:
  bstyle_current = This->cell_style_current.right;
  bstyle = cstyle->right;
  gc = This->gc_right;
  break;
 case BORDER_UP:
  bstyle_current = This->cell_style_current.up;
  bstyle = cstyle->up;
  gc = This->gc_up;
  break;
 case BORDER_DOWN:
  bstyle_current = This->cell_style_current.down;
  bstyle = cstyle->down;
  gc = This->gc_down;
  break;
 }
 if (diff_border(&bstyle_current, &bstyle))
 {
  masq_bin = GCForeground | GCBackground | GCFillStyle | GCLineStyle |
   GCLineWidth | GCCapStyle | GCJoinStyle | GCTileStipXOrigin | 
    GCTileStipYOrigin;
  gc_values.foreground = bstyle.border_foreground->cell.pixel;
  gc_values.background = bstyle.border_background->cell.pixel;
  gc_values.cap_style = bstyle.border_line_cap_style;
  gc_values.join_style = bstyle.border_line_join_style;
  gc_values.ts_x_origin = 0;
  gc_values.ts_y_origin = 0;
  if (bstyle.border_stipple_pattern != PATTERN_NONE)
  {
   bstyle.border_thickness = 
    Cell_Pattern[bstyle.border_stipple_pattern].height 
     * bstyle.border_line_thickness;
	  
   if (Cell_Pattern[bstyle.border_stipple_pattern].type == PATTERN_LINE)
   {
    gc_values.line_width = 0;
    gc_values.fill_style = FillSolid;
   }
   else
   {
    masq_bin |= GCStipple;
    gc_values.stipple = 
     Cell_Pattern[bstyle.border_stipple_pattern].
      definition.data.pixmap;
	      
    if (bstyle.border_background->transparency)
     gc_values.fill_style = FillStippled;
    else
     gc_values.fill_style = FillOpaqueStippled;
	      
    if (bstyle.border_thickness == 1)
     gc_values.line_width = 0;
    else
     gc_values.line_width = bstyle.border_thickness;
   }
  }
  else
  {
   bstyle.border_thickness = 0;
  }
  if (bstyle.border_dash_pattern != PATTERN_NONE)
  {
   if (gc_values.fill_style == FillSolid)
   {
    if (bstyle.border_background->transparency)
     gc_values.line_style = LineOnOffDash;
    else
     gc_values.line_style = LineDoubleDash;
   }
   else
    gc_values.line_style = LineOnOffDash;
   XSetDashes(This->display, gc, 
	      Cell_Dashes[bstyle.border_dash_pattern].dash_offset,
	      Cell_Dashes[bstyle.border_dash_pattern].dash_list,
	      Cell_Dashes[bstyle.border_dash_pattern].dash_length);
  }
  else
   gc_values.line_style = LineSolid;
  XChangeGC(This->display, gc, masq_bin, &gc_values);
  switch(border)
  {
  case BORDER_LEFT:
   memcpy(&This->cell_style_current.left, &cstyle->left,
	  sizeof(CellBorderStyle_t));
   break;
  case BORDER_RIGHT:
   memcpy(&This->cell_style_current.right, &cstyle->right,
	  sizeof(CellBorderStyle_t));
   break;
  case BORDER_UP:
   memcpy(&This->cell_style_current.up, &cstyle->up,
	  sizeof(CellBorderStyle_t));
   break;
  case BORDER_DOWN:
   memcpy(&This->cell_style_current.down, &cstyle->down,
	  sizeof(CellBorderStyle_t));
   break;
  }
 }
}

void CSdrawLine(display, window, gc, cborder, cellinfo, x1, y1, x2, y2)
Display *display;
Window window;
GC gc;
CellBorderStyle_t *cborder;
cell_info_t	*cellinfo;
int x1;
int y1;
int x2;
int y2;
{
 XSegment	seg_f[50];
 XSegment	seg_b[50];
 int		nb_seg_f = 0;
 int		nb_seg_b = 0;
 int		pattern, height, i;
 boolean	*definition;
  
 pattern = cborder->border_stipple_pattern;
 if (Cell_Pattern[pattern].type == PATTERN_LINE)
 {
  height = Cell_Pattern[pattern].height;
  definition = Cell_Pattern[pattern].definition.line.definition;
  if (x1 == x2)
  {
   x1 -= (height>>1);
   for(i=0; i<height; i++)
   {
    if (definition[i])
    {
     seg_f[nb_seg_f].x1 = seg_f[nb_seg_f].x2 = x1;
     seg_f[nb_seg_f].y1 = y1;
     seg_f[nb_seg_f++].y2 = y2;
    }
    else
    {
     seg_b[nb_seg_b].x1 = seg_b[nb_seg_b].x2 = x1;
     seg_b[nb_seg_b].y1 = y1;
     seg_b[nb_seg_b++].y2 = y2;
    }
    x1++;
   }
  }
  else
  {
   y1 -= (height>>1);
   for(i=0;i<height; i++)
   {
    if (definition[i])
    {
     seg_f[nb_seg_f].y1 = seg_f[nb_seg_f].y2 = y1;
     seg_f[nb_seg_f].x1 = x1;
     seg_f[nb_seg_f++].x2 = x2;
    }
    else
    {
     seg_b[nb_seg_b].y1 = seg_b[nb_seg_b].y2 = y1;
     seg_b[nb_seg_b].x1 = x1;
     seg_b[nb_seg_b++].x2 = x2;
    }
    y1++;
   }
  }
  if (nb_seg_f)
  {
   if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
    XDrawSegments(display, window, gc, seg_f, nb_seg_f);
   else
   {
    XSetClipMask(display, gc, cellinfo->clip_mask);
    XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
    XDrawSegments(display, window, gc, seg_f, nb_seg_f);
    XSetClipMask(display, gc, None);
   }
  }
  if (nb_seg_b)
  {
   XSetForeground(display, gc, cborder->border_background->cell.pixel);
   if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
    XDrawSegments(display, window, gc, seg_b, nb_seg_b);
   else
   {
    XSetClipMask(display, gc, cellinfo->clip_mask);
    XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
    XDrawSegments(display, window, gc, seg_b, nb_seg_b);
    XSetClipMask(display, gc, None);
   }
   XSetForeground(display, gc, cborder->border_foreground->cell.pixel);
  }
 }
 else
 {
  if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
   XDrawLine(display, window, gc, x1, y1, x2, y2);
  else
  {
   XSetClipMask(display, gc, cellinfo->clip_mask);
   XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
   XDrawLine(display, window, gc, x1, y1, x2, y2);
   XSetClipMask(display, gc, None);
  }
 }
}

static void CSdrawLines(display, window, gc, cborder, cellinfo, point, 
			nb_point)
Display *display;
Window window;
GC gc;
CellBorderStyle_t *cborder;
cell_info_t	*cellinfo;
XPoint *point; 
int nb_point;
{
 int pattern;
 int i;
  
 pattern = cborder->border_stipple_pattern;
 if (Cell_Pattern[pattern].type == PATTERN_LINE)
 {
  for(i=1;i<nb_point;i++)
  {
   CSdrawLine(display, window, gc, cborder, cellinfo,
	      point[i-1].x, point[i-1].y, point[i].x, point[i].y);
  }
 }
 else
 {
  if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
   XDrawLines(display, window, gc, point, nb_point, CoordModeOrigin);
  else
  {
   XSetClipMask(display, gc, cellinfo->clip_mask);
   XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
   XDrawLines(display, window, gc, point, nb_point, CoordModeOrigin);
   XSetClipMask(display, gc, None);
  }
 }
}

static void CSdrawRectangle(display, window, gc, cborder, cellinfo, 
			    x, y, width, height)
Display *display;
Window window;
GC gc;
CellBorderStyle_t *cborder;
cell_info_t	*cellinfo;
int x;
int y;
int width;
int height;
{
 XRectangle	rect_f[50];
 XRectangle	rect_b[50];
 int		nb_rect_f = 0;
 int		nb_rect_b = 0;
 int		pattern, thick, i;
 boolean	*definition;
  
 pattern = cborder->border_stipple_pattern;
 if (Cell_Pattern[pattern].type == PATTERN_LINE)
 {
  thick = Cell_Pattern[pattern].height;
  definition = Cell_Pattern[pattern].definition.line.definition;
  x -= (thick>>1);
  y -= (thick>>1);
  width +=  thick - 2;
  height += thick - 2;
  for(i=0; i<thick; i++)
  {
   if (definition[i])
   {
    rect_f[nb_rect_f].x = x;
    rect_f[nb_rect_f].y = y;
    rect_f[nb_rect_f].width = width;
    rect_f[nb_rect_f++].height = height;
   }
   else
   {
    rect_b[nb_rect_b].x = x; 
    rect_b[nb_rect_b].y = y;
    rect_b[nb_rect_b].width = width;
    rect_b[nb_rect_b++].height = height;
   }
   x++; y++; width -=2; height-=2;
  }
  if (nb_rect_f)
  {
   if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
    XDrawRectangles(display, window, gc, rect_f, nb_rect_f);
   else
   {
    XSetClipMask(display, gc, cellinfo->clip_mask);
    XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
    XDrawRectangles(display, window, gc, rect_f, nb_rect_f);
    XSetClipMask(display, gc, None);
   }
  }
  if (nb_rect_b)
  {
   XSetForeground(display, gc, cborder->border_background->cell.pixel);
   if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
    XDrawRectangles(display, window, gc, rect_b, nb_rect_b);
   else
   {
    XSetClipMask(display, gc, cellinfo->clip_mask);
    XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
    XDrawRectangles(display, window, gc, rect_b, nb_rect_b);
    XSetClipMask(display, gc, None);
   }
   XSetForeground(display, gc, cborder->border_foreground->cell.pixel);
  }
 }
 else
 {
  if (!cellinfo || !(cellinfo->clip_type & XcCTM_CLIPMASK))
   XDrawRectangle(display, window, gc, x, y, width, height);
  else
  {
   XSetClipMask(display, gc, cellinfo->clip_mask);
   XSetClipOrigin(display, gc, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
   XDrawRectangle(display, window, gc, x, y, width, height);
   XSetClipMask(display, gc, None);
  }
 }
}

void DisplayCellStyleBorders(This, cell_info, x, y, width, height, 
			     left_done, right_done, up_done, down_done)
c_CellStyle	*This;
cell_info_t	*cell_info;
int		x;
int		y;
int		width;
int		height;
boolean		left_done;
boolean		right_done;
boolean		up_done;
boolean		down_done;
{
 XPoint	point[5];
 CellStyle_t	*cell_style;
  
 cell_style = cell_info->cstyle;
 x--;  y--;
 if (!left_done)
 {
  if (diff_border(&cell_style->left,
		  &This->cell_style_current.left))
   change_gc_border(This, cell_style, BORDER_LEFT);
  if (down_done || diff_border(&cell_style->left, &cell_style->down))
  {
   if (up_done || diff_border(&cell_style->left, &cell_style->up))
   {
    CSdrawLine(This->display, This->window, This->gc_left,
	       &cell_style->left, cell_info, x, y, x, y+height);
   }
   else
   {
    point[0].x = point[1].x = x;
    point[2].x = x+width;
    point[0].y = y+height;
    point[1].y = point[2].y = y;
    up_done = TRUE;
    if (right_done || 
	diff_border(&cell_style->up, &cell_style->right))
    {
     CSdrawLines(This->display, This->window, 
		 This->gc_left, &cell_style->left,
		 cell_info, point, 3);
    }
    else
    {
     point[3].x = x+width;
     point[3].y = y+height;
     right_done = TRUE;
     CSdrawLines(This->display, This->window, 
		 This->gc_left, &cell_style->left, 
		 cell_info, point, 4);
    }
   }
  }
  else
  {
   if (!right_done && !up_done &&
       !diff_border(&cell_style->down, &cell_style->right) && 
       !diff_border(&cell_style->right, &cell_style->up) )
   {
    left_done = up_done = right_done = down_done = TRUE;
    CSdrawRectangle(This->display, This->window, This->gc_left, 
		    &cell_style->left, cell_info, 
		    x, y, width+1, height+1);
   }
   else
   {
    CSdrawLine(This->display, This->window, This->gc_left,
	       &cell_style->left, cell_info, x, y, x, y+height);
   }
  }
 }
 if (!up_done)
 {
  if (diff_border(&cell_style->up,
		  &This->cell_style_current.up))
   change_gc_border(This, cell_style, BORDER_UP);
  if (right_done || diff_border(&cell_style->up, &cell_style->right))
  {
   CSdrawLine(This->display, This->window, This->gc_up, 
	      &cell_style->up, cell_info, x, y, x+width, y);
  }
  else
  {
   point[0].x = x;
   point[1].x = point[2].x = x+width;
   point[0].y = point[1].y = y;
   point[2].y = y+height;
   right_done = TRUE;
   if (down_done || diff_border(&cell_style->right, &cell_style->down))
   {
    CSdrawLines(This->display, This->window, 
		This->gc_up, &cell_style->up, cell_info, point, 3);
   }
   else
   {
    point[3].x = x;
    point[3].y = y+height;
    down_done = TRUE;
    CSdrawLines(This->display, This->window, This->gc_up, 
		&cell_style->up, cell_info, point, 4);
   }
  }
 }
 if (!right_done)
 {
  if (diff_border(&cell_style->right,
		  &This->cell_style_current.right))
   change_gc_border(This, cell_style, BORDER_RIGHT);
  if (down_done || diff_border(&cell_style->right, &cell_style->down))
  {
   CSdrawLine(This->display, This->window, This->gc_right, 
	      &cell_style->right, cell_info, 
	      x+width, y, x+width, y+height);
  }
  else
  {
   point[0].x = point[1].x = x+width;
   point[2].x = x;
   point[0].y =  y;
   point[1].y = point[2].y = y+height;
   down_done = TRUE;
   if (left_done || diff_border(&cell_style->down, &cell_style->left))
   {
    CSdrawLines(This->display, This->window, 
		This->gc_right, &cell_style->right,
		cell_info, point, 3);
   }
   else
   {
    point[3].x = x;
    point[3].y = y;
    CSdrawLines(This->display, This->window, 
		This->gc_right, &cell_style->right,
		cell_info, point, 4);
   }
  }    
 }
 if (!down_done)
 {
  if (diff_border(&cell_style->down,
		  &This->cell_style_current.down))
   change_gc_border(This, cell_style, BORDER_DOWN);
  if (left_done || diff_border(&cell_style->down, &cell_style->left))
  {
   CSdrawLine(This->display, This->window, This->gc_down, 
	      &cell_style->down, cell_info, x, y+height,
	      x+width, y+height);
  }
  else
  {
   point[0].x =  x+width;
   point[1].x = point[2].x = x;
   point[0].y = point[1].y = y+height;
   point[2].y = y;
   if (up_done || diff_border(&cell_style->left, &cell_style->up))
   {
    CSdrawLines(This->display, This->window, 
		This->gc_down, &cell_style->down,
		cell_info, point, 3);
   }
   else
   {
    point[3].x = x+width;
    point[3].y = y;
    CSdrawLines(This->display, This->window, 
		This->gc_down, &cell_style->down,
		cell_info, point, 4);
   }
  }    
 }
}

/* ----------------------------------------------------------------- ** 
** diff_inside - Conpare inside parameters                           ** 
** ----------------------------------------------------------------- */
static boolean diff_inside(cell_style1, cell_style2) 
CellStyle_t	*cell_style1;
CellStyle_t	*cell_style2;
{
 if (cell_style1->inside_foreground==cell_style2->inside_foreground && 
     cell_style1->inside_background==cell_style2->inside_background && 
     cell_style1->inside_stipple_pattern==
     cell_style2->inside_stipple_pattern)
  return FALSE;
 return TRUE; 
}

void DisplayCellStyleInside(This, cell_info, x, y, width, height, xg, yg)
c_CellStyle	*This;
cell_info_t	*cell_info;
int		x;
int		y;
int		width;
int		height;
int		xg;
int		yg;
{
 int		masq_bin;
 XGCValues	gc_values;
 CellStyle_t	*cstyle;
 int		left_b, right_b, up_b, down_b;

 cstyle = cell_info->cstyle;
  
 left_b = (ABS(cell_info->left_border-1)>>1);
 right_b = cell_info->right_border?((cell_info->right_border+2)>>1):0;
 up_b = (ABS(cell_info->up_border-1)>>1);
 down_b = cell_info->down_border?((cell_info->down_border+2)>>1):0;
  
 x += left_b;
 y += up_b;
 width -=  left_b + right_b;
 height -= up_b + down_b;

 if (diff_inside(cstyle, &This->cell_style_current))
 {
  gc_values.foreground = cstyle->inside_foreground->cell.pixel;
  masq_bin = GCForeground | GCBackground | GCFillStyle;
  if (cstyle->inside_stipple_pattern != PATTERN_NONE)
  {
   if (cstyle->inside_background->transparency)
    gc_values.fill_style = FillStippled;
   else
    gc_values.fill_style = FillOpaqueStippled;
   masq_bin |= GCStipple;
   gc_values.stipple = 
    Cell_Pattern
     [cstyle->inside_stipple_pattern].definition.data.pixmap;
  }
  else
  {
   gc_values.foreground = F(GlobColormap).get_spec_color(GlobColormap, 0);
   gc_values.fill_style = FillSolid;
  }
  gc_values.background = cstyle->inside_background->cell.pixel;
  XChangeGC(This->display, This->gc_inside, masq_bin, &gc_values);
    
  This->cell_style_current.inside_foreground = cstyle->inside_foreground;
  This->cell_style_current.inside_background = cstyle->inside_background;
  This->cell_style_current.inside_stipple_pattern = 
   cstyle->inside_stipple_pattern;
 }
  
 if (cstyle->inside_stipple_pattern!=PATTERN_NONE)
 {
  XSetTSOrigin(This->display, This->gc_inside,  
	       (xg)%Cell_Pattern[cstyle->inside_stipple_pattern].width,
	       (yg)%Cell_Pattern[cstyle->inside_stipple_pattern].height);
 }
  
 if (cell_info->clip_type & XcCTM_CLIPMASK)
 {
  XSetClipMask(This->display, This->gc_inside, cell_info->clip_mask);
  XSetClipOrigin(This->display, This->gc_inside, cell_info->clip_origin_x,
		 cell_info->clip_origin_y);
  XFillRectangle(This->display, This->window, This->gc_inside,
		 x, y, width, height);
  XSetClipMask(This->display, This->gc_inside, None);
 }
 else
  XFillRectangle(This->display, This->window, This->gc_inside,
		 x, y, width, height);
}

/* ----------------------------------------------------------------- ** 
** diff_inside - Conpare inside parameters                           ** 
** ----------------------------------------------------------------- */
#if 0
static boolean diff_text(cell_style1, cell_style2) 
CellStyle_t	*cell_style1;
CellStyle_t	*cell_style2;
{
 if (cell_style1->text_style==cell_style2->text_style && 
     F(cell_style1->text_style).compare
     (cell_style1->text_style, cell_style2->text_style, CMP_WITH_NAME) )
  return FALSE;
 return TRUE; 
}
#endif

boolean CalculTextOverrun(This, txt, cstyle, over_pos, over_neg, factor)
c_CellStyle	*This;
char		*txt;
CellStyle_t	*cstyle;
coord_t		*over_pos;
coord_t		*over_neg;
scale_t		factor;
{
 int		length;
 ruler_t	ruler;
 angle_t	angle;
 boolean	angle0;
 boolean	swapa = FALSE;
 coord_t	txt_width;
 c_TextStyle	*tstyle;
  
 tstyle = cstyle->text_style;
 ruler = cstyle->ruler_type; 
 switch(ruler & XqR_A_MASK)
 {
 case XqR_A_90:
  angle = SCALE_FROM_DEGREES(90);
  angle0 = FALSE;
  swapa = TRUE;
  break;
 case XqR_A_270:
  angle = SCALE_FROM_DEGREES(270);
  angle0 = FALSE;
  break;
 case XqR_A_180: 
  angle = ANGLE_ZERO;
  angle0 = FALSE;
  break;
 case XqR_A_DEFAULT:
 default:
  angle = ANGLE_ZERO;
  angle0 = TRUE;
  break;
 }

 if (!cstyle->metrics || cstyle->metrics->angle != angle || 
     cstyle->factor != factor)
 {
  cstyle->metrics = 
   F(tstyle->metrics).transform(tstyle->metrics, factor,
				XcTS_GET_WIDTH(tstyle), angle);
  if (cstyle->rfont)
  {
   F(GlobFntMng).delete_font(GlobFntMng, cstyle->rfont);
   cstyle->rfont = NULL;
  }
  cstyle->factor = factor;
 }

 if (txt)
  length = strlen(txt);
 else
  length = 0;
  
 if (angle0)
  txt_width = F(cstyle->metrics).textWidth(cstyle->metrics, txt, length);
 else
  txt_width = 
   ABS(F(cstyle->metrics).textHeight(cstyle->metrics, txt, length));
 *over_pos = *over_neg = COORD_ZERO;
 if (ruler & XqR_MULTILINE)
  return angle0;
  
 switch(ruler & XqR_H_ALIGN_MASK)
 {
 case XqR_DEFAULT:	
 case XqR_H_ALIGN_LEFT: 
  if (swapa)
   *over_neg = txt_width;
  else
   *over_pos = txt_width;
  break;
 case XqR_H_ALIGN_RIGHT:
  if (swapa)
   *over_pos = txt_width;
  else
   *over_neg = txt_width;
  break;
 case XqR_H_ALIGN_CENTER:
  *over_pos = *over_neg = (txt_width >> 1);
  break;
 case XqR_H_JUSTIFIED:
  break;
 }
 return angle0;
}

#define IDEAL_MARGINS (8 * COORD_DEFAULT * factor)

coord_t	CalculIdealCellWidth(This, cellule, cstyle, height, factor)
c_CellStyle	*This;
cell		*cellule;
CellStyle_t	*cstyle;
coord_t		height;
scale_t		factor;
{
 char		value[Xq_FORMULA_LENGTH];
 int		value_type, length;
 ruler_t	ruler;
 angle_t	angle;
 boolean	angle0;
 c_TextStyle	*tstyle;
 coord_t	txt_width;
  
 tstyle = cstyle->text_style;
 ruler = cstyle->ruler_type; 
 switch(ruler & XqR_A_MASK)
 {
 case XqR_A_90:
  angle = SCALE_FROM_DEGREES(90);
  angle0 = FALSE;
  break;
 case XqR_A_270:
  angle = SCALE_FROM_DEGREES(270);
  angle0 = FALSE;
  break;
 case XqR_A_180: 
  angle = ANGLE_ZERO;
  angle0 = FALSE;
  break;
 case XqR_A_DEFAULT:
 default:
  angle = ANGLE_ZERO;
  angle0 = TRUE;
  break;
 }
  
 if (This->display_formula)
  value_type = 
   F(MOTEUR).get_string(MOTEUR, cellule, value, Xq_FORMULA);
 else    
  value_type = 
   F(MOTEUR).get_string(MOTEUR, cellule, value, Xq_VALUE);
 if (value[0]==0)
  return COORD_ZERO;
  
 if (Xq_IS_VALUE(value_type))
 {
  F(cstyle->format).convert_string(cstyle->format, value, value_type, NULL);
 }
 length = strlen(value);
  
 if (!cstyle->metrics || cstyle->metrics->angle != angle || 
     cstyle->factor != factor)
 {
  cstyle->metrics = 
   F(tstyle->metrics).transform(tstyle->metrics, factor,
				XcTS_GET_WIDTH(tstyle), angle);
  if (cstyle->rfont)
  {
   F(GlobFntMng).delete_font(GlobFntMng, cstyle->rfont);
   cstyle->rfont = NULL;
  }
  cstyle->factor = factor;
 }
  
 if (angle0)
 {
  txt_width = F(cstyle->metrics).textWidth(cstyle->metrics, value, length);
  if (value_type == Xq_TEXT && (ruler & XqR_MULTILINE)) 
  {
   if (txt_width < DEFAULT_COL_SIZE)
    return txt_width + IDEAL_MARGINS;
   else
    return DEFAULT_COL_SIZE + IDEAL_MARGINS;
  }
  return txt_width + IDEAL_MARGINS;
 }
 else
 {
  if (value_type == Xq_TEXT && (ruler & XqR_MULTILINE)) 
  {
   DS_line_t	*DS_line;
   DS_line_t	*DS_line2;
   int		nb_DS_line;
       
   DS_line = (DS_line_t *) Xc_malloc("ds_line base", sizeof(DS_line_t));
   memset(DS_line, 0, sizeof(DS_line_t));
   DS_line->char_inc = DS_line->word_inc = DS_line->offset = (coord_t)0; 
   nb_DS_line = 1;
   DS_line->string = value;
   DS_line->length = length;
   nb_DS_line = cut_text_for_multiline(DS_line, height, cstyle->metrics,
				       TRUE);
   while(DS_line)
   {
    DS_line2 = DS_line->Next;
    Xc_free(DS_line);
    DS_line = DS_line2;
   }
   return ((cstyle->metrics->ascender - cstyle->metrics->descender)
	   * nb_DS_line + IDEAL_MARGINS);
  }
  return (cstyle->metrics->ascender - cstyle->metrics->descender)
   + IDEAL_MARGINS;
 }
}

coord_t	CalculIdealCellHeight(This, cellule, cstyle, width, factor)
c_CellStyle	*This;
cell		*cellule;
CellStyle_t	*cstyle;
coord_t		width;
scale_t		factor;
{
 char		value[Xq_FORMULA_LENGTH];
 int		value_type, length;
 ruler_t	ruler;
 angle_t	angle;
 boolean	angle0;
 c_TextStyle	*tstyle;
 coord_t	txt_width;
  
 tstyle = cstyle->text_style;
 ruler = cstyle->ruler_type; 
 switch(ruler & XqR_A_MASK)
 {
 case XqR_A_90:
  angle = SCALE_FROM_DEGREES(90);
  angle0 = FALSE;
  break;
 case XqR_A_270:
  angle = SCALE_FROM_DEGREES(270);
  angle0 = FALSE;
  break;
 case XqR_A_180: 
  angle = ANGLE_ZERO;
  angle0 = FALSE;
  break;
 case XqR_A_DEFAULT:
 default:
  angle = ANGLE_ZERO;
  angle0 = TRUE;
  break;
 }
  
 if (This->display_formula)
  value_type = 
   F(MOTEUR).get_string(MOTEUR, cellule, value, Xq_FORMULA);
 else    
  value_type = 
   F(MOTEUR).get_string(MOTEUR, cellule, value, Xq_VALUE);
 if (value[0]==0)
  return COORD_ZERO;
  
 if (Xq_IS_VALUE(value_type))
  F(cstyle->format).convert_string(cstyle->format, value, value_type, NULL);

 length = strlen(value);
  
 if (!cstyle->metrics || cstyle->metrics->angle != angle || 
     cstyle->factor != factor)
 {
  cstyle->metrics = 
   F(tstyle->metrics).transform(tstyle->metrics, factor,
				XcTS_GET_WIDTH(tstyle), angle);
  if (cstyle->rfont)
  {
   F(GlobFntMng).delete_font(GlobFntMng, cstyle->rfont);
   cstyle->rfont = NULL;
  }
  cstyle->factor = factor;
 }
  
 if (angle0)
 {
  if (value_type == Xq_TEXT && (ruler & XqR_MULTILINE)) 
  {
   DS_line_t	*DS_line;
   DS_line_t	*DS_line2;
   int		nb_DS_line;
       
   DS_line = (DS_line_t *) Xc_malloc("ds_line base", sizeof(DS_line_t));
   memset(DS_line, 0, sizeof(DS_line_t));
   DS_line->char_inc = DS_line->word_inc = DS_line->offset = (coord_t)0; 
   nb_DS_line = 1;
   DS_line->string = value;
   DS_line->length = length;
   nb_DS_line = cut_text_for_multiline(DS_line, width, cstyle->metrics,
				       FALSE);
   while(DS_line)
   {
    DS_line2 = DS_line->Next;
    Xc_free(DS_line);
    DS_line = DS_line2;
   }
   return ((cstyle->metrics->ascender - cstyle->metrics->descender)
	   * nb_DS_line + IDEAL_MARGINS);
    
    
  }
  return (cstyle->metrics->ascender - cstyle->metrics->descender)
   + IDEAL_MARGINS;
 }
 else
 {
  txt_width = ABS(F(cstyle->metrics).textHeight(cstyle->metrics,
						value, length));
  if (value_type == Xq_TEXT && (ruler & XqR_MULTILINE)) 
  {
   if (txt_width < DEFAULT_ROW_SIZE)
    return txt_width + IDEAL_MARGINS;
   else
    return DEFAULT_ROW_SIZE + IDEAL_MARGINS;
  }
  return txt_width + IDEAL_MARGINS;
 }
}

void DisplayCellStyleText(This, cell_info, x, y, width, height, factor)
c_CellStyle	*This;
cell_info_t	*cell_info;
int		x;
int		y;
int		width;
int		height;
scale_t		factor;
{
 char		value[Xq_FORMULA_LENGTH];
 char		svg_value[Xq_FORMULA_LENGTH];
 char		*str_clip;
 int		length;
 ruler_t	ruler;
 CellStyle_t	*cstyle;
 int		value_type;
 coord_t	txt_width;
 coord_t	txt_height;
 coord_t	font_height;
 coord_t	sWidth;
 coord_t	sHeight;
 coord_t	sX;
 coord_t	sY;
 coord_t	sY_inc;
 coord_t	sY_cumul;
 coord_t	ascend, descend;
 coord_t	diese_width, string_width, inc_width;
 coord_t	sTmp;
 int		nb_diese, i, j, k;
 c_TextStyle	*tstyle;
 vector_t	pos, pos2;
 int		left_b, right_b, up_b, down_b;
 int		trunc_beg, trunc_end, comma_pos;
 conv_info_t	conv_info;
 c_Color	*color=NULL;
 int		nb_DS_line;
 DS_line_t	*DS_line;
 DS_line_t	*DS_line2;
 angle_t	angle = SCALE_ZERO;
 boolean	angle90 = FALSE, set_diese, comma, carry;
  
 factor = factor * COORD_SCALEVALUE / COORD_DEFAULT;

 left_b = (ABS(cell_info->left_border-1)>>1);
 right_b = cell_info->right_border?((cell_info->right_border+2)>>1):0;
 up_b = (ABS(cell_info->up_border-1)>>1);
 down_b = cell_info->down_border?((cell_info->down_border+2)>>1):0;
  
 x += left_b;
 y += up_b;
 width -=  left_b + right_b;
 height -= up_b + down_b;
  
 if (width<=2 || height<=2)
  return;
  
 if (This->display_formula)
  value_type = 
   F(MOTEUR).get_string(MOTEUR, cell_info->cellule, 
			value, Xq_FORMULA);
 else    
  value_type = 
   F(MOTEUR).get_string(MOTEUR, cell_info->cellule, 
			value, Xq_VALUE);
  
 if (value[0] == 0)
 {
  boolean overrun = FALSE;

  overrun = (cell_info->cell_over_down || cell_info->cell_over_up||
	     cell_info->cell_over_left || cell_info->cell_over_right);
  if (overrun)
  {
   if (cell_info->clip_type & XcCTM_CLIPMASK)
   {
    XSetClipMask(This->display, This->gc_paint, cell_info->clip_mask);
    XSetClipOrigin(This->display, This->gc_paint,
		   cell_info->clip_origin_x,
		   cell_info->clip_origin_y);
    if (This->gc_mask != (GC)0)
    {
     XSetClipMask(This->display,This->gc_mask,cell_info->clip_mask);
     XSetClipOrigin(This->display, This->gc_mask, 
		    cell_info->clip_origin_x,
		    cell_info->clip_origin_y);
    }
   }
   else
   {
    XRectangle rectangle;
	  
    rectangle.x = x;
    rectangle.y = y;
    rectangle.width = width;
    rectangle.height = height;
    XSetClipRectangles(This->display, This->gc_paint, 0, 0, 
		       &rectangle, 1, Unsorted);
    if (This->gc_mask != (GC)0)
     XSetClipRectangles(This->display, This->gc_mask, 0, 0, 
			&rectangle, 1, Unsorted);
   }
  }
  else
   return;
  /* ----------------------------------------------------------------- ** 
  ** Overrun on the bottom                                             ** 
  ** ----------------------------------------------------------------- */
  if (cell_info->cell_over_down)
  {
   F(MOTEUR).get_string(MOTEUR, cell_info->cell_over_down, 
			value, Xq_TEXT);
   cstyle = cell_info->cell_over_down->overrun_cstyle;
   tstyle = cstyle->text_style;
   ruler = cstyle->ruler_type;
   if ((ruler & XqR_A_MASK) == XqR_A_90)
    angle = SCALE_FROM_DEGREES(90);
   else
    angle = SCALE_FROM_DEGREES(270);
	
   update_metrics(This, cstyle, tstyle, angle, factor);
   if (This->text_color != tstyle->color->cell.pixel)
   {
    This->text_color = tstyle->color->cell.pixel;
    F(tstyle->color).setGC(tstyle->color,This->display, This->gc_paint);
   }
   ascend = cstyle->metrics->ascender;
   descend = cstyle->metrics->descender;
   txt_height = font_height = ascend - descend;
   sWidth = SCALE_FROM_POINTS(width - 2);
   sHeight = SCALE_FROM_POINTS(height);
   sX = SCALE_FROM_POINTS(x);
   sY = SCALE_FROM_POINTS(y);
	
	
   if ((ruler & XqR_A_MASK) == XqR_A_90)
   {
    switch(ruler & XqR_V_ALIGN_MASK)
    {
    case XqR_V_ALIGN_CENTER:
    case XqR_V_JUSTIFIED: 
     pos.dx = sX + ((sWidth-txt_height)>>1);
     break;
    case XqR_V_ALIGN_TOP:
     pos.dx = sX;
     break;
    case XqR_V_ALIGN_BOTTOM:
     pos.dx = sX + sWidth - txt_height;
     break;
    }
    pos.dx += ascend;
    pos.dy = cell_info->cell_over_down->overrun_size +
     cell_info->cell_over_down->overrun_pos +
      cell_info->cell_over_down->overrun_neg - 
       cell_info->offset_over_down;
	
    calcul_string_clipping(cstyle->metrics, TRUE, value, pos.dy,
			   sHeight, &str_clip, &length, &sTmp);
    pos.dy = sY + sHeight + pos.dy - sTmp;
	  
   }
   else
   {
    switch(ruler & XqR_V_ALIGN_MASK)
    {
    case XqR_V_ALIGN_CENTER:
    case XqR_V_JUSTIFIED: 
     pos.dx = sX + ((sWidth-txt_height)>>1);
     break;
    case XqR_V_ALIGN_TOP:
     pos.dx = sX + sWidth - txt_height;
     break;
    case XqR_V_ALIGN_BOTTOM:
     pos.dx = sX;
     break;
    }
    pos.dx -= descend;
    pos.dy = cell_info->offset_over_down - sHeight;
	  
    calcul_string_clipping(cstyle->metrics, TRUE, value, pos.dy,
			   sHeight, &str_clip, &length, &sTmp);
    pos.dy = sY - pos.dy + sTmp;
	  
   }
   F(cstyle->rfont).drawString(cstyle->rfont, This->display, This->window,
			       This->gc_mask, This->gc_paint, &pos, 
			       0, 0, str_clip, length, tstyle); 
  }
  /* ----------------------------------------------------------------- ** 
  ** Overrun on the top                                                ** 
  ** ----------------------------------------------------------------- */
  if (cell_info->cell_over_up)
  {
   F(MOTEUR).get_string(MOTEUR, cell_info->cell_over_up, 
			value, Xq_TEXT);
   cstyle = cell_info->cell_over_up->overrun_cstyle;
   tstyle = cstyle->text_style;
   ruler = cstyle->ruler_type;
   if ((ruler & XqR_A_MASK) == XqR_A_90)
    angle = SCALE_FROM_DEGREES(90);
   else
    angle = SCALE_FROM_DEGREES(270);
	
   update_metrics(This, cstyle, tstyle, angle, factor);
   if (This->text_color != tstyle->color->cell.pixel)
   {
    This->text_color = tstyle->color->cell.pixel;
    F(tstyle->color).setGC(tstyle->color,This->display, This->gc_paint);
   }
   ascend = cstyle->metrics->ascender;
   descend = cstyle->metrics->descender;
   txt_height = font_height = ascend - descend;
   sWidth = SCALE_FROM_POINTS(width - 2);
   sHeight = SCALE_FROM_POINTS(height);
   sX = SCALE_FROM_POINTS(x);
   sY = SCALE_FROM_POINTS(y);
	
   if ((ruler & XqR_A_MASK) == XqR_A_90)
   {
    switch(ruler & XqR_V_ALIGN_MASK)
    {
    case XqR_V_ALIGN_CENTER:
    case XqR_V_JUSTIFIED: 
     pos.dx = sX + ((sWidth-txt_height)>>1);
     break;
    case XqR_V_ALIGN_TOP:
     pos.dx = sX;
     break;
    case XqR_V_ALIGN_BOTTOM:
     pos.dx = sX + sWidth - txt_height;
     break;
    }
    pos.dx += ascend;
    pos.dy = cell_info->offset_over_up;
    calcul_string_clipping(cstyle->metrics, TRUE, value, 
			   pos.dy - sHeight, sHeight, 
			   &str_clip, &length, &sTmp);
    pos.dy = sY + pos.dy - sTmp;
   }
   else
   {
    switch(ruler & XqR_V_ALIGN_MASK)
    {
    case XqR_V_ALIGN_CENTER:
    case XqR_V_JUSTIFIED: 
     pos.dx = sX + ((sWidth-txt_height)>>1);
     break;
    case XqR_V_ALIGN_TOP:
     pos.dx = sX + sWidth - txt_height;
     break;
    case XqR_V_ALIGN_BOTTOM:
     pos.dx = sX;
     break;
    }
    pos.dx -= descend;
    pos.dy = cell_info->cell_over_up->overrun_size +
     cell_info->cell_over_up->overrun_pos + 
      cell_info->cell_over_up->overrun_neg  - 
       cell_info->offset_over_up;
	  
    calcul_string_clipping(cstyle->metrics, TRUE, value, pos.dy,
			   sHeight, &str_clip, &length, &sTmp);
    pos.dy = sY - pos.dy + sTmp;
   }
	
   F(cstyle->rfont).drawString(cstyle->rfont, This->display, This->window,
			       This->gc_mask, This->gc_paint, &pos, 
			       0, 0, str_clip, length, tstyle); 
  }
  /* ----------------------------------------------------------------- ** 
  ** Overrun on the right                                              ** 
  ** ----------------------------------------------------------------- */
  if (cell_info->cell_over_right)
  {
   F(MOTEUR).get_string(MOTEUR, cell_info->cell_over_right, 
			value, Xq_TEXT);
   cstyle = cell_info->cell_over_right->overrun_cstyle;
   tstyle = cstyle->text_style;
   ruler = cstyle->ruler_type;
   angle = ANGLE_ZERO;
	
   update_metrics(This, cstyle, tstyle, angle, factor);
   if (This->text_color != tstyle->color->cell.pixel)
   {
    This->text_color = tstyle->color->cell.pixel;
    F(tstyle->color).setGC(tstyle->color,This->display, This->gc_paint);
   }
   ascend = cstyle->metrics->ascender;
   descend = cstyle->metrics->descender;
   txt_height = font_height = ascend - descend;
   sWidth = SCALE_FROM_POINTS(width);
   sHeight = SCALE_FROM_POINTS(height - 2);
   sX = SCALE_FROM_POINTS(x);
   sY = SCALE_FROM_POINTS(y);
      
   pos.dx = cell_info->offset_over_right - sWidth;
   switch(ruler & XqR_V_ALIGN_MASK)
   {
   case XqR_V_ALIGN_CENTER:
   case XqR_V_JUSTIFIED: 
    pos.dy = sY + ((sHeight-txt_height)>>1) + ascend;
    break;
   case XqR_V_ALIGN_TOP:
    pos.dy = sY + ascend;
    break;
   case XqR_V_ALIGN_BOTTOM:
    pos.dy = sY + sHeight - txt_height + ascend;
    break;
   }
   calcul_string_clipping(cstyle->metrics, FALSE, value, pos.dx,
			  sWidth, &str_clip, &length, &sTmp);
   pos.dx = sX - pos.dx + sTmp;
	
   F(cstyle->rfont).drawString(cstyle->rfont, This->display, This->window,
			       This->gc_mask, This->gc_paint, &pos, 
			       0, 0, str_clip, length, tstyle); 
  }
  /* ----------------------------------------------------------------- ** 
  ** Overrun on the left                                               ** 
  ** ----------------------------------------------------------------- */
  if (cell_info->cell_over_left)
  {
   F(MOTEUR).get_string(MOTEUR, cell_info->cell_over_left, 
			value, Xq_TEXT);
   cstyle = cell_info->cell_over_left->overrun_cstyle;
   tstyle = cstyle->text_style;
   ruler = cstyle->ruler_type;
   angle = ANGLE_ZERO;
	
   update_metrics(This, cstyle, tstyle, angle, factor);
   if (This->text_color != tstyle->color->cell.pixel)
   {
    This->text_color = tstyle->color->cell.pixel;
    F(tstyle->color).setGC(tstyle->color,This->display, This->gc_paint);
   }
   ascend = cstyle->metrics->ascender;
   descend = cstyle->metrics->descender;
   txt_height = font_height = ascend - descend;
   sWidth = SCALE_FROM_POINTS(width - 2);
   sHeight = SCALE_FROM_POINTS(height - 2);
   sX = SCALE_FROM_POINTS(x);
   sY = SCALE_FROM_POINTS(y);
	
   pos.dx = cell_info->cell_over_left->overrun_size +
    cell_info->cell_over_left->overrun_pos + 
     cell_info->cell_over_left->overrun_neg  - 
      cell_info->offset_over_left;
	
   switch(ruler & XqR_V_ALIGN_MASK)
   {
   case XqR_V_ALIGN_CENTER:
   case XqR_V_JUSTIFIED: 
    pos.dy = sY + ((sHeight-txt_height)>>1) + ascend;
    break;
   case XqR_V_ALIGN_TOP:
    pos.dy = sY + ascend;
    break;
   case XqR_V_ALIGN_BOTTOM:
    pos.dy = sY + sHeight - txt_height + ascend;
    break;
   }
   calcul_string_clipping(cstyle->metrics, FALSE, value, pos.dx,
			  sWidth, &str_clip, &length, &sTmp);
   pos.dx = sX - pos.dx + sTmp;
	
   F(cstyle->rfont).drawString(cstyle->rfont, This->display, This->window,
			       This->gc_mask, This->gc_paint, &pos, 
			       0, 0, str_clip, length, tstyle); 
  }
  XSetClipMask(This->display,This->gc_paint, None);
  if(This->gc_mask != (GC)0) XSetClipMask(This->display,This->gc_mask, None);
  /* ----------------------------------------------------------------- ** 
  ** End of Overrun redraw                                             ** 
  ** ----------------------------------------------------------------- */
  return;
 }
  
 cstyle = cell_info->cstyle;
 tstyle = cstyle->text_style;
  
 if (Xq_IS_VALUE(value_type))
 {
  memcpy(svg_value, value, strlen(value) + 1);
  F(cstyle->format).convert_string(cstyle->format, value, value_type, 
				   &conv_info);
  color = conv_info.color;
  if (!This->display_zero_cell && strlen(value) == 1 && *value == '0')
   return;
 }
  
 /* ---------------------------------------------------------------- */
 /* ----------  test if the font must be rasterized  --------------- */
 /* ---------------------------------------------------------------- */
 ruler = cstyle->ruler_type;
 if ((ruler & XqR_H_ALIGN_MASK) == XqR_DEFAULT)
 {
  ruler = (ruler &  (~XqR_H_ALIGN_MASK)) | 
   (Xq_IS_VALUE(value_type) ? conv_info.ruler : XqR_H_ALIGN_LEFT);
 }
  
 switch(ruler & XqR_A_MASK)
 {
 case XqR_A_DEFAULT:
  angle = ANGLE_ZERO;
  angle90 = FALSE;
  break;
 case XqR_A_90:
  angle = SCALE_FROM_DEGREES(90);
  angle90 = TRUE;
  break;
 case XqR_A_270:
  angle = SCALE_FROM_DEGREES(270);
  angle90 = TRUE;
  break;
 case XqR_A_180: 
  angle = ANGLE_ZERO;
  angle90 = TRUE;
  break;
 }
  
 update_metrics(This, cstyle, tstyle, angle, factor);  
 if (color == NULL) color = tstyle->color;
 if (This->text_color != color->cell.pixel)
 {
  This->text_color = color->cell.pixel;
  F(color).setGC(color,This->display, This->gc_paint);
 }
  
 /* ------------------------------------------------------------------- */
 /* -------------------  miscellaneous calculs ------------------------ */
 /* ------------------------------------------------------------------- */
 length = strlen(value);
  
 if (!angle90)
  txt_width = F(cstyle->metrics).textWidth(cstyle->metrics, value, length);
 else
  txt_width = ABS(F(cstyle->metrics).textHeight(cstyle->metrics,
						value, length));
 ascend = cstyle->metrics->ascender;
 descend = cstyle->metrics->descender;
 txt_height = font_height = ascend - descend;
 if (angle90)
 {
  sWidth = SCALE_FROM_POINTS(width - 2);
  if (cell_info->cell_over_up || cell_info->cell_over_down)
   sHeight = SCALE_FROM_POINTS(height);
  else
   sHeight = SCALE_FROM_POINTS(height - 2);
 }
 else
 {
  if (cell_info->cell_over_right || cell_info->cell_over_left)
   sWidth = SCALE_FROM_POINTS(width);
  else
   sWidth = SCALE_FROM_POINTS(width - 2);
  sHeight = SCALE_FROM_POINTS(height - 2);
 }
 if (angle != ANGLE_ZERO)
  SWAP(coord_t, sWidth, sHeight);
  
 sX = SCALE_FROM_POINTS(x);
 sY = SCALE_FROM_POINTS(y);
 sY_inc = 0;
  
 DS_line = (DS_line_t *) Xc_malloc("ds_line base", sizeof(DS_line_t));
 memset(DS_line, 0, sizeof(DS_line_t));
 DS_line->char_inc = DS_line->word_inc = DS_line->offset = (coord_t)0; 
 nb_DS_line = 1;
 DS_line->string = value;
 DS_line->length = length;
  
 if (txt_width >= sWidth)
 {
  set_diese = TRUE;
  if (Xq_IS_VALUE(value_type))
  {
   if (conv_info.truncable)
   {
    comma_pos = conv_info.comma_pos;
    trunc_beg = conv_info.trunc_begin;
    trunc_end = conv_info.trunc_end;
    if (!angle90)
    {
     string_width = 
      F(cstyle->metrics).textWidth(cstyle->metrics, 
				   value, trunc_beg);
     string_width += 
      F(cstyle->metrics).textWidth(cstyle->metrics, 
				   value + trunc_end,
				   length - trunc_end);
    }
    else
    {
     string_width = 
      ABS(F(cstyle->metrics).textHeight(cstyle->metrics, 
					value, trunc_beg));
     string_width += 
      ABS(F(cstyle->metrics).textHeight(cstyle->metrics, 
					value + trunc_end,
					length - trunc_end));
    }
    if (string_width < sWidth)
    {
     inc_width = COORD_ZERO; i = trunc_beg;
     while( (inc_width + string_width) < sWidth && i < trunc_end)
     {
      if (!angle90)
       inc_width += 
	F(cstyle->metrics).textWidth(cstyle->metrics, 
				     value + i, 1);
      else
       inc_width += 
	ABS(F(cstyle->metrics).textHeight(cstyle->metrics, 
					  value + i, 1));
      i++;
     }
     if ( ((--i) == (trunc_beg + 1)) && (trunc_beg == comma_pos)) 
      i--;
     if ( (i == trunc_beg) && (trunc_beg == comma_pos))
      j = i+1;
     else
      j = i;
     if (value[j] >= '5')
     {
      k = 0;
      while(svg_value[k] != '.')
       k++;
      k += i - 1 - comma_pos;
      svg_value[k+1] = '\0';
      comma = FALSE; carry = TRUE;
      do
      {
       if (svg_value[k] == '.')
       {
	k--;
	comma = TRUE;
	continue;
       }
       if (svg_value[k] != '9')
       {
	if (svg_value[k]>='0' && svg_value[k]<='8')
	{
	 svg_value[k] += 1;
	 carry = FALSE;
	}
	else
	{
	 k++;
	 carry = TRUE;
	 break;
	}
       }
       else
       {
	svg_value[k] = '0';
	carry = TRUE;
	if (k==0) 
	 break;
	k--;
       }
      }
      while(carry);
      if (carry)
      {
       memmove(svg_value + k + 1, svg_value + k, 
	       strlen(svg_value)+1);
       svg_value[k] = '1';
      }
      memcpy(value, svg_value, strlen(svg_value) + 1);
      if (comma)
      {
       k = strlen(value)-1;
       while(value[k] == '0')
	k--;
       if (value[k]=='.')
	k--;
       value[k+1]='\0';
      }
	      
      Xc_TRACE(("before the second convertion: %s", value));
      F(cstyle->format).
       convert_string(cstyle->format, value, Xq_VALUE, NULL);
      length = strlen(value);
      if (!angle90)
       txt_width = 
	F(cstyle->metrics).textWidth(cstyle->metrics, 
				     value, length);
      else
       txt_width = 
	ABS(F(cstyle->metrics).textHeight(cstyle->metrics, 
					  value, length));
      if (txt_width < sWidth)
      {
       DS_line->string = value;
       DS_line->length = length;
       set_diese = FALSE;
      }
     }
     else
     {
      k = --trunc_beg;
      while(k < i && (value[k] < '1' || value[k] > '9'))
       k++;
	    
      if (k < i)
      {
       if (length - trunc_end + 1)
	memmove(value + i, value + trunc_end, 
		length - trunc_end + 1);
      }
      else
      {
       if (trunc_beg && value[trunc_beg-1] =='-')
       {
	value[trunc_beg-1] = '0';
	if (length - trunc_end + 1)
	 memcpy(value + trunc_beg, value + trunc_end, 
		length - trunc_end + 1);
       }
       else
       {
	if (length - trunc_end + 1)
	 memcpy(value + 1 + trunc_beg, value + trunc_end, 
		length - trunc_end + 1);		
       }
      }
      Xc_TRACE(("value truncated: result: %s", value));
	    
      length = strlen(value);
      if (!angle90)
       txt_width = 
	F(cstyle->metrics).textWidth(cstyle->metrics, 
				     value, length);
      else
       txt_width = 
	ABS(F(cstyle->metrics).textHeight(cstyle->metrics, 
					  value, length));
      DS_line->string = value;
      DS_line->length = length;
      set_diese = FALSE;
     }
    }
   }
  }
  if (Xq_IS_VALUE(value_type) || value_type == Xq_FORMULA)
  {
   if (set_diese)
   {
    if (!angle90)
     diese_width = 
      F(cstyle->metrics).textWidth(cstyle->metrics, "#", 1);
    else
     diese_width = 
      ABS(F(cstyle->metrics).textHeight(cstyle->metrics, "#", 1));
    nb_diese = (int)(sWidth/diese_width);
    if (nb_diese > 0)
    {
     for(i=0; i<nb_diese; i++)
      value[i]='#';
     value[i]='\0';
     length = nb_diese;
     if (!angle90)
      txt_width = F(cstyle->metrics).textWidth(cstyle->metrics, 
					       value, length);
     else
      txt_width = 
       ABS(F(cstyle->metrics).textHeight(cstyle->metrics, 
					 value, length));
     DS_line->string = value;
     DS_line->length = length;
    }
    else
    {
     Xc_free(DS_line);
     return;
    }
   }
  }
  else
  {
   if (ruler & XqR_MULTILINE) 
    nb_DS_line = cut_text_for_multiline(DS_line, sWidth, cstyle->metrics,
					angle90);
  }
 }
  
 pos.dx = sX;
 switch(ruler & XqR_H_ALIGN_MASK)
 {
 case XqR_DEFAULT:	
 case XqR_H_ALIGN_LEFT: 
  DS_line2 = DS_line;
  for(i=0; i<nb_DS_line; i++)
  {
   supress_space_and_get_width(DS_line2, 
			       cstyle->metrics, angle90, 
			       (i==0), (i==(nb_DS_line-1)));
   DS_line2 = DS_line2->Next;
  }
  break;
 case XqR_H_ALIGN_RIGHT:
  DS_line2 = DS_line;
  for(i=0; i<nb_DS_line; i++)
  {
   txt_width = 
    supress_space_and_get_width(DS_line2, 
				cstyle->metrics, angle90, 
				(i==0), (i==(nb_DS_line-1)));
   DS_line2->offset = sWidth-txt_width;
   DS_line2 = DS_line2->Next;
  }
  break;
 case XqR_H_ALIGN_CENTER:
  DS_line2 = DS_line;
  for(i=0; i<nb_DS_line; i++)
  {
   txt_width = 
    supress_space_and_get_width(DS_line2, 
				cstyle->metrics, angle90, 
				(i==0), (i==(nb_DS_line-1)));
   DS_line2->offset = (sWidth-txt_width)>>1;
   DS_line2 = DS_line2->Next;
  }
  break;
 case XqR_H_JUSTIFIED:
  DS_line2 = DS_line;
  for(i=0; i<nb_DS_line; i++)
  {
   txt_width = 
    supress_space_and_get_width(DS_line2,
				cstyle->metrics, angle90, 
				(i==0), (i==(nb_DS_line-1)));
   justify_DS_line(DS_line2, sWidth, txt_width);
   DS_line2 = DS_line2->Next;
  }
  break;
 }
  
 txt_height = font_height * nb_DS_line;
 switch(ruler & XqR_V_ALIGN_MASK)
 {
 case XqR_V_ALIGN_CENTER:
  pos.dy = sY + ((sHeight-txt_height)>>1);
  break;
 case XqR_V_ALIGN_TOP:
  pos.dy = sY;
  break;
 case XqR_V_ALIGN_BOTTOM:
  pos.dy = sY + sHeight - txt_height;
  break;
 case XqR_V_JUSTIFIED: 
  if (nb_DS_line == 1)
  {
   pos.dy = sY + ((sHeight-txt_height)>>1);
   sY_inc = 0;
  }
  else
  {
   pos.dy = sY;
   sY_inc = ((sHeight - txt_height) / (nb_DS_line - 1));
  }
  break;
 }
  
 if (cell_info->clip_type & XcCTM_CLIPMASK)
 {
  XSetClipMask(This->display, This->gc_paint, cell_info->clip_mask);
  XSetClipOrigin(This->display, This->gc_paint, cell_info->clip_origin_x,
		 cell_info->clip_origin_y);
  if (This->gc_mask != (GC)0)
  {
   XSetClipMask(This->display, This->gc_mask, cell_info->clip_mask);
   XSetClipOrigin(This->display, This->gc_mask, 
		  cell_info->clip_origin_x, cell_info->clip_origin_y);
  }
 }
 else
 {
  XRectangle rectangle;
    
  rectangle.x = x;
  rectangle.y = y;
  rectangle.width = width;
  rectangle.height = height;
  XSetClipRectangles(This->display, This->gc_paint, 0, 0, 
		     &rectangle, 1, Unsorted);
  if (This->gc_mask != (GC)0)
   XSetClipRectangles(This->display, This->gc_mask, 0, 0, 
		      &rectangle, 1, Unsorted);
 }
  
 if (nb_DS_line == 1 && DS_line->offset <= COORD_ZERO)
 {
  calcul_string_clipping(cstyle->metrics, angle90, DS_line->string, 
			 -DS_line->offset, sWidth, &str_clip, &length, &sTmp);
  DS_line->string = str_clip;
  DS_line->length = length;
  DS_line->offset += sTmp;
 }

 sY_cumul = 0;
 for(i=0; i<nb_DS_line; i++)
 {    
  pos2.dx = pos.dx + DS_line->offset;
  pos2.dy = pos.dy + sY_cumul + ascend;
  if (angle90)
  {
   if (angle == SCALE_FROM_DEGREES(90))
   {
    sTmp = pos2.dx;
    pos2.dx = sX + pos2.dy - sY;
    pos2.dy = (sY+sWidth)-(sTmp-sX);
   }
   else
   {
    sTmp = pos2.dx;
    pos2.dx = (sX+sHeight)-(pos2.dy-sY);
    pos2.dy = sY + (sTmp - sX);
   }
  }
  
  F(cstyle->rfont).drawString(cstyle->rfont, This->display, This->window,
			      This->gc_mask, This->gc_paint, &pos2, 
			      DS_line->char_inc, DS_line->word_inc,
			      DS_line->string, DS_line->length, tstyle); 
    
  sY_cumul += sY_inc + font_height;
    
  DS_line2 = DS_line;
  DS_line = DS_line->Next;
  Xc_free(DS_line2);
 }
  
 XSetClipMask(This->display,This->gc_paint, None);
 if(This->gc_mask != (GC)0) XSetClipMask(This->display,This->gc_mask, None);
}

static int cut_text_for_multiline(DS_line, sWidth, metrics, angle90)
DS_line_t	*DS_line;
coord_t		sWidth;
c_CharMetrics	*metrics;
boolean		angle90;
{
 coord_t	width_cumul;
 int		prev_space;
 int		i;
 int		cur_length;
 int		nb_DS_line;
 DS_line_t	*DS_line2;
 char	*txt;
 int	length;
  
 width_cumul = cur_length = 0;
 prev_space = -1;
 txt = DS_line->string;
 length = DS_line->length;
  
 nb_DS_line = 1;
 DS_line2 = DS_line;
 for(i=0; i<length; i++)
 {
  if (txt[i]==' ')
   prev_space = i;
  width_cumul += ((!angle90)?
		  F(metrics).textWidth(metrics, txt+i, 1):
		  ABS(F(metrics).textHeight(metrics, txt+i, 1)));
    
  if (width_cumul > sWidth)
  {
   if (!cur_length) return nb_DS_line;
   if (prev_space != -1)
   {
    DS_line2->Next = (DS_line_t *)Xc_malloc("ds_line", sizeof(DS_line_t)); 
    if (i == prev_space)
     DS_line2->length = cur_length;
    else
     DS_line2->length = cur_length - ((i-1) - prev_space);
    DS_line2 = DS_line2->Next;
    memset(DS_line2, 0, sizeof(DS_line_t));
    DS_line2->char_inc = DS_line2->word_inc = 
     DS_line2->offset = (coord_t)0; 
    DS_line2->string = txt + prev_space+1;
    DS_line2->length = length - prev_space - 1;
    nb_DS_line ++;
    width_cumul = 0;
    i = prev_space;
    prev_space = -1;
    cur_length = 0;
    continue;
   }
   else
   {
    DS_line2->Next = (DS_line_t *)Xc_malloc("ds_line", sizeof(DS_line_t)); 
    DS_line2->length = cur_length;

    DS_line2 = DS_line2->Next;
    memset(DS_line2, 0, sizeof(DS_line_t));
    DS_line2->char_inc = DS_line2->word_inc = 
     DS_line2->offset = (coord_t)0; 
    DS_line2->string = txt + i;
    DS_line2->length = length - i;
    nb_DS_line ++;
    width_cumul = 0;
    cur_length = 0;
   }
  }
  cur_length ++;
 }
 return nb_DS_line;
}

static coord_t supress_space_and_get_width(DS_line, metrics, angle90, 
					   first_line, last_line)
DS_line_t	*DS_line;
c_CharMetrics	*metrics;
boolean		angle90;
boolean		first_line;
boolean		last_line;
{
 int	nb_space_begin;
 int	nb_space_end;
 char	*txt;
 int	length;
  
 if (!DS_line->length) return 0;
  
 nb_space_begin = nb_space_end = 0;
 txt = DS_line->string;
 length = DS_line->length;
  
 if (!first_line)
 {
  while(nb_space_begin<length)
  {
   if (txt[nb_space_begin]!=' ')
    break;
   nb_space_begin ++;
  }
  if (nb_space_begin == length)
   return 0;
 }
 if (!last_line)
 {
  while(txt[length-nb_space_end-1] == ' ')
   nb_space_end ++;
 }
  
 DS_line->string = txt+nb_space_begin;
 DS_line->length = length - nb_space_begin - nb_space_end;
 return ( (!angle90) ? 
	 F(metrics).textWidth(metrics, DS_line->string, DS_line->length):
	 ABS(F(metrics).textHeight(metrics, DS_line->string,
				   DS_line->length)));
}

static void justify_DS_line(DS_line, sWidth, txt_width)
DS_line_t	*DS_line;     
coord_t		sWidth;
coord_t	txt_width;
{
 int		nb_space;
 int		nb_char;
 coord_t	delta_x;
 coord_t	prop;
 coord_t	char_inc;
 coord_t	word_inc;
 char		*txt;
 int		length;
 int		i;
  
 nb_char = nb_space = 0;
 txt = DS_line->string;
 length = DS_line->length;
  
 for(i=0; i<length; i++)
 {
  if (txt[i]==' ')
   nb_space ++;
  else
   nb_char ++;
 }
 nb_char += nb_space - 1;	/* last char with no space at his right */
  
 delta_x = sWidth - txt_width;

 if (nb_space)
 {
  prop = (delta_x * 70 / 100);
  word_inc = prop / nb_space;
  delta_x -= prop;
 }
 else
  word_inc = 0;
 if (nb_char)
  char_inc = delta_x / nb_char;
 else
  char_inc = 0;
  
 DS_line->char_inc = char_inc;
 DS_line->word_inc = word_inc;
}

static void calcul_string_clipping(metrics, angle90, orig, offset, 
				   width, dest, length, incrx)
c_CharMetrics	*metrics;
boolean	angle90;     
char	*orig;
coord_t	offset;
coord_t	width;
char	**dest;
int	*length;
coord_t	*incrx;
{
 coord_t	offcumul;
 coord_t	offprev;
 int		i, j, lng;
  
 offcumul = offprev = COORD_ZERO; i=0; lng = strlen(orig);
 while(offcumul < offset && i < lng)
 {
  offprev = offcumul;
  if (angle90)
   offcumul +=  ABS(F(metrics).textHeight(metrics, orig+i, 1));
  else
   offcumul += F(metrics).textWidth(metrics, orig+i, 1);
  i++;
 }
  
 if (i)
  i = j = i-1;
 else 
  j = 0;
 *dest = orig + j;
 
 *incrx = offprev;
 offcumul = offprev - offcumul;
 while(offcumul < width && i < lng)
 {
  if (angle90)
   offcumul +=  ABS(F(metrics).textHeight(metrics, orig+i, 1));
  else
   offcumul += F(metrics).textWidth(metrics, orig+i, 1);
  i++;
 }
 *length = i - j;
}

static void update_metrics(This, cstyle, tstyle, angle, factor)
c_CellStyle *This;
CellStyle_t *cstyle;
c_TextStyle *tstyle;
angle_t angle;
scale_t factor;
{
 if (!cstyle->rfont || cstyle->metrics->angle != angle || 
     cstyle->factor != factor)
 {
  Xc_TRACE(("regenerate"));
      
  cstyle->metrics = 
   F(tstyle->metrics).transform(tstyle->metrics, factor,
				XcTS_GET_WIDTH(tstyle), angle);
  if (cstyle->rfont)
  {
   F(GlobFntMng).delete_font(GlobFntMng, cstyle->rfont);
   cstyle->rfont = NULL;
  }
  cstyle->metrics->raster_font = 
   F(GlobFntMng).new_font(GlobFntMng, tstyle->font, cstyle->metrics);
  if((cstyle->rfont = cstyle->metrics->raster_font) == NULL)
  {
   F(tstyle->metrics).purgeImpure(tstyle->metrics, factor,
				  XcTS_GET_WIDTH(tstyle), angle);
   return;
  }
  cstyle->factor = factor;
 }
 F(cstyle->rfont).setGC(This->display, This->window, 
			&This->gc_mask, This->gc_paint);
}



