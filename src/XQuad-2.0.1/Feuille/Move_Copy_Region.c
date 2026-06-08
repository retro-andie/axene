/*
** Move_Copy_Region.c for XQuad in Feuille/
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
** Last update Thu Jun  5 02:15:15 1997 Antoine Buat
*/

#include "Select_Move_Resize_Cells.h"

void move_copy_region_init();
void move_copy_region_abort();
void move_copy_region_done();
void move_copy_region_to_xy();
void move_copy_region_trace();

void move_copy_region_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
 int	column;
 int	row;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
    
 memcpy(&d(region), cfeuille->cell_region, sizeof(region_t));
 F(cfeuille).GetCellByXY(cfeuille, xm, ym, &column, &row); 
 d(column) = column;
 d(row) = row;

 d(prems) = TRUE;
 d(source) = TRUE;
  
 F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
 move_copy_region_to_xy(cfeuille, xm, ym);  
  
 d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
  (cfeuille->X_info.display,
   0, 0, cfeuille->sheet_width, cfeuille->sheet_height,
   cfeuille->ScrollBarH, cfeuille->ScrollBarV, 
   eventlook_Select_Move_Resize_Cell, cfeuille);
}

void move_copy_region_abort(cfeuille)
c_Feuille *cfeuille;
{ 
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);
  
 if (d(AutoScroll))
 {
  DELETE(c_AutoScroll)(d(AutoScroll));
  d(AutoScroll) = (c_AutoScroll *)NULL;
 }  
  
 move_copy_region_trace(cfeuille);
 d(source) = FALSE;
}

void move_copy_region_done(cfeuille)
c_Feuille *cfeuille;
{  
 int	nx, ny;
 region_t	*region;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);

 if (d(AutoScroll))
 {
  DELETE(c_AutoScroll)(d(AutoScroll));
  d(AutoScroll) = (c_AutoScroll *)NULL;
 }
 move_copy_region_trace(cfeuille);

 if (d(shift))
  F(cfeuille->moteur).cut_zone(cfeuille->moteur, cfeuille->cell_region,
			       Xc_MODE_COPY);
 else
  F(cfeuille->moteur).cut_zone(cfeuille->moteur, cfeuille->cell_region,
			       Xc_MODE_CUT);
 nx = cfeuille->cell_x - cfeuille->cell_region->x_min + d(region).x_min;
 ny = cfeuille->cell_y - cfeuille->cell_region->y_min + d(region).y_min;
 F(cfeuille).DeselectRegion(cfeuille,Xq_ORIGIN,TRUE);
 F(cfeuille).SelectRegion(cfeuille, d(region).x_min, d(region).y_min,
			  d(region).x_max, d(region).y_max, Xq_CELL, TRUE);
 region = F(cfeuille->moteur).paste_zone(cfeuille->moteur,
					 cfeuille->cell_region,
					 d(region).x_min, d(region).y_min);
 F(cfeuille).SetActiveCellule(cfeuille, nx, ny, FALSE);

 if (region)
 {
  if (region->NextRegion)
  {
   F(cfeuille).RefreshRegion(cfeuille, region, Xq_CELLS);
   while (region)
   {
    region_t	*tregion;

    tregion = region->NextRegion;
    Xc_free(region);
    region = tregion;
   }
  }
  else
  {
   F(cfeuille).RefreshRegion(cfeuille, region, Xq_CELL);
   Xc_free(region);
  }
 }
 d(source)=FALSE;
}

void move_copy_region_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
 int	column;
 int	row;
 int	inc_x, inc_y;
 DATA(Select_Move_Resize_Cell);
  
 GET_DATA(Select_Move_Resize_Cell);

 if (xm < 0)
  xm = 0;
 if (ym < 0)
  ym = 0;
 if (xm >= cfeuille->sheet_width)
  xm = cfeuille->sheet_width -1;
 if (ym >= cfeuille->sheet_height)
  ym = cfeuille->sheet_height -1;
  
 if (F(cfeuille).GetCellByXY(cfeuille, xm, ym, &column, &row))
 {
  if (column!=d(column) || row!=d(row) || d(prems))
  {
   inc_x = column - d(column);
   inc_y = row - d(row);
   d(column) = column;
   d(row) = row;

   if (d(prems))
   {
    if (d(region).x_min + inc_x >=0 && 
	d(region).x_max + inc_x <=COLUMN_MAX)
    {
     d(region).x_min += inc_x;
     d(region).x_max += inc_x;
    }
    if (d(region).y_min + inc_y >=0 && 
	d(region).y_max + inc_y <=ROW_MAX)
    {
     d(region).y_min += inc_y;
     d(region).y_max += inc_y;
    }
    move_copy_region_trace(cfeuille);
    d(prems) = FALSE;
    return;
   }
	  
   move_copy_region_trace(cfeuille);
   if (d(region).x_min + inc_x >=0 && 
       d(region).x_max + inc_x <=COLUMN_MAX)
   {
    d(region).x_min += inc_x;
    d(region).x_max += inc_x;
   }
   if (d(region).y_min + inc_y >=0 && 
       d(region).y_max + inc_y <=ROW_MAX)
   {
    d(region).y_min += inc_y;
    d(region).y_max += inc_y;
   }
   move_copy_region_trace(cfeuille);
  }
 }
}

void move_copy_region_trace(cfeuille)
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




