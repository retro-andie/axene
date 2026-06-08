/*
** Resize_Row.c for XQuad in Feuille/
** Resize_Row 
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
** Last update Thu Nov  2 16:35:49 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"

void resize_row_init();
void resize_row_abort();
void resize_row_done();
void resize_row_to_xy();
void resize_row_trace();

void resize_row_init(cfeuille,xm,ym)
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

  F(cfeuille).GetRowBorderByY(cfeuille, ym, &d(row));
  d(yo) = get_row_pos(cfeuille, d(row));
  
  resize_row_to_xy(cfeuille, xm, ym);
}

void resize_row_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  resize_row_trace(cfeuille);
  d(source)=FALSE;
}

void resize_row_done(cfeuille)
c_Feuille *cfeuille;
{  
  int		min_row;
  coord_t	row_size;
  region_t	region;
  region_t	*rregion;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  
  resize_row_trace(cfeuille);
  
  row_size = (d(yc)-d(yo))*cfeuille->sScale;
  if (cfeuille->row_region)
  {
    rregion = cfeuille->row_region;
    while(rregion && (d(row) < rregion->y_min || d(row) > rregion->y_max))
      rregion = rregion->NextRegion;
    
    if (!rregion)
    {
      F(cfeuille->moteur).set_size(cfeuille->moteur, Xc_ROW, d(row), row_size);
      min_row = d(row);
    }
    else
    {
      rregion = cfeuille->row_region;
      min_row = ROW_MAX + 1;
      while(rregion)
      {
	if (rregion->y_min < min_row)
	  min_row = rregion->y_min;
	F(cfeuille->moteur).set_size_zone(cfeuille->moteur, Xc_ROW,
					  rregion, row_size);
	rregion = rregion->NextRegion;
      }
    }
  }
  else
  {
    F(cfeuille->moteur).set_size(cfeuille->moteur, Xc_ROW, d(row), row_size);
    min_row = d(row);
  }
  
  calcul_row_height(cfeuille);
  allocate_cell_info(cfeuille);
  calcul_frame_info(cfeuille);
  region.x_min = 0;
  region.x_max = COLUMN_MAX;
  region.y_min = min_row;
  region.y_max = ROW_MAX;
  if (!cfeuille->select_type)
    F(cfeuille->frmg).undisplay_selected_lcadre(cfeuille->frmg);
  if (cfeuille->page_mark && min_row > 0)
    region.y_min--;
  F(cfeuille).RefreshRegion(cfeuille, &region, Xq_CELL);
  F(cfeuille).RefreshRegion(cfeuille, &region, Xq_ROW);
  if (!cfeuille->select_type)
    F(cfeuille->frmg).display_selected_lcadre(cfeuille->frmg);
  d(source)=FALSE;  
}

void resize_row_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  if (!d(prems))
    resize_row_trace(cfeuille);
  else
    d(prems) = FALSE;
  
  if (ym<d(yo))
    ym = d(yo);
  if (ym>=cfeuille->sheet_height)
    ym = cfeuille->sheet_height-1;
  d(yc)= ym;
  resize_row_trace(cfeuille);
}

void resize_row_trace(cfeuille)
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
  
  XDrawLine(display, window, gc, 0, d(yc), cfeuille->sheet_width, d(yc));
  XDrawLine(display, cfeuille->X_info_R.window, gc, 0, d(yc), 
	    cfeuille->origin_width, d(yc));
}






