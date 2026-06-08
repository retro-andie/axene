/*
** Spread_Horizontaly_Region.c for XQuad in Feuille/
** Spread region horizontaly
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
** Last update Fri Dec 13 18:38:04 1996 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"

void spread_horizontaly_region_init();
void spread_horizontaly_region_abort();
void spread_horizontaly_region_done();
void spread_horizontaly_region_to_xy();
void spread_horizontaly_region_trace();

void spread_horizontaly_region_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
  int		column;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
    
  memcpy(&d(region), cfeuille->cell_region, sizeof(region_t));
  F(cfeuille).GetColumnByX(cfeuille, xm, &column); 
  d(column) = column;

  d(prems) = TRUE;
  d(source) = TRUE;
  
  F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
  spread_horizontaly_region_to_xy(cfeuille, xm, ym);  
  
  d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
    (cfeuille->X_info.display, 0, 0,
     cfeuille->sheet_width, 0, cfeuille->ScrollBarH, NULL,
     eventlook_Select_Move_Resize_Cell, cfeuille);
}

void spread_horizontaly_region_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  
  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }  
  
  spread_horizontaly_region_trace(cfeuille);
  d(source) = FALSE;
}

void spread_horizontaly_region_done(cfeuille)
c_Feuille *cfeuille;
{  
  region_t region;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);

  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }
  spread_horizontaly_region_trace(cfeuille);

  F(cfeuille->moteur).extend_zone( cfeuille->moteur, cfeuille->cell_region,
				  &(d(region)) );
  
  if (cfeuille->cell_region->x_min == d(region).x_min)
    {
      region.x_min = cfeuille->cell_region->x_max + 1;
      region.x_max = d(region).x_max;
    }
  else
    {
      region.x_min = d(region).x_min;
      region.x_max = cfeuille->cell_region->x_min - 1;
    }
  region.y_min = cfeuille->cell_region->y_min;
  region.y_max = cfeuille->cell_region->y_max;
  
  F(cfeuille).RefreshRegion(cfeuille, &region, Xq_CELL);
  F(cfeuille).DeselectRegion(cfeuille,Xq_ORIGIN,TRUE);
  F(cfeuille).SelectRegion(cfeuille, d(region).x_min, d(region).y_min,
			   d(region).x_max, d(region).y_max, Xq_CELL, TRUE);
  d(source)=FALSE;
}

void spread_horizontaly_region_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
  int	column;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);

  if (xm < 0)
    xm = 0;
  if (xm >= cfeuille->sheet_width)
    xm = cfeuille->sheet_width - 1;
  if (ym < 0 || ym >= cfeuille->origin_height)
    ym = 1;
  
  if (F(cfeuille).GetColumnByX(cfeuille, xm, &column) )
    {
      if (column < cfeuille->cell_region->x_max && 
	  column > cfeuille->cell_region->x_min)
	{
	  if (column < d(column))
	    column = cfeuille->cell_region->x_max;
	  if (column > d(column))
	    column = cfeuille->cell_region->x_min;
	}
      if (column >cfeuille->cell_region->x_max)
	F(CURSOR).set_cursor(CURSOR, C_SPREAD_RIGHT_REGION);
      else
	if (column < cfeuille->cell_region->x_min)
	  F(CURSOR).set_cursor(CURSOR, C_SPREAD_LEFT_REGION);
	else
	  F(CURSOR).set_cursor(CURSOR, C_SPREAD_H_CENTER_REGION);
      
      if (column!=d(column) || d(prems))
	{
	  d(column) = column;
	  
	  if (d(prems))
	    {
	      if (column >= cfeuille->cell_region->x_max)
		{
		  d(region).x_max = column;
		  d(region).x_min = cfeuille->cell_region->x_min;
		}
	      if (column <= cfeuille->cell_region->x_min)
		{
		  d(region).x_min = column;
		  d(region).x_max = cfeuille->cell_region->x_max;
		}
	      spread_horizontaly_region_trace(cfeuille);
	      d(prems) = FALSE;
	      return;
	    }
	  
	  spread_horizontaly_region_trace(cfeuille);
	  if (column >= cfeuille->cell_region->x_max)
	    {
	      d(region).x_max = column;
	      d(region).x_min = cfeuille->cell_region->x_min;
	    }
	  if (column <= cfeuille->cell_region->x_min)
	    {
	      d(region).x_min = column;
	      d(region).x_max = cfeuille->cell_region->x_max;
	    }
	  spread_horizontaly_region_trace(cfeuille);
	}
    }
}

void spread_horizontaly_region_trace(cfeuille)
c_Feuille *cfeuille;
{  
  Display	*display;
  Window	window;
  GC		gc;
  int		x1, y1, x2, y2;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  display=cfeuille->X_info.display;
  window=cfeuille->X_info.window;
  gc=cfeuille->X_info.gc_contour;
  
  x1 = get_column_pos(cfeuille, d(region).x_min);
  y1 = get_row_pos(cfeuille, d(region).y_min);
  x2 = get_column_pos(cfeuille, d(region).x_max+1);
  y2 = get_row_pos(cfeuille, d(region).y_max+1);
  XDrawRectangle(display, window, gc, x1, y1, x2-x1, y2-y1);
}




