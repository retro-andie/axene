/*
** Select_Cells_Region_For_EditBar.c for XQuad in Feuille/
** Select And Deselect Cells region
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
** Last update Fri Dec 13 17:02:10 1996 Emmanuel Paris
*/

#include "Select_Region_For_EditBar.h"
#include "Select_Column_Region_For_EditBar.h"
#include "Select_Row_Region_For_EditBar.h"

void select_cells_region_for_editbar_init();
void select_cells_region_for_editbar_abort();
void select_cells_region_for_editbar_done();
void select_cells_region_for_editbar_to_xy();
void select_cells_region_for_editbar_trace();

void select_cells_region_for_editbar_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
  int		column;
  int		row;
  region_t	*region;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
    
  if (!d(cntrl))
  {
    if (d(region))
      select_cells_region_for_editbar_abort(cfeuille);
    if (d(region_column))
      select_column_region_for_editbar_abort(cfeuille);
    if (d(region_row))
      select_row_region_for_editbar_abort(cfeuille);
    d(region) = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
    d(region)->NextRegion = NULL;
  }
  else
  {
    if (d(region))
    {
      region = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
      region->NextRegion = d(region);
      d(region) = region;
    }
    else
    {
      d(region) = (region_t *)Xc_malloc("SRFregion", sizeof(region_t));
      d(region)->NextRegion = NULL;
    }
  }
  
  F(cfeuille).GetCellByXY(cfeuille, xm, ym, &column, &row);
  
  d(column) = column;
  d(row) = row;
  d(prems) = TRUE;
  d(source) = TRUE;
  
  select_cells_region_for_editbar_to_xy(cfeuille, xm, ym);  
  
  F(GlobEditBar).NewSelect(GlobEditBar, d(region), Xq_CELL, d(cntrl));

  d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
    (cfeuille->X_info.display,
     0, 0, cfeuille->sheet_width, cfeuille->sheet_height,
     cfeuille->ScrollBarH, cfeuille->ScrollBarV, 
     eventlook_Select_Region_For_EditBar, cfeuille);
}

void select_cells_region_for_editbar_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  region_t	*region, *region_tmp;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  
  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }  

  select_cells_region_for_editbar_trace(cfeuille, TRUE);
  region = d(region);
  while(region)
  {
    region_tmp = region->NextRegion;
    Xc_free(region);
    region = region_tmp;
  }
  d(region) = NULL;
  d(source) = FALSE;
}

void select_cells_region_for_editbar_done(cfeuille)
c_Feuille *cfeuille;
{  
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);

  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }

/*  F(GlobEditBar).query_aff(GlobEditBar, d(column),d(row)); */
  d(source)=FALSE;
}

void select_cells_region_for_editbar_to_xy(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	xm;
int	ym;
{ 
  int	column;
  int	row;
   DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);

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
    if (column!=d(xc) || row!=d(yc) || d(prems))
    {
      d(xc) = column;
      d(yc) = row;

      if (d(prems))
      {
	d(region)->x_min = d(region)->x_max = column;
	d(region)->y_min = d(region)->y_max = row;
	
	select_cells_region_for_editbar_trace(cfeuille,FALSE);
	d(prems) = FALSE;
	return;
      }
      select_cells_region_for_editbar_trace(cfeuille,FALSE);
      d(region)->x_min = MIN(d(column),column);
      d(region)->y_min = MIN(d(row),row);
      d(region)->x_max = MAX(d(column),column);
      d(region)->y_max = MAX(d(row),row);
      select_cells_region_for_editbar_trace(cfeuille,FALSE);
      F(GlobEditBar).SelectMove(GlobEditBar, d(region), Xq_CELL);
    }
  }
}

void select_cells_region_for_editbar_trace(cfeuille, all_r)
c_Feuille *cfeuille;
boolean	all_r;
{  
  Display	*display;
  Window		window;
  GC		gc;
  region_t	*region;
  int		x1, y1, x2, y2;
  DATA(Select_Region_For_EditBar);
  
  GET_DATA(Select_Region_For_EditBar);
  display=cfeuille->X_info.display;
  window=cfeuille->X_info.window;
  gc=cfeuille->X_info.gc_contour;
  
  region = d(region);
  while(region)
  {
    x1 = get_column_pos(cfeuille, region->x_min);
    y1 = get_row_pos(cfeuille, region->y_min);
    x2 = get_column_pos(cfeuille, region->x_max+1);
    y2 = get_row_pos(cfeuille, region->y_max+1);
    XDrawRectangle(display, window, gc, x1, y1, x2-x1, y2-y1);
    
    if (!all_r)
      break;
    region = region->NextRegion;
  }
}




