/*
** Draw_Feuille.c for XQuad in Feuille/
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
** Started on  Fri Sep 30 15:33:47 1994 Emmanuel Paris
** Last update Sun Jul 12 01:12:13 1998 Emmanuel Paris
*/

#include "Feuille.h"


void focus_in_cellule(This, column, row)
c_Feuille	*This;
int		column;
int		row;
{
 int	x, y, width, height;
 int	lb, rb, ub, db;
 cell_info_t	*cellinfo;
  
 F(This).GetCellGeometry(This, column, row, Xq_CELL, &x, &y,
			 &width, &height);
  
 cellinfo = 
  &This->cell_info[column-This->column_orig][row-This->row_orig];
 lb = ABS(cellinfo->left_border-1)>>1;
 rb = (cellinfo->right_border+2)>>1;
 ub = ABS(cellinfo->up_border-1)>>1;
 db = (cellinfo->down_border+2)>>1;
 x += lb;
 y += ub;
 width -= lb + rb;
 height -= ub + db;

 if (x>=0 && y>=0 && width>1 && height>1 && 
     cellinfo->clip_type != XcCT_CLIPMASK_FULL)
 {
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
  {
   XSetClipMask(_display, _gc_select, cellinfo->clip_mask);
   XSetClipOrigin(_display, _gc_select, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
  }
    
  XFillRectangle(_display, _window, _gc_select, x, y, 
		 width, height);
    
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
   XSetClipMask(_display, _gc_select, None);
    
  if (cellinfo->clip_type & XcCTM_REDRAW)
   F(This->frmg).redraw_rectangle(This->frmg, x, y, x + width, y + height);
 }
}

void focus_out_cellule(This, column, row)
c_Feuille	*This;
int		column;
int		row;
{
 int	x, y, width, height;
 int	lb, rb, ub, db;
 cell_info_t	*cellinfo;

 F(This).GetCellGeometry(This, column, row, Xq_CELL, &x, &y,
			 &width, &height);  

 cellinfo = 
  &This->cell_info[column-This->column_orig][row-This->row_orig];
 lb = ABS(cellinfo->left_border-1)>>1;
 rb = (cellinfo->right_border+2)>>1;
 ub = ABS(cellinfo->up_border-1)>>1;
 db = (cellinfo->down_border+2)>>1;
 x += lb;
 y += ub;
 width -= lb + rb;
 height -= ub + db;

 if (x>=0 && y>=0 && width>1 && height>1 && 
     cellinfo->clip_type != XcCT_CLIPMASK_FULL)
 {
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
  {
   XSetClipMask(_display, _gc_select, cellinfo->clip_mask);
   XSetClipOrigin(_display, _gc_select, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
  }
    
  XFillRectangle(_display, _window, _gc_select, x, y, 
		 width, height);
    
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
   XSetClipMask(_display, _gc_select, None);
    
  if (cellinfo->clip_type & XcCTM_REDRAW)
   F(This->frmg).redraw_rectangle(This->frmg, x, y, x + width, y + height);
 }
}

void focus_in_active_cellule(This, column, row)
c_Feuille	*This;
int		column;
int		row;
{
 int	x, y, width, height;
 int	lb, rb, ub, db;
 cell_info_t	*cellinfo;
  
 F(This).GetCellGeometry(This, column, row, Xq_CELL, &x, &y,
			 &width, &height);

 cellinfo = 
  &This->cell_info[column-This->column_orig][row-This->row_orig];
 lb = ABS(cellinfo->left_border-1)>>1;
 rb = (cellinfo->right_border+2)>>1;
 ub = ABS(cellinfo->up_border-1)>>1;
 db = (cellinfo->down_border+2)>>1;
 x += lb;
 y += ub;
 width -= lb + rb;
 height -= ub + db;

 if (x>=0 && y>=0 && width>5 && height>5 && 
     cellinfo->clip_type != XcCT_CLIPMASK_FULL)
 {
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
  {
   XSetClipMask(_display, _gc_select, cellinfo->clip_mask);
   XSetClipOrigin(_display, _gc_select, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
  }
    
  XFillRectangle(_display, _window, _gc_select, x+2, y+2, 
		 width-5, height-5); 
    
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
   XSetClipMask(_display, _gc_select, None);
    
  if (cellinfo->clip_type & XcCTM_REDRAW)
   F(This->frmg).redraw_rectangle(This->frmg, x+2 , y+2, x+width-5, 
				  y+height-5);
 }
}

void focus_out_active_cellule(This, column, row)
c_Feuille	*This;
int		column;
int		row;
{
 int	x, y, width, height;
 int	lb, rb, ub, db;
 cell_info_t	*cellinfo;

 F(This).GetCellGeometry(This, column, row, Xq_CELL, &x, &y,
			 &width, &height);

 cellinfo = 
  &This->cell_info[column-This->column_orig][row-This->row_orig];
 lb = ABS(cellinfo->left_border-1)>>1;
 rb = (cellinfo->right_border+2)>>1;
 ub = ABS(cellinfo->up_border-1)>>1;
 db = (cellinfo->down_border+2)>>1;
 x += lb;
 y += ub;
 width -= lb + rb;
 height -= ub + db;

 if (x>=0 && y>=0 && width>5 && height>5 && 
     cellinfo->clip_type != XcCT_CLIPMASK_FULL)
 {
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
  {
   XSetClipMask(_display, _gc_select, cellinfo->clip_mask);
   XSetClipOrigin(_display, _gc_select, cellinfo->clip_origin_x,
		  cellinfo->clip_origin_y);
  }
    
  XFillRectangle(_display, _window, _gc_select, x+2, y+2, 
		 width-5, height-5); 
    
  if (cellinfo->clip_type & XcCTM_CLIPMASK)
   XSetClipMask(_display, _gc_select, None);
    
  if (cellinfo->clip_type & XcCTM_REDRAW)
   F(This->frmg).redraw_rectangle(This->frmg, x+2 , y+2, x+width-5, 
				  y+height-5);
 }
}

void focus_in_column(This, column)
c_Feuille	*This;
int		column;
{
 int   x,y,width,height;
  
 F(This).GetCellGeometry(This,column, 0, Xq_COLUMN, &x, &y, &width, &height);
 if (x>=0 && y>=0 && width>0 && height>0)
  draw_column(This, column, x, y, width, height);
}

void focus_in_row(This, row)
c_Feuille	*This;
int		row;
{
 int   x,y,width,height;
  
 F(This).GetCellGeometry(This, 0, row, Xq_ROW, &x, &y, &width, &height);
 if (x>=0 && y>=0 && width>0 && height>0)
  draw_row(This, row, x, y, width, height);
}

void focus_in_origin(This)
c_Feuille	*This;
{
 int   x,y,width,height;
  
 F(This).GetCellGeometry(This, 0, 0, Xq_ORIGIN, &x, &y, &width, &height);
 if (x>=0 && y>=0 && width>0 && height>0)
  draw_origin(This, x, y, width, height);
}

void draw_cellule(This,  column, row, x, y, width, height)
c_Feuille	*This;
int		column;
int		row;
int		x;
int		y;
int		width;
int		height;
{
 XPoint	point[3], point2[3];
 int		lb, rb, ub, db;
 int		xx,yy,ww,hh;
 scale_t	mfactor;
 cell_info_t	*cellinfo;
 CellStyle_t	*cstyle;
 boolean	redraw;
 boolean	clip_mask;
 boolean	grid_right = TRUE;
 boolean	grid_down = TRUE;
 int		i,j;
 int		xs, ys, ws, hs;

 /*  Xc_TRACE(("Draw cell %c%d  l:%d%c r:%d%c u:%d%c d:%d%c",
     column+'A', row+1, 
     cellinfo->left_border, (cellinfo->left_to_show?'T':'F'),
     cellinfo->right_border, (cellinfo->right_to_show?'T':'F'),
     cellinfo->up_border, (cellinfo->up_to_show?'T':'F'),
     cellinfo->down_border, (cellinfo->down_to_show?'T':'F') )); */
  
 /*  {
     char rc[50];
     colrowtostr(column, row, rc);
     printf("redraw: %s\n", rc);
     } */
  
 i = column - This->column_orig;
 j = row - This->row_orig;
  
 cellinfo = &(This->cell_info[i][j]);
 if (cellinfo->sequence == This->sequence) 
  return;
 else
  cellinfo->sequence = This->sequence;

 if (width <= 0 || height <= 0) return;

 cstyle = cellinfo->cstyle;
 redraw = (cellinfo->clip_type & XcCTM_REDRAW);
 clip_mask = (cellinfo->clip_type & XcCTM_CLIPMASK);
 xs = x; ys = y; ws = width; hs = height;
  
 mfactor = This->sFactor;
 DISPLAY_CELL_STYLE(This->BaseStd->cell_style_base, cellinfo, x, y,
		    width, height, mfactor, This->frmg->Orig_X,
		    This->frmg->Orig_Y, DISPLAY_ALL);
  
 if (!cellinfo->left_to_show && cellinfo->left_border && i>0)
 {
  F(This).GetCellGeometry(This,column-1, row, Xq_CELL, &xx, &yy, &ww, &hh);
  /* Xc_TRACE(("draw cell right: %c%d", column+'A'-1,row+1)); */
  DISPLAY_CELL_STYLE(This->BaseStd->cell_style_base, 
		     &(This->cell_info[i-1][j]), xx, yy, ww, hh,
		     mfactor, 0, 0, DISPLAY_RIGHT_BORDER);
 }
 if (!cellinfo->right_to_show && cellinfo->right_border 
     && i<This->nb_column)
 {
  F(This).GetCellGeometry(This,column+1, row, Xq_CELL, &xx, &yy, &ww, &hh);
  /* Xc_TRACE(("draw cell left: %c%d y: %d", column+'A'+1,row+1, yy)); */
  DISPLAY_CELL_STYLE(This->BaseStd->cell_style_base, 
		     &(This->cell_info[i+1][j]), xx, yy, ww, hh,
		     mfactor, 0, 0, DISPLAY_LEFT_BORDER);
 }
 if (!cellinfo->up_to_show && cellinfo->up_border && j>0)
 {
  F(This).GetCellGeometry(This,column, row-1, Xq_CELL, &xx, &yy, &ww, &hh);
  /* Xc_TRACE(("draw cell down: %c%d", column+'A',row)); */
  DISPLAY_CELL_STYLE(This->BaseStd->cell_style_base, 
		     &(This->cell_info[i][j-1]), xx, yy, ww, hh,
		     mfactor, 0, 0, DISPLAY_DOWN_BORDER);
 }
 if (!cellinfo->down_to_show && cellinfo->down_border && j<This->nb_row)
 {
  F(This).GetCellGeometry(This,column, row+1, Xq_CELL, &xx, &yy, &ww, &hh);
  /* Xc_TRACE(("draw cell up: %c%d", column+'A',row+2)); */
  DISPLAY_CELL_STYLE(This->BaseStd->cell_style_base, 
		     &(This->cell_info[i][j+1]), xx, yy, ww, hh,
		     mfactor, 0, 0, DISPLAY_UP_BORDER);
 }
  
 if (cellinfo->clip_type == XcCT_CLIPMASK_FULL)
  return;
  
 lb = ABS(cellinfo->left_border-1)>>1;
 rb = (cellinfo->right_border+2)>>1;
 ub = ABS(cellinfo->up_border-1)>>1;
 db = (cellinfo->down_border+2)>>1;
 x += lb;
 y += ub;
 width -= lb + rb;
 height -= ub + db;
  
 if ( (i<This->nb_column-1) &&
     ((cellinfo->cell_over_left && 
       cellinfo->cell_over_left == This->cell_info[i+1][j].cell_over_left) ||
      (cellinfo->cell_over_right && 
       cellinfo->cell_over_right == This->cell_info[i+1][j].cell_over_right)))
  grid_right = FALSE;
  
 if ( (j<This->nb_row-1) &&
     ((cellinfo->cell_over_up && 
       cellinfo->cell_over_up == This->cell_info[i][j+1].cell_over_up) ||
      (cellinfo->cell_over_down && 
       cellinfo->cell_over_down == This->cell_info[i][j+1].cell_over_down)))
  grid_down = FALSE;
  
 if (This->grid && cstyle->inside_stipple_pattern == PATTERN_NONE)
 {
  if (!cellinfo->right_border && !cellinfo->down_border && 
      grid_right && grid_down)
  {
   point[0].x = point[1].x = x+width;
   point2[0].x = point2[1].x = x+width-1;
   point[2].x = point2[2].x = x;
   point[0].y = point2[0].y = y;
   point[1].y = point[2].y = y+height;
   point2[1].y = point2[2].y = y+height-1;
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_grid1, cellinfo->clip_mask);
    XSetClipOrigin(_display, _gc_grid1, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
    XSetClipMask(_display, _gc_grid2, cellinfo->clip_mask);
    XSetClipOrigin(_display, _gc_grid2, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
   }
	  
   XDrawLines(_display, _window, _gc_grid2, point, 3, 
	      CoordModeOrigin);
   XDrawLines(_display, _window, _gc_grid1, point2, 3, 
	      CoordModeOrigin);
	  
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_grid2, None);
    XSetClipMask(_display, _gc_grid1, None);
   }
  }
  else
  {
   if (!cellinfo->right_border && grid_right)
   {
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_grid1, cellinfo->clip_mask);
     XSetClipOrigin(_display, _gc_grid1, 
		    cellinfo->clip_origin_x,
		    cellinfo->clip_origin_y);
     XSetClipMask(_display, _gc_grid2, cellinfo->clip_mask);
     XSetClipOrigin(_display, _gc_grid2, 
		    cellinfo->clip_origin_x,
		    cellinfo->clip_origin_y);
    }
	      
    XDrawLine(_display, _window, _gc_grid2, x+width, y,
	      x+width, y+height);
    XDrawLine(_display, _window, _gc_grid1, x+width-1, y,
	      x+width-1, y+height);
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_grid2, None);
     XSetClipMask(_display, _gc_grid1, None);
    }
   }
   if (!cellinfo->down_border && grid_down)
   {
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_grid1, cellinfo->clip_mask);
     XSetClipOrigin(_display, _gc_grid1, 
		    cellinfo->clip_origin_x,
		    cellinfo->clip_origin_y);
     XSetClipMask(_display, _gc_grid2, cellinfo->clip_mask);
     XSetClipOrigin(_display, _gc_grid2, 
		    cellinfo->clip_origin_x,
		    cellinfo->clip_origin_y);
    }
    XDrawLine(_display, _window, _gc_grid2, x, y+height,
	      x+width, y+height);
    XDrawLine(_display, _window, _gc_grid1, x, y+height-1,
	      x+width, y+height-1);
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_grid2, None);
     XSetClipMask(_display, _gc_grid1, None);
    }
   }
  }
 }
 if (This->page_mark)
 {
  if (This->column_page_mark[i] && This->row_page_mark[j])
  {
      
   point2[0].x = point2[1].x = x+width-1;
   point2[2].x = x;
   point2[0].y = y;
   point2[1].y = point2[2].y = y+height-1;
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_page_mark, cellinfo->clip_mask);
    XSetClipOrigin(_display, _gc_page_mark, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
   }
   XDrawLines(_display, _window, _gc_page_mark, 
	      point2, 3, CoordModeOrigin);
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_page_mark, None);
   }
  }
  else
  {
   if (This->column_page_mark[i])
   {
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_page_mark, cellinfo->clip_mask);
     XSetClipOrigin(_display, _gc_page_mark, 
		    cellinfo->clip_origin_x,
		    cellinfo->clip_origin_y);
    }
	
    XDrawLine(_display, _window, _gc_page_mark, x+width-1, y,
	      x+width-1, y+height-1);
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_page_mark, None);
    }
   }
   if (This->row_page_mark[j])
   {
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_page_mark, cellinfo->clip_mask);
     XSetClipOrigin(_display, _gc_page_mark, 
		    cellinfo->clip_origin_x,
		    cellinfo->clip_origin_y);
    }
    XDrawLine(_display, _window, _gc_page_mark, x, y+height-1,
	      x+width-1, y+height-1);
    if (clip_mask)
    {
     XSetClipMask(_display, _gc_page_mark, None);
    }
   }
  }
 }
  
 /*XFlush(_display);*/
 /*getchar();*/
 if (This->select_type)
 {
  if (column == This->cell_x && row == This->cell_y && width>5 && height>5)
  {
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_select, cellinfo->clip_mask);
    XSetClipOrigin(_display, _gc_select, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
   }
   XFillRectangle(_display, _window, _gc_select, x+2, y+2, 
		  width-5, height-5); 
   if (redraw)
   {
    F(This->frmg).redraw_rectangle(This->frmg, x+2 , y+2, 
				   x+width-5, 
				   y+height-5);
   }
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_select, None);
   }
  }
    
  if (F(This).IsSelected(This, column, row, Xq_CELL) 
      && width>1 && height>1)
  {
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_select, cellinfo->clip_mask);
    XSetClipOrigin(_display, _gc_select, cellinfo->clip_origin_x,
		   cellinfo->clip_origin_y);
   }
   XFillRectangle(_display, _window, _gc_select, x, y, 
		  width, height);
   if (redraw)
   {
    F(This->frmg).redraw_rectangle(This->frmg, x, y, x + width, 
				   y + height);
   }
      
   if (clip_mask)
   {
    XSetClipMask(_display, _gc_select, None);
   }
  }
 }
 if (redraw)
 {
  if (cellinfo->left_to_show)
  {
   xs -= lb;
   ws += lb;
  }
  if (cellinfo->right_to_show)
   ws += rb;
  if (cellinfo->up_to_show)
  {
   ys -= ub;
   hs += ub;
  }
  if (cellinfo->down_to_show)
   hs += db;
  F(This->frmg).redraw_rectangle(This->frmg, xs , ys, xs+ws, ys+hs);
 }
}

void draw_column(This, column, x, y, width, height)
c_Feuille	*This;
int		column;
int		x;
int		y;
int		width;
int		height;
{
 XSegment	xseg[4];
 boolean	selected;
 int		pnt;
 char		aff[8];
 vector_t	pos;
 coord_t	txt_width;
 coord_t	txt_height;
 int		sX, sY, sWidth, sHeight;
 XRectangle	rectangle;
  
 if (width == 0) return;

 selected = F(This).IsSelected(This, column, 0, Xq_COLUMN);
 xseg[0].x1 = x;
 xseg[0].y1 = xseg[0].y2 = xseg[1].y2 = y + height - 1;
 xseg[0].x2 = xseg[1].x1 = xseg[1].x2 = x + width - 1;
 xseg[1].y1 = y;
 XDrawSegments(_display, _C_window, _C_gc_contour, xseg, 2);
 
 if (width <= 3)
 {
  if (selected)
   XSetForeground(_display, _C_gc_inside, This->col_back_dark);
  XFillRectangle(_display, _C_window, _C_gc_inside, x, y, 
		 width - 1, height - 1);
  if (selected)
   XSetForeground(_display, _C_gc_inside, This->col_back_light);
  return;
 }

 if (selected)
  XSetForeground(_display, _C_gc_inside, This->col_back_dark);
 XFillRectangle(_display, _C_window, _C_gc_inside, x + 1, y + 1, 
		width - 3, height - 3);
 if (selected)
  XSetForeground(_display, _C_gc_inside, This->col_back_light);
  
 if (selected)
  XSetForeground(_display, _C_gc_contour, This->col_relief_light);
 else
  XSetForeground(_display, _C_gc_contour, This->col_relief_dark);
 xseg[0].x1 = x + 1;
 xseg[0].y1 = xseg[0].y2 = y + height - 2;
 xseg[0].x2 = x + width - 2;
 xseg[1].x1 = xseg[1].x2 = x + width - 2;
 xseg[1].y1 = y + 1;
 xseg[1].y2 = y+height - 2;
 XDrawSegments(_display, _C_window, _C_gc_contour, xseg, 2);
  
 if (selected)
  XSetForeground(_display, _C_gc_contour, This->col_relief_dark);
 else
  XSetForeground(_display, _C_gc_contour, This->col_relief_light);
 xseg[0].x1 = x;
 xseg[0].y1 = y;
 xseg[0].x2 = x;
 xseg[0].y2 = y + height - 2;
 xseg[1].x1 = x;
 xseg[1].y1 = xseg[1].y2 = y;
 xseg[1].x2 = x + width - 2;
 XDrawSegments(_display, _C_window, _C_gc_contour, xseg, 2);
 XSetForeground(_display, _C_gc_contour, This->X_info_C.contour_color);
  
 if (selected)
  XSetForeground(_display, _gc_font_paint, GlobColormap->Cwhite);
 else
  XSetForeground(_display, _gc_font_paint, GlobColormap->Cblack);
  
 coltostr(column, &pnt, aff);
  
 sX = SCALE_FROM_POINTS(x);
 sY = SCALE_FROM_POINTS(y);
 sWidth = SCALE_FROM_POINTS(width);
 sHeight = SCALE_FROM_POINTS(height);
 txt_width = F(This->orig_metrics).textWidth(This->orig_metrics, aff, pnt);
 txt_height = This->orig_metrics->ascender;
  
 if (txt_width >= sWidth || txt_height >= sHeight)
 {
  if (width - 2 && height - 2)
  {
   rectangle.x = x + 1;
   rectangle.y = y + 1;
   rectangle.width = width - 2;
   rectangle.height = height - 2;
   XSetClipRectangles(_display, _gc_font_paint, 0, 0, &rectangle,
		      1, Unsorted);
   if (_gc_font_mask)
    XSetClipRectangles(_display, _gc_font_mask, 0, 0, &rectangle, 
		       1, Unsorted);

   pos.dx = sX + ( (sWidth - txt_width) >> 1);
   pos.dy = sY + ( (sHeight + txt_height) >> 1);
   F(This->orig_font).drawString(This->orig_font, _display, _C_window,
				 _gc_font_mask, _gc_font_paint, &pos, 0, 0,
				 aff, pnt, This->orig_style);
   XSetClipMask(_display, _gc_font_paint, None);
   if (_gc_font_mask)
    XSetClipMask(_display, _gc_font_mask, None);
  }
 }
 else
 {
  pos.dx = sX + ( (sWidth - txt_width) >> 1);
  pos.dy = sY + ( (sHeight + txt_height) >> 1);
    
  F(This->orig_font).drawString(This->orig_font, _display, _C_window,
				_gc_font_mask, _gc_font_paint, &pos, 0, 0,
				aff, pnt, This->orig_style);
 }
}

void draw_row(This, row, x, y, width, height)
c_Feuille	*This;
int		row;
int		x;
int		y;
int		width;
int		height;
{
 XSegment	xseg[4];
 boolean	selected;
 char		aff[10];
 int		pnt;
 vector_t	pos;
 coord_t	txt_width;
 coord_t	txt_height;
 int		sX, sY, sWidth, sHeight;
 XRectangle	rectangle;
  
 if (height <= 1) return;

 selected = F(This).IsSelected(This, 0, row, Xq_ROW);
 xseg[0].x1 = x;
 xseg[0].y1 = xseg[0].y2 = xseg[1].y2 = y + height - 1;
 xseg[0].x2 = xseg[1].x1 = xseg[1].x2 = x + width - 1;
 xseg[1].y1 = y;
 XDrawSegments(_display, _R_window, _R_gc_contour, xseg, 2);
 
 if (height <= 3)
 {
  if (selected)
   XSetForeground(_display, _R_gc_inside, This->col_back_dark);
  XFillRectangle(_display, _R_window, _R_gc_inside, x, y, 
		 width - 1, height - 1);
  if (selected)
   XSetForeground(_display, _R_gc_inside, This->col_back_light);
  return;
 }

  if (selected)
   XSetForeground(_display, _R_gc_inside, This->col_back_dark);
 XFillRectangle(_display, _R_window, _R_gc_inside, x + 1, y + 1, 
		width - 3, height - 3);
 
 if (selected)
  XSetForeground(_display, _R_gc_inside, This->col_back_light);
 
 if (selected)
  XSetForeground(_display, _R_gc_contour, This->col_relief_light);
 else
  XSetForeground(_display, _R_gc_contour, This->col_relief_dark);

 xseg[0].x1 = x+1;
 xseg[0].y1 = xseg[0].y2 = y + height - 2;
 xseg[0].x2 = x + width - 2;
 xseg[1].x1 = xseg[1].x2 = x + width - 2;
 xseg[1].y1 = y + 1;
 xseg[1].y2 = y + height - 2;
 XDrawSegments(_display, _R_window, _R_gc_contour, xseg, 2);
  
 if (selected)
  XSetForeground(_display, _R_gc_contour, This->col_relief_dark);
 else
  XSetForeground(_display, _R_gc_contour, This->col_relief_light);

 xseg[0].x1 = x;
 xseg[0].y1 = y;
 xseg[0].x2 = x;
 xseg[0].y2 = y + height - 2;
 xseg[1].x1 = x;
 xseg[1].y1 = xseg[1].y2 = y;
 xseg[1].x2 = x + width - 2;
 XDrawSegments(_display, _R_window, _R_gc_contour, xseg, 2);
 XSetForeground(_display, _R_gc_contour, This->X_info_R.contour_color);
  
 if (selected)
  XSetForeground(_display,_gc_font_paint, GlobColormap->Cwhite);
 else
  XSetForeground(_display,_gc_font_paint, GlobColormap->Cblack);
 sprintf(aff,"%d",row+1);
 pnt=strlen(aff);
  
 sX = SCALE_FROM_POINTS(x);
 sY = SCALE_FROM_POINTS(y);
 sWidth = SCALE_FROM_POINTS(width);
 sHeight = SCALE_FROM_POINTS(height);
 txt_width = F(This->orig_metrics).textWidth(This->orig_metrics, aff, pnt);
 txt_height = This->orig_metrics->ascender;
  
 if (txt_width >= sWidth || txt_height >= sHeight)
 {
  if (width > 2 && height > 2)
  {
   rectangle.x = x + 1;
   rectangle.y = y + 1;
   rectangle.width = width - 2;
   rectangle.height = height - 2;
   XSetClipRectangles(_display, _gc_font_paint, 0, 0, &rectangle,
		      1, Unsorted);
   if (_gc_font_mask)
    XSetClipRectangles(_display, _gc_font_mask, 0, 0, &rectangle, 
		       1, Unsorted);
      
   pos.dx = sX + ( (sWidth - txt_width) >> 1);
   pos.dy = sY + ( (sHeight + txt_height) >> 1);
   F(This->orig_font).drawString(This->orig_font, _display, _R_window,
				 _gc_font_mask, _gc_font_paint, &pos, 0, 0,
				 aff, pnt, This->orig_style);
      
   XSetClipMask(_display, _gc_font_paint, None);
   if (_gc_font_mask)
    XSetClipMask(_display, _gc_font_mask, None);
  }
 }
 else
 {
  pos.dx = sX + ( (sWidth - txt_width) >> 1);
  pos.dy = sY + ( (sHeight + txt_height) >> 1);

  F(This->orig_font).drawString(This->orig_font, _display, _R_window,
				_gc_font_mask, _gc_font_paint, &pos, 0, 0,
				aff, pnt, This->orig_style);
 }
}

void draw_origin(This, x, y, width, height)
c_Feuille	*This;
int		x;
int		y;
int		width;
int		height;
{
 XSegment	xseg[8];
 boolean	selected;

 selected = F(This).IsSelected(This, 0, 0, Xq_ORIGIN);
 xseg[0].x1 = x;
 xseg[0].y1 = xseg[0].y2 = xseg[1].y2 = y + height - 1;
 xseg[0].x2 = xseg[1].x1 = xseg[1].x2 = x + width - 1;
 xseg[1].y1 = y;
 XDrawSegments(_display, _O_window, _O_gc_contour, xseg, 2);
  
 if (selected)
  XSetForeground(_display, _O_gc_inside, This->col_back_dark);
 XFillRectangle(_display, _O_window, _O_gc_inside, x+1, y+1, 
		width-3, height-3);
 if (selected)
  XSetForeground(_display, _O_gc_inside, This->col_back_light);
  
 if (selected)
  XSetForeground(_display, _O_gc_contour, This->col_relief_light);
 else
  XSetForeground(_display, _O_gc_contour, This->col_relief_dark);
 xseg[0].x1 = x + 1;
 xseg[0].y1 = xseg[0].y2 = y + height - 2;
 xseg[0].x2 = x + width - 2;
 xseg[1].x1 = xseg[1].x2 = x + width - 2;
 xseg[1].y1 = y + 1;
 xseg[1].y2 = y + height - 2;
 XDrawSegments(_display, _O_window, _O_gc_contour, xseg, 2);
  
 if (selected)
  XSetForeground(_display, _O_gc_contour, This->col_relief_dark);
 else
  XSetForeground(_display, _O_gc_contour, This->col_relief_light);
 xseg[0].x1 = x;
 xseg[0].y1 = y;
 xseg[0].x2 = x;
 xseg[0].y2 = y + height - 2;
 xseg[1].x1 = x;
 xseg[1].y1 = xseg[1].y2 = y;
 xseg[1].x2 = x + width - 2;
 XDrawSegments(_display, _O_window, _O_gc_contour, xseg, 2);
  
 XSetForeground(_display, _O_gc_contour, This->X_info_O.contour_color);
}





