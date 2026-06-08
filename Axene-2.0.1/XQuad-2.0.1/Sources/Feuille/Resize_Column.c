/*
** Resize_Column.c for XQuad in Feuille/
** Resize_Column 
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
** Started on  Mon Feb 20 11:56:48 1995 Emmanuel Paris
** Last update Thu Nov  2 16:36:08 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"

void resize_column_init();
void resize_column_abort();
void resize_column_done();
void resize_column_to_xy();
void resize_column_trace();

void resize_column_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  d(prems)	= TRUE;
  d(source)	= TRUE;
  d(xc)		= xm;
  d(yc)		= ym;
  
  F(cfeuille).GetColumnBorderByX(cfeuille, xm, &d(column));
  d(xo) = get_column_pos(cfeuille, d(column));
  
  resize_column_to_xy(cfeuille, xm, ym);
}

void resize_column_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  resize_column_trace(cfeuille);
  d(source)=FALSE;
}

void resize_column_done(cfeuille)
c_Feuille *cfeuille;
{  
  int		min_col;
  coord_t	col_size;
  region_t	region;
  region_t	*rregion;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  
  resize_column_trace(cfeuille);
    
  col_size = (d(xc)-d(xo))*cfeuille->sScale;
  if (cfeuille->col_region)
  {
    rregion = cfeuille->col_region;
    while(rregion && (d(column) < rregion->x_min ||
		      d(column) > rregion->x_max))
      rregion = rregion->NextRegion;
    
    if (!rregion)
    {
      F(cfeuille->moteur).set_size(cfeuille->moteur, Xc_COLUMN, d(column),
				   col_size);
      min_col = d(column);
    }
    else
    {
      rregion = cfeuille->col_region;
      min_col = COLUMN_MAX + 1;
      while(rregion)
      {
	if (rregion->x_min < min_col)
	  min_col = rregion->x_min;
	F(cfeuille->moteur).set_size_zone(cfeuille->moteur, Xc_COLUMN,
					  rregion, col_size);
	rregion = rregion->NextRegion;
      }
    }
  }
  else
  {
    F(cfeuille->moteur).set_size(cfeuille->moteur, Xc_COLUMN,
				 d(column), col_size);
    min_col = d(column);
  }

  calcul_column_width(cfeuille);
  allocate_cell_info(cfeuille);
  calcul_frame_info(cfeuille);
  region.x_min = min_col;
  region.x_max = COLUMN_MAX;
  region.y_min = 0;
  region.y_max = ROW_MAX;
  if (!cfeuille->select_type)
    F(cfeuille->frmg).undisplay_selected_lcadre(cfeuille->frmg);
  if (cfeuille->page_mark && min_col > 0)
    region.x_min--;
  F(cfeuille).RefreshRegion(cfeuille, &region, Xq_CELL);
  F(cfeuille).RefreshRegion(cfeuille, &region, Xq_COLUMN);
  if (!cfeuille->select_type)
    F(cfeuille->frmg).display_selected_lcadre(cfeuille->frmg);
  d(source)=FALSE;
}

void resize_column_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  if (!d(prems))
    resize_column_trace(cfeuille);
  else
    d(prems) = FALSE;
  
  if (xm<d(xo))
    xm = d(xo);
  if (xm>=cfeuille->sheet_width)
    xm = cfeuille->sheet_width-1;
  d(xc)=xm;
  resize_column_trace(cfeuille);
}

void resize_column_trace(cfeuille)
c_Feuille *cfeuille;
{ 
  Display	*display;
  Window	window;
  GC		gc;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  display=cfeuille->X_info.display;
  window=cfeuille->X_info.window;
  gc=cfeuille->X_info.gc_contour;
  
  XDrawLine(display, window, gc, d(xc), 0, d(xc), cfeuille->sheet_height);
  XDrawLine(display, cfeuille->X_info_C.window, gc, d(xc), 0, d(xc), 
	    cfeuille->origin_height);
}

