/*
** Select_Column_Region.c for XQuad in Feuille/
** Select And Deselect Column region
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
** Last update Mon Aug 28 13:41:10 1995 Emmanuel Paris
*/

#include "Select_Move_Resize_Cells.h"
#include "Focus_Region_Utils.h"

void select_column_region_init();
void select_column_region_abort();
void select_column_region_done();
void select_column_region_to_xy();
void select_column_region_trace();

void select_column_region_init(cfeuille,xm,ym)
c_Feuille *cfeuille;
int	  xm;
int	  ym;
{
  boolean	select_type;
  int		column, column2;
  region_t	*region, *region2;
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  
  d(old_column)	= cfeuille->cell_x;
  d(old_row)	= cfeuille->cell_y;
  d(prems)	= TRUE;
  d(source)	= TRUE;
  
  
  select_type = cfeuille->select_type;
  if (!select_type)
    F(cfeuille->frmg).deselect_all_cadre(cfeuille->frmg, TRUE);
  if (!d(cntrl))
    F(cfeuille).DeselectRegion(cfeuille,Xq_ORIGIN,TRUE);
  F(cfeuille).GetColumnByX(cfeuille, xm, &column);
  
  if (d(shift) && select_type)
  {
    if (column >= d(old_column))
      column2 = d(old_column);
    else
    {
      column2 = column;
      column = d(old_column);
    }
     region = cfeuille->cell_region;
    while(region && region->NextRegion)
      region = region->NextRegion;
    
    if (region)
    {
      if (region->x_min == 0 && region->x_max == COLUMN_MAX)
      {
	if (region->y_min == 0 && region->y_max == ROW_MAX)
	{
	  if (d(cntrl))
	  {
	    d(source)=FALSE;
	    return;
	  }
  	}
	else
	{
	  region2 = cfeuille->row_region;
	  while(region2 && (region2->y_min != region->y_min || 
			    region2->y_max != region->y_max))
	    region2 = region2->NextRegion;
	  if (region2)
	  {
	    remove_region_from_list(cfeuille, region, Xq_CELL);
	    remove_region_from_list(cfeuille, region2, Xq_ROW);
	    focus_out_region(cfeuille, region, Xq_CELL);
	    focus_out_region(cfeuille, region2, Xq_ROW);
	    Xc_free(region);
	    Xc_free(region2);
	  }
	  else
	  {
	    remove_region_from_list(cfeuille, region, Xq_CELL);
	    focus_out_region(cfeuille, region, Xq_CELL);
	    Xc_free(region);
	  }
	}
      }
      else
      {
	if (region->y_min == 0 && region->y_max == ROW_MAX)
	{
	  region2 = cfeuille->col_region;
	  while(region2 && (region2->x_min != region->x_min || 
			    region2->x_max != region->x_max))
	    region2 = region2->NextRegion;
	  if (region2)
	  {
	    remove_region_from_list(cfeuille, region, Xq_CELL);
	    remove_region_from_list(cfeuille, region2, Xq_COLUMN);
	    focus_out_region(cfeuille, region, Xq_CELL);
	    focus_out_region(cfeuille, region2, Xq_COLUMN);
	    Xc_free(region);
	    Xc_free(region2);
	  }
	  else
	  {
	    remove_region_from_list(cfeuille, region, Xq_CELL);
	    focus_out_region(cfeuille, region, Xq_CELL);
	    Xc_free(region);
	  }
	}
	else
	{
	  remove_region_from_list(cfeuille, region, Xq_CELL);
	  focus_out_region(cfeuille, region, Xq_CELL);
	  Xc_free(region);
	}
      }
    }
    F(cfeuille).SelectRegion(cfeuille, column2, 0, column, 0, 
			     Xq_COLUMN, TRUE);
    d(source) = FALSE;
    return;
  }
  
  d(column) = column;
  
  F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);
  F(cfeuille).SetActiveCellule(cfeuille, column, cfeuille->row_orig, FALSE);
  select_column_region_to_xy(cfeuille, xm, ym);  

  d(AutoScroll) = (c_AutoScroll *)NEW(c_AutoScroll)
    (cfeuille->X_info.display, 0, 0,
     cfeuille->sheet_width, 0, cfeuille->ScrollBarH, NULL,
     eventlook_Select_Move_Resize_Cell, cfeuille);
}

void select_column_region_abort(cfeuille)
c_Feuille *cfeuille;
{ 
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);

  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }
  remove_region_from_list(cfeuille, &d(region), Xq_CELL);
  remove_region_from_list(cfeuille, &d(region_column), Xq_COLUMN);
  
  focus_out_region(cfeuille, &d(region_column), Xq_COLUMN);
  focus_out_region(cfeuille, &d(region), Xq_CELL);
  F(cfeuille).SetActiveCellule(cfeuille, d(old_column), d(old_row), FALSE);
  if (cfeuille->nb_cell_region==0)
    F(cfeuille).SelectRegion(cfeuille, d(old_column), d(old_row),
			     d(old_column), d(old_row), Xq_CELL, TRUE);
}

void select_column_region_done(cfeuille)
c_Feuille *cfeuille;
{  
  DATA(Select_Move_Resize_Cell);
  
  GET_DATA(Select_Move_Resize_Cell);
  
  if (d(AutoScroll))
    {
      DELETE(c_AutoScroll)(d(AutoScroll));
      d(AutoScroll) = (c_AutoScroll *)NULL;
    }
  remove_region_from_list(cfeuille, &d(region), Xq_CELL);
  remove_region_from_list(cfeuille, &d(region_column), Xq_COLUMN);
  
  F(cfeuille).SelectRegion(cfeuille, d(region_column).x_min, 0,
			   d(region_column).x_max, 0, Xq_COLUMN, FALSE);
  F(GlobEditBar).query_aff(GlobEditBar, d(column), 0);
  d(source)=FALSE;
}

void select_column_region_to_xy(cfeuille,xm,ym)
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
      if (!(column==d(region).x_min || column==d(region).x_max) ||
	  (column==d(column)))
	{
	  if (d(prems))
	    {
	      d(region).x_min = d(region).x_max = column;
	      d(region).y_min = 0;
	      d(region).y_max = ROW_MAX;
	      d(region_column).x_min = d(region_column).x_max = column;
	      d(region_column).y_min = d(region_column).y_max = 0;
	      d(region).NextRegion = NULL;
	      d(region_column).NextRegion = NULL;
	      
	      add_region_in_list(cfeuille, &d(region), Xq_CELL);
	      add_region_in_list(cfeuille, &d(region_column), Xq_COLUMN);
	      
	      focus_in_region(cfeuille, &d(region), Xq_CELL);
	      focus_in_region(cfeuille, &d(region_column), Xq_COLUMN);
	      d(prems) = FALSE;
	      return;
	    }
	  focus_column_region_optimized(cfeuille, &d(region), 
					d(column), column);
	  
	  d(region).x_min = MIN(d(column),column);
	  d(region).y_min = 0;
	  d(region).x_max = MAX(d(column),column);
	  d(region).y_max = ROW_MAX;
	  d(region_column).x_min = MIN(d(column),column);
	  d(region_column).x_max = MAX(d(column),column);
	  d(region_column).y_min = d(region_column).y_max = 0;
	  F(GlobEditBar).drag_aff(GlobEditBar, 
				  d(region).x_max-d(region).x_min+1, 0);
	}
    }
}

void select_column_region_trace(cfeuille)
c_Feuille *cfeuille;
{  
}
